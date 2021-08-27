#ifndef WEBSERV_CGICONNECTOR_HPP_
#define WEBSERV_CGICONNECTOR_HPP_

#include <vector>
#include <string>
#include "Webserv.hpp"
#include "Re3.hpp"
#include "Location.hpp"
#include <cstdlib>
#include <arpa/inet.h>

class CgiConnector {
	private:
		ServerStatus	prepareResource(Re3* re3, const Location* loc, unsigned int port_num);
		ServerStatus	prepareResponse(Re3* re3);
		ServerStatus	waitCgi(Resource* rsc);

	public:
		ServerStatus	makeCgiResponse(Re3* re3, const Location* loc, unsigned int port_num);
		CgiConnector();
		CgiConnector(const CgiConnector& ref);
		~CgiConnector();
		CgiConnector&	operator=(const CgiConnector& ref);
};

#endif
