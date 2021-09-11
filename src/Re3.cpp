#include "Re3.hpp"

Re3::Re3(): req_ptr(NULL), rsp_ptr(NULL), rsc_ptr(NULL) {}

Re3::Re3(int client_fd)
	: req_ptr(NULL),
	  rsp_ptr(NULL),
	  rsc_ptr(NULL),
	  client_id(client_fd),
	  port_manager_id(STDOUT) {}

Re3::Re3(Request* request): req_ptr(request), rsp_ptr(NULL), rsc_ptr(NULL) {}

Re3::~Re3() {
	if (this->req_ptr)
		delete this->req_ptr;
	if (this->rsp_ptr)
		delete this->rsp_ptr;
	if (this->rsc_ptr)
		delete this->rsc_ptr;
}

int	Re3::setReqPtr(Request* req) {
	if (this->req_ptr)
	// 에러메시지 출력
		return ERROR;
	this->req_ptr = req;
	return OK;
}

int	Re3::setRspPtr(Response* rsp){
	if (this->rsp_ptr)
	// 에러메시지 출력
		return ERROR;
	this->rsp_ptr = rsp;
	return OK;
}

int	Re3::setRscPtr(Resource* rsc){
	if (this->rsc_ptr)
	// 에러메시지 출력
		return ERROR;
	this->rsc_ptr = rsc;
	return OK;
}

void	Re3::setClientId(int client_fd) {
	this->client_id = client_fd;
}

void	Re3::setPortId(int port_fd) {
	this->port_manager_id = port_fd;
}

Request*	Re3::getReqPtr() {
	return this->req_ptr;
}

Response*	Re3::getRspPtr() {
	return this->rsp_ptr;
}

Resource*	Re3::getRscPtr() {
	return this->rsc_ptr;
}

int	Re3::getClientId() {
	return this->client_id;
}

int	Re3::getPortId() {
	return this->port_manager_id;
}
