#include "Request.hpp"

Request::Request()
{
	this->status = nothing;
	this->method = 0;
	this->uri = nullptr;
	this->version = nullptr;
	this->header = nullptr;
	this->body = nullptr;
}

Request::Request(const Request &ref)
{
	*this = ref;
}

Request::~Request()
{
}

Request& Request::operator=(const Request &ref)
{
	return (*this);
}
