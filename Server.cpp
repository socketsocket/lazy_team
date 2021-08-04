#include "Server.hpp"

ServerStatus Server::makeResponse(Re3Iter re3) {
	Request *request = re3->getReqPtr();

	if (request->getStatus() == Finished) {
		Location* curr_location = this->currLocation(request->getUri());
		
		stat_type stat = this->requestValidCheck(request, curr_location);		
		if (stat != C200)
			return this->makeErrorResponse(re3, curr_location, stat);	
		//리소스 상태는 'empty'/읽는중/읽음완료/에러 네가지로 들어옴
		//만약 리소스 상태가 == 에러라면
		if (re3->getRscPtr()->getStatus() == Disconnect \
		|| re3->getRscPtr()->getStatus() == ReadFail)
			return this->makeErrorResponse(re3, curr_location, C500);
		//만약 리소스 상태가 == 읽는중이라면
		else if (re3->getRscPtr()->getStatus() == Reading)
		{
			if (request->getMethod() & GET)
				return ResourceReadWaiting;
			if (request->getMethod() & POST)
				return ResourceWriteWaiting;
		}
		std::string resource_path = request->getUri();
		size_t path_pos = resource_path.find(curr_location->getPath());
		resource_path.replace(path_pos, curr_location->getPath().length(), curr_location->getRoot());

		if (request->getMethod() & GET == true)
			return this->makeGETResponse(re3, curr_location, resource_path);
		if (request->getMethod() & POST == true)
			return this->makePOSTResponse(re3, curr_location, resource_path);
		if (request->getMethod() & DELETE == true)
			return this->makeDELETEResponse(re3, curr_location, resource_path);
	}
}

//@return: 디폴트 에러파일을 열 때 - ResourceReadWaiting
//@return: 자체 에러페이지를 제작할 때 - ResponseMakingDone
ServerStatus Server::makeErrorResponse(Re3Iter re3, Location* location, stat_type http_status_code) {
	std::map<std::string, std::string> headers;

	headers["Date"] = this->dateHeaderInfo();
	headers["Server"] = "Passive Server";
	headers["Content-Type"] = this->contentTypeHeaderInfo(".html");
	
	//해당 에러코드의 디폴트 에러페이지가 있으명
	if (location->getDefaultErrorPages().count(http_status_code)) {
		int fd = open(location->getDefaultErrorPages()[http_status_code].c_str(), O_RDONLY);
		// 디폴트 에러페이지 오픈 실패 -> 디폴트페이지가 아니라 자체적으로 만들어내는 페이지로 리턴 
		if (fd == ERROR) {
			std::string error_page_body = this->makeHTMLPage(http_status_code);
			std::stringstream length;
			length << error_page_body.length();
			headers["Content-Length"] = length.str();
			assert(re3->getRspPtr() == NULL);
			re3->setRspPtr(new Response(Nothing, std::string(http_status_code), headers, error_page_body, re3->getReqPtr()->getVersion()));
			return ResponseMakingDone;
		}
		else {
			struct stat sb;
			fstat(fd, &sb);
			std::stringstream length;
			length << (int)sb.st_size;
			headers["Content-Length"] = length.str();

			assert(re3->getRspPtr() == NULL);
			re3->setRscPtr(new Resource(Reading, fd));
			return ResourceReadWaiting;
		}
	}
	//디폴트 에러페이지가 없으면 새로만듦
	else {
		std::string error_page_body = this->makeHTMLPage(http_status_code);
		std::stringstream length;
		length << error_page_body.length();
		headers["Content-Length"] = length.str();
		assert(re3->getRspPtr() == NULL);
		re3->setRspPtr(new Response(Nothing, std::string(http_status_code), headers, error_page_body, re3->getReqPtr()->getVersion()));
		return ResponseMakingDone;
	}
}

