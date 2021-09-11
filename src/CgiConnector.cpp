#include "CgiConnector.hpp"

static std::string	getIP(int client_fd) {
	struct sockaddr_in	client_addr;
	socklen_t			addr_len = sizeof(struct sockaddr_in);
	char				ip[16];

	getsockname(client_fd, (struct sockaddr *)&client_addr, &addr_len);
	strncpy(ip, inet_ntoa(client_addr.sin_addr), 16);
	return (ip);
}

static char**	setEnvVariable(Re3* re3, const Location* loc, std::string& path, std::string& query_string, unsigned int port_num) {
	Request*							req = re3->getReqPtr();
	std::map<std::string, std::string>	tmp;

	tmp["AUTH_TYPE"] = "";
	tmp["CONTENT_TYPE"] = req->getHeaderValue("content_type");
	tmp["CONTENT_LENGTH"] = req->getHeaderValue("content_length");
	tmp["PATH_TRANSLATED"] = (loc->getRoot() + path.substr(1));
	tmp["PATH_INFO"] = path;
	tmp["SCRIPT_NAME"] = path;
	tmp["SCRIPT_FILENAME"] = loc->getRoot() + path.substr(1);
	tmp["SERVER_PROTOCOL"] = "HTTP/1.1";
	tmp["SERVER_NAME"] = req->getHeaderValue("host");
	tmp["SERVER_SOFTWARE"] = "passive_server/1.1";
	tmp["SERVER_PORT"] = std::to_string(port_num);
	tmp["GATEWAY_INTERFACE"] = "CGI/1.1";
	tmp["HTTP_ACCEPT"] = req->getHeaderValue("accept");
	tmp["HTTP_ACCEPT_CHARSET"] = req->getHeaderValue("accept_charset");
	tmp["HTTP_ACCEPT_ENCODING"] = req->getHeaderValue("accept_encoding");
	tmp["HTTP_ACCEPT_LANGUAGE"] = req->getHeaderValue("accept_language");
	tmp["HTTP_CONNECTION"] = req->getHeaderValue("connection");
	tmp["HTTP_COOKIE"] = req->getHeaderValue("cookie");
	tmp["HTTP_FORWARDED"] = req->getHeaderValue("forwarded");
	tmp["HTTP_HOST"] = req->getHeaderValue("host");
	tmp["HTTP_PROXY_AUTHORIZATION"] = req->getHeaderValue("proxy_authorization");
	tmp["HTTP_REFERER"] = req->getHeaderValue("referer");
	tmp["HTTP_USER_AGENT"] = req->getHeaderValue("user_agent");
	tmp["REMOTE_ADDR"] = getIP(re3->getClientId());
	tmp["REMOTE_HOST"] = req->getHeaderValue("host");
	tmp["REQUEST_URI"] = req->getUri();
	if (req->getMethod() & GET)
		tmp["REQUEST_METHOD"] = "GET";
	if (req->getMethod() & POST)
		tmp["REQUEST_METHOD"] = "POST";
	if (req->getMethod() & DELETE)
		tmp["REQUEST_METHOD"] = "DELETE";
	tmp["REMOTE_USER"] = "";
	tmp["REMOTE_IDENT"] = "";
	tmp["QUERY_STRING"] = query_string;

	char** envp = static_cast<char**>(new char*[sizeof(char*) * (tmp.size() + 1)]);

	if (envp == NULL)
		return NULL;
	int i = 0;
	for(std::map<std::string, std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		envp[i] = strdup((it->first + "=" + it->second).c_str());
		++i;
	}
	envp[i] = NULL;
	return envp;
}

