#include "Server.hpp"

/*
if (client Request message read done)
{
	Request curr_request = client->getRequest();
	curr_request.setLocation = this->CurrLocation(curr_request.GetUri());


	stat = RequestValidCheck();
	if (stat == OK)
		MakeResponse(Client)
	else
		ErrorResponse(status)
}
*/

int Server::RequestValidCheck(Client& client) {
	Location curr_location = client.GetRequest().GetLocation();
	if (client.GetRequest().GetMethod() & GET == false)
		if (client.GetRequest().GetMethod() & curr_location.GetMethodsAllowed() == 0)
			return 405;
	if (this->client_body_limit != 0)
		if (client.GetRequest().GetHeader().count("Content-Length")) {
			int content_length;
			std::stringstream temp(client.GetRequest().GetHeader()["Content-Length"]);
			temp >> content_length;
			if (content_length > this->client_body_limit)
				return 413;
		}
	return OK;
}

Location& Server::CurrLocation(std::string request_uri) {
	Location	res;

	for (std::vector<Location>::iterator it = this->locations.begin(); it != this->locations.end(); it++) {
		std::string path = it.GetPath();
		if (request_uri.compare(0, path.length(), path) == 0)
			res = *it;
	}
	return res;
}

void Server::MakeResponse(Client& client) {
	Location curr_location = client.GetRequest().GetLocation();
	std::string resource_path = client.GetRequest().GetUri();
	size_t path_pos = resource_path.find_first_of(curr_location.GetPath());
	resource_path.replace(path_pos, curr_location.GetPath().length(), curr_location.GetRoot());

	if (curr_request->getMethod() == "GET")
		MakeGetResponse(client, resource_path);
	/*if (curr_request->getMethod() == "POST")
	{};
	if (curr_request->getMethod() == "DELETE")
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
	return Notfound;
}

std::string Server::DateHeaderInfo() {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	return buffer;
}

std::string Server::LastModifiedHeaderInfo(struct stat sb) {
	struct tm*	timeinfo = localtime(&sb.st_mtime);
	char buffer[80];
	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	return buffer;
}

void Server::MakeGetResponse(Client& client, std::string resource_path) {
	struct stat	sb;
	int fd;
	std::map<std::string, std::string> header;
	Request request = client.GetRequest();

	header["Date"] = DateHeaderInfo();
	header["Server"] = "Passive Server";

	if (checkPath(resource_path) == Directory) {
		if (resource_path[resource_path.length() - 1] != '/')
			resource_path += '/';

		bool indexFileFlag = false;
		if (!request.GetLocation().GetIndex().empty()) {
			for (std::vector<std::string>::iterator iter = request.GetLocation().GetIndex().begin();
					iter != request.GetLocation().GetIndex().end(); iter++) {
				struct stat buffer;
				if (stat((resource_path + *iter).c_str(), &buffer) == 0) {
					resource_path = resource_path + *iter;
					indexFileFlag = true;
					break ;
				}
			}
		}
		if (indexFileFlag == false && request.GetLocation().GetAutoIndex() == true) {
			header["Content-Type"] = ContentTypeHeaderInfo(".html");
			std::string autoindex_body = MakeAutoIndexPage(request, resource_path);
			std::stringstream length;
			length << (int)sb.st_size;
			header["Content-Length"] = length.str();
			client.PushResponse(Response(finished, "200 OK", header, autoindex_body, request.GetHttpVersion()));
		}
		if (checkPath(resource_path) == Notfound || checkPath(resource_path) == Directory)
			ErrorResponse(404);
	}
	if ((fd = open(resource_path.c_str(), O_RDONLY)) < 0)
		ErrorResponse(404);
	if (fstat(fd, &sb) < 0) {
		close(fd);
		ErrorResponse(500);
	}
	header["Content-Type"] = ContentTypeHeaderInfo(fileExtension(resource_path.substr(1)));
	header["Content-Language"] = "ko-KR";
	header["Content-Location"] = resource_path.substr(1);
	std::stringstream length;
	length << (int)sb.st_size;
	header["Content-Length"] = length.str();
	header["Last-Modified"] = LastModifiedHeaderInfo(sb);

	setResource(working, client, fd);
}

std::string Server::ContentTypeHeaderInfo(std::string extension) {
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

std::string	Server::MakeAutoIndexPage(Request& request, std::string resource_path) {
	std::string body;
	std::string addr = "http://" + request.GetHeader()["Host"] + "/"; //하이퍼링크용 경로

	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1> Index of "+ request.GetUri() + "</h1>\n";

	DIR *dir = NULL;
	if ((dir = opendir(resource_path.c_str())) == NULL)
		ErrorResponse(500);

	struct dirent *file = NULL;
	while ((file = readdir(dir)) != NULL)
		body += "<a href=\"" + addr + file->d_name + "\">" + file->d_name + "</a><br>\n";
	closedir(dir);

	body += "</body>\n";
	body += "</html>\n";

	return (body);
}

//이녀석의 리턴값에 대한 고민 필요 및 리스폰스 전달방식에대하 고민필요
void Server::ErrorResponse(Client& client, int http_status_code) {
	std::map<std::string, std::string> header;
	Location& location = client.GetRequest().GetLocation();

	header["Date"] = DateHeaderInfo();
	header["Server"] = "Passive Server";
	header["Content-Type"] = ContentTypeHeaderInfo(".html");

	if (location.GetDefaultErrorPages(http_status_code).empty())
		// setResponse(makeErrorResponse(http_status_code));
		// client.PushResponse(Response(finished, "200 OK", header, autoindex_body, request.GetHttpVersion()));
	else {
		int fd = open(location.GetDefaultErrorPages(http_status_code).c_str(), O_RDONLY);
		if (fd == -1) // 실패 -> 디폴트페이지가 아니라 자체적으로 만들어내는 페이지로 리턴
			// setResponse(makeErrorResponse(http_status_code));
		else {
			struct stat sb;

			fstat(fd, &sb);
				std::stringstream length;
			length << (int)sb.st_size;
			header["Content-Length"] = length.str();
			setResource(working, fd, client);
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

