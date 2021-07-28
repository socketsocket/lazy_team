#ifndef WEBSERV_REQUEST_HPP_
#define WEBSERV_REQUEST_HPP_

#include "Webserv.hpp"
#include <string>
#include <iostream>
#include <map>

class Request {
	private:
		/* data */
		int									state;
		std::string							method;
		std::string							uri;
		std::string							version;
		std::map<std::string, std::string>	header;
		std::string							body;
	public:
		Request(/* args*/);
		Request(const Request &ref);
		~Request();
		Request& operator=(const Request &ref);

		//setter
		//getter
		const std::string&							getMethod();
		const std::string&							getUri();
		const std::string&							getVersion();
		const std::map<std::string, std::string>&	getHeader();
		const std::string&							getBody();
};

#endif
