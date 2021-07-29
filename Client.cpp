#include "Client.hpp"

Client::Client(int client_fd, Server& linked_server)
	:client_fd(client_fd), linked_server(linked_server) {
	this->status = nothing;
	// last_request_time = 0;
	// last_response_time = 0;
	this->read_buff = nullptr;
}

Client::Client(const Client &ref)
	:client_fd(ref.client_fd), linked_server(ref.linked_server) {
	*this = ref;
}

Client::~Client() {
}

Client& Client::operator=(const Client &ref) {
	return *this;
}

int	Client::chunkedParser(Request& request) {
	std::string tmp;
	size_t		len;
	size_t		pos_len;
	size_t		pos_contents;

	for(;;) {
		pos_len = this->read_buff.find("\r\n");
		pos_contents = this->read_buff.find("\r\n", pos_len + 2);
		if (pos_len == std::string::npos || pos_contents == std::string::npos)
			break ; // return;
		tmp = this->read_buff.substr(0, pos_len);
		len = atoi(tmp.c_str());
		tmp = this->read_buff.substr(pos_len + 2, pos_contents - pos_len - 2);
		this->read_buff.erase(0, pos_contents + 2);
		if (len == 0)
			break ;
		request.appendBody(tmp);
	}
	if (len == 0)
		request.setStatus(finished);
	return good;
}
int	Client::lengthParser(Request& request) {
	std::string	tmp;
	size_t		len;

	len = atoi((request.getHeader())["Content-Length"].c_str());
	if (this->read_buff.size() >= len) {
		tmp = this->read_buff.substr(0, len);
		this->read_buff.erase(0, len)
		request.appendBody(tmp);
		request.setStatus(finished);
	} else {
		return next_time; // return
	}
	return good;
}

int	Client::headerParser(Request& request) {
	std::string	tmp
	size_t		pos;

	request.setStatus(header);
	if (request.getMethod() == 0) {
		std::stringstream ss(tmp);
		pos = this->read_buff.find("\r\n");
		if (pos == std::string::npos)
			return 0; // 개행이 없음.
		tmp = this->read_buff.substr(0, pos);
		read_buff.erase(0, pos + 2);
		ss >> tmp;
		if (tmp == "GET")
			request.setMethod(GET);
		else if (tmp == "POST")
			request.setMethod(POST);
		else if (tmp == "DELETE")
			request.setMethod(DELETE);
		ss >> tmp;
		request.setUri(tmp);
		ss >> tmp;
		request.setVersion(tmp);
	}
	for (;;) {
		std::string key, value;
		pos = this->read_buff.find("\r\n");
		if (pos == std::string::npos || pos == 0)
			break ;
		tmp = this->read_buff.substr(0, pos);
		read_buff.erase(0, pos + 2);
		pos = tmp.find(":");
		key = tmp.substr(0, pos);
		value = tmp.substr(pos + 2);
		request.insertHeader(key, value);
	}
	if (pos == 0)
		request.setStatus(body);
}

int	Client::Parser(void) {
	Request& request = requests.back();

	if (request.getStatus() == nothing || request.getStatus() == header) {
		this->headerParser(request);
	}
	if (request.getStatus() == body) {
		if (request.getHeader().find("Transfer-Encoding") != request.getHeader().end()
		&& *request.getHeader().find("Transfer-Encoding") == "chunked")
			this->chunkedParser(request);
		else if (request.getHeader().find("Content-Length") != request.getHeader().end())
			this->lengthParser(request);
		else if (request.getMethod() == POST)
			return 411; //Length Required code
	}
	return good;
}


int	Client::readRequest() {
	char	tmp_buff[READSIZE];
	int		read_size = read(client_fd, tmp_buff, READSIZE);

	if (read_size == 0)
		return disconnect; // disconnect
	else if (read_size == -1) {
		std::cerr << "Read failed: " << this->client_fd << " client!!" << std::endl;
		return readFail; // read fail
	}
	tmp_buff[read_size] = '\0';
	this->read_buff += tmp_buff;
	if (this->requests.empty() || this->requests.back()->getStatus() == finished)
		this->requests.push(Request());
	return this->Parser();
}

int	Client::writeResponse() {
	while (this->responses.back().getStatus)
}
