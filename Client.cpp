#include "Client.hpp"

Client::Client(int client_fd, PortManager& port_manager)
	: client_fd(client_fd), port_manager(port_manager) {
	this->status = Nothing;
	// last_request_time = 0;
	// last_response_time = 0;
	this->read_buff = "";
}

Client::Client(const Client& ref)
	: client_fd(ref.client_fd), port_manager(ref.port_manager) {
	*this = ref;
}

Client::~Client() {
}

int	Client::chunkedParser(Request& request) {
	std::string tmp;
	size_t		len;
	size_t		len_end;
	size_t		contents_end;

	for(;;) {
		len_end = this->read_buff.find("\r\n");
		contents_end = this->read_buff.find("\r\n", len_end + 2);
		if (len_end == std::string::npos || contents_end == std::string::npos)
			break ; // return;
		tmp = this->read_buff.substr(0, len_end);
		len = strtol(tmp.c_str(), NULL, 16);
		tmp = this->read_buff.substr(len_end + 2, contents_end - len_end - 2);
		this->read_buff.erase(0, contents_end + 2);
		if (len == 0)
			break ;
		request.appendBody(tmp);
	}
	if (len == 0) {
		request.setStatus(Finished);
		return 1; //one_more_times;
	}
	return OK;
}

int	Client::lengthParser(Request& request) {
	std::string	tmp;
	size_t		len;

	len = atoi(request.getHeader()["Content-Length"].c_str());
	if (this->read_buff.size() >= len) {
		tmp = this->read_buff.substr(0, len);
		this->read_buff.erase(0, len);
		request.appendBody(tmp);
		request.setStatus(Finished);
		return 1; // one_more_times;
	} else {
		return OK;//next_time; // return
	}
	return OK;
}

int	Client::headerParser(Request& request) {
	std::string	tmp;
	size_t		pos;

	request.setStatus(Header);
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
		request.setStatus(Body);
}

int	Client::Parser(void) {
	if (this->requests.empty() || this->requests.back().getStatus() == Finished)
		this->requests.push(Request());
	Request& request = requests.back();
	if (request.getStatus() == Nothing || request.getStatus() == Header)
		this->headerParser(request);
	if (request.getStatus() == Body) {
		if (request.getHeader().find("Transfer-Encoding") != request.getHeader().end()
		&& request.getHeader()["Transfer-Encoding"] == "chunked")
			return (this->chunkedParser(request));
		else if (request.getHeader().find("Content-Length") != request.getHeader().end())
			return (this->lengthParser(request));
		else if (request.getMethod() == POST)
			return 411; //Length Required code
	}
	return OK;
}

int	Client::readRequest() {
	char	tmp_buff[READSIZE];
	int		read_size = read(client_fd, tmp_buff, READSIZE);
	int		one_more_times;

	if (read_size == 0)
		return ERROR; // disconnect
	else if (read_size == -1) {
		std::cerr << "Read failed: " << this->client_fd << " client!!" << std::endl;
		return ERROR; // read fail
	}
	tmp_buff[read_size] = '\0';
	this->read_buff += tmp_buff;
	while (this->read_buff.size() && one_more_times)
		one_more_times = this->Parser();
	return OK;
}

int	Client::writeResponse() {
	// multiflex을 위해서 쪼개서 써주기 고민.
	while (!this->responses.empty()\
	&& this->responses.front().getStatus() == Finished) {
		std::string tmp = this->responses.front().getResponseMessage();
		//write error check
		write(client_fd, tmp.c_str(), tmp.size());
		this->responses.pop();
	}
}
