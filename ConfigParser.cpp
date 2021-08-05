#include "ConfigParser.hpp"


bool	ConfigParser::is_used = false;

std::pair<std::string, Directive>	ConfigParser::server_config_arr[6] = {
		std::pair<std::string, Directive>("listen", kListen),
		std::pair<std::string, Directive>("server_name", kServerName),
		std::pair<std::string, Directive>("root", kRoot),
		std::pair<std::string, Directive>("error_page", kErrorPage),
		std::pair<std::string, Directive>("client_body_limit", kClientBodyLimit),
		std::pair<std::string, Directive>("return", kReturn)
};

std::pair<std::string, Directive>	ConfigParser::location_config_arr[7] = {
		std::pair<std::string, Directive>("root", kRoot),
		std::pair<std::string, Directive>("index", kIndex),
		std::pair<std::string, Directive>("auto_index", kAutoIndex),
		std::pair<std::string, Directive>("error_page", kErrorPage),
		std::pair<std::string, Directive>("method_allowed", kMethodAllowed),
		std::pair<std::string, Directive>("cgi_info", kCgiInfo),
		std::pair<std::string, Directive>("return", kReturn)
};

//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------

// get a line from config file which does not contain comment and is not blank.
int	ConfigParser::getSemanticLine(std::string& line) {
	this->method_name = "getSemanticLine";

	char	buffer[LINE_BUFF_SIZE];

	line = "";

	// skipping semantically blank line
	while (line.length() == 0) {
		// get line till the end of line
		do {
			this->config_file.getline(buffer, LINE_BUFF_SIZE);
			if (this->config_file.bad())
				return this->putError(READ_LINE_ERR);
			line += buffer;
		} while (this->config_file.fail());

		// Delete comment.
		size_t sharp_pos = line.find_first_of('#');
		if (sharp_pos != std::string::npos) {
			line.resize(sharp_pos);
		}

		// Trim whitespaces.
		line = trimWhitespace(line);
	}
	return OK;
}

// Identify whether the line is block start or not.
int ConfigParser::getIntoBlock(std::string block_name, std::vector<std::string> elements) {
	this->method_name = "getIntoBlock";

	if (elements.size() == 0)
		if (this->getLineElements(elements));
			return ERROR;

	if (elements.size() > 2)
		return this->putError(SEMANTIC_ERR);

	// Compare block name and leading part of the line
	if (block_name.compare(elements[0]))
		return this->putError(NAME_MATCH_ERR);

	// Check if there is only a opening bracket. If it is not block end, error.
	if (elements.size() == 1) {
		getLineElements(elements);
		if (elements.size() != 1 || elements[0].compare("{"))
			return this->putError(SEMANTIC_ERR);
		return OK;
	}

	if (elements[1].compare("{"))
		return this->putError(SEMANTIC_ERR);

	return OK;
}

// Get path for location block. '{' can be included in the line or not.
int	ConfigParser::getPath(std::string& path, \
	std::vector<std::string>& line_elements) {
	this->method_name = "getPath";

	if (line_elements.size() < 2 || line_elements.size() > 3)
		return this->putError(SEMANTIC_ERR);
	// check if the block name is location
	if (line_elements[0].compare("location"))
		return this->putError(SEMANTIC_ERR);
	path = line_elements[1];
	// if it seems like having starting bracket
	if (line_elements.size() == 3) {
		// but when it is not starting bracket
		if (line_elements[2].compare("{"))
			return this->putError(SEMANTIC_ERR);
		return OK;
	}
	if (this->getLineElements(line_elements))
		return ERROR;
	if (line_elements.size() != 1)
		return this->putError(SEMANTIC_ERR);
	if (line_elements[0].compare("{"))
		return this->putError(SEMANTIC_ERR);
	return OK;
}

int	ConfigParser::getLineElements(std::vector<std::string>& elements) {
	this->method_name = "getLineElements";

	std::string	line;
	if (this->getSemanticLine(line))
		return ERROR;
	elements.clear();

	// split
	size_t	pos = 0;
	while ((pos = line.find_first_of(WHITE_SPACES)) != std::string::npos) {
		elements.push_back(trimWhitespace(line.substr(0, pos)));
		line.erase(0, pos + 1);
	}
	return OK;
}

