#ifndef WEBSERV_WEBSERV_HPP_
#define WEBSERV_WEBSERV_HPP_

#define ERROR -1
#define OK     0

#define BLOCK_END 101

#define OPEN_FILE_ERR "Failed to open file."
#define READ_LINE_ERR "Failed to read a line."
#define NAME_MATCH_ERR "Name is not matching."
#define SEMANTIC_ERR "Wrong semantics"

#define LINE_BUFF_SIZE 1024


enum	FdType {
	server,
	client,
	resource
};

enum	Status{
	nothing,
	working,
	finished
};

#define GET    0b001
#define POST   0b010
#define DELETE 0b100

typedef	unsigned char	method;

#endif
