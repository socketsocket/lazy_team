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
#include <vector>

#include "Webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Resource.hpp"

class ServerManager {
	private:
		std::vector<FdType>		types;
		std::vector<Server>		servers;
		std::vector<Client>		clients;
		std::vector<Resource>	resources;

		unsigned long			send_time_out;
		unsigned long			recv_time_out;

		std::vector<struct kevent>	event_changes;
		struct kevent

	public:
		ServerManager(/* args*/);
		ServerManager(const ServerManager &ref);
		~ServerManager();
		ServerManager& operator=(const ServerManager &ref);
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
