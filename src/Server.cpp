#include "Server.hpp"

std::map<std::string, std::string>	Server::mime_types;

bool Server::handleCGI(Request* request, const Location* location) const
{
	std::string	uri = request->getUri();
	const std::map<std::string, std::string>& cgi_infos = location->getCgiInfos();

	for (std::map<std::string, std::string>::const_iterator it = cgi_infos.begin(); it != cgi_infos.end(); ++it)
		if (uri.find(it->first) != std::string::npos)
			return true;
	return false;
}

ServerStatus Server::makeResponse(Re3* re3) const {
	Request*	request = re3->getReqPtr();
	Resource*	resource = re3->getRscPtr();
	const Status req_status = request->getStatus();
	const Status rsc_status = resource->getStatus();
	const Location* curr_location = this->currLocation(request->getUri());
	if (this->return_to.second.length() || curr_location->getReturnTo().second.length()) {
		std::pair<stat_type, std::string>	return_to = this->return_to.second.length() ? this->return_to : curr_location->getReturnTo();
		return this->makeRedirectResponse(re3, return_to);
	}

	if (request->getHeaderValue("host") == "" || req_status == kNothing)
		return this->makeErrorResponse(re3, curr_location, C400);
	stat_type stat = this->requestValidCheck(request, curr_location);
	if (req_status == kLengthReq)
		return this->makeErrorResponse(re3, curr_location, C411);
	if (std::string(stat).compare(C200))
		return this->makeErrorResponse(re3, curr_location, stat);
	if (req_status == kReadFail || rsc_status == kDisconnect || rsc_status == kReadFail || rsc_status == kCgiFail)
		return this->makeErrorResponse(re3, curr_location, C500);
	if (rsc_status == kReading /*&& request->getMethod() & GET*/)
		return kResourceReadWaiting;
	if (rsc_status == kWriting /*&& request->getMethod() & POST*/)
		return kResourceWriteWaiting;

	std::string resource_path;
	std::string	root = curr_location->getRoot().length() ? curr_location->getRoot() : this->default_root;

	if (re3->getRscPtr()->getStatus() == kReadDone) {
		resource_path = re3->getRscPtr()->getResourceUri();
	} else {
		resource_path = request->getUri();
		size_t path_pos = resource_path.find(curr_location->getPath());
		resource_path.replace(path_pos, curr_location->getPath().length(), root);
		re3->getRscPtr()->setResourceUri(resource_path);
	}

	if (handleCGI(request, curr_location))
	{
		if (checkPath(resource_path) == kNotFound)
			return this->makeErrorResponse(re3, curr_location, C404);
		CgiConnector	tmp;
		ServerStatus	ret = tmp.makeCgiResponse(re3, curr_location, this->port_num, root);
		if (ret == kResponseError)
			return this->makeErrorResponse(re3, curr_location, C500);
		return ret;
	}

	if (request->getMethod() & GET)
		return this->makeGETResponse(re3, curr_location, resource_path);
	if (request->getMethod() & POST)
		return this->makePOSTResponse(re3, curr_location, resource_path);
	if (request->getMethod() & DELETE)
		return this->makeDELETEResponse(re3, curr_location, resource_path);
	return this->makeErrorResponse(re3, curr_location, C501);
}

ServerStatus	Server::makeRedirectResponse(Re3* re3, const std::pair<stat_type, std::string> return_to) const {
	std::map<std::string, std::string> headers;

	headers["Date"] = this->dateHeaderInfo();
	headers["Server"] = "Passive Server";
	headers["Content-Type"] = this->contentTypeHeaderInfo(".html");
	headers["Location"] = return_to.second;

	std::string error_page_body = this->makeHTMLPage(return_to.first);
	std::stringstream length;
	length << error_page_body.length();
	headers["Content-Length"] = length.str();
	assert(re3->getRspPtr() == NULL);
	re3->setRspPtr(new Response(kFinished, std::string(return_to.first), headers, error_page_body, re3->getReqPtr()->getVersion()));
	return kResponseMakingDone;
}

