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

		std::map<stat_type, std::string>	error_pages;
		Method								methods_allowed; // discerned by bits
		std::map<std::string, std::string>	cgi_infos;
		std::pair<stat_type, std::string>	return_to;
 
		Location();
		Location&							operator=(const Location& ref);

	public:
		Location(const Location& ref);
		Location(
			std::string							path,
			std::string							root,
			std::vector<std::string>			indexes,
			bool								auto_index,
			std::map<stat_type, std::string>	error_pages,
			Method								methods_allowed,
			std::map<std::string, std::string>	cgi_infos,
			std::pair<stat_type, std::string>	return_to
		);
		~Location();

		std::string							getPath();
		std::string							getRoot();
		Method								getMethodsAllowed();
		std::vector<std::string> 			getIndexes();
		bool								isAutoIndex();
		std::map<stat_type, std::string>	getDefaultErrorPages();
};

#endif
