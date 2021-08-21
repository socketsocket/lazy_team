#include "CgiConnector.hpp"
//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------

// const char**	CgiConnector::makeEnvp() const {
// 	const char**	envp = new char*[this->env_map.size() + 1];
// 	envp[this->env_map.size()] = NULL;
// 	int i = 0;
// 	for(std::map<std::string, std::string>::const_iterator it = this->env_map.begin(); \
// 	it != this->env_map.end(); ++it) {
// 		envp[i] = strdup((it->first + "=" + it->second).c_str());
// 		i++;
// 	}
// 	return envp;
// }

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

ServerStatus	CgiConnector::makeCgiResponse(Re3* re3, Location* loc/*, std::string target*/) {
	int		pipe_fd[2];
	int		ret;
	pid_t	pid;
	Request* req = re3->getReqPtr();


	ret = pipe(pipe_fd);
	write(pipe_fd[1], req->getBody().c_str(), req->getBody().length());
	if (ret < 0)
		return kResponseError;
	pid = fork();
	if (pid < 0) {
		//500 에러 리턴.
		return kResponseError;
	} else if (pid == 0) {
		dup2(pipe_fd[0], STDIN_FILENO);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[0]);
		close(pipe_fd[1]);

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

		char *av[3] = {const_cast<char*>(bin.c_str()), const_cast<char*>(path.c_str()), NULL};

		setenv("AUTH_TYPE", "", 1);
		setenv("CONTENT_TYPE", req->getHeaderValue("content_type").c_str(), 1);
		setenv("CONTENT_LENGTH", req->getHeaderValue("content_length").c_str(), 1);
		setenv("PATH_INFO", path.c_str(), 1);
		setenv("PATH_TRANSLATED", (loc->getRoot() + path).c_str(), 1);
		setenv("SCRIPT_NAME", path.c_str(), 1);
		setenv("SCRIPT_FILENAME", (loc->getRoot() + path).c_str(), 1); // 절대경로.
		setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
		setenv("SERVER_NAME", "passive_server", 1); // NOTE 맞나?
		setenv("SERVER_SOFTWARE", "passive_server/1.1", 1);
		// setenv("SERVER_PORT", ; 포트번호를 어떻게 알죠?
		setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
		setenv("HTTP_ACCEPT", req->getHeaderValue("accept").c_str(), 1);
		setenv("HTTP_ACCEPT_CHARSET", req->getHeaderValue("accept_charset").c_str(), 1);
		setenv("HTTP_ACCEPT_ENCODING", req->getHeaderValue("accept_encoding").c_str(), 1);
		setenv("HTTP_ACCEPT_LANGUAGE", req->getHeaderValue("accept_language").c_str(), 1);
		setenv("HTTP_CONNECTION", req->getHeaderValue("connection").c_str(), 1);
		setenv("HTTP_COOKIE", req->getHeaderValue("cookie").c_str(), 1);
		setenv("HTTP_FORWARDED", req->getHeaderValue("forwarded").c_str(), 1);
		setenv("HTTP_HOST", req->getHeaderValue("host").c_str(), 1);
		setenv("HTTP_PROXY_AUTHORIZATION", req->getHeaderValue("proxy_authorization").c_str(), 1);
		setenv("HTTP_REFERER", req->getHeaderValue("referer").c_str(), 1);
		setenv("HTTP_USER_AGENT", req->getHeaderValue("user_agent").c_str(), 1);
		// setenv("REMOTE_ADDR", "", 1); // ip 주소?
		setenv("REMOTE_HOST", req->getHeaderValue("host").c_str(), 1);;
		setenv("REQUEST_URI", path.c_str(), 1);
		if (req->getMethod() & GET)
			setenv("REQUEST_METHOD", "GET", 1);
		if (req->getMethod() & POST)
			setenv("REQUEST_METHOD", "POST", 1);
		if (req->getMethod() & DELETE)
			setenv("REQUEST_METHOD", "DELETE", 1);
		setenv("REMOTE_USER", "", 1);
		setenv("REMOTE_IDENT", "", 1);
		// setenv("REDIRECT_STATUS"); 200?
		setenv("QUERY_STRING", query_string.c_str(), 1);

		ret = execv(bin.c_str(), av);
		close(STDOUT_FILENO);
		exit(ret);
	} else {
		close(pipe_fd[1]);
		waitpid(-1, NULL, 0); // NOTE 비동기로 해줄 수 있나?
		// pipe_fd[0] read event set;
		re3->getRscPtr()->setResourceFd(pipe_fd[0]);
	}
	return kResourceReadInit;
}

CgiConnector::CgiConnector() {}

CgiConnector::CgiConnector(const CgiConnector& ref): env_map(ref.env_map) {}

CgiConnector::~CgiConnector() {}

CgiConnector&	CgiConnector::operator=(const CgiConnector& ref) {
	this->env_map = ref.env_map;
	return *this;
}
