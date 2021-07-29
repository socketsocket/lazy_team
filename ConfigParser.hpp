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

// static class. all member functions and variables are static.
class ConfigParser {
	private:
		// no instance will be created.
		ConfigParser();
		ConfigParser(const ConfigParser& ref);
		ConfigParser&	operator=(const ConfigParser& ref);
		~ConfigParser();

		static const std::string			server_config_arr[6];
		static const std::string			location_config_arr[7];
		static const std::set<std::string>	server_config;
		static const std::set<std::string>	location_config;
		static std::ifstream				config_file;


		static void	initStatusCodeMap();

		static int	getSemanticLine(std::string& line);
		static int	getIntoBlock(std::string block_name, std::string line = "");
		static int	getPath(std::string& path, \
			std::vector<std::string>& elements);
		static int	getLineElements(std::vector<std::string>& elements);

		static int	httpBlock(std::vector<Server>& servers);
		static int	serverBlock(std::vector<Server>& server);
		static int	locationBlock(std::vector<Location>& location, \
			std::vector<std::string>& elements);

	public:
		static int	readConfigFile(ServerManager& server_manager, \
			const char* config_path);
};

std::string	trimWhitespace(std::string str);

#endif