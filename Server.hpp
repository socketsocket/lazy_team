#ifndef WEBSERV_SERVER_HPP_
#define WEBSERV_SERVER_HPP_

#include "Location.hpp"
#include "Client.hpp"
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>


class Server
{
	private:
		int								server_fd;
		int								port;
		std::string						server_name;
		std::string						default_root;
		std::map<int, std::string>		default_error_page;
		unsigned long					client_body_limit;
		std::vector<Location>			locations;

		Location&	CurrLocation(std::string request_uri);
		int			RequestValidCheck(Client& client);
		void		MakeResponse(Client& client);
		void 		MakeGetResponse(Client& client, std::string resource_path);
		int			checkPath(std::string path);
		std::string	DateHeaderInfo();
		std::string	LastModifiedHeaderInfo(struct stat sb);
		std::string ContentTypeHeaderInfo(std::string extension);
		std::string fileExtension(std::string resource_path);
		std::string	MakeAutoIndexPage(Request& request, std::string resource_path);
		void ErrorResponse(int http_status_code);

	public:
		Server(/* args*/);
		Server(const Server &ref);
		~Server();
		Server& operator=(const Server &ref);
};

#endif