ServerStatus	CgiConnector::prepareResource(Re3* re3, const Location* loc, unsigned int port_num, std::string root) {
	Request*	req = re3->getReqPtr();
	Resource*	rsc = re3->getRscPtr();
	int			ret1, ret2;
	pid_t		pid;
	int			read_fd[2];
	int			write_fd[2];

	ret1 = pipe(read_fd);
	ret2 = pipe(write_fd);
	if (ret1 < 0 || ret2 < 0)
		return kResponseError;
	pid = fork();
	rsc->setPid(pid);
	if (pid < 0) {
		return kResponseError;
	} else if (pid == 0) {

		size_t	idx = req->getUri().find("?");
		std::string query_string, path, bin, extension;

		if (idx != std::string::npos) {
			query_string = req->getUri().substr(idx + 1);
			path = req->getUri().substr(0, idx);
		} else {
			query_string = "";
			path = req->getUri();
		}
		extension = path.substr(path.find(".") + 1);
		bin = loc->getCgiBinary(extension);
		root = root + path.substr(1);
		char *av[3] = {const_cast<char*>(bin.c_str()), const_cast<char*>(root.c_str()), NULL};
		char** envp = setEnvVariable(re3, loc, path, query_string, port_num);
		dup2(write_fd[0], STDIN_FILENO);
		dup2(read_fd[1], STDOUT_FILENO);
		close(write_fd[0]);
		close(write_fd[1]);
		close(read_fd[0]);
		close(read_fd[1]);
		if (envp != NULL)
			ret1 = execve(av[0], av, envp);
		else
			exit(1);
		exit(ret1);
	} else {
		close(write_fd[0]);
		close(read_fd[1]);
		rsc->setReadFd(read_fd[0]);
		rsc->setWriteFd(write_fd[1]);
		if (req->getMethod() == POST) {
			rsc->setResourceFd(write_fd[1]);
			rsc->setStatus(kWriting);
			return kResourceWriteInit;
		} else {
			close(write_fd[1]);
			return kProcessWaiting;
		}
	}
}

// ServerStatus	CgiConnector::waitCgi(Resource* rsc) {
// 	int		statloc;
// 	pid_t	ret = waitpid(rsc->getPid(), &statloc, WNOHANG);

// 	if (ret == 0)
// 		return kProcessWaiting;
// 	if (ret == -1)
// 		return kResponseError;
// 	if (WIFEXITED(statloc) && WEXITSTATUS(statloc))
// 		return kResponseError;
// 	if (WIFSIGNALED(statloc))
// 		return kResponseError;
// 	rsc->setResourceFd(rsc->getReadFd());
// 	rsc->setStatus(kReading);
// 	return kResourceReadInit;
// }

ServerStatus	CgiConnector::prepareResponse(Re3* re3) {
	Request*	req = re3->getReqPtr();
	Resource*	rsc = re3->getRscPtr();
	std::string&	buff = rsc->getContent();
	std::string							http_status = C200;
	std::map<std::string, std::string>	headers;
	std::string							body;
	size_t	idx1, idx2;

	close(rsc->getResourceFd());
	if ((idx1 = buff.find("Status: ")) != std::string::npos) {
		idx2 = buff.find("\r\n");
		http_status = buff.substr(idx1 + 8, idx2 - (idx1 + 8));
		buff.erase(idx1, idx2 + 2);
	}
	if ((idx1 = buff.find("Content-Type: ")) != std::string::npos) {
		idx2 = buff.find("\r\n");
		headers["Content-Type"] = buff.substr(idx1 + 14, idx2 - (idx1 + 14));
		buff.erase(idx1, idx2 + 2);
	}
	if ((idx1 = buff.find("Location: ")) != std::string::npos) {
		idx2 = buff.find("\r\n");
		headers["Content-Type"] = buff.substr(idx1 + 10, idx2 - (idx1 + 10));
		buff.erase(idx1, idx2 + 2);
	}
	while ((idx1 = buff.find("\r\n")) != std::string::npos)
		buff.erase(0, idx1 + 2);
	headers["Content-Language"] = "ko-KR";
	headers["Content-Length"] = std::to_string(buff.size());
	re3->setRspPtr(new Response(kFinished, http_status, headers, buff, req->getVersion()));
	return kResponseMakingDone;
}

ServerStatus	CgiConnector::makeCgiResponse(Re3* re3, const Location* loc, unsigned int port_num, std::string root) {
	Resource*	rsc = re3->getRscPtr();

	if (rsc->getStatus() == kNothing)
		return prepareResource(re3, loc, port_num, root);
	if (rsc->getStatus() == kWriteDone)
		return kProcessWaiting;
	if (rsc->getStatus() == kReadDone)
		return prepareResponse(re3);
	return kProcessWaiting;
}

CgiConnector::CgiConnector(const CgiConnector& ref) {
	*this = ref;
}

CgiConnector&	CgiConnector::operator=(const CgiConnector& ref) {
	(void)ref;
	return *this;
}

CgiConnector::CgiConnector() {}
CgiConnector::~CgiConnector() {}