//@return: 디폴트 에러파일을 열 때 - ResourceReadWaiting
//@return: 자체 에러페이지를 제작할 때 - ResponseMakingDone
ServerStatus Server::makeErrorResponse(Re3* re3, const Location* location, stat_type http_status_code) const {
	std::map<std::string, std::string> headers;

	headers["Date"] = this->dateHeaderInfo();
	headers["Server"] = "Passive Server";
	headers["Content-Type"] = this->contentTypeHeaderInfo(".html");

	//해당 에러코드의 디폴트 에러페이지가 있으명
	// NOTE 리소스를 읽다가 실패를 해서 온거면, 기존에 있던 resource fd라던지 처리해야할게 있지 않을까요?
	if (!location->getDefaultErrorPage(http_status_code).empty()) {
		int fd = open(location->getDefaultErrorPage(http_status_code).c_str(), O_RDONLY);
		if (fd != ERROR) {
			struct stat sb;
			fstat(fd, &sb);
			std::stringstream length;
			length << (int)sb.st_size;
			headers["Content-Length"] = length.str();

			assert(re3->getRscPtr()->getStatus() == kNothing);
			re3->getRscPtr()->setStatus(kReading);
			re3->getRscPtr()->setResourceFd(fd);
			return kResourceReadInit;
		}
		// 디폴트 에러페이지 오픈 실패 -> 디폴트페이지가 아니라 자체적으로 만들어내는 페이지로 리턴
	}
	//디폴트 에러페이지가 없으면 새로만듦
	std::string error_page_body = this->makeHTMLPage(http_status_code);
	std::stringstream length;
	length << error_page_body.length();
	headers["Content-Length"] = length.str();
	assert(re3->getRspPtr() == NULL);
	re3->setRspPtr(new Response(kFinished, std::string(http_status_code), headers, error_page_body, re3->getReqPtr()->getVersion()));
	return kResponseMakingDone;
}

ServerStatus Server::makeGETResponse(Re3* re3, const Location* curr_location, std::string resource_path) const {
	struct stat	sb;
	int fd;
	std::map<std::string, std::string> headers;
	Request* request = re3->getReqPtr();
	Resource* resource = re3->getRscPtr();
	Response* response = re3->getRspPtr();

	headers["Date"] = this->dateHeaderInfo();
	headers["Server"] = "Passive Server";

	//만약 리소스 상태가 == Nothing
	if (resource->getStatus() == kNothing) {
		//경로가 디렉토리면
		if (checkPath(resource_path) == kDirectory) {
			if (resource_path[resource_path.length() - 1] != '/')
				resource_path += '/';
			bool indexFileFlag = false;
			//default indexfile값이 존재한다면
			if (!curr_location->getIndexes().empty())
			//indexes 이터레이터 돌면서 열리는 인덱스파일이 있는지 확인
				for (std::vector<const std::string>::iterator iter = curr_location->getIndexes().begin();
						iter != curr_location->getIndexes().end(); ++iter) {
					struct stat buffer;
					if (stat((resource_path + *iter).c_str(), &buffer) == 0) {
						resource_path = resource_path + *iter;
						resource->setResourceUri(resource_path);
						indexFileFlag = true;
						break;
					}
				}
			//대응하는 default indexfile이 없었는데, autoindex가 켜져있다면
			if (indexFileFlag == false && curr_location->isAutoIndex() == true) {
				headers["Content-Type"] = this->contentTypeHeaderInfo(".html");
				std::string autoindex_body = makeAutoIndexPage(request, resource_path);
				if (autoindex_body.empty())
					return this->makeErrorResponse(re3, curr_location, C500);
				std::stringstream length;
				length << autoindex_body.length();
				headers["Content-Length"] = length.str();
				//Re3에 Response 추가
				assert(response == NULL);
				re3->setRspPtr(new Response(kFinished, std::string(C200), headers, autoindex_body, request->getVersion()));
				return kResponseMakingDone;
			}
			//default indexfile과 대응하는 파일이 있었는데, 그 파일이 NotFounde거나, 디렉토리라면
			if (checkPath(resource_path) == kNotFound || checkPath(resource_path) == kDirectory)
				return this->makeErrorResponse(re3, curr_location, C404);
		}
		//default indexfile과 대응하는 파일이 있거나, 요청 uri가 파일이면
		if ((fd = open(resource_path.c_str(), O_RDONLY)) < 0)
			return this->makeErrorResponse(re3, curr_location, C404);
		if (fstat(fd, &sb) < 0) {
			close(fd);
			return this->makeErrorResponse(re3, curr_location, C500);
		}
		if (resource->getStatus() == kNothing)
		{
			resource->setStatus(kReading);
			resource->setResourceFd(fd);
		}
		return kResourceReadInit;
	} else if (re3->getRscPtr()->getStatus() == kReadDone) {
		// close(resource->getResourceFd());
		headers["Content-Type"] = this->contentTypeHeaderInfo(fileExtension(resource_path.substr()));
		headers["Content-Language"] = "ko-KR";
		headers["Content-Location"] = resource_path.substr(1);
		std::stringstream length;
		length << resource->getContent().length();
		headers["Content-Length"] = length.str();
		stat(resource_path.c_str(), &sb);
		headers["Last-Modified"] = this->lastModifiedHeaderInfo(sb);
		assert(response == NULL);
		re3->setRspPtr(new Response(kFinished, std::string(C200), headers, resource->getContent(), request->getVersion()));
		return kResponseMakingDone;
	}
	return kResourceReadWaiting; // compile에러 잡기위한 아무 코드
}

