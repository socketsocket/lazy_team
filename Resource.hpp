#ifndef WEBSERV_RESOURCE_HPP_
#define WEBSERV_RESOURCE_HPP_

#include "Webserv.hpp"
#include "Client.hpp"
#include <string>
#include <iostream>

/*
* open한 uri의 fd와 그 상태를 백업하고, client 주소를 저장해두기 위한 구조체
* 1. 요청이 GET인 경우 Server.ResponseMaker에서는 resource uri를 open
* 2. open한 fd를 kqueue(ServerManager)에 넘거주면 Server에서 이 fd의 타입이 resource인지 판별
* 3. Server는 파일을 read 하며 response->body에 수납
* 4. Resource struct는 open한 uri의 fd와 그 상태를 백업하고, client 주소를 저장
*/
struct Resource {
	private:
		int				status;
		Client&			client;
		int				resource_fd;
		Resource();

	public:
		Resource(int status, Client& client, int resource_fd);
		Resource(const Resource &ref);
		~Resource();
		Resource& operator=(const Resource &ref);

		int getStatus();
		int getResourceFD();
		Client &getClientAddress();

		void setStatus(int status);
};

#endif
