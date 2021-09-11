#ifndef WEBSERV_CLIENT_HPP_
#define WEBSERV_CLIENT_HPP_

#include "Webserv.hpp"
#include "TermPrinter.hpp"
#include "PortManager.hpp"
#include "Re3.hpp"
#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>

#define READSIZE 1024

class Client {
	private:
		Status				status;
		const int			client_fd;
		const PortManager&	port_manager;
		unsigned long		last_request_time;
		unsigned long		last_response_time;
		std::string			read_buff;
		std::deque<Re3>		re3_deque;

		int	reqLineParser(Request* request);
		int	chunkedParser(Request* request);
		int	lengthParser(Request* request);
		int	headerParser(Request* request);
		int	initParser(Request* request);
		void	putRspIntoBuff(size_t& network_buff_left, std::string& to_be_sent, std::string& data);

	public:
		Client(int client_fd, PortManager& port_manager);
		Client(const Client& ref);
		~Client();
		Client& operator=(const Client& ref);

		int	getClientFd() const;
		std::vector<std::pair<Re3*, ServerStatus> >	recvRequest(std::string rawRequest);
		std::string	passResponse();
};

#endif
