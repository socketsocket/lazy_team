#include "Client.hpp"

Client::Client(int client_fd, Server& linked_server):client_fd(client_fd)
{
	status = Status::nothing;



}

Client::Client(const Client &ref)
{
	*this = ref;
}

Client::~Client()
{
}

Client& Client::operator=(const Client &ref)
{
	return (*this);
}