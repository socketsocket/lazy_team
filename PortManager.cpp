#include "PortManager.hpp"

PortManager::PortManager(const PortManager& ref)
	: port_fd(ref.port_fd), servers(ref.servers) {}

PortManager::~PortManager() {}

std::vector<const Server>::iterator PortManager::findServer(std::string& host_name) {
	for (std::vector<const Server>::iterator serv_it = servers.begin();\
	serv_it < servers.end(); ++serv_it) {
		if (host_name == serv_it->getServerName())
			return serv_it;
	}
	return servers.begin();
}

int	PortManager::passRequest(Re3_iter it) {
	Request& req = *it->getReqPtr();
	this->findServer(req.getHeader()["host"])->MakeResponse(it);
	return OK;
}
