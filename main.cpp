#include "Webserv.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"

void	initStatusCodeMap() {
	for (size_t i = 0; i < sizeof(status_code_arr); ++i)
		status_code_map[std::string(status_code_arr[i]).substr(0, 3)]
			= status_code_arr[i];
}

int	main(int argc, char* argv[]) {
	initStatusCodeMap();

	std::vector<Server>	servers;
	{
		ConfigParser	config_parser(argv[1]);
		config_parser.setServers(servers);
	}

	ServerManager& ServerManager = ServerManager::getServerManager(servers);
	while (true) {

	}
	return (OK);
}