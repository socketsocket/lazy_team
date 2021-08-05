#ifndef WEBSERV_RESPONSE_HPP
#define WEBSERV_RESPONSE_HPP

#include "Webserv.hpp"
#include <string>
#include <iostream>
#include <map>

class Response {
	private:
		//response를 쓰고있는 상태
		Method								status;
		//예시:200 OK
		std::string							http_status_code;
		std::string							version;
		std::map<std::string, std::string>	headers;
		std::string							body;
		size_t								size;

	public:
		Response();
		Response(int status, std::string http_status_code, std::map<std::string, std::string> headers, std::string body, std::string version);
		Response(const Response& ref);
		~Response();
		Response& operator=(const Response& ref);

		Method		getStatus();
		std::string getResponseMessage();
		size_t		getSize() const;
		void		deductSize(size_t send);
		void	appendBody(std::string buffer);
		void	addHeader(std::string key, std::string value);
};

#endif
