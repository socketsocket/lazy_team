#ifndef WEBSERV_CGICONNECTOR_HPP
#define WEBSERV_CGICONNECTOR_HPP

#include <vector>
#include "Webserv.hpp"

class CgiConnector {
	private:
		std::vector<std::string>	env_var;

		const char**	makeEnvp() const;

	public:
		CgiConnector();
		CgiConnector(const CgiConnector& ref);
		~CgiConnector();
		CgiConnector&	operator=(const CgiConnector& ref);
};

#endif
