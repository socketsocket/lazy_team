#include "Request.hpp"

Request::Request() {
	this->status = kNothing;
	this->method = 0;
	this->uri = "";
	this->version = "";
	this->body = "";
}

Request::~Request() {
}

Request& Request::operator=(const Request &ref) {
	return *this;
}

int	Request::setStatus(int status) {
	this->status = status;
	return OK;
}

int	Request::setMethod(int method) {
	this->method = method;
	return OK;
}

int	Request::setUri(std::string& uri) {
	this->uri = uri;
	return OK;
}

int	Request::setVersion(std::string& version) {
	this->version = version;
	return OK;
}

int	Request::insertHeader(std::string& key, std::string& value) {
	this->headers.insert(std::make_pair(key, value));
	return OK;
}

int	Request::appendBody(std::string& buffer) {
	this->body += buffer;
	return OK;
}

const int&									Request::getStatus() {
	return this->status;
}

const Method&								Request::getMethod() {
	return this->method;
}

const std::string&							Request::getUri() {
	return this->uri;
}

const std::string&							Request::getVersion() {
	return this->version;
}

std::map<std::string, std::string>&			Request::getHeaders() {
	return this->headers;
}

const std::string&							Request::getBody() {
	return this->body;
}
