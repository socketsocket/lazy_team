#include "ServerManager.hpp"

//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------

ServerManager::ServerManager()
	: status(INITIATED) {}

int	ServerManager::makeClient(int port_fd, PortManager& port_manager) {
	int	client_fd;
	if (client_fd = accept(port_fd, NULL, NULL) < 0) {
		putError("accept error");
		return ERROR;
	}

	if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&this->recv_timeout, sizeof(unsigned long)) < 0)
		return putError("setsockopt: recv_timeout set failed");
	if (setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&this->send_timeout, sizeof(unsigned long)) < 0)
		return putError("setsockopt: send_timeout set failed");

	fcntl(client_fd, F_SETFL, O_NONBLOCK);

	EV_SET(&event_current, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	this->event_changes.push_back(event_current);
	EV_SET(&event_current, client_fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	this->event_changes.push_back(event_current);

	if (this->types.size() < client_fd)
		this->types.resize(client_fd, Blank);
	if (this->clients.size() < client_fd)
		this->clients.resize(client_fd, NULL);
	types[client_fd] = ClientFd;
	clients[client_fd] = new Client(client_fd, port_manager);
	return OK;
}

int	ServerManager::callKevent() {
	int	num_events = kevent(this->kq, &this->event_changes[0], event_changes.size(), event_list, EVENT_SIZE, NULL);
	event_changes.clear();
	return num_events;
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

ServerManager::~ServerManager() {
	for (size_t i = 0; i < this->types.size(); ++i) {
		if (types[i] == PortFd) {
			delete managers[i];
		} else if (types[i] == ClientFd) {
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
	EV_SET(&this->event_current, STDERR, EVFILT_WRITE, EV_ADD, 0, 0, NULL); // set stderr kevent.
	this->event_changes.push_back(this->event_current);
	this->types[STDERR] = StderrFd;

	std::map<unsigned int, std::vector<Server&> >	server_sorter;

	for (size_t i = 0; i < configs.size(); ++i) {
		servers.push_back(configs[i].first);
		for (size_t j = 0; j < configs[i].second.size(); ++j) {
			server_sorter[configs[i].second[j]].push_back(servers.back());
		}
	}

	// Set port fd.
	int					server_socket;
	struct sockaddr_in	server_addr;
	for (std::map<unsigned int, std::vector<Server&> >::iterator it = server_sorter.begin(); \
		it != server_sorter.end(); ++it) {
		if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
			putError("socket error\n");
			this->status = ERROR;
			break;
		}
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons((*it).first);

		if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
			putError("bind error\n");
			this->status = ERROR;
			break;
		}

		if (listen(server_socket, MAX_CLIENT) == -1) {
			putError("listen error\n");
			this->status = ERROR;
			break;
		}

		fcntl(server_socket, F_SETFL, O_NONBLOCK);

		if (this->types.size() < server_socket)
			this->types.resize(server_socket, Blank);
		if (this->managers.size() < server_socket)
			this->managers.resize(server_socket, NULL);
		this->managers[server_socket] = new PortManager(it->first, server_socket, (*it).second);
	}
}

void	ServerManager::setStatus(int status) {
	this->status = status;
}

int		ServerManager::getStatus() {
	return this->status;
}

std::vector<Server>&	ServerManager::getServersRef() {
	return this->servers;
}

int	ServerManager::processEvent() {
	int	num = this->callKevent();
	if (num < 0)
		return num;

	int	fd;
	for (size_t i = 0; i < num; ++i) {
		fd = this->event_list[i].ident;
		switch (this->types[fd]) {
			case PortFd: {
				// managers[i].callPortManager;
				break;
			}
			case ClientFd: {
				if (event_list[i].filter == EVFILT_READ) {
					//
				} else if (event_list[i].filter == EVFILT_WRITE) {
					//
				}
				// clients[i].callClient;
				break;
			}
			case ResourceFd: {
				// resources[i].readContent;
				// if (resources[i].status() == done)
				//	resources[i].callServer();
				break;
			}
			case StderrFd: {
				sendError();
				break;
			}
			default: {
				return ERROR;
			}
		}
	}
}
