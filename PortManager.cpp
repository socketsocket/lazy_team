#include "PortManager.hpp"

PortManager::PortManager(const int port_fd, const std::vector<Server> servers)
	: port_fd(port_fd), servers(servers) {}

PortManager::~PortManager() {}