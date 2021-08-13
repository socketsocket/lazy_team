#ifndef WEBSERV_LOCATION_HPP_
#define WEBSERV_LOCATION_HPP_

#include <string>
#include <vector>
#include <map>
#include "Webserv.hpp"

class Location {
	private:
		const std::string							path; // uri
		const std::string							root;
		const std::vector<std::string>				indexes;
		const bool									auto_index;
		const std::map<stat_type, std::string>		error_pages;
		const Method								methods_allowed; // discerned by bits
		const std::map<std::string, std::string>	cgi_infos;
		const std::pair<stat_type, std::string>		return_to;

		Location(); // unuse
		Location&	operator=(const Location& ref); // unuse

	public:
		Location(const Location& ref);
		Location(
			std::string							path, \
			std::string							root, \
			std::vector<std::string>			indexes, \
			bool								auto_index, \
			std::map<stat_type, std::string>	error_pages, \
			Method								methods_allowed, \
			std::map<std::string, std::string>	cgi_infos, \
			std::pair<stat_type, std::string>	return_to
		);
		~Location();

		const std::string&						getPath() const;
		const std::string&						getRoot() const;
		Method									getMethodsAllowed() const;
		const std::vector<std::string>&			getIndexes() const;
		const bool&								isAutoIndex() const;
		const std::map<stat_type, std::string>&	getDefaultErrorPages() const;
		const std::string						getDefaultErrorPage(stat_type stat) const;
};

#endif
