#include "Request.hpp"

Request::Request() {
	this->status = Nothing;
	this->method = 0;
	this->uri = "";
	this->version = "";
	this->body = "";
}

Request::Request(const Request &ref) {
	*this = ref;
}

Request::~Request() {
}

Request& Request::operator=(const Request &ref) {
	return (*this);
}

int	Request::insertHeader(std::string& key, std::string& value) {
	this->headers.insert(std::make_pair(key, value));
}

Location	Request::getLocation() const
{
	return location;
}
