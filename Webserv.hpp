#ifndef WEBSERV_WEBSERV_HPP_
#define WEBSERV_WEBSERV_HPP_

#include <csignal>

#define ERROR -1
#define OK     0

#define STDERR 2

#define BLOCK_END 101
#define	QUIT 24

#define OPEN_FILE_ERR	"Failed to open file."
#define READ_LINE_ERR	"Failed to read a line."
#define NAME_MATCH_ERR	"Name is not matching."
#define SEMANTIC_ERR	"Wrong semantics."
#define NAME_DUP_ERR	"Token or name is duplicated."
#define NO_ENTITY_ERR	"There is no entity."

#define LINE_BUFF_SIZE 1024
#define	IO_BUFF_SIZE 65536
#define MAX_CLIENT 5

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

typedef const char*	stat_type;

static stat_type	status_code_arr[] = {C100, C101, C200, C201, C202,
	C203, C204, C205, C206, C300, C301, C302, C303, C304, C305, C307, C400,
	C401, C402, C403, C404, C405, C406, C407, C408, C409, C410, C411, C412,
	C413, C414, C415, C416, C417, C500, C501, C502, C503, C504, C505};

#include <map>
#include <string>
static std::map<std::string, stat_type>	status_code_map;

#define RE3 triplet<Request, Response, Resource>

enum	FdType {
	Blank,
	PortFd,
	ClientFd,
	ResourceFd,
	StderrFd
};


enum	Status {
	Nothing,
	Header,
	Body,
	Finished,
};

enum	ReadError {
	Good,
	Disconnect,
	ReadFail,
};

enum	FileType {
	File,
	Directory,
	Notfound
};

#define NOT    0b000
#define GET    0b001
#define POST   0b010
#define DELETE 0b100
#define OTHER  0b1000

typedef	unsigned char	Method;

void	initStatusCodeMap();
void	sigIntHandler(int param);


#endif
