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

int	ConfigParser::getSemanticLine(std::string& line) {
	char	buffer[LINE_BUFF_SIZE];

	while (line.length() == 0) {
		ConfigParser::config_file.getline(buffer, LINE_BUFF_SIZE);
		if (ConfigParser::config_file.bad()) {
			std::cerr << "ConfigParser: " << "getSemanticLine: " \
				<< READ_LINE_ERR << std::endl;;
			return ERROR;
		}
		line = buffer;
		while (ConfigParser::config_file.fail()) {
			ConfigParser::config_file.getline(buffer, LINE_BUFF_SIZE);
			if (config_file.bad()) {
				std::cerr << "ConfigParser: " << "getSemanticLine: " \
					<< READ_LINE_ERR << std::endl;
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
		std::cerr << "ConfigParser: " << "getIntoBlock: " \
			<< NAME_MATCH_ERR << std::endl;
		return ERROR;
	}

	// Check if there is only a opening bracket.
	line.erase(0, block_name.length());
	line = trimWhitespace(line);
	if (line.length() == 0)
		if (ConfigParser::getSemanticLine(line));
			return ERROR;

	if (line.compare("{")) {
		std::cerr << "ConfigParser: " << "getIntoBlock: " \
			<< SEMANTIC_ERR << std::endl;
		return ERROR;
	}
	return OK;
}

int	ConfigParser::getPath(std::string& path, \
	std::vector<std::string>& elements) {
	if (elements.size() < 2 || elements.size() > 3) {
		std::cerr << "ConfigParser: " << "getPath: " \
			<< SEMANTIC_ERR << std::endl;
		return ERROR;
	}
	if (elements[0].compare("location")) {
		std::cerr << "ConfigParser: " << "getPath: " \
			<< SEMANTIC_ERR << std::endl;
		return ERROR;
	}
	path = elements[1];
	if (elements.size() == 3) {
		// if it is not stargin bracket
		if (elements[2].compare("{")) {
			std::cerr << "ConfigParser: " << "getPath: " << \
				SEMANTIC_ERR << std::endl;
			return ERROR;
		}
		return OK;
	}
	if (ConfigParser::getLineElements(elements))
		return ERROR;
	if (elements.size() != 1) {
		std::cerr << "ConfigParser: " << "getPath: " << \
			SEMANTIC_ERR << std::endl;
		return ERROR;
	}
	if (elements[0].compare("{")) {
		std::cerr << "ConfigParser: " << "getPath: " << \
			SEMANTIC_ERR << std::endl;
		return ERROR;
	}
}

int	ConfigParser::getLineElements(std::vector<std::string>& elements) {
	std::string	line;
	if (ConfigParser::getSemanticLine(line))
		return ERROR;
	elements.clear();

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
	if (getIntoBlock("http"))
		return ERROR;

	int	ret = 0;

	while (!ret)
		ret = ConfigParser::serverBlock(servers);
	if (servers.size() == 0) {
		std::cerr << "ConfigParser: " << "httpBlock: " \
			<< NO_ENTITY_ERR << std::endl;
		return ERROR;
	} else if (ret == BLOCK_END) {
		//OK
	}
	return ret;
}

int	ConfigParser::serverBlock(std::vector<Server>& servers) {
	int	ret;
	if ((ret = getIntoBlock("server")))
		return ret;

	unsigned int						port = 80;
	bool								port_check = false;
	std::string							server_name = "localhost";
	bool								server_name_check = false;
	std::string							default_root;
	std::map<stat_type, std::string>	default_error_pages;
	unsigned long						client_body_limit = 0;
	bool								client_body_limit_check = false;
	std::vector<Location>				locations;
	std::pair<stat_type, std::string>	return_to = std::make_pair(NULL, "");

	std::vector<std::string>	elements;
	do { // while (elements.size() > 1);
		if (ConfigParser::getLineElements(elements))
			return ERROR;

		// Find something else, assuming it a location block.
		if ((ConfigParser::location_config.find(elements[0])) \
			== ConfigParser::location_config.end()) {
			if (ConfigParser::locationBlock(locations, elements)) {
				return ERROR;
			}
		// listen directive
		} else if (!elements[0].compare("listen")) {
			if (port_check) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"listen: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			if (elements.size() != 2) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					<< "listen: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			// input port number and also check if it only contains digits.
			std::istringstream	iss(elements[1]);
			iss >> port;
			if (iss.fail()) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					<< "listen: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			port_check = true;
		// server_name directive
		} else if (!elements[0].compare("server_name")) {
			if (server_name_check) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"server_name: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			if (elements.size() != 2) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"server_name: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			server_name = elements[1];
			server_name_check = true;
		// root directive
		} else if (!elements[0].compare("root")) {
			if (default_root.length()) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"root: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			if (elements.size() != 2) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"server_name: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			default_root = elements[1];
		// error_page directive
		} else if (!elements[0].compare("error_page")) {
			if (!(elements.size() % 2)) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					<< "error_page:" << SEMANTIC_ERR << std::endl;
			}
			// error_page 1 2 3 4 a b c d :config
			// 0          1 2 3 4 5 6 7 8 :index
			for (size_t i = 1; i <= elements.size() / 2; ++i) {
				std::map<std::string, stat_type>::iterator error_code
					= status_code_map.find(elements[i]);
				if (error_code == status_code_map.end()) {
					std::cerr << "ConfigParser: " << "serverBlock: " \
						<< "error_page: " << SEMANTIC_ERR << std::endl;
					return ERROR;
				}
				// if the code already exists
				if (default_error_pages.count((*error_code).second)) {
					std::cerr << "ConfigParser: " << "serverBlock: " \
						<< "error_page: " << NAME_DUP_ERR << std::endl;
					return ERROR;
				}
				default_error_pages[(*error_code).second] = elements[i * 2];
			}
		// client_body_limit directive
		} else if (!elements[0].compare("client_body_limit")) {
			if (client_body_limit_check) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"client_body_limit: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			if (elements.size() != 2) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"client_body_limit: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			std::istringstream	iss(elements[1]);
			iss >> client_body_limit;
			if (iss.fail()) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"client_body_limit: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			client_body_limit_check = true;
		// return directive
		} else {
			if (return_to.first) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"return: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			if (elements.size() != 3) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"return: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			if (status_code_map.find(elements[1]) == status_code_map.end()) {
				std::cerr << "ConfigParser: " << "serverBlock: " \
					"return: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			return_to.first = status_code_map[elements[1]];
			return_to.second = elements[2];
		}
	} while (elements.size() > 1);
	// if the code block is not ending
	if (elements[0].compare("}")) {
		std::cerr << "ConfigParser: " << "locationBlock: " \
			<< SEMANTIC_ERR << std::endl;
		return ERROR;
	}
	// if there is no root directive and location or no return directive
	if ((!default_root.length() && !locations.size()) || !return_to.first) {
		std::cerr << "ConfigParser: " << "locationBlock: " \
			<< "root: " << NO_ENTITY_ERR << std::endl;
		return ERROR;
	}
	servers.push_back(Server(port, server_name, default_root, \
		default_error_pages, client_body_limit, locations, return_to));
	return OK;
}

int	ConfigParser::locationBlock(std::vector<Location>& locations, \
	std::vector<std::string>& elements) {
	std::string	path;
	if (ConfigParser::getPath(path, elements))
		return ERROR;

	// Arguments for Location constructor.
	std::string							root = ""; // when there is no root
	std::vector<std::string>			indexes(0); // when there is no index pages
	bool								auto_index = false; // the default value is false.
	bool								auto_index_check = false; // to check if there is more than one auto_index.
	std::map<stat_type, std::string>	error_pages;
	Method								methods_allowed = GET; // default method is get
	bool								method_allowed_check = false; // to check duplicated directives
	std::map<std::string, std::string>	cgi_infos;
	std::pair<stat_type, std::string>	return_to = std::make_pair(NULL, "");

	// Elements included in one line.
	do { // while (elements.size() > 1);
		if (ConfigParser::getLineElements(elements))
			return ERROR;
		if ((ConfigParser::location_config.find(elements[0]) \
			== ConfigParser::location_config.end())) {
			std::cerr << "ConfigParser: " << "locationBlock: " \
				<< NAME_MATCH_ERR << std::endl;
			return ERROR;
		}
		// root directive. check directive size and duplication.
		if (!elements[0].compare("root")) {
			if (elements.size() != 2) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "root: "<< SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			if (root.length()) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "root: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			root = elements[1];
		// error_page directive. check directive number.
		} else if (!elements[0].compare("error_page")) {
			if (!(elements.size() % 2)) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "error_page:" << SEMANTIC_ERR << std::endl;
			}
			// error_page 1 2 3 4 a b c d :config
			// 0          1 2 3 4 5 6 7 8 :index
			for (size_t i = 1; i <= elements.size() / 2; ++i) {
				std::map<std::string, stat_type>::iterator error_code
					= status_code_map.find(elements[i]);
				if (error_code == status_code_map.end()) {
					std::cerr << "ConfigParser: " << "locationBlock: " \
						<< "error_page: " << SEMANTIC_ERR << std::endl;
					return ERROR;
				}
				// if the code already exists
				if (error_pages.count((*error_code).second)) {
					std::cerr << "ConfigParser: " << "locationBlock: " \
						<< "error_page: " << NAME_DUP_ERR << std::endl;
					return ERROR;
				}
				error_pages[(*error_code).second] = elements[i * 2];
			}
		// index directive. check directive size.
		} else if (!elements[0].compare("index")) {
			if (elements.size() == 1) {
				std::cerr << "ConfigParer: " << "locationBlock: " \
					<< "index: " << NO_ENTITY_ERR << std::endl;
			}
			for (size_t i = 1; i < elements.size(); ++i)
				indexes.push_back(elements[i]);
		// auto_index directive. check duplication, directive size, and value.
		} else if (!elements[0].compare("auto_index")) {
			if (auto_index_check) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "auto_index: " << NAME_DUP_ERR << std::endl;
			}
			if (elements.size() != 2) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "auto_index: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			if (!elements[1].compare("on")) {
				auto_index = true;
			} else if (!elements[1].compare("off")) {
				auto_index = false;
			} else {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "auto_index: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			auto_index_check = true;
		// method_allowed directive. check duplication, directive size, and method names.
		} else if (!elements[0].compare("method_allowed")) {
			if (method_allowed_check) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "method_allowed: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			if (elements.size() > 4) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "method_allowed: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			bool	get = false, post = false, del = false;
			for (size_t i = 1; i < elements.size(); ++i) {
				if (get || post || del) {
					std::cerr << "ConfigParser: " << "locationBlock: " \
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
					std::cerr << "ConfigParser: " << "locationBlock: " \
						<< "method_allowed: " << SEMANTIC_ERR << std::endl;
				}
			}
			method_allowed_check = true;
		// cgi_info directive. check directive size, cgi extension duplication.
		} else if (!elements[0].compare("cgi_info")) {
			if (elements.size() != 3) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "cgi_info: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			if (cgi_infos.count(elements[1])) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "cgi_info: " << NAME_DUP_ERR << std::endl;
				return ERROR;
			}
			cgi_infos[elements[1]] = elements[2];
		// return directive. check duplication, directive size, and code check.
		} else {
			if (return_to.first) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "return: " << NAME_DUP_ERR<< std::endl;
			}
			if (elements.size() != 3) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "return: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			if (!status_code_map.count(elements[1])) {
				std::cerr << "ConfigParser: " << "locationBlock: " \
					<< "return: " << SEMANTIC_ERR << std::endl;
				return ERROR;
			}
			return_to.first = status_code_map[elements[1]];
			return_to.second = elements[2];
		}
	} while (elements.size() > 1);
	// if the code block is not ending
	if (elements[0].compare("}")) {
		std::cerr << "ConfigParser: " << "locationBlock: " \
			<< SEMANTIC_ERR << std::endl;
		return ERROR;
	}
	// if there is no root directive or no return directive
	if (!root.length() || !return_to.first) {
		std::cerr << "ConfigParser: " << "locationBlock: " \
			<< "root: " << NO_ENTITY_ERR << std::endl;
		return ERROR;
	}
	locations.push_back(Location(path, root, indexes, auto_index, error_pages, \
		methods_allowed, cgi_infos, return_to));
	return OK;
}

int ConfigParser::readFile(std::vector<Server>& servers, \
	const char* config_path) {
	// Open config file
	ConfigParser::config_file.open(config_path);
	if (!ConfigParser::config_file.is_open()) {
		std::cerr << std::string("") + "ConfigParser: " \
			+ "ReadFile: " + OPEN_FILE_ERR;
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