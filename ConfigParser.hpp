#ifndef WEBSERV_CONFIGPARSER_HPP
#define WEBSERV_CONFIGPARSER_HPP

#include <fstream>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "Webserv.hpp"
#include "Server.hpp"
#include "Location.hpp"

#define WHITE_SPACES " \v\n\r\t"

class ConfigParser {
	private:
		static const std::string			server_config_arr[6];
		static const std::string			location_config_arr[7];
		static const std::set<std::string>	server_config;
		static const std::set<std::string>	location_config;
		static const char*					status_code_set[];
		static std::map<std::string, const char*>	status_code_map;
		static std::ifstream				config_file;

		ConfigParser();
		ConfigParser(const ConfigParser& ref);
		ConfigParser&	operator=(const ConfigParser& ref);
		~ConfigParser();

		static void	initStatusCodeMap();

		static int	getSemanticLine(std::string& line);
		static int	getIntoBlock(std::string block_name, std::string line = "");
		static int	getPath(std::string& line);
		static int	getLineElements(std::vector<std::string> elements);

		static int	httpBlock(std::vector<Server>& servers);
		static int	serverBlock(std::vector<Server>& server);
		static int	locationBlock(std::vector<Location>& location, std::string& line);

	public:
		static int	readFile(std::vector<Server>& servers, const char* config_path);
};

std::string	trimWhitespace(std::string str);

#endif