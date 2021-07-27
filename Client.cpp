#include "Client.hpp"

Client::Client(int client_fd)
{
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