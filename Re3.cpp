#include "Re3.hpp"

Re3::Re3(): req_ptr(NULL), rsp_ptr(NULL), rsc_ptr(NULL) {}

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
}

int	Re3::setRspPtr(Response* rsp){
	if (this->rsp_ptr)
	// 에러메시지 출력
		return ERROR;
	this->rsp_ptr = rsp;
}

int	Re3::setRscPtr(Resource* rsc){
	if (this->rsc_ptr)
	// 에러메시지 출력
		return ERROR;
	this->rsc_ptr = rsc;
}

Request*	Re3::getReqPtr() {
	return req_ptr;
}

Response*	Re3::getRspPtr() {
	return rsp_ptr;
}

Resource*	Re3::getRscPtr() {
	return rsc_ptr;
}

int	Re3::getClientId() {
	return client_fd;
}

int	Re3::getPortId() {
	return port_manager_fd;
}