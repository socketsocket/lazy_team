#ifndef WEBSERV_SERVER_HPP_
#define WEBSERV_SERVER_HPP_

#include "Location.hpp"

class Server
{
	private:
		int								server_fd;
		int								port;
		std::string						server_name;
		std::string						default_root;
		std::map<int, std::string>		default_error_page;
		unsigned long					client_body_limit;
		std::map<std::string, Location>	locations;

	public:
		Server(/* args*/);
		Server(const Server &ref);
		~Server();
		Server& operator=(const Server &ref);
};

#endif
