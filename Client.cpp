#include "Client.hpp"

Client::Client(int client_fd, PortManager& port_manager)
	: status(0),
	client_fd(client_fd),
	port_manager(port_manager),
	last_request_time(0),
	last_response_time(0),
	read_buff("") {
	this->re3_deque.push_back(Re3());
	this->re3_deque.back().setReqPtr(new Request);
}

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

Client::~Client() {

}

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
			return ERROR; // parsing이 꼬임. ㅈ댐
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
	std::istringstream iss(request->getHeaderValue("Content-Length"));

	iss >> len;
	if (this->read_buff.size() >= len) {
		tmp = this->read_buff.substr(0, len);
		this->read_buff.erase(0, len);
		request->appendBody(tmp);
		request->setStatus(kFinished);
	}
	return OK;
}

int	Client::reqLineParser(Request* request) {
	size_t				pos;
	std::string			tmp;
	std::stringstream	ss;

	pos = this->read_buff.find("\r\n");
	if (pos == std::string::npos)
		return OK; // 개행이 없음. 버퍼가 중간에 끊김.
	tmp = this->read_buff.substr(0, pos);
	read_buff.erase(0, pos + 2);
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
	request->setUri(tmp);
	ss >> tmp;
	request->setVersion(tmp);
	request->setStatus(kHeader);
	return OK;
}

int	Client::headerParser(Request* request) {
	std::string tmp, key, value;
	size_t		pos;

	pos = this->read_buff.find("\r\n");
	while (pos != std::string::npos && pos != 0) {
		tmp = this->read_buff.substr(0, pos);
		read_buff.erase(0, pos + 2);
		pos = tmp.find(":");
		key = tmp.substr(0, pos);
		value = tmp.substr(pos + 2);
		request->insertHeader(key, value);
		pos = this->read_buff.find("\r\n");
	}
	if (pos == 0)
		request->setStatus(kBody);
	return OK;
}

int	Client::initParser(Request* request) {
	if (request->getStatus() == kNothing)
		this->reqLineParser(request);
	if (request->getStatus() == kHeader)
		this->headerParser(request);
	if (request->getStatus() == kBody) {
		if (request->getHeaderValue("Transfer-Encoding").find("chunked") != std::string::npos)
			return this->chunkedParser(request); // chunked error인 경우에 servermanager까지 전해줘서 연결 닫도록 해야돼요
		if (request->getHeaderValue("Content-Length") != "")
			return this->lengthParser(request);
		if (request->getMethod() == POST) {
			this->re3_deque.back().getRscPtr()->setStatus(411); // 411 error를 반환해야함.
			// response http_status_code 를 바꿔놓을지?
			this->re3_deque.back().getReqPtr()->setStatus(kFinished);
		}
			// can be changed.
	}
	return OK;
}

// std::vector<Re3*>	Client::rscToEnroll(void) {
// 	std::vector<Re3*> ret;
// 	for (Re3* it = re3_deque.begin(); it != re3_deque.end(); ++it) {
// 		//to be enroll
// 		if (it->getRscPtr()->getStatus() == to_be_enroll)
// 			ret.push_back(it);
// 	}
// 	return ret;
// }

// std::vector<Re3*>	Client::recvRequest(std::string& rawRequest) {
int	Client::recvRequest(std::string rawRequest) {
	this->read_buff += rawRequest;
	std::vector<Re3*>	RscClaim(0);
	do {
		if (this->re3_deque.back().getReqPtr()->getStatus() == kFinished) {
			this->port_manager.passRequest(&this->re3_deque.back());
			this->re3_deque.push_back(Re3());
			this->re3_deque.back().setReqPtr(new Request);
		}
		this->initParser(this->re3_deque.back().getReqPtr());
	} while (this->re3_deque.back().getReqPtr()->getStatus() == kFinished);
	// return this->rscToEnroll();
	return OK;
}

std::string	Client::passResponse() {
	size_t	network_buff_left = NETWORK_BUFF;
	std::string to_be_sent;
	Response*	response;

	while (network_buff_left && this->re3_deque.front().getRspPtr()->getStatus() == kFinished) {
		Re3* ptr = &this->re3_deque.front();
		response = ptr->getRspPtr();
		response->makeHead();
		if (response->getHead().size() > 0) {
			if (network_buff_left >= response->getHead().size()) {
				network_buff_left -= response->getHead().size();
				to_be_sent += response->getHead();
				response->getHead().erase(0);
			} else {
				to_be_sent += response->getHead().substr(0, network_buff_left);
				response->getHead().erase(0, network_buff_left);
				network_buff_left = 0;
			}
		} else if (response->getBody().size() > 0) {
			if (network_buff_left >= response->getBody().size()) {
				network_buff_left -= response->getBody().size();
				to_be_sent += response->getHead();
				response->getHead().erase(0);
			} else {
				to_be_sent += response->getBody().substr(0, network_buff_left);
				response->getBody().erase(0, network_buff_left);
				network_buff_left = 0;
			}
		} else {
			this->re3_deque.pop_front();
		}
	}
	return to_be_sent;
}
