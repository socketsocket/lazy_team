#include "ServerManager.hpp"

//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------

ServerManager::ServerManager()
	: status(INITIATED) {}

int	ServerManager::callKevent() {
	int	num_events = kevent(this->kq, &this->event_changes[0], event_changes.size(), event_list, EVENT_SIZE, NULL);
	event_changes.clear();
	return num_events;
}

int	ServerManager::makeClient(PortManager& port_manager) {
	size_t	client_fd;
	if ((client_fd = accept(port_manager.getPortFd(), NULL, NULL)) < 0) {
		putErr("accept error\n");
		return ERROR;
	}
	if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&this->recv_timeout, sizeof(struct timeval)) < 0)
		return putErr("setsockopt: recv_timeout set failed\n");
	if (setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, (void*)&this->send_timeout, sizeof(struct timeval)) < 0)
		return putErr("setsockopt: send_timeout set failed\n");

	fcntl(client_fd, F_SETFL, O_NONBLOCK);

	this->setEvent(client_fd, EVFILT_READ, EV_ADD);
	this->setEvent(client_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE);

	if (this->types.size() < client_fd + 1)
		this->types.resize(client_fd + 1, kBlank);
	if (this->clients.size() < client_fd + 1)
		this->clients.resize(client_fd + 1, NULL);
	types[client_fd] = kClientFd;
	clients[client_fd] = new Client(client_fd, port_manager);
	putMsg(std::string() + "New client connected on port " \
		+ std::to_string(port_manager.getPortNum()) + '\n');
	return OK;
}

void	ServerManager::checkStdBuffer() {
	if (this->stdFdSwitch[STDOUT] && !hasMsg(STDOUT)) {
		this->setEvent(STDOUT, EVFILT_WRITE, EV_DISABLE);
		this->stdFdSwitch[STDOUT] = false;
	} else if (!this->stdFdSwitch[STDOUT] && hasMsg(STDOUT)) {
		this->setEvent(STDOUT, EVFILT_WRITE, EV_ENABLE);
		this->stdFdSwitch[STDOUT] = true;
	}
	if (this->stdFdSwitch[STDERR] && !hasMsg(STDERR)) {
		this->setEvent(STDERR, EVFILT_WRITE, EV_DISABLE);
		this->stdFdSwitch[STDERR] = false;
	} else if (!this->stdFdSwitch[STDERR] && hasMsg(STDERR)) {
		this->setEvent(STDERR, EVFILT_WRITE, EV_ENABLE);
		this->stdFdSwitch[STDERR] = true;
	}
}

void	ServerManager::setEvent(int fd, int filter, int flag) {
	EV_SET(&this->event_current, fd, filter, flag, 0, 0, NULL); // set stderr kevent.
	this->event_changes.push_back(this->event_current);
}

void	ServerManager::setClients(int fd, Client* client) {
	if (static_cast<int>(this->types.size()) < fd + 1)
		this->types.resize(fd + 1);
	this->types[fd] = kClientFd;
	if (static_cast<int>(this->clients.size()) < fd + 1)
		this->clients.resize(fd + 1);
	this->clients[fd] = client;
}

void	ServerManager::setRe3s(int fd, Re3* re3) {
	if (static_cast<int>(this->types.size()) < fd + 1)
		this->types.resize(fd + 1);
	this->types[fd] = kResourceFd;
	if (static_cast<int>(this->re3s.size()) < fd + 1)
		this->re3s.resize(fd + 1);
	this->re3s[fd] = re3;
}

void	ServerManager::setAndPassResource(Re3* re3, Status status) {
	Resource*	resource = re3->getRscPtr();
	resource->setStatus(status);
	ServerStatus	ss = this->managers[re3->getPortId()]->passResource(re3);
	if (ss == kResponseMakingDone)
		this->setEvent(re3->getClientId(), EVFILT_WRITE, EV_ENABLE);
}

int	ServerManager::clientReadEvent() {
	this->checker = recv(this->cur_fd, this->recv_buffer, NETWORK_BUFF, 0);
	putMsg(this->recv_buffer); // ANCHOR for test
	if (this->checker == ERROR)
		return ERROR;
	std::vector<std::pair<Re3*, ServerStatus> >	resource_status \
		= this->clients[this->cur_fd]->recvRequest(\
		std::string(this->recv_buffer, this->checker));
	for (size_t i = 0; i < resource_status.size(); ++i) {
		Re3* re3 = resource_status[i].first;
		if (re3->getReqPtr()->getStatus() == kReadFail) {
			close(this->clients[this->cur_fd]->getClientFd());
			delete clients[this->cur_fd];
			break;
		}
		ServerStatus ss = resource_status[i].second;
		int fd = re3->getRscPtr()->getResourceFd();
		if (ss == kResourceReadInit) {
			this->setEvent(fd, EVFILT_READ, EV_ADD);
			this->setRe3s(fd, re3);
		} else if (ss == kResourceWriteInit) {
			int fd = re3->getRscPtr()->getResourceFd();
			this->setEvent(fd, EVFILT_WRITE, EV_ADD);
			this->setRe3s(fd, re3);
		}
		else if (ss == kResponseMakingDone)
			this->setEvent(re3->getClientId(), EVFILT_WRITE, EV_ENABLE);
		else if (ss == kResponseError) {
			// 500 ?? 종료
		}
	}
	return OK;
}

