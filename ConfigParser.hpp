#ifndef WEBSERV_CONFIGPARSER_HPP
#define WEBSERV_CONFIGPARSER_HPP


#include <sstream>
#include <fstream>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "Webserv.hpp"
#include "ServerManager.hpp"
#include "Server.hpp"
#include "Location.hpp"

#define WHITE_SPACES " \v\n\r\t"

enum Directive {
	kRoot,
	kIndex,
	kMethodAllowed,
	kCgiInfo,
	kListen,
	kServerName,
	kAutoIndex,
	kErrorPage,
	kClientBodyLimit,
	kReturn
};

// This class can be used twice.
class ConfigParser {
	private:
		ConfigParser();
		ConfigParser(const ConfigParser& ref);
		ConfigParser&	operator=(const ConfigParser& ref);

		static bool				is_used; // check for duplicated use.
		static std::pair<std::string, Directive>	server_config_arr[6];
		static std::pair<std::string, Directive>	location_config_arr[7];
		std::map<std::string, Directive>	server_config;
		std::map<std::string, Directive>	location_config;
		std::ifstream				config_file;

		// variables for error_message;
		size_t						line_num;
		std::string					class_name;
		std::vector<std::string>	method_name;

		int		getSemanticLine(std::string& line);
		int		getIntoBlock(std::string block_name, \
			std::vector<std::string> elements = std::vector<std::string>(0));
		int		getPath(std::string& path, std::vector<std::string>& line_elements);
		int		getLineElements(std::vector<std::string>& elements);

		int		httpBlock(ServerManager& server_manager);
		int		serverBlock(std::vector<std::pair<Server, std::vector<unsigned int> > >& configs, \
			std::vector<std::string>& line_elements);
		int		locationBlock(std::vector<Location>& location, \
			std::vector<std::string>& line_elements);

		int		putError(const char* err_msg, std::string opt = "");

	public:
		ConfigParser(std::string config_path);
		~ConfigParser();
		int	setData(ServerManager& server_manager);

};

std::string	trimWhitespace(std::string str);

#endif