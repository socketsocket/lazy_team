#include "Client.hpp"

Client::Client(int client_fd, Server& linked_server)
	:client_fd(client_fd), linked_server(linked_server)
{
	this->state = nothing;
	// last_request_time = 0;
	// last_response_time = 0;
	this->buff_size = 0;
	this->read_buff = "";
}

Client::Client(const Client &ref)
	:client_fd(ref.client_fd), linked_server(ref.linked_server)
{
	*this = ref;
}

Client::~Client()
{
}

Client& Client::operator=(const Client &ref)
{
	return *this;
}

int	Client::chunkedParser(Request& request);
int	Client::lengthParser(Request& request);
int	Client::headerParser(Request& request){
	this->state = header;
	if (!request.getMethod())
	if (!request.getUri())
	if (!request.getVersion())
	if (!request.getHeader())
	if (!request.getBody())
}

int	Client::Parser(void){
	Request& request = requests.back();

	if (request.state == nothing || request.state == header){
		this->headerParser(request);
	}
	if (request.state == body){
		if (request.header.find("Transfer-Encoding") != request.header.end())
			this->chunkedParser(request);
		else if (request.header.find("Content-Length") != request.header.end())
			this->lengthParser(request);
		else if (this->getMethod() == POST)
			return 411; //Length Required code
	}
	return good;
}


int	Client::readRequest(){
	char	tmp_buff[READSIZE];
	int		read_size = read(client_fd, tmp_buff, READSIZE);

	if (this->read_size == 0)
		return disconnect; // disconnect
	else if (this->read_size == -1){
		std::cerr << "Read failed " << this->client_fd << " client!!" << std::endl;
		return readFail; // read fail
	}
	tmp_buff[read_size] = '\0';
	this->read_buff << tmp_buff;
	if (this->requests.empty() || this->requests.back().state == finished)
		this->requests.push(new Request());
	return this->Parser();
}

int	Client::writeResponse(){

}