int	ConfigParser::httpBlock(ServerManager& server_manager) {
	this->method_name = "httpBlock";

	if (getIntoBlock("http"))
		return ERROR;

	std::vector<std::pair<Server, std::vector<unsigned int> > > configs;
	std::set<std::string>		server_name_set;
	std::vector<std::string>	server_name_vec;

	std::vector<std::string>	line_elements;
	unsigned long				send_timeout = 0;
	bool						send_timeout_check = false;
	unsigned long				recv_timeout = 0;
	bool						recv_timeout_check = false;
	int							ret = 0;

	while (!ret)
	{
		if (this->getLineElements(line_elements))
			return ERROR;
		if (line_elements.size() == 1) {
			if (line_elements[0].compare("}"))
				return this->putError(SEMANTIC_ERR);
			break;
		} else if (line_elements.size() == 2) {
			if (line_elements[0].compare("send_timeout")) {
				if (send_timeout_check)
					return this->putError(NAME_DUP_ERR, "send_timeout");
				std::istringstream	iss(line_elements[1]);
				iss >> send_timeout;
				if (iss.fail())
					return this->putError(SEMANTIC_ERR, "send_timeout");
				server_manager.setSendTimeOut(send_timeout * 1000);
				send_timeout_check = true;
				continue;
			} else if (line_elements[0].compare("recv_timeout")) {
				if (recv_timeout_check)
					return this->putError(NAME_DUP_ERR, "recv_timeout");
				std::istringstream	iss(line_elements[1]);
				iss >> recv_timeout;
				if (iss.fail())
					return this->putError(SEMANTIC_ERR, "recv_timeout");
				server_manager.setRecvTimeOut(recv_timeout * 1000);
				recv_timeout_check = true;
				continue;
			} else if (line_elements.size() > 2) {
				return this->putError(SEMANTIC_ERR);
			}
		}
		ret = this->serverBlock(configs, line_elements);

		// Check server_name duplication by comparing set and vector.
		std::string	server_name = configs.back().first.getServerName();
		if (server_name.length()) {
			server_name_set.insert(server_name);
			server_name_vec.push_back(server_name);
		}
	}

	if (configs.size() == 0)
		return this->putError(NO_ENTITY_ERR);
	if (ret < 0)
		return ret;
	// compare duplicated server_name here
	if (server_name_set.size() != server_name_vec.size())
		return putError(NAME_DUP_ERR, "server_name");
	server_manager.initServerManager(configs);
	return OK;
}

