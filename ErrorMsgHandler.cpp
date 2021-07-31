#include "ErrorMsgHandler.hpp"

ErrorMsgHandler::ErrorMsgHandler()
	: error_msgs("") {}

ErrorMsgHandler::~ErrorMsgHandler() {}

ErrorMsgHandler&	ErrorMsgHandler::getInstance() {
	static ErrorMsgHandler	err_msg_handler;
	return err_msg_handler;
}

void	ErrorMsgHandler::outError() {
	const char* out_string;
	int			string_len;
	if (this->error_msgs.length() > IO_BUFF_SIZE) {
		string_len = IO_BUFF_SIZE;
		out_string = this->error_msgs.substr(0, IO_BUFF_SIZE).c_str();
		error_msgs = error_msgs.substr(IO_BUFF_SIZE);
	} else {
		string_len = error_msgs.length();
		out_string = this->error_msgs.c_str();
		error_msgs = "";
	}
	send(STDERR, out_string, string_len, 0);
}

void	ErrorMsgHandler::addError(std::string err_msg) {
	this->error_msgs + '\n' + err_msg;
}

void	sendError() {
	ErrorMsgHandler err_msg_handler = ErrorMsgHandler::getInstance();
	err_msg_handler.outError();
}

void	putError(std::string err_msg) {
	ErrorMsgHandler err_msg_handler = ErrorMsgHandler::getInstance();
	err_msg_handler.addError(err_msg);
}
