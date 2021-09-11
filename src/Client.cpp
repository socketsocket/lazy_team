#include "Client.hpp"

Client::Client(int client_fd, PortManager& port_manager)
	: status(kNothing),
	client_fd(client_fd),
	port_manager(port_manager),
	last_request_time(0),
	last_response_time(0),
	read_buff("") { }

Client::Client(const Client& ref)
	: status(ref.status),
	client_fd(ref.client_fd),
	port_manager(ref.port_manager),
	last_request_time(ref.last_request_time),
	last_response_time(ref.last_response_time),
	read_buff(ref.read_buff),
	re3_deque(ref.re3_deque) {
	*this = ref;
}

Client::~Client() {}

Client& Client::operator=(const Client& ref) {
	// const clinet_fd, refence port_manager
	this->status = ref.status;
	this->last_request_time = ref.last_request_time;
	this->last_response_time = ref.last_response_time;
	this->read_buff = ref.read_buff;
	this->re3_deque = ref.re3_deque;
	return *this;
}

int	Client::chunkedParser(Request* request) {
	std::string tmp;
	size_t		len_end, contents_end;
	size_t		len = 1;
	std::istringstream iss;

	while (len) {
		len_end = this->read_buff.find("\r\n");
		contents_end = this->read_buff.find("\r\n", len_end + 2);
		if (len_end == std::string::npos || contents_end == std::string::npos)
			break;
		tmp = this->read_buff.substr(0, len_end);
		if (tmp.find_first_of("0123456789ABCDEFabcdef") != std::string::npos) {
			putErr("An invalid chunked body received");
			request->setStatus(kReadFail);
			this->read_buff.erase(0);
			return ERROR;
		}
		iss.str(tmp);
		iss >> std::hex >> len;
		tmp = this->read_buff.substr(len_end + 2, len);
		this->read_buff.erase(0, contents_end + 2);
		request->appendBody(tmp);
	}
	if (len == 0)
		request->setStatus(kFinished);
	return OK;
}

int	Client::lengthParser(Request* request) {
	std::string	tmp;
	size_t		len;
	std::istringstream iss(request->getHeaderValue("content-length"));

	iss >> len;
	if (this->read_buff.length() >= len) {
		tmp = this->read_buff.substr(0, len);
		this->read_buff.erase(0, len);
		request->appendBody(tmp);
		request->setStatus(kFinished);
	}
	return OK;
}

int	Client::headerParser(Request* request) {
	std::string tmp, key, value;
	size_t		pos;

	pos = this->read_buff.find("\r\n");
	while (pos != std::string::npos && pos != 0) {
		tmp = this->read_buff.substr(0, pos);
		this->read_buff.erase(0, pos + 2);
		pos = tmp.find(":");
		key = tmp.substr(0, pos);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		value = tmp.substr(pos + 2);
		request->insertHeader(key, value);
		pos = this->read_buff.find("\r\n");
	}
	if (pos == 0) {
		request->setStatus(kBody);
		this->read_buff.erase(pos, pos + 2);
	}
	return OK;
}

int	Client::reqLineParser(Request* request) {
	size_t				pos;
	std::string			tmp;
	std::stringstream	ss;

	pos = this->read_buff.find("\r\n");
	if (pos == std::string::npos || pos == 0)
		return ERROR;
	tmp = this->read_buff.substr(0, pos);
	this->read_buff.erase(0, pos + 2);
	ss.str(tmp);
	ss >> tmp;
	if (tmp == "GET")
		request->setMethod(GET);
	else if (tmp == "POST")
		request->setMethod(POST);
	else if (tmp == "DELETE")
		request->setMethod(DELETE);
	else
		request->setMethod(OTHER);
	ss >> tmp;
	if (tmp.find("://") != std::string::npos) {
		tmp = tmp.substr(tmp.find("://") + 3);
		tmp = tmp.substr(tmp.find("/"));
	}
	request->setUri(tmp);
	ss >> tmp;
	request->setVersion(tmp);
	request->setStatus(kHeader);
	return OK;
}

int	Client::initParser(Request* request) {
	if (!this->read_buff.length())
		return OK;
	if (request->getStatus() == kNothing)
		if (this->reqLineParser(request) == ERROR)
			return ERROR;
	if (request->getStatus() == kHeader)
		this->headerParser(request);
	if (request->getStatus() == kBody) {
		if (request->getHeaderValue("transfer-encoding").find("chunked") != std::string::npos)
			return this->chunkedParser(request); // chunked error인 경우에 servermanager까지 전해줘서 연결 닫도록 해야돼요
		if (request->getHeaderValue("content-length") != "")
			return this->lengthParser(request);
		if (request->getMethod() == POST) {
			this->re3_deque.back().getReqPtr()->setStatus(kLengthReq);
			return ERROR;
		}
		else {
			this->re3_deque.back().getReqPtr()->setStatus(kFinished);
		}
	}
	return OK;
}

std::vector<std::pair<Re3*, ServerStatus> >	Client::recvRequest(std::string rawRequest) {
	std::vector<std::pair<Re3*, ServerStatus> >	rsc_claim(0);
	ServerStatus	server_response;

	if (this->re3_deque.empty()) {
		this->re3_deque.push_back(Re3(client_fd));
		this->re3_deque.back().setReqPtr(new Request);
	}
	if (!rawRequest.length())
		return rsc_claim;
	this->read_buff += rawRequest;
	do {
		if (this->re3_deque.back().getReqPtr()->getStatus() == kFinished) {
			server_response = this->port_manager.passRequest(&this->re3_deque.back());
			rsc_claim.push_back(std::make_pair(&re3_deque.back(), server_response));
			this->re3_deque.push_back(Re3(this->client_fd));
			this->re3_deque.back().setReqPtr(new Request);
		}
		if (ERROR == this->initParser(this->re3_deque.back().getReqPtr())) {
			server_response = this->port_manager.passRequest(&this->re3_deque.back());
			std::vector<std::pair<Re3*, ServerStatus> >	request_error(1, std::make_pair(&re3_deque.back(), server_response));
			return request_error;
		}
	} while (this->re3_deque.back().getReqPtr()->getStatus() == kFinished);
	return rsc_claim;
}

void Client::putRspIntoBuff(size_t& network_buff_left, std::string& to_be_sent, std::string& data) {
	if (network_buff_left >= data.length()) {
		network_buff_left -= data.length();
		to_be_sent += data;
		data.erase(0);
	} else {
		to_be_sent += data.substr(0, network_buff_left);
		data.erase(0, network_buff_left);
		network_buff_left = 0;
	}
}

std::string	Client::passResponse() {
	size_t		network_buff_left = NETWORK_BUFF;
	std::string	to_be_sent;
	Response*	response = this->re3_deque.front().getRspPtr();

	assert(this->re3_deque.size());
	assert(response != NULL);
	while (network_buff_left && this->re3_deque.size() \
		&& response->getStatus() == kFinished) {
		if (response->getHead().length() > 0) {
			this->putRspIntoBuff(network_buff_left, to_be_sent, response->getHead());
		} else if (response->getBody().length() > 0) {
			this->putRspIntoBuff(network_buff_left, to_be_sent, response->getBody());
		} else {
			this->re3_deque.pop_front();
			if (this->re3_deque.size() && this->re3_deque.front().getRspPtr()) {
				response = this->re3_deque.front().getRspPtr();
			} else {
				break;
			}
		}
	}
	return to_be_sent;
}

int	Client::getClientFd() const {
	return this->client_fd;
}
