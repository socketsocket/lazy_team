#ifndef WEBSERV_REQUEST_HPP_
#define WEBSERV_REQUEST_HPP_

#include "Webserv.hpp"
#include "Server.hpp"
#include <string>
#include <iostream>
#include <map>

class Request {
	private:
		/* data */
		int									status;
		Method								method;
		std::string							uri;
		std::string							version;
		std::map<std::string, std::string>	header;
		std::string							body;
		Location							location;

	public :
		Request(/* args*/);
		Request(const Request &ref);
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
		std::map<std::string, std::string>&			getHeader();
		const std::string&							getBody();
		Location									getLocation() const;
		void										setLocation(const Location& location);
};

#endif
// void Response::AppendBody(std::string buffer){
// 	this->body += buffer;
// }
// Location	Request::getLocation() const
// {
// 	return location;
// }
// void	Request::setLocation(const Location &location)
// {
// 	this->location = location;
// }
