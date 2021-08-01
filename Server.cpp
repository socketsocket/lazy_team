#include "Server.hpp"

/*
Response (Request& request, Resource& resource)
if (request Request message read done)
{
	request.setLocation = this->currLocation(request.getUri());
	
	
	stat = requestValidCheck();
	if (stat == OK)
		makeResponse(request)
	else
		errorResponse(status)
}
*/

int Server::requestValidCheck(Request& request) {
	Location curr_location = request.getLocation();
	if (request.getMethod() & GET == false)
		if (request.getMethod() & curr_location.getMethodsAllowed() == 0)
			return 405;
	if (this->client_body_limit != 0)
		if (request.getHeader().count("Content-Length")) {
			int content_length;
			std::stringstream temp(request.getHeader()["Content-Length"]);
			temp >> content_length;
			if (content_length > this->client_body_limit)
				return 413;
		}
	return OK;
}

Location& Server::currLocation(std::string request_uri) {
	Location	res;

	for (std::vector<Location>::iterator it = this->locations.begin(); it != this->locations.end(); it++) {
		std::string path = it.getPath();
		if (request_uri.compare(0, path.length(), path) == 0)
			res = *it;
	}
	return res;
}

Response Server::makeResponse(Request& request, Resource& resource) {
	Location curr_location = request.getLocation();
	std::string resource_path = request.getUri();
	size_t path_pos = resource_path.find_first_of(curr_location.getPath());
	resource_path.replace(path_pos, curr_location.getPath().length(), curr_location.getRoot());

	if (request.getMethod() == "GET")
		return(makeGetResponse(request, resource_path, resource));
	/*if (request->getMethod() == "POST")
	{};
	if (request->getMethod() == "DELETE")
	{};*/
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

Response Server::makeGetResponse(Request& request, std::string resource_path, Resource& resource) {
	struct stat	sb;
	int fd;
	std::map<std::string, std::string> headers;

	headers["Date"] = dateHeaderInfo();
	headers["Server"] = "Passive Server";

	//리소스.스테이터스 는 empty로 들어오거나
	//읽는 중
	//읽음 완료
	
	// 세가지로 나뉨

	//만약 리소스.스테이터스 == Nothing
	if (checkPath(resource_path) == Directory) {
		if (resource_path[resource_path.length() - 1] != '/')
			resource_path += '/';

		bool indexFileFlag = false;
		if (!request.getLocation().getIndex().empty()) {
			for (std::vector<std::string>::iterator iter = request.getLocation().getIndex().begin();
					iter != request.getLocation().getIndex().end(); iter++) {
				struct stat buffer;
				if (stat((resource_path + *iter).c_str(), &buffer) == 0) {
					resource_path = resource_path + *iter;
					indexFileFlag = true;
					break ;
				}
			}
		}
		if (indexFileFlag == false && request.getLocation().getAutoIndex() == true) {
			headers["Content-Type"] = contentTypeHeaderInfo(".html");
			std::string autoindex_body = makeAutoIndexPage(request, resource_path);
			if (autoindex_body.empty())
				return errorResponse(request, "500");
			std::stringstream length;
			length << (int)sb.st_size;
			headers["Content-Length"] = length.str();
			return Response(finished, "200 OK", headers, autoindex_body, request.getHttpVersion());
		}
		if (checkPath(resource_path) == NotFound || checkPath(resource_path) == Directory)
			return errorResponse(request, "404");
	}
	if ((fd = open(resource_path.c_str(), O_RDONLY)) < 0)
		return errorResponse(request, "404");
	if (fstat(fd, &sb) < 0) {
		close(fd);
		return errorResponse(request, "500");
	}

	headers["Content-Type"] = contentTypeHeaderInfo(fileExtension(resource_path.substr(1)));
	headers["Content-Language"] = "ko-KR";
	headers["Content-Location"] = resource_path.substr(1);
	std::stringstream length;
	length << (int)sb.st_size;
	headers["Content-Length"] = length.str();
	headers["Last-Modified"] = lastModifiedHeaderInfo(sb);
	
	resource.push(working, request, fd);

	//만약 리소스.스테이터스 == Reading

	//만약 리소스.스테이터스 == Finished

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

std::string	Server::makeAutoIndexPage(Request& request, std::string resource_path) {
	std::string body;
	std::string addr = "http://" + request.getHeader()["Host"] + "/"; //하이퍼링크용 경로

	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1> Index of "+ request.getUri() + "</h1>\n";

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

Response Server::errorResponse(Request& request, std::string http_status_code) {
	std::map<std::string, std::string> headers;
	Location& location = request.getLocation();

	headers["Date"] = dateHeaderInfo();
	headers["Server"] = "Passive Server";
	headers["Content-Type"] = contentTypeHeaderInfo(".html");

	//해당 에러코드의 디폴트 에러페이지가 없으면 새로만듦
	if (location.getDefaultErrorPages(http_status_code).empty()) {
		std::string error_page_body = makeHTMLPage(http_status_code);
		std::stringstream length;
		length << error_page_body.length();
		headers["Content-Length"] = length.str();
		return Response(error, status_code_map[http_status_code], headers, error_page_body, request.getHttpVersion());
	}
	else {
		int fd = open(location.getDefaultErrorPages(http_status_code).c_str(), O_RDONLY);
		// 디폴트 에러페이지 오픈 실패 -> 디폴트페이지가 아니라 자체적으로 만들어내는 페이지로 리턴 
		if (fd == -1) {
			std::string error_page_body = makeHTMLPage(http_status_code);
			std::stringstream length;
			length << error_page_body.length();
			headers["Content-Length"] = length.str();
			return Response(error, status_code_map[http_status_code], headers, error_page_body, request.getHttpVersion());
		}
		else {
			struct stat sb;
			fstat(fd, &sb);
			std::stringstream length;
			length << (int)sb.st_size;
			headers["Content-Length"] = length.str();
			resource.push(working, fd, request);
		}
	}
}

std::string Server::fileExtension(std::string resource_path)
{
	if (resource_path.find('.') != std::string::npos)
		return (resource_path.substr(resource_path.find('.')));
	return resource_path;
}

Server::Server(const Server &ref)
	: server_fd(ref.server_fd),
	  port(ref.port),
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

void	Server::setServerFd(const int fd) {
	this->server_fd = fd;
}
