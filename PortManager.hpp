#ifndef WEBSERV_PORTMANAGER_HPP
#define WEBSERV_PORTMANAGER_HPP

#include <map>
#include "Webserv.hpp"
#include "Server.hpp"

class PortManager {
	private:
		PortManager();
		const int							port_fd;
		const std::vector<Server>			servers;
		PortManager&	operator=(const PortManager& ref);
		std::vector<const Server>::iterator	findServer(std::string&);

	public:
		PortManager(const int port_fd, const std::vector<Server> servers);
		PortManager(const PortManager& ref);
		~PortManager();
		void	sendRequest(std::deque<Re3>::iterator);
};

#endif