ServerStatus Server::makePOSTResponse(Re3* re3, const Location* curr_location, std::string resource_path) const {
	int 								fd;
	std::map<std::string, std::string>	headers;
	Resource*							resource = re3->getRscPtr();

	if (re3->getRscPtr()->getStatus() == kWriteDone) {
		Request* request = re3->getReqPtr();
		headers["Date"] = this->dateHeaderInfo();
		headers["Server"] = "Passive Server";
		headers["Content-Location"] = resource_path.substr(1);
		headers["Content-Length"] = "0";
		assert(re3->getRspPtr() == NULL);
		re3->setRspPtr(new Response(kFinished, std::string(resource->getIsCreated()), headers, "", request->getVersion()));
		return kResponseMakingDone;
	}
	switch (checkPath(resource_path)) {
		case kNotFound : {
			if ((fd = open(resource_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
				return this->makeErrorResponse(re3, curr_location, C500);
			resource->setStatus(kWriting);
			resource->setResourceFd(fd);
			resource->setIsCreated(C201);
			return kResourceWriteInit;
		}
		case kFile : {
			if ((fd = open(resource_path.c_str(), O_WRONLY | O_APPEND )) < 0)
				return this->makeErrorResponse(re3, curr_location, C500);
			resource->setStatus(kWriting);
			resource->setResourceFd(fd);
			resource->setIsCreated(C200);
			return kResourceWriteInit;
		}
		default :
			return this->makeErrorResponse(re3, curr_location, C403);
	}
}

ServerStatus Server::makeDELETEResponse(Re3* re3, const Location* curr_location, std::string resource_path) const {
	std::map<std::string, std::string> headers;

	headers["Date"] = this->dateHeaderInfo();
	headers["Server"] = "Passive Server";
	headers["Content-Length"] = "0";
	if (this->checkPath(resource_path) == kFile)
	{
		unlink(resource_path.c_str());
		assert(re3->getRspPtr() == NULL);
		re3->setRspPtr(new Response(kFinished, std::string(C200), headers, this->makeHTMLPage("File deleted"), re3->getReqPtr()->getVersion()));
		return kResponseMakingDone;
	}
	return this->makeErrorResponse(re3, curr_location, C404);
}

const Location* Server::currLocation(std::string request_uri) const {
	if (this->locations.size() == 0)
		return NULL;

	std::vector<Location>::const_iterator res = this->locations.begin();
	unsigned long	longest = 0;

	for (std::vector<Location>::const_iterator it = this->locations.begin(); \
	it != this->locations.end(); it++) {
		std::string path = it->getPath();
		if (request_uri.compare(0, path.length(), path) == 0 \
		&& longest < path.length()) {
			longest = path.length();
			res = it;
		}
	}
	return &*res;
}

stat_type Server::requestValidCheck(Request* request, const Location* curr_location) const {
	if ((request->getMethod() & curr_location->getMethodsAllowed()) == false)
		return C405;
	if (this->client_body_limit != 0)
		if (!request->getHeaderValue("content-length").empty()) {
			unsigned long	content_length;
			std::stringstream temp(request->getHeaderValue("content-length"));
			temp >> content_length;
			if (content_length > this->client_body_limit)
				return C413;
		}
	return C200;
}

int Server::  checkPath(std::string path) const {
	struct stat buffer;

	int exist = stat(path.c_str(), &buffer);
	if (exist == 0) {
		if (S_ISREG(buffer.st_mode))
			return kFile;
		else if (S_ISDIR(buffer.st_mode))
			return kDirectory;
	}
	return kNotFound;
}

std::string Server::dateHeaderInfo() const {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	return buffer;
}

std::string Server::lastModifiedHeaderInfo(struct stat sb) const {
	struct tm*	timeinfo = localtime(&sb.st_mtime);
	char buffer[80];
	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	return std::string(buffer);
}

std::string Server::contentTypeHeaderInfo(std::string extension) const {
	std::map<std::string, std::string>& mime_type = this->mime_types;
	if (mime_type.size() == 0) {
		mime_type[".aac"] = "audio/aac";
		mime_type[".abw"] = "application/x-abiword";
		mime_type[".arc"] = "application/octet-stream";
		mime_type[".avi"] = "video/x-msvideo";
		mime_type[".azw"] = "application/vnd.amazon.ebook";
		mime_type[".bin"] = "application/octet-stream";
		mime_type[".bz"] = "application/x-bzip";
		mime_type[".bz2"] = "application/x-bzip2";
		mime_type[".csh"] = "application/x-csh";
		mime_type[".css"] = "text/css";
		mime_type[".csv"] = "text/csv";
		mime_type[".doc"] = "application/msword";
		mime_type[".epub"] = "application/epub+zip";
		mime_type[".Gif"] = "image/gif";
		mime_type[".htm"] = "text/html";
		mime_type[".html"] = "text/html";
		mime_type[".ico"] = "image/x-icon";
		mime_type[".ics"] = "text/calendar";
		mime_type[".jar"] = "Temporary Redirect";
		mime_type[".jpeg"] = "image/jpeg";
		mime_type[".jpg"] = "image/jpeg";
		mime_type[".js"] = "application/js";
		mime_type[".json"] = "application/json";
		mime_type[".mid"] = "audio/midi";
		mime_type[".midi"] = "audio/midi";
		mime_type[".mpeg"] = "video/mpeg";
		mime_type[".mpkg"] = "application/vnd.apple.installer+xml";
		mime_type[".odp"] = "application/vnd.oasis.opendocument.presentation";
		mime_type[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
		mime_type[".odt"] = "application/vnd.oasis.opendocument.text";
		mime_type[".oga"] = "audio/ogg";
		mime_type[".ogv"] = "video/ogg";
		mime_type[".ogx"] = "application/ogg";
		mime_type[".pdf"] = "application/pdf";
		mime_type[".ppt"] = "application/vnd.ms-powerpoint";
		mime_type[".rar"] = "application/x-rar-compressed";
		mime_type[".rtf"] = "application/rtf";
		mime_type[".sh"] = "application/x-sh";
		mime_type[".svg"] = "image/svg+xml";
		mime_type[".swf"] = "application/x-shockwave-flash";
		mime_type[".tar"] = "application/x-tar";
		mime_type[".tif"] = "image/tiff";
		mime_type[".tiff"] = "image/tiff";
		mime_type[".ttf"] = "application/x-font-ttf";
		mime_type[".vsd"] = " application/vnd.visio";
		mime_type[".wav"] = "audio/x-wav";
		mime_type[".weba"] = "audio/webm";
		mime_type[".webm"] = "video/webm";
		mime_type[".webp"] = "image/webp";
		mime_type[".woff"] = "application/x-font-woff";
		mime_type[".xhtml"] = "application/xhtml+xml";
		mime_type[".xls"] = "application/vnd.ms-excel";
		mime_type[".xml"] = "application/xml";
		mime_type[".xul"] = "application/vnd.mozilla.xul+xml";
		mime_type[".zip"] = "application/zip";
		mime_type[".3gp"] = "video/3gpp audio/3gpp";
		mime_type[".3g2"] = "video/3gpp2 audio/3gpp2";
		mime_type[".7z"] = "application/x-7z-compressed";
	}
	if (mime_type.count(extension) == 0)
		return "text/plain";
	else
		return mime_type[extension];
}

std::string	Server::makeAutoIndexPage(Request* request, std::string resource_path) const {
	std::string body;
	std::cout << request->getHeaderValue("host") << ", " << resource_path << ", " << request->getUri() << std::endl;
	std::string addr = "http://" + request->getHeaderValue("host") + request->getUri() + "/";
	//  request->getHeaderValue("host") + "/"; //하이퍼링크용 경로

	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1> Index of "+ request->getUri() + "</h1>\n";

	DIR *dir = NULL;
	if ((dir = opendir(resource_path.c_str())) == NULL)
		return NULL;

	struct dirent *file = NULL;
	while ((file = readdir(dir)) != NULL)
		body += "<a href=\"" + addr + file->d_name + "\">" + file->d_name + "</a><br>\n";
	closedir(dir);

	body += "</body>\n";
	body += "</html>\n";

	return (body);
}

//@param HTML 상에 출력할 string
//@return HTML 코드 string
std::string Server::makeHTMLPage(std::string str) const {
	std::string body;

	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1>";
	body += str;
	body += "<h1>\n";
	body += "</body>\n";
	body += "</html>";
	return (body);
}

std::string Server::fileExtension(std::string resource_path) const
{
	if (resource_path.rfind('.') != std::string::npos)
		return (resource_path.substr(resource_path.rfind('.')));
	return resource_path;
}

Server::Server(const Server &ref)
	: server_name(ref.server_name),
	  default_root(ref.default_root),
	  default_error_pages(ref.default_error_pages),
	  client_body_limit(ref.client_body_limit),
	  locations(ref.locations),
	  return_to(ref.return_to) {}

Server::Server(
	std::string server_name,
	std::string default_root,
	std::map<stat_type, std::string> default_error_pages,
	unsigned long client_body_limit,
	std::vector<Location> locations,
	std::pair<stat_type, std::string> return_to)
	: server_name(server_name),
	  default_root(default_root),
	  default_error_pages(default_error_pages),
	  client_body_limit(client_body_limit),
	  locations(locations),
	  return_to(return_to) {}

Server::~Server() {}

const std::string&	Server::getServerName() const {
	return this->server_name;
}

void				Server::setPortNum(unsigned int num) {
	this->port_num = num;
}

unsigned int		Server::getPortNum() const {
	return this->port_num;
}
