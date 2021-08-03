#ifndef WEBSERV_SERVER_HPP_
#define WEBSERV_SERVER_HPP_

#include "Webserv.hpp"
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
		std::string							server_name;
		std::string							default_root;
		std::map<stat_type, std::string>	default_error_pages;
		unsigned long						client_body_limit;
		std::vector<Location>				locations;
		std::pair<stat_type, std::string>	return_to;

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

		Server();
		Server&	operator=(const Server &ref);

	public:
		Server(const Server &ref);
		Server(
			std::string server_name,
			std::string default_root,
			std::map<stat_type, std::string> default_error_pages,
			unsigned long client_body_limit,
			std::vector<Location> locations,
			std::pair<stat_type, std::string> return_to);
		~Server();

		unsigned int	getPortNum() const;
};

#endif