ServerStatus Server::makeGETResponse(Re3Iter re3, Location* curr_location, std::string resource_path) {
	struct stat	sb;
	int fd;
	std::map<std::string, std::string> headers;
	Request* request = re3->getReqPtr();
	Resource* resource = re3->getRscPtr();
	Response* response = re3->getRspPtr();

	headers["Date"] = this->dateHeaderInfo();
	headers["Server"] = "Passive Server";

	//만약 리소스 상태가 == Nothing
	if (resource->getStatus() == Nothing) {
		//경로가 디렉토리면
		if (checkPath(resource_path) == Directory) {
			if (resource_path[resource_path.length() - 1] != '/')
				resource_path += '/';
			bool indexFileFlag = false;
			//default indexfile값이 존재한다면
			if (!request->getLocation().getIndexes().empty())
			//indexes 이터레이터 돌면서 열리는 인덱스파일이 있는지 확인
				for (std::vector<const std::string>::iterator iter = request->getLocation().getIndexes().begin();
						iter != request->getLocation().getIndexes().end(); ++iter) {
					struct stat buffer;
					if (stat((resource_path + *iter).c_str(), &buffer) == 0) {
						resource_path = resource_path + *iter;
						indexFileFlag = true;
						break;
					}
				}
			//대응하는 default indexfile이 없었는데, autoindex가 켜져있다면
			if (indexFileFlag == false && request->getLocation().isAutoIndex() == true) {
				headers["Content-Type"] = this->contentTypeHeaderInfo(".html");
				std::string autoindex_body = makeAutoIndexPage(request, resource_path);
				if (autoindex_body.empty())
					return this->makeErrorResponse(re3, curr_location, C500);
				std::stringstream length;
				length << autoindex_body.length();
				headers["Content-Length"] = length.str();
				//Re3에 Response 추가
				assert(response == NULL);
				re3->setRspPtr(new Response(Nothing, std::string(C200), headers, autoindex_body, request->getVersion()));
				return ResponseMakingDone;
			}
			//default indexfile과 대응하는 파일이 있었는데, 그 파일이 NotFounde거나, 디렉토리라면
			if (checkPath(resource_path) == NotFound || checkPath(resource_path) == Directory)
				return this->makeErrorResponse(re3, curr_location, C404);
		}
		//default indexfile과 대응하는 파일이 있거나, 요청 uri가 파일이면
		if ((fd = open(resource_path.c_str(), O_RDONLY)) < 0)
			return this->makeErrorResponse(re3, curr_location, C404);
		if (fstat(fd, &sb) < 0) {
			close(fd);
			return this->makeErrorResponse(re3, curr_location, C500);
		}
		//Re3에 resource fd 추가 
		if (resource == NULL)
			re3->setRscPtr(new Resource(Reading, fd));

		return ResourceReadWaiting;
	}
	//만약 리소스 상태가 == Finished
	else if (re3->getRscPtr()->getStatus() == Finished) {
		headers["Content-Type"] = this->contentTypeHeaderInfo(fileExtension(resource_path.substr(1)));
		headers["Content-Language"] = "ko-KR";
		headers["Content-Location"] = resource_path.substr(1);
		std::stringstream length;
		length << resource->getContent().length();
		headers["Content-Length"] = length.str();
		headers["Last-Modified"] = this->lastModifiedHeaderInfo(sb);

		//Re3에 Response 추가
		assert(response == NULL);
		re3->setRspPtr(new Response(Nothing, std::string(C200), headers, resource->getContent(), request->getVersion()));

		return ResponseMakingDone;
	}
}

