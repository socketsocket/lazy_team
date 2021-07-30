#ifndef WEBSERV_PORTMANAGER_HPP
#define WEBSERV_PORTMANAGER_HPP

#include <map>
#include "Webserv.hpp"
#include "Server.hpp"

class PortManager {
	private:
		const int							port_fd;
		const Server&						default_server;
		const std::map<std::string, Server>	servers;

		PortManager();
		PortManager(const PortManager& ref);
		PortManager&	operator=(const PortManager& ref);

	public:
		PortManager(const int port_fd, const Server& default_server,\
			const std::map<std::string, Server> servers);
		~PortManager();


		//fd   	 openResource(client); // 리스폰스 조금 만들고 리소스 오픈까지{
				//  servers[client.request.headaer.host].openResource(client);
		// }
		//result makeResponse(client, resource);       /// 나머지 리스폰스 만들기{
				//  servers[client.request.headaer.host].makeresponse(client, resource);
		// }
};

#endif
