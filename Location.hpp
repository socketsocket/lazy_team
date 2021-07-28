#ifndef WEBSERV_LOCATION_HPP_
#define WEBSERV_LOCATION_HPP_

#include <string>
#include <vector>
#include <map>
#include "Webserv.hpp"

class Location {
	private:
		std::string							path; // uri
		std::string							root;
		std::vector<std::string>			indexes;
		bool								auto_index;
		std::map<std::string, std::string>			error_pages;
		Method								methods_allowed; // discerned by bits
		std::map<std::string, std::string>	cgi_infos;
		std::pair<const char*, std::string>	return_to;

		Location();
		Location(const Location& ref);
		Location&	operator=(const Location& ref);

	public:
		Location(
			std::string path,
			std::string root,
			std::vector<std::string> indexes,
			bool auto_index,
			std::map<std::string, std::string> error_pages,
			Method methods_allowed,
			std::map<std::string, std::string> cgi_infos,
			std::pair<const char*, std::string> return_to
		);
		~Location();
};

#endif
