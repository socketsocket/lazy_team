#include "CgiConnector.hpp"
#include <arpa/inet.h>

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
	tmp["PATH_TRANSLATED"] = (loc->getRoot() + path);
	tmp["PATH_INFO"] = path;
	tmp["SCRIPT_NAME"] = path;
	tmp["SCRIPT_FILENAME"] = loc->getRoot() + path;
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

	char** envp = static_cast<char**>(malloc(sizeof(char*) * tmp.size()));

	if (envp == NULL)
		return NULL;
	int i = 0;
	for(std::map<std::string, std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		envp[i] = strdup((it->first + "=" + it->second).c_str());
		++i;
	}
	return envp;
}

ServerStatus	CgiConnector::prepareResource(Re3* re3, const Location* loc, unsigned int port_num) {
	Request*	req = re3->getReqPtr();
	Resource*	rsc = re3->getRscPtr();
	int		ret;

	ret = pipe(this->pipe_fd);
	if (ret < 0)
		return kResponseError;
	this->pid = fork();
	if (this->pid < 0) {
		return kResponseError;
	} else if (this->pid == 0) {
		dup2(this->pipe_fd[0], STDIN_FILENO);
		dup2(this->pipe_fd[1], STDOUT_FILENO);
		close(this->pipe_fd[0]);
		close(this->pipe_fd[1]);

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
		char** envp = setEnvVariable(re3, loc, path, query_string, port_num);
		if (envp != NULL)
			ret = execve(bin.c_str(), av, envp);
		else
			exit(1);
		close(STDOUT_FILENO);
		exit(ret);
	} else {
		rsc->setResourceFd(this->pipe_fd[1]);
		rsc->setStatus(kWriting);
	}
	return kResourceWriteInit;
}

ServerStatus	CgiConnector::waitCgi(Resource* rsc) {
	int		statloc;
	pid_t	ret = waitpid(this->pid, &statloc, WNOHANG);

	if (ret == 0)
		return kWaiting;
	close(this->pipe_fd[1]);
	if (ret == -1)
		return kResponseError;
	if (WIFEXITED(statloc) && WEXITSTATUS(statloc))
		return kResponseError;
	if (WIFSIGNALED(statloc))
		return kResponseError;
	rsc->setResourceFd(pipe_fd[0]);
	rsc->setStatus(kReading);
	return kResourceReadInit;
}

ServerStatus	CgiConnector::prepareResponse(Re3* re3) {
	Request*	req = re3->getReqPtr();
	Resource*	rsc = re3->getRscPtr();
	std::map<std::string, std::string> headers;


	close(rsc->getResourceFd());
//	TODO cgi 응답 만들기
	// re3->setRspPtr(new Response(kFinished, std::string(C200), headers, "", request->getVersion()));
	return kResponseMakingDone;
}

ServerStatus	CgiConnector::makeCgiResponse(Re3* re3, const Location* loc, unsigned int port_num) {
	Resource*	rsc = re3->getRscPtr();

	if (rsc->getStatus() == kNothing)
		return prepareResource(re3, loc, port_num);
	if (rsc->getStatus() == kWriteDone)
		return waitCgi(rsc);
	if (rsc->getStatus() == kReadDone)
		return prepareResponse(re3);
}

CgiConnector::CgiConnector(const CgiConnector& ref): env_map(ref.env_map) {
	*this = ref;
}

CgiConnector&	CgiConnector::operator=(const CgiConnector& ref) {
	this->env_map = ref.env_map;
	return *this;
}
