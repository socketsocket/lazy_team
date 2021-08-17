#include "Resource.hpp"

Resource::Resource(Status status)
	: status(status) {}

Resource::Resource(const Resource &ref)
	: status(ref.status), resource_fd(ref.resource_fd) {}

Resource::~Resource() {
	close(this->resource_fd);
}

Resource& Resource::operator=(const Resource &ref) {
	if (this == &ref)
		return *this;
	this->status = ref.status;
	this->resource_fd = ref.resource_fd;
	return *this;
}

Status	Resource::getStatus() const {
	return this->status;
}

int	Resource::getResourceFd() const {
	return this->resource_fd;
}

const std::string&	Resource::getContent() const {
	return this->content;
}

void	Resource::addContent(const std::string& str) {
	this->content += str;
}

std::string	Resource::getContent(size_t size) {
	std::string ret = this->content.substr(0, size);
	this->content.erase(0, size);
	return ret;
}

std::string Resource::getResourceUri() const {
	return this->uri;
}

void Resource::setStatus(Status status) {
	this->status = status;
}

void Resource::setResourceUri(std::string input) {
	this->uri = input;
}

void Resource::setResourceFd(int fd) {
	this->resource_fd = fd;
}
