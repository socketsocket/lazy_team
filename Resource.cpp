#include "Resource.hpp"

Resource::Resource(int status, Client& client, int resource_fd)
:status(status), client(client), resource_fd(resource_fd){
};

Resource::Resource(const Resource &ref)
:status(ref.status), client(ref.client), resource_fd(ref.resource_fd){
};

Resource::~Resource(){  
};

Resource& Resource::operator=(const Resource &ref){
	if (this == &ref)
		return ;
	this->status = ref.status;
	this->client = ref.client;
	this->resource_fd = ref.resource_fd;
}

int Resource::getStatus(){
	return this->status;
}

Client &Resource::getClientAddress(){
	return this->client;
}

int Resource::getResourceFD(){
	return this->resource_fd;
}

void Resource::setStatus(int status){
	this->status = status;
}