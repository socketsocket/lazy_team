#ifndef WEBSERV_SERVER_HPP_
#define WEBSERV_SERVER_HPP_

#include "Webserv.hpp"
#include "Location.hpp"
#include "Client.hpp"
#include "Re3.hpp"
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>


class Server
{
	private:
		unsigned int						port;
		std::string							server_name;
		std::string							default_root;
		std::map<stat_type, std::string>	default_error_pages;
		unsigned long						client_body_limit;
		std::vector<Location>				locations;
		std::pair<stat_type, std::string>	return_to;

		int 		makeResponse(Re3_iter re3);
		int 		makeGetResponse(Re3_iter re3, Location* curr_location, std::string resource_path);
		int 		makePostResponse(Re3_iter re3, Location* curr_location, std::string resource_path);
		int 		makeDeleteResponse(Re3_iter re3, Location* curr_location, std::string resource_path);
		int			errorResponse(Re3_iter re3, Location* curr_location, stat_type http_status_code);
		
		stat_type	requestValidCheck(Request* request, Location* curr_location);
		Location*	currLocation(std::string request_uri);
		int			checkPath(std::string path);
		std::string	dateHeaderInfo();
		std::string	lastModifiedHeaderInfo(struct stat sb);
		std::string contentTypeHeaderInfo(std::string extension);
		std::string fileExtension(std::string resource_path);
		std::string	makeAutoIndexPage(Request* request, std::string resource_path);
		std::string makeHTMLPage(std::string str);

		Server();
		Server&	operator=(const Server &ref);

	public:
		Server(const Server &ref);
		Server(
			unsigned int port,
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
