#ifndef WEBSERV_CGICONNECTOR_HPP_
#define WEBSERV_CGICONNECTOR_HPP_

#include <vector>
#include <string>
#include "Webserv.hpp"
#include "Re3.hpp"

class CgiConnector {
	private:
		std::map<std::string, std::string>	env_map;
		const char**	makeEnvp() const;

	public:
		void	makeEnvMap(Request* req);

		CgiConnector();
		CgiConnector(const CgiConnector& ref);
		~CgiConnector();
		CgiConnector&	operator=(const CgiConnector& ref);
};

#endif