int	ConfigParser::serverBlock( \
	std::vector<std::pair<Server, std::vector<unsigned int> > >& configs, \
	std::vector<std::string>& line_elements) {
	this->method_name = "serverBlock";

	if (this->getIntoBlock("server", line_elements))
		return ERROR;

	std::vector<unsigned long>			ports;
	unsigned int						port;
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
		if (this->getLineElements(elements))
			return ERROR;

		// if find something else, assuming it a location block.
		if (this->server_config.count(elements[0] == 0)) {
			if (this->locationBlock(locations, elements))
				return ERROR;
		} else {
			switch (this->server_config[elements[0]]) {
				case kListen: {
					if (elements.size() != 2)
						return this->putError(SEMANTIC_ERR, "listen");
					// input port number and also check if it only contains digits.
					std::istringstream	iss(elements[1]);
					iss >> port;
					if (iss.fail())
						return this->putError(SEMANTIC_ERR, "listen");
					ports.push_back(port);
					break;
				}
				case kServerName: {
					if (server_name_check)
						return this->putError(NAME_DUP_ERR, "server_name");
					if (elements.size() != 2)
						return this->putError(SEMANTIC_ERR, "server_name");
					server_name = elements[1];
					server_name_check = true;
				}
				case kRoot: {
					if (default_root.length())
						return this->putError(NAME_DUP_ERR, "root");
					if (elements.size() != 2)
						return this->putError(SEMANTIC_ERR, "root");
					default_root = elements[1];
					break;
				}
				case kErrorPage: {
					if (!(elements.size() % 2))
						return this->putError(SEMANTIC_ERR, "error_page");
					// error_page 1 2 3 4 a b c d :config
					// 0          1 2 3 4 5 6 7 8 :index
					for (size_t i = 1; i <= elements.size() / 2; ++i) {
						std::map<std::string, stat_type>::iterator error_code
							= status_code_map.find(elements[i]);
						if (error_code == status_code_map.end())
							return this->putError(SEMANTIC_ERR, "error_page");
						// if the code already exists
						if (default_error_pages.count((*error_code).second))
							return this->putError(NAME_DUP_ERR, "error_page");
						default_error_pages[(*error_code).second] = elements[i * 2];
					}
					break;
				}
				case kClientBodyLimit: {
					if (client_body_limit_check)
						return this->putError(NAME_DUP_ERR, "client_body_limit");
					if (elements.size() != 2)
						return this->putError(SEMANTIC_ERR, "client_body_limit");
					std::istringstream	iss(elements[1]);
					iss >> client_body_limit;
					if (iss.fail())
						return this->putError(SEMANTIC_ERR, "client_body_limit");
					client_body_limit_check = true;
					break;
				}
				case kReturn: {
					if (return_to.first)
						return this->putError(NAME_DUP_ERR, "return");
					if (elements.size() != 3 && elements.size() != 2)
						return this->putError(SEMANTIC_ERR, "return");
					if (status_code_map.find(elements[1]) == status_code_map.end())
						return this->putError(SEMANTIC_ERR, "return");
					return_to.first = status_code_map[elements[1]];
					if (elements.size() == 3)
						return_to.second = elements[2];
					break;
				}
				default: {
					assert(false);
				}
			}
		}
	} while (elements.size() > 1);

	// if the code block is not ending
	if (elements[0].compare("}"))
		return this->putError(SEMANTIC_ERR);
	// if there is no root directive and location or no return directive
	if ((!default_root.length() && !locations.size()) || !return_to.first)
		return this->putError(NO_ENTITY_ERR, "root");
	configs.push_back(std::make_pair(Server(server_name, default_root, \
		default_error_pages, client_body_limit, locations, return_to), ports));
	return OK;
}

