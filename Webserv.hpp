#ifndef WEBSERV_WEBSERV_HPP_
#define WEBSERV_WEBSERV_HPP_

#include <csignal>
#include <climits>

#define CHUNK_ERR -22
#define ERROR -1
#define OK     0
#define END_OF_FILE 26

#define STDOUT 1
#define STDERR 2

#define BLOCK_END 101
#define	INTR 24
#define INITIATED 48

#define OPEN_FILE_ERR	"Failed to open file."
#define READ_LINE_ERR	"Failed to read a line."
#define NAME_MATCH_ERR	"Name is not matching."
#define SEMANTIC_ERR	"Wrong semantics."
#define NAME_DUP_ERR	"Token or name is duplicated."
#define NO_ENTITY_ERR	"There is no entity."

#define LINE_BUFF 1024
#define	NETWORK_BUFF 16384
#define LOCAL_BUFF 8192
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

extern stat_type	stat_code_arr[40];

#define DEFAULT_ROUTE "default.config"

#include <map>
#include <string>
extern std::map<std::string, stat_type>	stat_code_map;

#define RE3 triplet<Request, Response, Resource>

enum	FdType {
	kBlank,
	kPortFd,
	kClientFd,
	kResourceFd,
	kStdOutErrFd,
	kCgiInput,
};

// Request, Response, Resource의 Status.
enum	Status {
	kNothing, // Request, Resource
	kHeader, // Request
	kBody, // Request
	kReading, // Resource
	kWriting, // Resource
	kReadDone, // NOTE newly add
	kWriteDone, // NOTE newly add
	kFinished, // Request, Resource
	kDisconnect,
	kReadFail, // Resource, Request
	kWriteFail,
	kLengthReq,
};

//Server가 리턴하는 값들
enum	ServerStatus {
	kResourceWriteInit,
	kResourceReadInit,
	kResourceWriteWaiting,
	kResourceReadWaiting,
	kResponseMakingDone,
	kResponseError,
};


enum	FileType {
	kFile,
	kDirectory,
	kNotFound,
};

#define NOT    0b000
#define GET    0b001
#define POST   0b010
#define DELETE 0b100
#define OTHER  0b1000

typedef	unsigned char	Method;

void	initStatCodeMap();
void	sigIntHandler(int param);


#endif
