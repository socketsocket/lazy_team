#ifndef WEBSERV_SERVERMANAGER_HPP_
#define WEBSERV_SERVERMANAGER_HPP_

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "Webserv.hpp"
#include "ErrorMsgHandler.hpp"
#include "PortManager.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Re3.hpp"

#define EVENT_SIZE	1024

// ServerManager is a singleton class which manages I/O of servers, clients, resources.
class ServerManager {
	private:
		int							status;
		std::vector<Server>			servers; // contains original servers.

// Variable types stores the type of fd, which can be connected to a server, a client or a resource.
// These connection can be reached by variables servers, clients, resources.
		std::vector<FdType>			types;
		std::vector<PortManager*>	managers;
		std::vector<Client*>		clients;
		std::vector<Re3*>			Re3s;

// Send_time_out and recv_time_out is determined by configuration file.
		unsigned long				send_timeout;
		unsigned long				recv_timeout;

// These variables are needed for using kqueue
		int							kq;
		std::vector<struct kevent>	event_changes;
		struct kevent				event_list[EVENT_SIZE];
		struct kevent				event_current;

		ServerManager();
// copy constructor, and assignation operator are disabled.
		ServerManager(const ServerManager& ref);
		ServerManager&	operator=(const ServerManager& ref);

		int	callKevent();
		int	makeClient(PortManager& port_manager);

	public:
		~ServerManager();
		int						getStatus();
		static ServerManager&	getServerManager();
		std::vector<Server>&	getServersRef();
		void					setStatus(int status);
		void					setSendTimeOut(unsigned long send_time_out);
		void					setRecvTimeOut(unsigned long recv_time_out);
		int						processEvent();
		int						initServerManager(const std::vector<std::pair<Server, std::vector<unsigned int> > > configs);
};

#endif
