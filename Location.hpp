#ifndef WEBSERV_LOCATION_HPP_
#define WEBSERV_LOCATION_HPP_

#include <string>
#include <vector>
#include <map>
#include "Webserv.hpp"

class Location {
	std::string							path; // uri
	std::string							root;
	std::vector<std::string>			indexes;
	bool								auto_index;
	std::map<int, std::string>			error_pages;
	Method								methods_allowed; // discerned by bits
	std::map<std::string, std::string>	cgi_infos;

	public:
		Location();
		Location(Location& ref);
		Location&	operator=(Location& ref);
		~Location();
};

#endif