int	ServerManager::clientWriteEvent() {
	std::string	str = clients[this->cur_fd]->passResponse();
	putMsg(str + "!"); // ANCHOR for test
	if (str.length() < NETWORK_BUFF)
		this->setEvent(this->cur_fd, EVFILT_WRITE, EV_DISABLE);

	this->checker = send(this->cur_fd, str.c_str(), str.length(), 0);
	if (this->checker == ERROR) {
		this->setEvent(this->cur_fd, EVFILT_WRITE, EV_DISABLE);
		return ERROR;
	}
	if (this->checker < static_cast<int>(str.length())) {
		this->setEvent(this->cur_fd, EVFILT_WRITE, EV_DISABLE);
		return ERROR;
	}
	return OK;
}

int	ServerManager::resourceReadEvent() {
	Re3*		re3 = this->re3s[this->cur_fd];
	Resource*	resource = re3->getRscPtr();
	this->checker = read(this->cur_fd, this->read_buffer, LOCAL_BUFF);
	// error check
	if (this->checker < 0) {
		putErr("Read resource failed\n");
		this->setAndPassResource(re3, kReadFail);
		return ERROR;
	}
	if (this->checker == 0 && resource->getContent().length() == 0)
		return OK;

	// read done.
	if (this->checker < LOCAL_BUFF) {
		resource->addContent(std::string(this->read_buffer, this->checker));
		this->setAndPassResource(re3, kFinished);
	} else { // this->checker == LOCAL_BUFF
		resource->addContent(std::string(this->read_buffer, LOCAL_BUFF));
	}
	return OK;
}

int	ServerManager::resourceWriteEvent() {
	Re3*		re3 = this->re3s[this->cur_fd];
	Resource*	resource = re3->getRscPtr();
	std::string	write_str = resource->getContent(LOCAL_BUFF);
	if (write_str.length() == 0) {
		this->setAndPassResource(re3, kFinished);
	}
	this->checker = write(this->cur_fd, write_str.c_str(), write_str.length());

	if (this->checker < 0) {
		putErr("Write resource failed\n");
		this->setAndPassResource(re3, kWriteFail);
		return ERROR;
	}

	// nothing left in resource
	if (write_str.length() < LOCAL_BUFF) {
		// error
		if (this->checker < static_cast<int>(write_str.length())) {
			putErr("Write resource not finished\n");
			this->setAndPassResource(re3, kWriteFail);
			return ERROR;
		}
		// No error
		this->setAndPassResource(re3, kFinished);
	}
	return OK;
}

void	ServerManager::clientSocketClose() {
	close(this->cur_fd);
	delete this->clients[this->cur_fd];
	this->types[this->cur_fd] = kBlank;
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

ServerManager::~ServerManager() {
	for (size_t i = 0; i < this->types.size(); ++i) {
		if (types[i] == kPortFd) {
			close(i);
			delete managers[i];
		} else if (types[i] == kClientFd) {
			close(i);
			delete clients[i];
		}
	}
}

// Returns an instance of singleton class, ServerManager.
ServerManager&	ServerManager::getServerManager() {
	static ServerManager	instance = ServerManager();
	return instance;
}

int	ServerManager::initServerManager( \
	const std::vector<std::pair<Server, std::vector<unsigned int> > > configs) {
	if (this->status != INITIATED)
		return ERROR;
	this->status = OK;

	// get ready to add stderr fd
	this->kq = kqueue();
	this->setEvent(STDOUT, EVFILT_WRITE, EV_ADD);
	this->setEvent(STDERR, EVFILT_WRITE, EV_ADD);
	types.resize(STDERR + 1);
	this->types[STDOUT] = kStdOutErrFd;
	this->types[STDERR] = kStdOutErrFd;
	this->stdFdSwitch[STDOUT] = true;
	this->stdFdSwitch[STDERR] = true;

	std::map<unsigned int, std::vector<Server*> >	server_sorter;

	this->servers.reserve(configs.size());
	for (size_t i = 0; i < configs.size(); ++i) {
		this->servers.push_back(configs[i].first);
		for (size_t j = 0; j < configs[i].second.size(); ++j) {
			server_sorter[configs[i].second[j]].push_back(&this->servers.back());
		}
	}

	// Set port fd.
	unsigned long		server_socket;
	struct sockaddr_in	server_addr;
	for (std::map<unsigned int, std::vector<Server*> >::iterator it = server_sorter.begin(); \
		it != server_sorter.end(); ++it) {
		if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
			putErr("socket error\n");
			this->status = ERROR;
			break;
		}
		if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&this->recv_timeout, sizeof(struct timeval)) < 0) {
			this->status = ERROR;
			return putErr("setsockopt: recv_timeout set failed\n");
		}
		if (setsockopt(server_socket, SOL_SOCKET, SO_SNDTIMEO, (void*)&this->send_timeout, sizeof(struct timeval)) < 0) {
			this->status = ERROR;
			return putErr("setsockopt: send_timeout set failed\n");
		}
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons((*it).first);

		if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
			putErr("bind error\n");
			this->status = ERROR;
			break;
		}

		if (listen(server_socket, MAX_CLIENT)) {
			putErr("listen error\n");
			this->status = ERROR;
			break;
		}

		fcntl(server_socket, F_SETFL, O_NONBLOCK);

		this->setEvent(server_socket, EVFILT_READ, EV_ADD);
		putMsg(std::string("Port number: ") + std::to_string((*it).first) + "\n");
		if (this->types.size() < server_socket)
			this->types.resize(server_socket + 1, kBlank);
		this->types[server_socket] = kPortFd;
		if (this->managers.size() < server_socket)
			this->managers.resize(server_socket + 1, NULL);
		this->managers[server_socket] = new PortManager(it->first, server_socket, (*it).second);
	}
	putMsg("Server initialized\n");
	return OK;
}

