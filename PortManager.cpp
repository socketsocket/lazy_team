#include "PortManager.hpp"

PortManager::PortManager(const unsigned int port_num, const int port_fd, const std::vector<Server&> servers)
	: port_num(port_num), port_fd(port_fd), servers(servers) {}

PortManager::~PortManager() {}

unsigned int	PortManager::getPortNum() const {
	return port_num;
}