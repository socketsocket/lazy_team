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

		Location&	currLocation(std::string request_uri);
		int			requestValidCheck(Request& request);
		Response		makeResponse(Request& request, Resource& resource_queue);
		Response 		makeGetResponse(Request& request, std::string resource_path, Resource& resource_queue);
		int			checkPath(std::string path);
		std::string	dateHeaderInfo();
		std::string	lastModifiedHeaderInfo(struct stat sb);
		std::string contentTypeHeaderInfo(std::string extension);
		std::string fileExtension(std::string resource_path);
		std::string	makeAutoIndexPage(Request& request, std::string resource_path);
		std::string makeHTMLPage(std::string str);
		Response	errorResponse(Request& request, std::string http_status_code);

	public:
		Server(/* args*/);
		Server(const Server &ref);
		~Server();
		Server& operator=(const Server &ref);
};

#endif
