#ifndef WEBSERV_WEBSERV_HPP_
#define WEBSERV_WEBSERV_HPP_

#define ERROR -1
#define OK     0

#define C100 "100 Continue"
#define C101 "101 Switching Protocols"
#define C200 "200 OK"
#define C201 "201 Created"
#define C202 "202 Accepted"
#define C203 "203 Non-Authoritative Information"
#define C204 "204 No Content"
#define C205 "205 Reset Content"
#define C206 "206 Partial Content"
#define C300 "300 Multiple Choices"
#define C301 "301 Moved Permanently"
#define C302 "302 Found"
#define C303 "303 See Other"
#define C304 "304 Not Modified"
#define C305 "305 Use Proxy"
#define C307 "307 Temporary Redirect"
#define C400 "400 Bad Request"
#define C401 "401 Unauthorized"
#define C402 "402 Payment Required"
#define C403 "403 Forbidden"
#define C404 "404 Not Found"
#define C405 "405 Method Not Allowed"
#define C406 "406 Not Accceptable"
#define C407 "407 Proxy Authentication Required"
#define C408 "408 Request Time-out"
#define C409 "409 Conflict"
#define C410 "410 Gone"
#define C411 "411 Length Required"
#define C412 "412 Precondition Failed"
#define C413 "413 Request Entity Too Large"
#define C414 "414 Request-URI Too Large"
#define C415 "415 Unsupported Media Type"
#define C416 "416 Requested range not satisfiable"
#define C417 "417 Exception Failed"
#define C500 "500 Internal Server Error"
#define C501 "501 Not Implemented"
#define C502 "502 Bad Gateway"
#define C503 "503 Service Unavailable"
#define C504 "504 Gateway Time-out"
#define C505 "505 HTTP Version not supported"

enum	FdType {
	server,
	client,
	resource
};

enum	Status {
	Nothing,
	Header,
	Body,
	Reading, 
	Finished,
	Error
};

enum	FileType{
	File,
	Directory,
	NotFound
};

#define GET    0b001
#define POST   0b010
#define DELETE 0b100

typedef	unsigned char	method;
static std::map<std::string, const char*>   status_code_map;

#endif
