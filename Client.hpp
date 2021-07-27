#ifndef WEBSERV_CLIENT_HPP_
#define WEBSERV_CLIENT_HPP_

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <queue>

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

class Client{
	private:
		Client();

		/* data */
		int				status;
		int				client_fd;
		Server&			linked_server;

		unsigned long	last_request_time;
		unsigned long	last_response_time;

		std::queue<Request>		requests;
		std::queue<Response>	responses;

	public:
		Client(int client_fd);
		Client(const Client &ref);
		~Client();
		Client& operator=(const Client &ref);
};

#endif
