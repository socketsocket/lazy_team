#include "ConfigParser.hpp"

const std::string	ConfigParser::serverConfigArr[6] = {
	"listen",
	"server_name",
	"root",
	"error_page",
	"client_body_limit",
	"return"
};

const std::string	ConfigParser::locationConfigArr[6] = {
	"root",
	"error_page"
	"index",
	"auto_index",
	"method_allowed",
	"cgi_info"
};

const std::set<std::string>	ConfigParser::serverConfig(
	ConfigParser::serverConfigArr,
	ConfigParser::serverConfigArr
		+ sizeof(ConfigParser::serverConfigArr) / sizeof(std::string)
);

const std::set<std::string>	ConfigParser::locationConfig(
	ConfigParser::locationConfigArr,
	ConfigParser::locationConfigArr
		+ sizeof(ConfigParser::locationConfigArr) / sizeof(std::string)
);


int	ConfigParser::getSemanticLine(std::string& line) {
	char	buffer[LINE_BUFF_SIZE];

	while (line.length() == 0) {
		ConfigParser::config_file.getline(buffer, LINE_BUFF_SIZE);
		if (ConfigParser::config_file.bad()) {
			std::cerr << std::string("") + "ConfigParser: " + "getSemanticLine: " + READ_LINE_ERR;
			return ERROR;
		}
		line = buffer;
		while (ConfigParser::config_file.fail()) {
			ConfigParser::config_file.getline(buffer, LINE_BUFF_SIZE);
			if (config_file.bad()) {
				std::cerr << std::string("") + "ConfigParser: " + "getSemanticLine: " + READ_LINE_ERR;
				return ERROR;
			}
			line += buffer;
		}

		// Delete comment.
		size_t sharp_pos = line.find_first_of('#');
		if (sharp_pos != std::string::npos) {
			line.resize(sharp_pos - 1);
		}

		// Trim whitespaces.
		trimWhitespace(line);
	}
	return OK;
}

// Identify whether the line is block start or not.
int ConfigParser::getIntoBlock(std::string block_name, std::string line = "") {
	if (line.compare("}"))
		return BLOCK_END;

	if (line.length() == 0)
		if (ConfigParser::getSemanticLine(line));
			return ERROR;

	// Compare block name and leading part of the line
	std::string candidate = line.substr(0, block_name.length());
	if (block_name.compare(candidate)) {
		std::cerr << std::string("") + "ConfigParser: " + "getIntoBlock: " + NAME_MATCH_ERR;
		return ERROR;
	}

	// Check if there is only a opening bracket.
	line.erase(0, block_name.length());
	trimWhitespace(line);
	if (line.length() == 0)
		if (ConfigParser::getSemanticLine(line));
			return ERROR;

	if (line.compare("{")) {
		std::cerr << std::string("") + "ConfigParser: " + "getIntoBlock: " + SEMANTIC_ERR;
		return ERROR;
	}
	return OK;
}

int	ConfigParser::getPath(std::string& line) {
	std::string block_name = "location";

	if (line.length() == 0)
		ConfigParser::getSemanticLine(line);

	// Compare block name and leading part of the line
	std::string candidate = line.substr(0, block_name.length());
	if (block_name.compare(candidate)) {
		std::cerr << std::string("") + "ConfigParser: " + "getPath: " + NAME_MATCH_ERR;
		return ERROR;
	}

	// Delete opening bracket and white spaces
	line.erase(0, block_name.length());
	trimWhitespace(line);
	if (*(line.end() - 1) != '{') {
		std::string temp;
		if (getSemanticLine(temp))
			return ERROR;
		if (temp.compare("{")) {
			std::cerr << std::string("") + "ConfigParser: " + "getPath: " + NAME_MATCH_ERR;
			return ERROR;
		}
	} else {

	}
	trimWhitespace(line);
	return OK;
}

// int	ConfigParser::getOutOfLine(std::string line = "") {
// 	if (line.length() == 0)
// 		ConfigParser::getSemanticLine(line);
// 	if (line.compare("}")) {
// 		std::cerr << std::string("") + "ConfigParser: " + "getOutOfBlock: " + SEMANTIC_ERR;
// 		return ERROR;
// 	}
// 	return OK;
// }


int	ConfigParser::httpBlock(std::vector<Server>& servers) {
	if (getIntoBlock("http"));
		return ERROR;

	Server		server;
	std::string	trails = "";
	int			ret = 0;

	while (!ret) {
		ret = serverBlock(server);
		if (ret == ERROR)
			return ERROR;

	}
	return OK;
}

int	ConfigParser::serverBlock(Server& server) {

}

int	ConfigParser::locationBlock(Location& location, trails, ) {
	getPath()
}

int ConfigParser::readFile(std::vector<Server>& servers, const char* config_path) {
	ConfigParser::config_file.open(config_path);
	if (!ConfigParser::config_file.is_open()) {
		std::cerr << std::string("") + "ConfigParser: " + "ReadFile: " + OPEN_FILE_ERR;
		return ERROR;
	}
	ConfigParser::httpBlock(servers);
	ConfigParser::config_file.close();
	return OK;
}

void	trimWhitespace(std::string& str) {
	str.erase(str.begin(), str.begin() + str.find_first_not_of(WHITE_SPACES));
	str.erase(str.find_last_not_of(WHITE_SPACES) + 1);
}