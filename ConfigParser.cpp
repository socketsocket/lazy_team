#include "ConfigParser.hpp"


bool	ConfigParser::is_used = false;

std::string	ConfigParser::server_config_arr[6] = {
		"listen",
		"server_name",
		"root",
		"error_page",
		"client_body_limit",
		"return"
};

std::string	ConfigParser::location_config_arr[7] = {
		"root",
		"index",
		"auto_index",
		"error_page",
		"method_allowed",
		"cgi_info",
		"return"
};

//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------

// get a line from config file which does not contain comment and is not blank.
int	ConfigParser::getSemanticLine(std::string& line) {
	this->method_name = "getSemanticLine";

	char	buffer[LINE_BUFF_SIZE];

	while (line.length() == 0) { // skipping semantically blank line

		// first trial for get a line
		this->config_file.getline(buffer, LINE_BUFF_SIZE);
		if (this->config_file.bad())
			return this->putError(READ_LINE_ERR);
		line = buffer;

		// get line till the end of line
		while (this->config_file.fail()) {
			this->config_file.getline(buffer, LINE_BUFF_SIZE);
			if (config_file.bad())
				return this->putError(READ_LINE_ERR);
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
	this->method_name = "getIntoBlock";

	if (line.length() == 0)
		if (this->getSemanticLine(line));
			return ERROR;

	if (line.compare("}"))
		return BLOCK_END;

	// Compare block name and leading part of the line
	std::string candidate = line.substr(0, block_name.length());
	if (block_name.compare(candidate))
		return this->putError(NAME_MATCH_ERR);

	// Check if there is only a opening bracket.
	line.erase(0, block_name.length());
	line = trimWhitespace(line);
	if (line.length() == 0)
		if (this->getSemanticLine(line));
			return ERROR;

	// if it is not block end, error.
	if (line.compare("{"))
		return this->putError(SEMANTIC_ERR);
	return OK;
}

// Get path for location block. '{' can be included in the line or not.
int	ConfigParser::getPath(std::string& path, \
	std::vector<std::string>& elements) {
	this->method_name = "getPath";

	if (elements.size() < 2 || elements.size() > 3)
		return this->putError(SEMANTIC_ERR);
	// check if the block name is location
	if (elements[0].compare("location"))
		return this->putError(SEMANTIC_ERR);
	path = elements[1];
	// if it seems like having starting bracket
	if (elements.size() == 3) {
		// but when it is not starting bracket
		if (elements[2].compare("{"))
			return this->putError(SEMANTIC_ERR);
		return OK;
	}
	if (this->getLineElements(elements))
		return ERROR;
	if (elements.size() != 1)
		return this->putError(SEMANTIC_ERR);
	if (elements[0].compare("{"))
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

	std::vector<std::string>	elements;
	unsigned long				send_timeout = 0;
	bool						send_timeout_check = false;
	unsigned long				recv_timeout = 0;
	bool						recv_timeout_check = false;
	int							ret = 0;
	std::string					line = "";

	while (!ret)
	{
		if (this->getLineElements(elements))
			return ERROR;
		if (elements.size() == 2) {
			if (elements[0].compare("send_timeout")) {
				if (send_timeout_check)
					return putError(NAME_DUP_ERR, "send_timeout");
				std::istringstream	iss(elements[1]);
				iss >> send_timeout;
				if (iss.fail())
					return putError(SEMANTIC_ERR, "send_timeout");
				server_manager.setSendTimeOut(send_timeout);
				send_timeout_check = true;
			} else if (elements[0].compare("recv_timeout")) {
				if (recv_timeout_check)
					return putError(NAME_DUP_ERR, "recv_timeout");
				std::istringstream	iss(elements[1]);
				iss >> recv_timeout;
				if (iss.fail())
					return putError(SEMANTIC_ERR, "recv_timeout");
				server_manager.setRecvTimeOut(send_timeout);
				recv_timeout_check = true;
			}
		}
		for (size_t i = 0; i < elements.size(); ++i)
			line += elements[i] + " ";
		if ((ret = getIntoBlock("server", line)))
			return ret;
		ret = this->serverBlock(configs);

		// Check server_name duplication by comparing set and vector.
		std::string	server_name = configs.back().first.getServerName();
		if (server_name.length()) {
			server_name_set.insert(server_name);
			server_name_vec.push_back(server_name);
		}
	}
	if (configs.size() == 0) {
		return this->putError(NO_ENTITY_ERR);
	} else if (ret == BLOCK_END) {
		// compare here
		if (server_name_set.size() != server_name_vec.size())
			return putError(NAME_DUP_ERR, "server_name");
		server_manager.initServerManager(configs);
		return OK;
	}
	return ret;
}

int	ConfigParser::serverBlock( \
	std::vector<std::pair<Server, std::vector<unsigned int> > >& configs) {
	this->method_name = "serverBlock";

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

		// Find something else, assuming it a location block.
		if ((this->location_config.find(elements[0])) \
			== this->location_config.end()) {
			if (this->locationBlock(locations, elements)) {
				return ERROR;
			}
		// listen directive
		} else if (!elements[0].compare("listen")) {
			if (elements.size() != 2)
				return this->putError(SEMANTIC_ERR, "listen");
			// input port number and also check if it only contains digits.
			std::istringstream	iss(elements[1]);
			iss >> port;
			if (iss.fail())
				return this->putError(SEMANTIC_ERR, "listen");
			ports.push_back(port);
		// server_name directive
		} else if (!elements[0].compare("server_name")) {
			if (server_name_check)
				return this->putError(NAME_DUP_ERR, "server_name");
			if (elements.size() != 2)
				return this->putError(SEMANTIC_ERR, "server_name");
			server_name = elements[1];
			server_name_check = true;
		// root directive
		} else if (!elements[0].compare("root")) {
			if (default_root.length())
				return this->putError(NAME_DUP_ERR, "root");
			if (elements.size() != 2)
				return this->putError(SEMANTIC_ERR, "root");
			default_root = elements[1];
		// error_page directive
		} else if (!elements[0].compare("error_page")) {
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
		// client_body_limit directive
		} else if (!elements[0].compare("client_body_limit")) {
			if (client_body_limit_check)
				return this->putError(NAME_DUP_ERR, "client_body_limit");
			if (elements.size() != 2)
				return this->putError(SEMANTIC_ERR, "client_body_limit");
			std::istringstream	iss(elements[1]);
			iss >> client_body_limit;
			if (iss.fail())
				return this->putError(SEMANTIC_ERR, "client_body_limit");
			client_body_limit_check = true;
		// return directive
		} else {
			// if return directive is duplicated
			if (return_to.first)
				return this->putError(NAME_DUP_ERR, "return");
			if (elements.size() != 3 && elements.size() != 2)
				return this->putError(SEMANTIC_ERR, "return");
			if (status_code_map.find(elements[1]) == status_code_map.end())
				return this->putError(SEMANTIC_ERR, "return");
			return_to.first = status_code_map[elements[1]];
			if (elements.size() == 3)
				return_to.second = elements[2];
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
	std::vector<std::string>& elements) {
	this->method_name = "locationBlock";

	std::string	path;
	if (this->getPath(path, elements))
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
		if (this->getLineElements(elements))
			return ERROR;
		if ((this->location_config.find(elements[0]) \
			== this->location_config.end()))
			return this->putError(NAME_MATCH_ERR);
		// root directive. check directive size and duplication.
		if (!elements[0].compare("root")) {
			if (elements.size() != 2)
				return this->putError(SEMANTIC_ERR, "root");
			if (root.length())
				return this->putError(NAME_DUP_ERR, "root");
			root = elements[1];
		// error_page directive. check directive number.
		} else if (!elements[0].compare("error_page")) {
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
				if (error_pages.count((*error_code).second))
					return this->putError(NAME_DUP_ERR, "error_page");
				error_pages[(*error_code).second] = elements[i * 2];
			}
		// index directive. check directive size.
		} else if (!elements[0].compare("index")) {
			if (elements.size() == 1)
				return this->putError(NO_ENTITY_ERR, "index");
			for (size_t i = 1; i < elements.size(); ++i)
				indexes.push_back(elements[i]);
		// auto_index directive. check duplication, directive size, and value.
		} else if (!elements[0].compare("auto_index")) {
			if (auto_index_check)
				return this->putError(NAME_DUP_ERR, "auto_index");
			if (elements.size() != 2)
				return this->putError(SEMANTIC_ERR, "auto_index");
			if (!elements[1].compare("on")) {
				auto_index = true;
			} else if (!elements[1].compare("off")) {
				auto_index = false;
			} else {
				return this->putError(SEMANTIC_ERR, "auto_index");
			}
			auto_index_check = true;
		// method_allowed directive. check duplication, directive size, and method names.
		} else if (!elements[0].compare("method_allowed")) {
			if (method_allowed_check)
				return this->putError(NAME_DUP_ERR, "method_allowed");
			if (elements.size() > 4)
				return this->putError(SEMANTIC_ERR, "method_allowed");
			bool	get = false, post = false, del = false;
			for (size_t i = 1; i < elements.size(); ++i) {
				if (get || post || del)
					return this->putError(NAME_DUP_ERR, "method_allowed");
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
					return this->putError(SEMANTIC_ERR, "method_allowed");
				}
			}
			method_allowed_check = true;
		// cgi_info directive. check directive size, cgi extension duplication.
		} else if (!elements[0].compare("cgi_info")) {
			if (elements.size() != 3)
				return this->putError(SEMANTIC_ERR, "cgi_info");
			if (cgi_infos.count(elements[1]))
				return this->putError(NAME_DUP_ERR, "cgi_info");
			cgi_infos[elements[1]] = elements[2];
		// return directive. check duplication, directive size, and code check.
		} else {
			if (return_to.first)
				return this->putError(NAME_DUP_ERR, "return");
			if (elements.size() != 3)
				return this->putError(SEMANTIC_ERR, "return");
			if (!status_code_map.count(elements[1]))
				return this->putError(SEMANTIC_ERR, "return");
			return_to.first = status_code_map[elements[1]];
			return_to.second = elements[2];
		}
	} while (elements.size() > 1);
	// if the code block is not ending
	if (elements[0].compare("}"))
		return this->putError(SEMANTIC_ERR);
	// if there is no root directive or no return directive
	if (!root.length() || !return_to.first)
		return this->putError(NO_ENTITY_ERR, "root");
	locations.push_back(Location(path, root, indexes, auto_index, error_pages, \
		methods_allowed, cgi_infos, return_to));
	return OK;
}

int		ConfigParser::putError(const char* err_msg, std::string opt = "") {
	std::string	seperator(": ");
	std::cerr << class_name << seperator << method_name << seperator;
	if (opt.length())
		std::cerr << opt << seperator;
	std::cerr << err_msg << std::endl;
	return ERROR;
}


//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

ConfigParser::ConfigParser(const char* config_path)
	: server_config(ConfigParser::server_config_arr, ConfigParser::server_config_arr \
			+ sizeof(ConfigParser::server_config_arr) / sizeof(std::string)),
	  location_config(ConfigParser::location_config_arr, ConfigParser::location_config_arr \
			+ sizeof(ConfigParser::location_config_arr) / sizeof(std::string)),
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