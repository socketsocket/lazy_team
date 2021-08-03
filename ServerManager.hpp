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
		int	makeClient(int port_fd, PortManager& port_manager);

	public:
		~ServerManager();
		static ServerManager&	getServerManager();
		int		initServerManager(const std::vector<std::pair<Server, std::vector<unsigned int> > > configs);
		void	setStatus(int status);
		void	setSendTimeOut(unsigned long send_time_out);
		void	setRecvTimeOut(unsigned long recv_time_out);
		int		getStatus();
		std::vector<Server>&	getServersRef();
		int	processEvent();
};


/*
class	ServerManager
{
	vector<type>		types

	vector<Server>		servers
	vector<Client>		clients
	vector<Resource>	resources

	unsigned long		send_time_out
	unsigned long		recv_time_out

	vector<kevent>		change_events
	kevent				event_list[EVENT_SIZE]
	kevent				current_event
	int					type

	acceptClient(int fd)
	{
		int client_fd = accept()
		client(client_fd, servers[fd])
	}

public
	parseConfig(std::string config_path)
	initWebserv()
	initKqueue()
	enrollServer()
	callKevent()
	reactEvent(int event_num)
	{
		current_event = event_list[event_num]
		type = types[current_event.ident]
		if (error)
			deal_with_it
		if (type == SERVER)
			acceptClient(fd)
		else if (type == CLIENT)
		{
			if (filter == EVFILT_READ)
			{
				clients[fd].readRequest

				status = status_check();
				if (status == BLANK || status == FINISH)

				read(fd, 1024);
				parsing();
				status_mark();
			}
			else if (filter == EVFILT_WRITE)
			{
				client[fd].writeResponse

				status_check();
				write(respond);
			}
		}
		else
		{
			read resource
			clients[fd].processResource()
		}
	}
	clientClose()

}
*/

#endif
