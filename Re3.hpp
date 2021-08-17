#ifndef WEBSERV_RE3_HPP
#define WEBSERV_RE3_HPP

#include <deque>
#include "Request.hpp"
#include "Response.hpp"
#include "Resource.hpp"

class Re3
{
	private:
		Request*	req_ptr;
		Response*	rsp_ptr;
		Resource*	rsc_ptr;
		int			client_id;
		int			port_manager_id;

	public:
		Re3();
		Re3(int clinet_id);
		Re3(Request* req);
		~Re3();

		int	setReqPtr(Request* req);
		int	setRspPtr(Response* rsp);
		int	setRscPtr(Resource* rsc);

		void setClientId(int client_fd);
		void setPortId(int port_fd);

		Request*	getReqPtr();
		Response*	getRspPtr();
		Resource*	getRscPtr();

		int	getClientId();
		int	getPortId();
};

#endif
