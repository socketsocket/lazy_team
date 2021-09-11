#include "TermPrinter.hpp"

TermPrinter::TermPrinter()
	: err_msgs(""), out_msgs("") {}

TermPrinter::~TermPrinter() {}

TermPrinter&	TermPrinter::getInstance() {
	static TermPrinter	term_printer;
	return term_printer;
}

bool TermPrinter::hasMsgInside(int fd) {
	if (fd == STDOUT)
		return !!this->out_msgs.length();
	return !!this->err_msgs.length();
}


std::string	TermPrinter::trimMsg(int fd) {
	assert(fd == STDERR || fd == STDOUT);
	std::string&	msg = (fd == STDERR) ? this->err_msgs : this->out_msgs;

	std::string out_string;
	if (msg.length() > BC_STRING_MAX) {
		out_string = msg.substr(0, BC_STRING_MAX);
		msg.erase(0, BC_STRING_MAX);
	} else {
		out_string = msg;
		msg = "";
	}
	return out_string;
}

void	TermPrinter::addMsg(std::string msg) {
	this->out_msgs += msg;
}

void	TermPrinter::addErrMsg(std::string err_msg) {
	this->err_msgs += err_msg;
}

bool	hasMsg(int fd) {
	TermPrinter& term_printer = TermPrinter::getInstance();
	return term_printer.hasMsgInside(fd);
}

std::string	getMsg(int fd) {
	TermPrinter& term_printer = TermPrinter::getInstance();
	return term_printer.trimMsg(fd);
}

int	putMsg(std::string msg) {
	TermPrinter& term_printer = TermPrinter::getInstance();
	term_printer.addMsg(msg);
	return OK;
}

int putErr(std::string err) {
	TermPrinter& term_printer = TermPrinter::getInstance();
	term_printer.addErrMsg(err);
	return ERROR;
}