#include "ConfigParser.hpp"

const std::string	ConfigParser::server_config_arr[6] = {
	"listen",
	"server_name",
	"root",
	"error_page",
	"client_body_limit",
	"return"
};

const std::string	ConfigParser::location_config_arr[7] = {
	"root",
	"error_page"
	"index",
	"auto_index",
	"method_allowed",
	"cgi_info",
	"return"
};

const std::set<std::string>	ConfigParser::server_config(
	ConfigParser::server_config_arr,
	ConfigParser::server_config_arr
		+ sizeof(ConfigParser::server_config_arr) / sizeof(std::string)
);

const std::set<std::string>	ConfigParser::location_config(
	ConfigParser::location_config_arr,
	ConfigParser::location_config_arr
		+ sizeof(ConfigParser::location_config_arr) / sizeof(std::string)
);

stat_type	ConfigParser::status_code_arr[] = {C100, C101, C200, C201, C202,
	C203, C204, C205, C206, C300, C301, C302, C303, C304, C305, C307, C400,
	C401, C402, C403, C404, C405, C406, C407, C408, C409, C410, C411, C412,
	C413, C414, C415, C416, C417, C500, C501, C502, C503, C504, C505};

std::map<std::string, stat_type>	ConfigParser::status_code_map;

void	ConfigParser::initStatusCodeMap() {

	for (size_t i = 0; i < sizeof(status_code_arr); ++i)
		ConfigParser::status_code_map[std::string(status_code_arr[i]).substr(0, 3)]
			= status_code_arr[i];
}

