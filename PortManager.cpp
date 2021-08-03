#include "PortManager.hpp"

PortManager::PortManager(const int port_fd, const std::vector<Server> servers)
	: port_fd(port_fd), servers(servers) {}

PortManager::PortManager(const PortManager& ref)
	: port_fd(ref.port_fd), servers(ref.servers) {}

PortManager::~PortManager() {}

std::vector<const Server>::iterator PortManager::findServer(std::string& host_name) {
	for (std::vector<const Server>::iterator serv_it = servers.begin();\
	serv_it < servers.end(); ++serv_it) {
		if (host_name == serv_it->getServerName())
			return serv_it;
	}
}

void	PortManager::sendRequest(std::deque<Re3>::iterator it) {
	Request& req = *it->getReqPtr();
	this->findServer(req.getHeader()["host"])->MakeResponse(it);
}
