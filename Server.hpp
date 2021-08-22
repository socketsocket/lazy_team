#ifndef WEBSERV_SERVER_HPP_
#define WEBSERV_SERVER_HPP_

#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "Webserv.hpp"
#include "Location.hpp"
#include "Re3.hpp"
#include "TermPrinter.hpp"
#include "CgiConnector.hpp"

class Server
{
	private:
		static std::map<std::string, std::string>	mime_types;

		const std::string							server_name;
		const std::string							default_root;
		std::map<stat_type, std::string>			default_error_pages;
		unsigned long								client_body_limit;
		const std::vector<Location>					locations;
		const std::pair<stat_type, std::string>		return_to;

		ServerStatus		makeGETResponse(Re3* re3, const Location* curr_location, std::string resource_path) const;
		ServerStatus		makePOSTResponse(Re3* re3, const Location* curr_location, std::string resource_path) const;
		ServerStatus		makeDELETEResponse(Re3* re3, const Location* curr_location, std::string resource_path) const;
		ServerStatus		makeErrorResponse(Re3* re3, const Location* curr_location, stat_type http_status_code) const;

		stat_type	requestValidCheck(Request* request, const Location* curr_location) const;
		const Location*	currLocation(std::string request_uri) const;
		int			checkPath(std::string path) const;
		std::string	dateHeaderInfo() const ;
		std::string	lastModifiedHeaderInfo(struct stat sb) const;
		std::string contentTypeHeaderInfo(std::string extension) const;
		std::string fileExtension(std::string resource_path) const;
		std::string	makeAutoIndexPage(Request* request, std::string resource_path) const;
		std::string makeHTMLPage(std::string str) const;
		bool		isCgi(Request* request, const Location* location) const;

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

		ServerStatus		makeResponse(Re3* re3) const;
		const std::string&	getServerName() const;
};

#endif
