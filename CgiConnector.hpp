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
		std::map<std::string, std::string>	env_map;
		const char**	makeEnvp() const;

	public:
		ServerStatus	makeCgiResponse(Re3* re3, Location* loc/*, std::string target*/);

		CgiConnector();
		CgiConnector(const CgiConnector& ref);
		~CgiConnector();
		CgiConnector&	operator=(const CgiConnector& ref);
};

#endif
