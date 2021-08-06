#ifndef WEBSERV_REQUEST_HPP_
#define WEBSERV_REQUEST_HPP_

#include "Webserv.hpp"
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
		Request(const Request &ref);
		~Request();
		Request& operator=(const Request &ref);

		//setter
		void	setStatus(int);
		void	setMethod(int);
		void	setUri(std::string&);
		void	setVersion(std::string&);
		void	insertHeader(std::string& key, std::string& value);
		void	appendBody(std::string& buffer);

		//getter
		const int&			getStatus() const;
		const Method&		getMethod() const;
		const std::string&	getUri() const;
		const std::string&	getVersion() const;
		std::string			getHeaderValue(const std::string& key) const;
		const std::string&	getBody() const;
};

#endif
