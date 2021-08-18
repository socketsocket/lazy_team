#include "CgiConnector.hpp"
//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------

const char**	CgiConnector::makeEnvp() const {
	const char**	envp = new char*[this->env_map.size() + 1];
	envp[this->env_map.size()] = NULL;
	int i = 0;
	for(std::map<std::string, std::string>::const_iterator it = this->env_map.begin(); \
	it != this->env_map.end(); ++it) {
		envp[i] = strdup((it->first + "=" + it->second).c_str());
		i++;
	}
	return envp;
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

// GET / HTTP/1.1
// Host: localhost:8080
// Connection: keep-alive
// Cache-Control: max-age=0
// sec-ch-ua: "Chromium";v="92", " Not A;Brand";v="99", "Google Chrome";v="92"
// sec-ch-ua-mobile: ?0
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7,ja;q=0.6

// POST /index/dd HTTP/1.1
// user-agent: vscode-restclient
// content-type: application/json
// accept-encoding: gzip, deflate
// content-length: 69
// Host: 127.0.0.1:8080
// Connection: close

// {
//     "name": "sample",
//     "time": "Wed, 21 Oct 2015 18:27:50 GMT"
// }
void	CgiConnector::makeEnvMap(Request* req) {
	// temp_map["AUTH_TYPE"]
	// temp_map["REMOTE_USER"]
	// temp_map["REMOTE_IDENT"]
	if (req->getHeaderValue("Content-Type") != "")
		this->env_map["CONTENT_TYPE"] = req->getHeaderValue("Content-Type");
	this->env_map["CONTENT_LENGTH"];
	this->env_map["GATEWAY_INTERFACE"];
	this->env_map["PATH_INFO"];
	this->env_map["PATH_TRANSLATED"];
	this->env_map["QUERY_STRING"];
	this->env_map["REMOTE_ADDR"];
	this->env_map["REMOTE_HOST"];
	this->env_map["REQUEST_METHOD"];
	this->env_map["REQUEST_URI"];
	this->env_map["REDIRECT_STATUS"];
	this->env_map["SCRIPT_NAME"];
	// this->env_map["SCRIPT_FILENAME"];
	this->env_map["SERVER_NAME"];
	this->env_map["SERVER_PORT"];
	this->env_map["SERVER_PROTOCOL"];
	this->env_map["SERVER_SOFTWARE"];

}

CgiConnector::CgiConnector() {}

CgiConnector::CgiConnector(const CgiConnector& ref): env_map(ref.env_map) {}

CgiConnector::~CgiConnector() {}

CgiConnector&	CgiConnector::operator=(const CgiConnector& ref) {
	this->env_map = ref.env_map;
	return *this;
}