ServerStatus Server::makePOSTResponse(Re3Iter re3, Location* curr_location, std::string resource_path) {
	int fd;
	std::map<std::string, std::string> headers;
	Resource* resource = re3->getRscPtr();

	if (resource->getStatus() == Nothing) {
		headers["Content-Location"] = resource_path.substr(1);
		switch (checkPath(resource_path)) {
			case NotFound : {
				if ((fd = open(resource_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
					return this->makeErrorResponse(re3, curr_location, C500);
				if (resource == NULL)
					re3->setRscPtr(new Resource(Writing, fd));

				return ResourceWriteWaiting;
			}
			case File :
			{
				if ((fd = open(resource_path.c_str(), O_WRONLY | O_APPEND )) < 0)
					return this->makeErrorResponse(re3, curr_location, C500);
				if (resource == NULL)
					re3->setRscPtr(new Resource(Writing, fd));

				return ResourceWriteWaiting;
			}
			default :
				return this->makeErrorResponse(re3, curr_location, C403);
		}
		return;
	}
	else if (re3->getRscPtr()->getStatus() == Finished) {
		Request* request = re3->getReqPtr();	
		assert(re3->getRspPtr() == NULL);
		if (checkPath(resource_path) == Directory)
			re3->setRspPtr(new Response(Nothing, std::string(C201), headers, resource->getContent(), request->getVersion()));
		else
			re3->setRspPtr(new Response(Nothing, std::string(C200), headers, resource->getContent(), request->getVersion()));
		return ResponseMakingDone;
	}
}
	

ServerStatus Server::makeDELETEResponse(Re3Iter re3, Location* curr_location, std::string resource_path) {
	int fd;
	std::map<std::string, std::string> headers;
	Resource* resource = re3->getRscPtr();

	headers["Date"] = this->dateHeaderInfo();
	headers["Server"] = "Passive Server";
	if (checkPath(resource_path) == File)
	{
		unlink(resource_path.c_str());
		assert(re3->getRspPtr() == NULL);
		re3->setRspPtr(new Response(Nothing, std::string(C200), headers, this->makeHTMLPage("File deleted"), re3->getReqPtr()->getVersion()));
		return ResponseMakingDone;
	}
	return this->makeErrorResponse(re3, curr_location, C404);
}

Location* Server::currLocation(std::string request_uri) {
	std::vector<Location>::iterator res;
	int longest = 0;
	
	for (std::vector<Location>::iterator it = this->locations.begin(); \
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

stat_type Server::requestValidCheck(Request* request, Location* curr_location) {
	Location curr_location = request->getLocation();
	if (request->getMethod() & GET == false)
		if (request->getMethod() & curr_location->getMethodsAllowed() == false)
			return C405;
	if (this->client_body_limit != 0)
		if (request->getHeaders().count("Content-Length")) {
			int content_length;
			std::stringstream temp(request->getHeaders()["Content-Length"]);
			temp >> content_length;
			if (content_length > this->client_body_limit)
				return C413;
		}
	return C200;
}

int Server::checkPath(std::string path) {
	struct stat buffer;

	int exist = stat(path.c_str(), &buffer);
	if (exist == 0) {
		if (S_ISREG(buffer.st_mode))
			return File;
		else if (S_ISDIR(buffer.st_mode))
			return Directory;
	}
	return NotFound;
}

std::string Server::dateHeaderInfo() {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	return buffer;
}

std::string Server::lastModifiedHeaderInfo(struct stat sb) {
	struct tm*	timeinfo = localtime(&sb.st_mtime);
	char buffer[80];
	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	return buffer;
}

std::string Server::contentTypeHeaderInfo(std::string extension) {
	std::map<std::string, std::string> mimeType;
	mimeType[".aac"] = "audio/aac";
	mimeType[".abw"] = "application/x-abiword";
	mimeType[".arc"] = "application/octet-stream";
	mimeType[".avi"] = "video/x-msvideo";
	mimeType[".azw"] = "application/vnd.amazon.ebook";
	mimeType[".bin"] = "application/octet-stream";
	mimeType[".bz"] = "application/x-bzip";
	mimeType[".bz2"] = "application/x-bzip2";
	mimeType[".csh"] = "application/x-csh";
	mimeType[".css"] = "text/css";
	mimeType[".csv"] = "text/csv";
	mimeType[".doc"] = "application/msword";
	mimeType[".epub"] = "application/epub+zip";
	mimeType[".Gif"] = "image/gif";
	mimeType[".htm"] = "text/html";
	mimeType[".html"] = "text/html";
	mimeType[".ico"] = "image/x-icon";
	mimeType[".ics"] = "text/calendar";
	mimeType[".jar"] = "Temporary Redirect";
	mimeType[".jpeg"] = "image/jpeg";
	mimeType[".jpg"] = "image/jpeg";
	mimeType[".js"] = "application/js";
	mimeType[".json"] = "application/json";
	mimeType[".mid"] = "audio/midi";
	mimeType[".midi"] = "audio/midi";
	mimeType[".mpeg"] = "video/mpeg";
	mimeType[".mpkg"] = "application/vnd.apple.installer+xml";
	mimeType[".odp"] = "application/vnd.oasis.opendocument.presentation";
	mimeType[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	mimeType[".odt"] = "application/vnd.oasis.opendocument.text";
	mimeType[".oga"] = "audio/ogg";
	mimeType[".ogv"] = "video/ogg";
	mimeType[".ogx"] = "application/ogg";
	mimeType[".pdf"] = "application/pdf";
	mimeType[".ppt"] = "application/vnd.ms-powerpoint";
	mimeType[".rar"] = "application/x-rar-compressed";
	mimeType[".rtf"] = "application/rtf";
	mimeType[".sh"] = "application/x-sh";
	mimeType[".svg"] = "image/svg+xml";
	mimeType[".swf"] = "application/x-shockwave-flash";
	mimeType[".tar"] = "application/x-tar";
	mimeType[".tif"] = "image/tiff";
	mimeType[".tiff"] = "image/tiff";
	mimeType[".ttf"] = "application/x-font-ttf";
	mimeType[".vsd"] = " application/vnd.visio";
	mimeType[".wav"] = "audio/x-wav";
	mimeType[".weba"] = "audio/webm";
	mimeType[".webm"] = "video/webm";
	mimeType[".webp"] = "image/webp";
	mimeType[".woff"] = "application/x-font-woff";
	mimeType[".xhtml"] = "application/xhtml+xml";
	mimeType[".xls"] = "application/vnd.ms-excel";
	mimeType[".xml"] = "application/xml";
	mimeType[".xul"] = "application/vnd.mozilla.xul+xml";
	mimeType[".zip"] = "application/zip";
	mimeType[".3gp"] = "video/3gpp audio/3gpp";
	mimeType[".3g2"] = "video/3gpp2 audio/3gpp2";
	mimeType[".7z"] = "application/x-7z-compressed";
	if (mimeType.count(extension) == 0)
		return "text/plain";
	else
		return mimeType[extension];
}

std::string	Server::makeAutoIndexPage(Request* request, std::string resource_path) {
	std::string body;
	std::string addr = "http://" + request->getHeaders()["Host"] + "/"; //하이퍼링크용 경로

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
/*
* HTML 페이지를 만듭니다.
* @param HTML 상에 출력할 string
* @return HTML 코드 string
*/
std::string Server::makeHTMLPage(std::string str) {
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

std::string Server::fileExtension(std::string resource_path)
{
	if (resource_path.rfind('.') != std::string::npos)
		return (resource_path.substr(resource_path.find('.')));
	return resource_path;
}

Server::Server(const Server &ref)
	: port(ref.port),
	  server_name(ref.server_name),
	  default_root(ref.default_root),
	  default_error_pages(ref.default_error_pages),
	  client_body_limit(ref.client_body_limit),
	  locations(ref.locations),
	  return_to(ref.return_to) {}

Server::Server(
	unsigned int port,
	std::string server_name,
	std::string default_root,
	std::map<stat_type, std::string> default_error_pages,
	unsigned long client_body_limit,
	std::vector<Location> locations,
	std::pair<stat_type, std::string> return_to)
	: port(port),
	  server_name(server_name),
	  default_root(default_root),
	  default_error_pages(default_error_pages),
	  client_body_limit(client_body_limit),
	  locations(locations),
	  return_to(return_to) {}

Server::~Server() {}

unsigned int	Server::getPortNum() const {
	return (this->port);
}

