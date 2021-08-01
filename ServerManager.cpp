#include "ServerManager.hpp"

ServerManager::ServerManager(const std::vector<Server> servers) {
	// get ready to add stderr fd
	this->kq = kqueue();
	EV_SET(&this->event_current, STDERR, EVFILT_WRITE, EV_ADD, 0, 0, NULL); // set stderr kevent.
	this->event_changes.push_back(this->event_current);
	this->types[STDERR] = StderrFd;

	std::map<unsigned int, std::vector<Server> >	server_sorter;

	for (size_t i = 0; i < servers.size(); ++i) {
		server_sorter[servers[i].getPortNum()].push_back(servers[i]);
	}

	// Set port fd.
	int					server_socket;
	struct sockaddr_in	server_addr;
	for (std::map<unsigned int, std::vector<Server> >::iterator it = server_sorter.begin();
		it != server_sorter.end(); ++it) {
		if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
			putError("socket error\n");
			this->status = ERROR;
			break;
		}
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons((*it).first));

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

		if (managers.size() < server_socket) {
			managers.resize(server_socket, NULL);
		}
		managers[server_socket] = new PortManager(server_socket, (*it).second);
	}

}

int	ServerManager::callKevent() {
	int	num_events = kevent(this->kq, &this->event_changes[0], event_changes.size(), event_list, EVENT_SIZE, NULL);
	event_changes.clear();
	return num_events;
}

// Returns an instance of singleton class, ServerManager.
ServerManager&	ServerManager::getServerManager(const std::vector<Server> servers = std::vector<Server>(0)) {
	static ServerManager	instance = servers;
	return instance;
}

int	ServerManager::getStatus() {
	return (this->status);
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
