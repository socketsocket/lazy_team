#include "Request.hpp"

Request::Request() {
	this->status = nothing;
	this->method = 0;
	this->uri = NULL;
	this->version = NULL;
	this->header = NULL;
	this->body = NULL;
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
	std::map<std::map
	this->header.insert(std::pair<key, value>);
}
Location	Request::getLocation() const
{
	return location;
}
void	Request::setLocation(const Location &location)
{
	this->location = location;
}
