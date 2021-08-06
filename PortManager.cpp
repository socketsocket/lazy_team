#include "PortManager.hpp"

PortManager::PortManager(const PortManager& ref)
	: port_num(ref.port_num), port_fd(ref.port_fd), servers(ref.servers) {}

PortManager::PortManager(
	const unsigned int port_num,
	const int port_fd,
	const std::vector<Server*> servers)
	: port_num(port_num), port_fd(port_fd), servers(servers) {}


PortManager::~PortManager() {}

std::vector<Server*>::const_iterator PortManager::findServer(const std::string& host_name) const {
	for (std::vector<Server*>::const_iterator serv_it = servers.begin(); \
	serv_it < servers.end(); ++serv_it) {
		if (host_name == (*serv_it)->getServerName())
			return serv_it;
	}
	return servers.begin();
}

int	PortManager::getPortNum() const {
	return this->port_num;
}

int	PortManager::passRequest(Re3* ptr) const {
	Server*	tmp = *this->findServer(ptr->getReqPtr()->getHeaderValue("host"));
	tmp->makeResponse(ptr);
	// FIXME makeResponse의 리턴값 이용하지 않음.
	return OK;
}
