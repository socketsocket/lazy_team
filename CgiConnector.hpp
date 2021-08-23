#ifndef WEBSERV_CGICONNECTOR_HPP_
#define WEBSERV_CGICONNECTOR_HPP_

#include <vector>
#include <string>
#include "Webserv.hpp"
#include "Re3.hpp"
#include "Location.hpp"
#include <cstdlib>

class CgiConnector {
	private:
		int									pipe_fd[2];
		std::map<std::string, std::string>	env_map;

		ServerStatus	prepareResource(Request* req, Resource* rsc, const Location* loc);
		ServerStatus	prepareResponse(Re3* re3);
		ServerStatus	CgiConnector::waitCgi(Resource* rsc);


	public:
		ServerStatus	makeCgiResponse(Re3* re3, const Location* loc);

		CgiConnector();
		CgiConnector(const CgiConnector& ref);
		~CgiConnector();
		CgiConnector&	operator=(const CgiConnector& ref);
};

#endif
