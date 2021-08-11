#include "Resource.hpp"

Resource::Resource(Status status, int port_fd)
	: status(status), port_fd(port_fd) {}

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

int	Resource::getStatus() const {
	return this->status;
}

int	Resource::getResourceFd() const {
	return this->resource_fd;
}

int	Resource::getPortFd() const {
	return this->port_fd;
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

void Resource::setStatus(int status) {
	this->status = status;
}

void Resource::setResourceFd(int fd) {
	this->resource_fd = fd;
}
