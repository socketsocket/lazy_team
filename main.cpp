#include "Webserv.hpp"
#include "ConfigParser.hpp"
#include "ServerManager.hpp"
#include "Server.hpp"

void	initStatusCodeMap() {
	for (size_t i = 0; i < sizeof(status_code_arr); ++i)
		status_code_map[std::string(status_code_arr[i]).substr(0, 3)]
			= status_code_arr[i];
}

void	sigIntHandler(int param) {
	ServerManager& server_manager = ServerManager::getServerManager();
	server_manager.setStatus(INTR);
};

int	main(int argc, char* argv[]) {
	initStatusCodeMap();

	ServerManager& server_manager = ServerManager::getServerManager();
	{
		ConfigParser	config_parser(argv[1]);
		config_parser.setData(server_manager);
	}

	// When CTRL-C is pressed, deallocate everything and end the server.
	if ((signal(SIGINT, sigIntHandler) == SIG_ERR));
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