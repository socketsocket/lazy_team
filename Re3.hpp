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
		int			port_manager_fd;

	public:
		Re3();
		Re3(Request* req);
		~Re3();

		int	setReqPtr(Request* req);
		int	setRspPtr(Response* rsp);
		int	setRscPtr(Resource* rsc);

		Request*	getReqPtr();
		Response*	getRspPtr();
		Resource*	getRscPtr();

		int	getClientId();
		int	getPortId();
};

typedef std::deque<Re3>::iterator   Re3_iter;

#endif
