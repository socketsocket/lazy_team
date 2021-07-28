#ifndef WEBSERV_RESPONSE_HPP
#define WEBSERV_RESPONSE_HPP

#include <string>
#include <iostream>
#include <map>

class Response {
	private:
		/* data */
		int		state;
		std::string	status_code;
		std::map<std::string, std::string> header;
		std::string	body;

	public:
		Response(/* args*/);
		Response(const Response& ref);
		~Response();
		Response& operator=(const Response& ref);
};

#endif