int	ConfigParser::getSemanticLine(std::string& line) {
	char	buffer[LINE_BUFF_SIZE];

	while (line.length() == 0) {
		ConfigParser::config_file.getline(buffer, LINE_BUFF_SIZE);
		if (ConfigParser::config_file.bad()) {
			std::cerr << "ConfigParser: " << "getSemanticLine: " << READ_LINE_ERR << std::endl;;
			return ERROR;
		}
		line = buffer;
		while (ConfigParser::config_file.fail()) {
			ConfigParser::config_file.getline(buffer, LINE_BUFF_SIZE);
			if (config_file.bad()) {
				std::cerr << "ConfigParser: " << "getSemanticLine: " << READ_LINE_ERR << std::endl;
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
		line = trimWhitespace(line);
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
		std::cerr << "ConfigParser: " << "getIntoBlock: " << NAME_MATCH_ERR << std::endl;
		return ERROR;
	}

	// Check if there is only a opening bracket.
	line.erase(0, block_name.length());
	line = trimWhitespace(line);
	if (line.length() == 0)
		if (ConfigParser::getSemanticLine(line));
			return ERROR;

	if (line.compare("{")) {
		std::cerr << "ConfigParser: " << "getIntoBlock: " << SEMANTIC_ERR << std::endl;;
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
		std::cerr << "ConfigParser: " << "getPath: " << NAME_MATCH_ERR << std::endl;
		return ERROR;
	}

	// Delete opening bracket and white spaces
	line.erase(0, block_name.length());
	line = trimWhitespace(line);
	if (*(line.end() - 1) != '{') {
		std::string temp;
		if (getSemanticLine(temp))
			return ERROR;
		if (temp.compare("{")) {
			std::cerr << "ConfigParser: " << "getPath: " << NAME_MATCH_ERR << std::endl;
			return ERROR;
		}
	} else {
		line.erase(line.begin(), line.end() - 1);
	}
	line = trimWhitespace(line);
	if (line.length() == 0) {
		std::cerr << "ConfigParser: " << "getPath: " << SEMANTIC_ERR << std::endl;
		return ERROR;
	}
	return OK;
}

int	ConfigParser::getLineElements(std::vector<std::string> elements) {
	std::string	line;
	if (ConfigParser::getSemanticLine(line))
		return ERROR;

	size_t	pos = 0;
	while ((pos = line.find_first_of(WHITE_SPACES)) != std::string::npos) {
		elements.push_back(trimWhitespace(line.substr(0, pos)));
		line.erase(0, pos + 1);
	}
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

	std::string	trails = "";
	int			ret = 0;

	while (!ret) {
		ret = ConfigParser::serverBlock(servers);
		if (ret == ERROR)
			return ERROR;
		// what does it do when there is no server block?
	}
	return OK;
}

int	ConfigParser::serverBlock(std::vector<Server>& servers) {

}

int	ConfigParser::locationBlock(std::vector<Location>& locations, std::string& line) {
	if (ConfigParser::getPath(line))
		return ERROR;

	// Arguments for Location constructor.
	std::string							path = line;
	std::string							root = ""; // when there is no root
	std::vector<std::string>			indexes(0); // when there is no index pages
	bool								auto_index = false; // the default value is false.
	bool								auto_index_check = false; // to check if there is more than one auto_index.
	std::map<stat_type, std::string>	error_pages;
	Method								methods_allowed = GET; // default method is get
	bool								method_allowed_check = false; // to check duplicated directives
	std::map<std::string, std::string>	cgi_infos;
	std::pair<stat_type, std::string>	return_to;
	bool								return_check = false; // to check duplicated return

	// Elements included in one line.
	std::vector<std::string>	elements;
	while (elements.size() > 1) {
		if (ConfigParser::getLineElements(elements))
			return ERROR;
		if ((ConfigParser::location_config.find(elements[0]) == ConfigParser::location_config.end())) {
			std::cerr << "ConfigParser: " << "locationBlock: " << NAME_MATCH_ERR << std::endl;
			return ERROR;
		}

		if (!elements[0].compare("root")) {
			if (elements.size() != 2) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "root: "<< SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			if (root.length()) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "root: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			root = elements[1];

		} else if (!elements[0].compare("error_page")) {
			if (!(elements.size() % 2)) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "error_page:" << SEMANTIC_ERR << std::endl;
			}
			// error_page 1 2 3 4 a b c d :config
			// 0          1 2 3 4 5 6 7 8 :index
			for (size_t i = 1; i <= elements.size() / 2; ++i) {
				std::map<std::string, stat_type>::iterator error_code
					= ConfigParser::status_code_map.find(elements[i]);
				if (error_code == ConfigParser::status_code_map.end()) {
					std::cerr << "ConfigParser: " << "locationBlock: " << "error_page: " << SEMANTIC_ERR << std::endl;
					return ERROR;
				}
				// if the code already exists
				if (error_pages.count((*error_code).second)) {
					std::cerr << "ConfigParser: " << "locationBlock: " << "error_page: " << NAME_DUP_ERR << std::endl;
					return ERROR;
				}
				error_pages[(*error_code).second] = elements[i * 2];
			}

		} else if (!elements[0].compare("index")) {
			if (elements.size() == 1) {
				std::cerr << "ConfigParer: " << "locationBlock: " << "index: " << NO_ENTITY_ERR << std::endl;
			}
			for (size_t i = 1; i < elements.size(); ++i)
				indexes.push_back(elements[i]);

		} else if (!elements[0].compare("auto_index")) {
			if (auto_index_check) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "auto_index: " << NAME_DUP_ERR << std::endl;
			}
			if (elements.size() != 2) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "auto_index: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			if (!elements[1].compare("on")) {
				auto_index = true;
			} else if (!elements[1].compare("off")) {
				auto_index = false;
			} else {
				std::cerr << "ConfigParser: " << "locationBlock: " << "auto_index: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			auto_index_check = true;

		} else if (!elements[0].compare("method_allowed")) {
			if (method_allowed_check) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "method_allowed: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			if (elements.size() > 4) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "method_allowed: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			bool	get = false, post = false, del = false;
			for (size_t i = 1; i < elements.size(); ++i) {
				if (get || post || del) {
					std::cerr << "ConfigParser: " << "locationBlock: "
						<< "method_allowed: " << NAME_DUP_ERR << std::endl;
					return ERROR;
				}
				if (!elements[i].compare("GET")) {
					methods_allowed |= GET;
					get = true;
				} else if (!elements[i].compare("POST")) {
					methods_allowed |= POST;
					post = true;
				} else if (!elements[i].compare("DELETE")) {
					methods_allowed |= DELETE;
					del = true;
				} else {
					std::cerr << "ConfigParser: " << "locationBlock: "
						<< "method_allowed: " << SEMANTIC_ERR << std::endl;
				}
			}
			method_allowed_check = true;

		} else if (!elements[0].compare("cgi_info")) {
			if (elements.size() != 3) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "cgi_info: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			if (cgi_infos.count(elements[1])) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "cgi_info: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			cgi_infos[elements[1]] = elements[2];

		} else { // "return"
			if (return_check) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "return: " << NAME_DUP_ERR<< std::endl;
			}
			if (elements.size() != 3) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "return: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			if (!status_code_map.count(elements[1])) {
				std::cerr << "ConfigParser: " << "locationBlock: " << "return: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			return_to.first = status_code_map[elements[1]];
			return_to.second = elements[2];
			return_check = true;
		}
	}
	if (elements[0].compare("}")) {
		std::cerr << "ConfigParser: " << "locationBlock: " << SEMANTIC_ERR << std::endl;
		return ERROR;
	}
	if (!root.length()) {
		std::cerr << "ConfigParser: " << "locationBlock: " << "root: " << NO_ENTITY_ERR << std::endl;
		return ERROR;
	}
	locations.push_back(Location(path, root, indexes, auto_index, error_pages, methods_allowed, cgi_infos, return_to));
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

std::string	trimWhitespace(std::string str) {
	str.erase(str.begin(), str.begin() + str.find_first_not_of(WHITE_SPACES));
	str.erase(str.find_last_not_of(WHITE_SPACES) + 1);
	return str;
}