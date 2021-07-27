#ifndef WEBSERV_RESPONSE_HPP
#define WEBSERV_RESPONSE_HPP

#include <string>
#include <iostream>

class Response
{
	private:
		/* data */
	public:
		Response(/* args*/);
		Response(const Response &ref);
		~Response();
		Response& operator=(const Response &ref);
};

#endif
