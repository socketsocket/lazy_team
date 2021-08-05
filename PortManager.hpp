#ifndef WEBSERV_PORTMANAGER_HPP
#define WEBSERV_PORTMANAGER_HPP

#include <map>
#include "Webserv.hpp"
#include "Server.hpp"
#include "Re3.hpp"

class PortManager {
	private:
		const unsigned int					port_num;
		const int							port_fd;
		const std::vector<Server&>			servers;

		PortManager();
		PortManager(const PortManager& ref);
		PortManager&	operator=(const PortManager& ref);

		std::vector<const Server>::iterator		findServer(std::string& host_name);
	public:
		PortManager(const unsigned int port_num, const int port_fd, const std::vector<Server&> servers);
		~PortManager();

		int	passRequest(Re3Iter it);
};

#endif
