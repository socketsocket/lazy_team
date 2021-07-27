#ifndef WEBSERV_REQUEST_HPP_
#define WEBSERV_REQUEST_HPP_

#include <string>
#include <iostream>

class Request
{
	private:
		/* data */
	public:
		Request(/* args*/);
		Request(const Request &ref);
		~Request();
		Request& operator=(const Request &ref);
};

#endif
