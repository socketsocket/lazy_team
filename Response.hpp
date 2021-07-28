#ifndef WEBSERV_RESPONSE_HPP
#define WEBSERV_RESPONSE_HPP

#include "Webserv.hpp"
#include <string>
#include <iostream>
#include <map>

class Response {
	private:
		//response를 쓰고있는 상태
		int		status;
		//예시:200 OK
		std::string	http_status_code;
		std::map<std::string, std::string> header;
		std::string	body;
		std::string version;

	public:
		Response();
		Response(int status, std::string http_status_code, std::map<std::string, std::string> header, std::string body, std::string version);
		Response(const Response& ref);
		~Response();
		Response& operator=(const Response& ref);

		std::string GetResponseMessage();
		void AppendBody(std::string buffer);
		void AddHeader(std::string key, std::string value);
};

#endif
