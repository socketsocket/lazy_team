#ifndef WEBSERV_ERRORMSGHANDLER_HPP
#define WEBSERV_ERRORMSGHANDLER_HPP

#include "Webserv.hpp"
#include <sys/socket.h>
#include <string>

class ErrorMsgHandler
{
	private:
		std::string		error_msgs;

		ErrorMsgHandler();
		ErrorMsgHandler(const ErrorMsgHandler& ref);
		~ErrorMsgHandler();
		ErrorMsgHandler&	operator=(const ErrorMsgHandler& ref);

		static ErrorMsgHandler&	getInstance();
		void	outError();
		void	addError(std::string err_msg);


	public:
		friend void				sendError();
		friend void				putError(std::string err_msg);
};


#endif