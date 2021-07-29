#include "Client.hpp"

Client::Client(int client_fd, Server& linked_server)
	:client_fd(client_fd), linked_server(linked_server)
{
	this->status = nothing;
	// last_request_time = 0;
	// last_response_time = 0;
	this->read_buff = nullptr;
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

int	Client::chunkedParser(Request* request){
	return good;
}
int	Client::lengthParser(Request* request){
	return good;
}

int	Client::headerParser(Request* request){
	std::string	tmp;
	size_t		pos;

	request->setStatus(header);
	if (request->getMethod() == 0){
		std::stringstream ss(tmp);
		pos = this->read_buff.find("\r\n");
		if (pos == std::string::npos)
			return 0; // 개행이 없음.
		tmp = this->read_buff.substr(0, pos - 1);
		read_buff.erase(0, pos + 1);
		ss >> tmp;
		if (tmp == "GET")
			request->setMethod(GET);
		else if (tmp == "POST")
			request->setMethod(POST);
		else if (tmp == "DELETE")
			request->setMethod(DELETE);
		ss >> tmp;
		request->setUri(tmp);
		ss >> tmp;
		request->setVersion(tmp);
	}
	for (;;){
		std::string key, value;
		pos = this->read_buff.find("\r\n");
		if (pos == std::string::npos || pos == 0)
			break ;
		tmp = this->read_buff.substr(0, pos - 1);
		read_buff.erase(0, pos + 1);
		pos = tmp.find(":");
		key = tmp.substr(0, pos - 1);
		value = tmp.substr(pos + 2);
	}
	if (pos == 0)
		request->setStatus(body);
}

int	Client::Parser(void){
	Request* request = requests.back();

	if (request->getStatus() == nothing || request->getStatus() == header){
		this->headerParser(request);
	}
	if (request->getStatus() == body){
		if (request->getHeader().find("Transfer-Encoding") != request->getHeader().end()
		&& *request->getHeader().find("Transfer-Encoding") == "chunked")
			this->chunkedParser(request);
		else if (request->getHeader().find("Content-Length") != request->getHeader().end())
			this->lengthParser(request);
		else if (request->getMethod() == POST)
			return 411; //Length Required code
	}
	return good;
}


int	Client::readRequest(){
	char	tmp_buff[READSIZE];
	int		read_size = read(client_fd, tmp_buff, READSIZE);

	if (read_size == 0)
		return disconnect; // disconnect
	else if (read_size == -1){
		std::cerr << "Read failed: " << this->client_fd << " client!!" << std::endl;
		return readFail; // read fail
	}
	tmp_buff[read_size] = '\0';
	this->read_buff += tmp_buff;
	if (this->requests.empty() || this->requests.back()->getStatus() == finished)
		this->requests.push(new Request());
	return this->Parser();
}

int	Client::writeResponse(){

}