void	ServerManager::setStatus(int status) {
	this->status = status;
}

void	ServerManager::setSendTimeOut(unsigned long send_timeout) {
	this->send_timeout.tv_sec = send_timeout;
	this->send_timeout.tv_usec = 0;
}

void	ServerManager::setRecvTimeOut(unsigned long recv_timeout) {
	this->recv_timeout.tv_sec = recv_timeout;
	this->recv_timeout.tv_usec = 0;
}

int		ServerManager::getStatus() {
	return this->status;
}

std::vector<Server>&	ServerManager::getServersRef() {
	return this->servers;
}

int	ServerManager::processEvent() {
	this->kevent_num = this->callKevent();
	if (this->kevent_num < 0)
		return this->kevent_num;

	for (int i = 0; i < this->kevent_num; ++i) {
		this->cur_fd = this->event_list[i].ident;
		if (this->event_list[i].flags & EV_ERROR) {
			switch (this->types[this->cur_fd]) {
				case kPortFd: {
					putErr("Server socket error\n");
					close(this->cur_fd);
					delete this->managers[this->cur_fd];
					this->managers[this->cur_fd] = NULL;
					this->types[this->cur_fd] = kBlank;
					break;
				}
				case kClientFd: {
					putErr("Client socket error\n");
					this->clientSocketClose();
					break;
				}
				case kResourceFd: {
					putErr("Resource error\n");
					close(this->cur_fd);
					if (this->event_list[i].filter == EVFILT_WRITE) {
						this->re3s[this->cur_fd]->getRscPtr()->setStatus(kWriteFail);
					} else if (this->event_list[i].filter == EVFILT_READ) {
						this->re3s[this->cur_fd]->getRscPtr()->setStatus(kReadFail);
					} else {
						assert(false);
					}
					break;
				}
				default: {
					assert(false);
				}
			}
		}
		switch (this->types[this->cur_fd]) {
			case kPortFd: {
				if (this->makeClient(*this->managers[this->cur_fd]) == ERROR) {
					putErr("Recv error\n");
				}
				break;
			}
			case kClientFd: {
				if (event_list[i].flags & EV_EOF) {
					putMsg("Client closed connection\n");
					this->clientSocketClose();
					continue;
				}
				if (event_list[i].filter == EVFILT_READ) {
					if (this->clientReadEvent() == ERROR) {
						putErr("Client read error\n");
						this->clientSocketClose();
					}
				} else if (event_list[i].filter == EVFILT_WRITE) {
					if (this->clientWriteEvent() == ERROR) {
						putErr("Client write error\n");
						this->clientSocketClose();
					}
					// send(this->cur_fd, this->recv_buffer, this->checker, 0); // ANCHOR for test
				} else {
					assert(false);
				}
				// clients[i].callClient;
				break;
			}
			case kResourceFd: {
				if (event_list[i].filter == EVFILT_READ) {
					if (this->resourceReadEvent() == ERROR) {
						putErr("Resource read error");
					}
				} else if (event_list[i].filter == EVFILT_WRITE) {
					if (this->resourceWriteEvent() == ERROR) {
						putErr("Resource write error");
					}
				}
				break;
			}
			case kStdOutErrFd: {
				this->msg = getMsg(this->cur_fd);
				if (this->msg.length())
					write(this->cur_fd, this->msg.c_str(), this->msg.length());
				break;
			}
			default: {
				this->status = ERROR;
				return ERROR;
			}
		}
	}
	this->checkStdBuffer();
	// if (ptr != NULL) // ANCHOR
	// {
	// 	delete[] ptr;
	// 	ptr = NULL;
	// }
	return OK;
}
