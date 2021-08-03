#include "Resource.hpp"

Resource::Resource(int status, int resource_fd)
: status(status), resource_fd(resource_fd) {};

Resource::Resource(const Resource &ref)
: status(ref.status), resource_fd(ref.resource_fd) {};

Resource::~Resource() {};

Resource& Resource::operator=(const Resource &ref) {
	if (this == &ref)
		return ;
	this->status = ref.status;
	this->resource_fd = ref.resource_fd;
}

int Resource::getStatus() {
	return this->status;
}

int Resource::getResourceFD() {
	return this->resource_fd;
}

void Resource::setStatus(int status) {
	this->status = status;
}
