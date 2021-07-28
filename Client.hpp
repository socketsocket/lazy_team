#ifndef WEBSERV_CLIENT_HPP_
#define WEBSERV_CLIENT_HPP_

#include "Webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <queue>
#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>

#define READSIZE 1024


// class	Client
// {
// 	int				status
// 	int				client_fd;
// 	Server&			linked_server

// 	unsigned long	last_request_time;
// 	unsigned long	last_response_time;

// 	queue<Request>	requests;
// 	queue<Response>	responses;
// }

class Client {
	private:
		// Client();
		const int		client_fd;
		Server&			linked_server;

		int				state;

		unsigned long	last_request_time;
		unsigned long	last_response_time;

		size_t					buff_size;
		std::stringstream		read_buff;
		std::queue<Request*>	requests;
		std::queue<Response*>	responses;

		int	chunkedParser(Request& request);
		int	lengthParser(Request& request);
		int	headerParser(Request& request);
		int	Parser(void);

	public:
		Client(int client_fd, Server& linked_server);
		Client(const Client &ref);
		~Client();
		Client& operator=(const Client &ref);

		int	readRequest();
		int	writeResponse();
};

#endif
