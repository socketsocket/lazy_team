#include "ServerManager.hpp"

ServerManager*	ServerManager::instance = NULL;

ServerManager::ServerManager()
	: types(0),
	  servers(0),
	  clients(0),
	  resources(0),
	  send_time_out(0),
	  recv_time_out(0),
	  event_changes(0) {}

// Returns an instance of singleton class, ServerManager.
ServerManager&	ServerManager::getServerManager() {
	static ServerManager instance = ServerManager();
	return instance;
}