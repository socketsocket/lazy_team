#ifndef WEBSERV_CONFIGPARSER_HPP
#define WEBSERV_CONFIGPARSER_HPP

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

// This class can be used twice.
class ConfigParser {
	private:
		ConfigParser();
		ConfigParser(const ConfigParser& ref);
		ConfigParser&	operator=(const ConfigParser& ref);

		static bool				is_used; // check for duplicated use.
		static std::string		server_config_arr[6];
		static std::string		location_config_arr[7];
		std::set<std::string>	server_config;
		std::set<std::string>	location_config;
		std::ifstream			config_file;

		// variables for error_message;
		std::string				class_name;
		std::string				method_name;

		int		getSemanticLine(std::string& line);
		int		getIntoBlock(std::string block_name, std::string line = "");
		int		getPath(std::string& path, std::vector<std::string>& elements);
		int		getLineElements(std::vector<std::string>& elements);

		int		httpBlock(ServerManager& server_manager);
		int		serverBlock(std::vector<Server>& server);
		int		locationBlock(std::vector<Location>& location, \
			std::vector<std::string>& elements);

		int		putError(const char* err_msg, std::string opt);

	public:
		ConfigParser(const char* config_path);
		~ConfigParser();
		int	setServerManager(ServerManager& server_manager);
};

std::string	trimWhitespace(std::string str);

#endif