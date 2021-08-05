#ifndef WEBSERV_REQUEST_HPP_
#define WEBSERV_REQUEST_HPP_

#include "Webserv.hpp"
#include "Server.hpp"
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <map>

class Request {
	private:
		int									status;
		Method								method;
		std::string							uri;
		std::string							version;
		std::map<std::string, std::string>	headers;
		std::string							body;

	public :
		Request(/* args*/);
		~Request();
		Request& operator=(const Request &ref);

		//setter
		int	setStatus(int);
		int	setMethod(int);
		int	setUri(std::string&);
		int	setVersion(std::string&);
		int	insertHeader(std::string& key, std::string& value);
		int	appendBody(std::string& buffer);

		//getter
		const int&									getStatus();
		const Method&								getMethod();
		const std::string&							getUri();
		const std::string&							getVersion();
		std::map<std::string, std::string>&			getHeaders();
		const std::string&							getBody();
};

#endif
