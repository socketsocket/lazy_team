#ifndef WEBSERV_CONFIGPARSER_HPP
#define WEBSERV_CONFIGPARSER_HPP

#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include "Webserv.hpp"
#include "Server.hpp"
#include "Location.hpp"

#define WHITE_SPACES " \v\n\r\t"

class ConfigParser {
	private:
		static const std::string			serverConfigArr[6];
		static const std::string			locationConfigArr[6];
		static const std::set<std::string>	serverConfig;
		static const std::set<std::string>	locationConfig;
		static std::ifstream			config_file;

		ConfigParser();
		ConfigParser(const ConfigParser& ref);
		ConfigParser&	operator=(const ConfigParser& ref);
		~ConfigParser();

		static int	getSemanticLine(std::string& line);
		static int	getIntoBlock(std::string block_name, std::string line = ""));
		static int	getPath(std::string& line);

		static int	httpBlock(std::vector<Server>& servers);
		static int	serverBlock(Server& server);
		static int	locationBlock(Location& location);

	public:
		static int	readFile(std::vector<Server>& servers, const char* config_path);
};

void	trimWhitespace(std::string& str);

#endif