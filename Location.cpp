#include "Location.hpp"

Location::Location(
	std::string path,
	std::string root,
	std::vector<std::string> indexes,
	bool auto_index,
	std::map<std::string, std::string> error_pages,
	Method methods_allowed,
	std::map<std::string, std::string> cgi_infos,
	std::pair<const char*, std::string> return_to)
	: path(path),
	  root(root),
	  indexes(indexes),
	  auto_index(auto_index),
	  error_pages(error_pages),
	  methods_allowed(methods_allowed),
	  cgi_infos(cgi_infos),
	  return_to(return_to) {}

Location::~Location() {}