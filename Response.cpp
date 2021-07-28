#include "Response.hpp"

Response::Response(){
}

Response::Response(int status, std::string http_status_code, 
std::map<std::string, std::string> header, std::string body, std::string version)
:status(status), http_status_code(http_status_code), header(header), body(body), version(version){
}

Response::Response(const Response& ref){
	if (this == &ref)
		return ;
	this->status = ref.status;
	this->http_status_code = ref.http_status_code;
	this->header = ref.header;
	this->body = ref.body;
	this->version = ref.version;
}

Response::~Response(){
}

Response& Response::operator=(const Response& ref){
	if (this == &ref)
		return ;
	this->status = ref.status;
	this->http_status_code = ref.http_status_code;
	this->header = ref.header;
	this->body = ref.body;
	this->version = ref.version;
}

std::string Response::GetResponseMessage(){
	std::string res;

	res = this->version + " " + this->http_status_code + "\r\n";
	for (std::map<std::string, std::string>::iterator iter = this->header.begin();
	iter != this->header.end(); iter++)
		res += iter->first + ": " + iter->second + "\r\n";
	res += "\r\n" + this->body;
	return (res);
}

//resource를 읽을때마다 body에 이어붙이기위함
void Response::AppendBody(std::string buffer){
	this->body += buffer;
}

void Response::AddHeader(std::string key, std::string value)
{
	this->header[key] = value;
}
