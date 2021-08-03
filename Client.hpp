#ifndef WEBSERV_CLIENT_HPP_
#define WEBSERV_CLIENT_HPP_

#include "Webserv.hpp"
#include "PortManager.hpp"
#include "Re3.hpp"
#include <vector>
#include <deque>
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

typedef std::deque<Re3>::iterator	Re3_iter;
class Client {
	private:
		int				status;
		const int		client_fd;
		PortManager&	port_manager;
		unsigned long	last_request_time;
		unsigned long	last_response_time;
		std::string				read_buff;
		std::deque<Re3>			re3_deque;

		int	reqLineParser(Request* request);
		int	chunkedParser(Request* request);
		int	lengthParser(Request* request);
		int	headerParser(Request* request);
		int	initParser(Request* request);
		// std::vector<Re3_iter>	rscToEnroll(void);

	public:
		Client(int client_fd, PortManager& port_manager);
		Client(const Client& ref);
		~Client();
		Client& operator=(const Client& ref);

		// std::vector<Re3_iter>	recvRequest(std::string& rawRequest);
		int	recvRequest(std::string& rawRequest);
		int	sendResponse(void);
};

#endif
