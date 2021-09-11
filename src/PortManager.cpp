#include "PortManager.hpp"

PortManager::PortManager(const PortManager& ref)
	: port_num(ref.port_num), port_fd(ref.port_fd), servers(ref.servers) {
	for (std::vector<Server*>::const_iterator serv_it = servers.begin(); \
	serv_it < servers.end(); ++serv_it)
		(*serv_it)->setPortNum(this->port_num);
}

PortManager::PortManager(
	const unsigned int port_num,
	const int port_fd,
	const std::vector<Server*> servers)
	: port_num(port_num), port_fd(port_fd), servers(servers) {
	for (std::vector<Server*>::const_iterator serv_it = servers.begin(); \
	serv_it < servers.end(); ++serv_it)
		(*serv_it)->setPortNum(this->port_num);
}

PortManager::~PortManager() {}

Server* PortManager::findServer(const std::string& host_name) const {
	for (std::vector<Server*>::const_iterator serv_it = servers.begin(); \
	serv_it < servers.end(); ++serv_it) {
		if (host_name == (*serv_it)->getServerName())
			return *serv_it;
	}
	return *servers.begin();
}

int	PortManager::getPortNum() const {
	return this->port_num;
}

int	PortManager::getPortFd() const {
	return this->port_fd;
}

ServerStatus	PortManager::passRequest(Re3* ptr) const {
	ptr->setPortId(this->port_fd);
	ptr->setRscPtr(new Resource(kNothing));
	std::string	host_name = ptr->getReqPtr()->getHeaderValue("host");
	return this->findServer(host_name)->makeResponse(ptr);
}

ServerStatus	PortManager::passResource(Re3* ptr) const {
	std::string	host_name = ptr->getReqPtr()->getHeaderValue("host");
	return this->findServer(host_name)->makeResponse(ptr);
}
