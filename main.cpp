#include "Webserv.hpp"
#include "ConfigParser.hpp"
#include "ServerManager.hpp"
#include "Server.hpp"

stat_type	stat_code_arr[] = {C100, C101, C200, C201, C202,
	C203, C204, C205, C206, C300, C301, C302, C303, C304, C305, C307, C400,
	C401, C402, C403, C404, C405, C406, C407, C408, C409, C410, C411, C412,
	C413, C414, C415, C416, C417, C500, C501, C502, C503, C504, C505};

std::map<std::string, stat_type>	stat_code_map;

void	initStatCodeMap() {
	for (size_t i = 0; i < 40; ++i)
		stat_code_map[std::string(stat_code_arr[i]).substr(0, 3)]
			= stat_code_arr[i];
}

void	sigIntHandler(int param) {
	(void)param;
	ServerManager& server_manager = ServerManager::getServerManager();
	server_manager.setStatus(INTR);
};

int	main(int argc, char* argv[]) {
	initStatCodeMap();

	ServerManager& server_manager = ServerManager::getServerManager();
	{
		std::string	config_path;
		if (argc == 1) {
			config_path = DEFAULT_ROUTE;
		} else if (argc == 2) {
			config_path = argv[1];
		} else {
			std::cerr << "Too many arguments.\n";
			return ERROR;
		}
		ConfigParser	config_parser(config_path);
		if (config_parser.setData(server_manager))
			return ERROR;
	}

	// When CTRL-C is pressed, deallocate everything and end the server.
	if ((signal(SIGINT, sigIntHandler) == SIG_ERR))
		return ERROR;

	while (true) {
		server_manager.processEvent();
		if (server_manager.getStatus() != OK && !hasMsg(STDERR))
			break;
	}
	if (server_manager.getStatus() == ERROR)
		return ERROR;
	if (server_manager.getStatus() == INTR) {
		// Adequate processing
	}
	return OK;
}