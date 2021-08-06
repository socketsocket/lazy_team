#include "Resource.hpp"

Resource::Resource(int status, int resource_fd)
: status(status), resource_fd(resource_fd) {};

Resource::Resource(const Resource &ref)
: status(ref.status), resource_fd(ref.resource_fd) {};

Resource::~Resource() {};

Resource& Resource::operator=(const Resource &ref) {
	if (this == &ref)
		return *this;
	this->status = ref.status;
	this->resource_fd = ref.resource_fd;
	return *this;
}

const int& Resource::getStatus() const {
	return this->status;
}

const int& Resource::getResourceFd() const {
	return this->resource_fd;
}

const std::string&	Resource::getContent() const {
	return this->content;
}


void Resource::setStatus(int status) {
	this->status = status;
}
