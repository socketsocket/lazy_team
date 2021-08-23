#ifndef WEBSERV_RESOURCE_HPP_
#define WEBSERV_RESOURCE_HPP_

#include <string>
#include <iostream>
#include <unistd.h>
#include "Webserv.hpp"


// open한 uri의 fd와 그 상태를 백업하고, 읽으며 읽은 내용을 저장하기 위한 구조체
// 1. 요청이 GET인 경우 Server.ResponseMaker에서는 resource uri를 open
// 2. open한 fd를 kqueue(ServerManager)에 넘거주면 Server에서 이 fd의 타입이 resource인지 판별
// 3. Server는 파일을 read 하며 response->body에 수납
// 4. Resource struct는 open한 uri의 fd와 그 상태를 백업
struct Resource {
	private:
		Status			status;
		int				resource_fd; // status를 보고 소멸자에서 close;
		std::string		content;
		std::string		uri;
		stat_type		is_created; //POST 요청에서 쓰게 될 response Stat. 파일이 신규생성되었는지/기존에 존재했는지 백업
		Resource();

	public:
		Resource(Status status);
		Resource(const Resource &ref);
		~Resource();
		Resource& operator=(const Resource &ref);

		Status					getStatus() const;
		int						getResourceFd() const;
		int						getPortFd() const;
		std::string				getContent(size_t size);
		const std::string&		getContent() const;
		std::string				getResourceUri() const;
		void					setResourceUri(std::string uri);
		void					addContent(const std::string& str);
		void					setStatus(Status status);
		void					setResourceFd(int fd);
		void					setIsCreated(stat_type stat);
		stat_type				getIsCreated() const;
};

#endif