int	ConfigParser::locationBlock(std::vector<Location>& locations, \
	std::vector<std::string>& line_elements) {
	this->method_name = "locationBlock";

	std::string	path;
	if (this->getPath(path, line_elements))
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
	if (this->getLineElements(line_elements))
		return ERROR;

	do { // while (line_elements.size() > 1);
		if (this->location_config.count(line_elements[0]) == 0)
			return this->putError(NAME_MATCH_ERR);

		switch (this->location_config[line_elements[0]]) {
			case kRoot: {
				if (line_elements.size() != 2)
					return this->putError(SEMANTIC_ERR, "root");
				if (root.length())
					return this->putError(NAME_DUP_ERR, "root");
				root = line_elements[1];
				break;
			}
			case kErrorPage: {
				if (!(line_elements.size() % 2))
					return this->putError(SEMANTIC_ERR, "error_page");
				// error_page 1 2 3 4 a b c d :config
				// 0          1 2 3 4 5 6 7 8 :index
				for (size_t i = 1; i <= line_elements.size() / 2; ++i) {
					std::map<std::string, stat_type>::iterator error_code
						= status_code_map.find(line_elements[i]);
					if (error_code == status_code_map.end())
						return this->putError(SEMANTIC_ERR, "error_page");
					// if the code already exists
					if (error_pages.count((*error_code).second))
						return this->putError(NAME_DUP_ERR, "error_page");
					error_pages[(*error_code).second] = line_elements[i * 2];
				}
				break;
			}
			case kIndex: {
				if (line_elements.size() == 1)
					return this->putError(NO_ENTITY_ERR, "index");
				for (size_t i = 1; i < line_elements.size(); ++i)
					indexes.push_back(line_elements[i]);
				break;
			}
			case kAutoIndex: {
				if (auto_index_check)
					return this->putError(NAME_DUP_ERR, "auto_index");
				if (line_elements.size() != 2)
					return this->putError(SEMANTIC_ERR, "auto_index");
				if (!line_elements[1].compare("on")) {
					auto_index = true;
				} else if (!line_elements[1].compare("off")) {
					auto_index = false;
				} else {
					return this->putError(SEMANTIC_ERR, "auto_index");
				}
				auto_index_check = true;
				break;
			}
			case kMethodAllowed: {
				if (method_allowed_check)
					return this->putError(NAME_DUP_ERR, "method_allowed");
				if (line_elements.size() > 4)
					return this->putError(SEMANTIC_ERR, "method_allowed");
				bool	get = false, post = false, del = false;
				for (size_t i = 1; i < line_elements.size(); ++i) {
					if (get || post || del)
						return this->putError(NAME_DUP_ERR, "method_allowed");
					if (!line_elements[i].compare("GET")) {
						methods_allowed |= GET;
						get = true;
					} else if (!line_elements[i].compare("POST")) {
						methods_allowed |= POST;
						post = true;
					} else if (!line_elements[i].compare("DELETE")) {
						methods_allowed |= DELETE;
						del = true;
					} else {
						return this->putError(SEMANTIC_ERR, "method_allowed");
					}
				}
				method_allowed_check = true;
				break;
			}
			case kCgiInfo: {
				if (line_elements.size() != 3)
					return this->putError(SEMANTIC_ERR, "cgi_info");
				if (cgi_infos.count(line_elements[1]))
					return this->putError(NAME_DUP_ERR, "cgi_info");
				cgi_infos[line_elements[1]] = line_elements[2];
				break;
			}
			case kReturn: {
				if (return_to.first)
					return this->putError(NAME_DUP_ERR, "return");
				if (line_elements.size() != 3)
					return this->putError(SEMANTIC_ERR, "return");
				if (!status_code_map.count(line_elements[1]))
					return this->putError(SEMANTIC_ERR, "return");
				return_to.first = status_code_map[line_elements[1]];
				return_to.second = line_elements[2];
				break;
			}
			default: {
				assert(false);
			}
		}
		if (this->getLineElements(line_elements))
			return ERROR;
	} while (line_elements.size() > 1);

	// if the code block is not ending
	if (line_elements[0].compare("}"))
		return this->putError(SEMANTIC_ERR);
	locations.push_back(Location(path, root, indexes, auto_index, error_pages, \
		methods_allowed, cgi_infos, return_to));
	return OK;
}

int		ConfigParser::putError(const char* err_msg, std::string opt = "") {
	std::string	seperator(": ");
	std::cerr << class_name + seperator + method_name + seperator;
	if (opt.length())
		std::cerr << opt + seperator;
	std::cerr << err_msg << std::endl;
	return ERROR;
}


//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

ConfigParser::ConfigParser(std::string config_path)
	: server_config(ConfigParser::server_config_arr, ConfigParser::server_config_arr \
			+ sizeof(ConfigParser::server_config_arr) / sizeof(std::pair<std::string, Directive>)),
	  location_config(ConfigParser::location_config_arr, ConfigParser::location_config_arr \
			+ sizeof(ConfigParser::location_config_arr) / sizeof(std::pair<std::string, Directive>)),
	  config_file(config_path),
	  class_name("Config_parser") {}

ConfigParser::~ConfigParser() {}

// the only public function of the class. initilized server_manager.
int	ConfigParser::setData(ServerManager& server_manager) {
	this->method_name = "getData";

	if (ConfigParser::is_used)
		return ERROR;
	ConfigParser::is_used = true;
	// Check if configfile is properly opened.
	if (!this->config_file.is_open())
		return this->putError(OPEN_FILE_ERR, "readFile");
	this->httpBlock(server_manager);
	this->config_file.close();
	return OK;
}

std::string	trimWhitespace(std::string str) {
	str.erase(str.begin(), str.begin() + str.find_first_not_of(WHITE_SPACES));
	str.erase(str.find_last_not_of(WHITE_SPACES) + 1);
	return str;
}