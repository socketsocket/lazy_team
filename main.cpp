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
	ServerManager& server_manager = ServerManager::getServerManager(std::vector<Server>(0));
	server_manager.setStatus(ERROR);
};

int	main(int argc, char* argv[]) {
	initStatusCodeMap();

	std::vector<Server>	servers;
	{
		ConfigParser	config_parser(argv[1]);
		config_parser.setServers(servers);
	}

	// When CTRL-C is pressed, deallocate everything and end the server.
	signal(SIGINT, sigIntHandler);

	ServerManager& server_manager = ServerManager::getServerManager(servers);
	while (true) {
		if (server_manager.getStatus() == ERROR)
			break;
		server_manager.processEvent();
	}
	return OK;
}