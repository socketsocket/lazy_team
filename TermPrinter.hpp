#ifndef WEBSERV_TERMPRINTER_HPP
#define WEBSERV_TERMPRINTER_HPP

#include "Webserv.hpp"
#include <sys/socket.h>
#include <string>

class TermPrinter
{
	private:
		std::string		err_msgs;
		std::string		out_msgs;

		TermPrinter();
		TermPrinter(const TermPrinter& ref);
		~TermPrinter();
		TermPrinter&	operator=(const TermPrinter& ref);


	public:
		static TermPrinter&	getInstance();

		bool		hasMsgInside(int fd);
		std::string	trimMsg(int fd);
		void		addMsg(std::string msg);
		void		addErrMsg(std::string err_msg);
};

bool		hasMsg(int fd);
std::string	getMsg(int fd);
int			putMsg(std::string msg);
int			putErr(std::string err);

#endif