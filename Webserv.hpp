#ifndef WEBSERV_WEBSERV_HPP_
#define WEBSERV_WEBSERV_HPP_

enum	FdType {
	server,
	client,
	resource
};

enum	Status{
	nothing,
	working,
	finished
};

#define GET    0b001
#define POST   0b010
#define DELETE 0b100

typedef	unsigned char	method;

#endif
