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
		const std::vector<Server*>			servers;

		PortManager();
		PortManager(const PortManager& ref);
		PortManager&	operator=(const PortManager& ref);
		Server*	findServer(const std::string& host_name) const; // NOTE 나중에 const 붙일 것

	public:
		PortManager(const unsigned int port_num, const int port_fd, const std::vector<Server*> servers);
		~PortManager();
		int	getPortNum() const;
		int	getPortFd() const;
		ServerStatus	passRequest(Re3* ptr) const; // NOTE 얘도 const
		ServerStatus	passResource(Re3* ptr) const;
};

#endif
