#include "PortManager.hpp"

PortManager::PortManager(const int port_fd, const Server& default_server,\
	const std::map<std::string, Server> servers)
	: port_fd(port_fd), default_server(default_server), servers(servers) {}

PortManager::~PortManager() {}