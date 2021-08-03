#ifndef WEBSERV_RESOURCE_HPP_
#define WEBSERV_RESOURCE_HPP_

#include "Webserv.hpp"
#include "Client.hpp"
#include <string>
#include <iostream>


// open한 uri의 fd와 그 상태를 백업하고, 읽으며 읽은 내용을 저장하기 위한 구조체
// 1. 요청이 GET인 경우 Server.ResponseMaker에서는 resource uri를 open
// 2. open한 fd를 kqueue(ServerManager)에 넘거주면 Server에서 이 fd의 타입이 resource인지 판별
// 3. Server는 파일을 read 하며 response->body에 수납
// 4. Resource struct는 open한 uri의 fd와 그 상태를 백업
struct Resource {
	private:
		int				status;
		int				resource_fd;
		std::string		content;
		Resource();

	public:
		Resource(int status, int resource_fd);
		Resource(const Resource &ref);
		~Resource();
		Resource& operator=(const Resource &ref);

		const int&				getStatus() const;
		const int&				getResourceFd() const;
		const std::string&		getContent() const;
		void setStatus(int status);
};

#endif
