#ifndef WEBSERV_RESOURCE_HPP_
#define WEBSERV_RESOURCE_HPP_

#include <string>
#include <iostream>

class Resource
{
	private:
		/* data */
	public:
		Resource(/* args*/);
		Resource(const Resource &ref);
		~Resource();
		Resource& operator=(const Resource &ref);
};

#endif
