#include "Webserv.hpp"
#include "ConfigParser.hpp"
#include "ServerManager.hpp"
#include "Server.hpp"

void	initStatusCodeMap() {
	for (size_t i = 0; i < 40; ++i)
		status_code_map[std::string(status_code_arr[i]).substr(0, 3)]
			= status_code_arr[i];
}

void	sigIntHandler(int param) {
	(void)param;
	ServerManager& server_manager = ServerManager::getServerManager();
	server_manager.setStatus(INTR);
};

int	main(int argc, char* argv[]) {
	initStatusCodeMap();

	ServerManager& server_manager = ServerManager::getServerManager();
	{
		std::string	config_path;
		if (argc == 1) {
			config_path = DEFAULT_ROUTE;
		} else if (argc == 2) {
			config_path = argv[1];
		} else {
			std::cerr << "Too many arguments.";
			return ERROR;
		}
		ConfigParser	config_parser(config_path);
		config_parser.setData(server_manager);
	}

	// When CTRL-C is pressed, deallocate everything and end the server.
	if ((signal(SIGINT, sigIntHandler) == SIG_ERR))
		return ERROR;

	while (true) {
		if (server_manager.getStatus() != OK)
			break;
		server_manager.processEvent();
	}
	if (server_manager.getStatus() == ERROR)
		return ERROR;
	if (server_manager.getStatus() == INTR) {
		// Adequate processing
	}
	return OK;
}