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
		putErr("accept error");
		return ERROR;
	}
	if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&this->recv_timeout, sizeof(struct timeval)) < 0) {
		return putErr("setsockopt: recv_timeout set failed");
	}
	if (setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, (void*)&this->send_timeout, sizeof(struct timeval)) < 0)
		return putErr("setsockopt: send_timeout set failed");

	fcntl(client_fd, F_SETFL, O_NONBLOCK);

	EV_SET(&event_current, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	this->event_changes.push_back(event_current);
	EV_SET(&event_current, client_fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	this->event_changes.push_back(event_current);

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
		changeFdFlag(STDOUT, EVFILT_WRITE, EV_DISABLE);
		this->stdFdSwitch[STDOUT] = false;
	}
	else if (!this->stdFdSwitch[STDOUT] && hasMsg(STDOUT)) {
		changeFdFlag(STDOUT, EVFILT_WRITE, EV_ENABLE);
		this->stdFdSwitch[STDOUT] = true;
	}
	if (this->stdFdSwitch[STDERR] && !hasMsg(STDERR)) {
		changeFdFlag(STDERR, EVFILT_WRITE, EV_DISABLE);
		this->stdFdSwitch[STDERR] = false;
	}
	else if (!this->stdFdSwitch[STDERR] && hasMsg(STDERR)) {
		changeFdFlag(STDERR, EVFILT_WRITE, EV_ENABLE);
		this->stdFdSwitch[STDERR] = true;
	}
}

void	ServerManager::changeFdFlag(int fd, int filter, int flag) {
	EV_SET(&this->event_current, fd, filter, flag, 0, 0, NULL); // set stderr kevent.
	this->event_changes.push_back(this->event_current);
}


//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

ServerManager::~ServerManager() {
	for (size_t i = 0; i < this->types.size(); ++i) {
		if (types[i] == kPortFd) {
			delete managers[i];
		} else if (types[i] == kClientFd) {
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
	EV_SET(&this->event_current, STDOUT, EVFILT_WRITE, EV_ADD, 0, 0, NULL); // set stderr kevent.
	this->event_changes.push_back(this->event_current);
	EV_SET(&this->event_current, STDERR, EVFILT_WRITE, EV_ADD, 0, 0, NULL); // set stderr kevent.
	this->event_changes.push_back(this->event_current);
	types.resize(STDERR + 1);
	this->types[STDOUT] = kStdOutErrFd;
	this->types[STDERR] = kStdOutErrFd;
	this->stdFdSwitch[STDOUT] = false;
	this->stdFdSwitch[STDERR] = false;

	std::map<unsigned int, std::vector<Server*> >	server_sorter;

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
			return putErr("setsockopt: recv_timeout set failed");
		}
		if (setsockopt(server_socket, SOL_SOCKET, SO_SNDTIMEO, (void*)&this->send_timeout, sizeof(struct timeval)) < 0) {
			this->status = ERROR;
			return putErr("setsockopt: send_timeout set failed");
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

		EV_SET(&this->event_current, server_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
		this->event_changes.push_back(this->event_current);
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
		switch (this->types[this->cur_fd]) {
			case kPortFd: {
				this->makeClient(*this->managers[this->cur_fd]); // TODO ERROR need to be handled
				break;
			}
			case kClientFd: {
				if (event_list[i].filter == EVFILT_READ) {
					this->checker = recv(this->cur_fd, this->recv_buffer, NETWORK_BUFF, 0);
					putMsg(this->recv_buffer); // ANCHOR for test
					// if (this->checker == ERROR) NOTE need to discuss
					this->clients[this->cur_fd]->recvRequest( \
						std::string(this->recv_buffer, this->checker));
				} else if (event_list[i].filter == EVFILT_WRITE) {
					send(this->cur_fd, this->recv_buffer, this->checker, 0);
					//
				}
				// clients[i].callClient;
				break;
			}
			case kResourceFd: {
				if (event_list[i].filter == EVFILT_READ) {
				} else if (event_list[i].filter == EVFILT_WRITE) {
				}
				// resources[i].readContent;
				// if (resources[i].status() == done)
				//	resources[i].callServer();
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
	return OK;
}
