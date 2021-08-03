#include "Location.hpp"

Location::Location(const Location& ref)
	: path(ref.path),
	  root(ref.root),
	  indexes(ref.indexes),
	  auto_index(ref.auto_index),
	  error_pages(ref.error_pages),
	  methods_allowed(ref.methods_allowed),
	  cgi_infos(ref.cgi_infos),
	  return_to(ref.return_to) {}


Location::Location(
	std::string path,
	std::string root,
	std::vector<std::string> indexes,
	bool auto_index,
	std::map<stat_type, std::string> error_pages,
	Method methods_allowed,
	std::map<std::string, std::string> cgi_infos,
	std::pair<stat_type, std::string> return_to)
	: path(path),
	  root(root),
	  indexes(indexes),
	  auto_index(auto_index),
	  error_pages(error_pages),
	  methods_allowed(methods_allowed),
	  cgi_infos(cgi_infos),
	  return_to(return_to) {}

Location::~Location() {}

std::string Location::getPath() {
	return this->path;
}

std::string Location::getRoot() {
	return this->root;
}

Method Location::getMethodsAllowed() {
	return this->methods_allowed;
}

std::vector<std::string> Location::getIndexes() {
	return this->indexes;
}

bool Location::isAutoIndex() {
	return this->auto_index;
}

std::map<stat_type, std::string> Location::getDefaultErrorPages() {
	return this->error_pages;
}
