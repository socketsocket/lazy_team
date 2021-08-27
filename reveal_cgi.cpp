#include <unistd.h>

#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>

#define CHUNK_LEN	8162UL
#define MAX_OFFSET	8162L

static void
	printHeader(void)
{
	std::cout << "Content-Type: text/html" << "\r\n";
	std::cout << "\r\n";
}

static void
	printHTML(const std::string &html)
{
	std::cout << html;
	return ;
}

static std::string
	makeHTML(
		std::ifstream &in,
		double total_duration,
		double top_duration,
		double bot_duration,
		std::string top_string,
		std::string bot_string,
		std::string top_color,
		std::string bot_color )
{
	std::stringstream	html;

	html << "<!DOCTYPE html>" << "\r\n";
	html << "<html lang=\"en\">" << "\r\n";
	html << "<head>" << "\r\n";
	html << "	<style>" << "\r\n";
	html << "		/* reset */" << "\r\n";
	html << "		*," << "\r\n";
	html << "		*::before," << "\r\n";
	html << "		*::after {" << "\r\n";
	html << "		margin: 0;" << "\r\n";
	html << "		padding: 0;" << "\r\n";
	html << "		box-sizing: border-box;" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		body {" << "\r\n";
	html << "		display: flex;" << "\r\n";
	html << "		flex-direction: column;" << "\r\n";
	html << "		justify-content: space-around;" << "\r\n";
	html << "		align-items: center;" << "\r\n";
	html << "" << "\r\n";
	html << "		min-height: 100vh;" << "\r\n";
	html << "		padding: 20px;" << "\r\n";
	html << "" << "\r\n";
	html << "		font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "" << "\r\n";
	html << "		/* other */" << "\r\n";
	html << "		.info {" << "\r\n";
	html << "		margin: 20px 0;" << "\r\n";
	html << "		text-align: center;" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		p {" << "\r\n";
	html << "		color: #2e2e2e;" << "\r\n";
	html << "		margin-bottom: 20px;" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "" << "\r\n";
	html << "		/* block-$ */" << "\r\n";
	html << "		.block-effect {" << "\r\n";
	html << "		font-size: calc(8px + 6vw);" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		.block-reveal {" << "\r\n";
	html << "		--t: calc(var(--td) + var(--d));" << "\r\n";
	html << "" << "\r\n";
	html << "		color: transparent;" << "\r\n";
	html << "		padding: 4px;" << "\r\n";
	html << "" << "\r\n";
	html << "		position: relative;" << "\r\n";
	html << "		overflow: hidden;" << "\r\n";
	html << "" << "\r\n";
	html << "		animation: revealBlock 0s var(--t) forwards;" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		.block-reveal::after {" << "\r\n";
	html << "		content: '';" << "\r\n";
	html << "" << "\r\n";
	html << "		width: 0%;" << "\r\n";
	html << "		height: 100%;" << "\r\n";
	html << "		padding-bottom: 4px;" << "\r\n";
	html << "" << "\r\n";
	html << "		position: absolute;" << "\r\n";
	html << "		top: 0;" << "\r\n";
	html << "		left: 0;" << "\r\n";
	html << "" << "\r\n";
	html << "		background: var(--bc);" << "\r\n";
	html << "		animation: revealingIn var(--td) var(--d) forwards, revealingOut var(--td) var(--t) forwards;" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "" << "\r\n";
	html << "		/* animations */" << "\r\n";
	html << "		@keyframes revealBlock {" << "\r\n";
	html << "		100% {" << "\r\n";
	html << "			color: #0f0f0f;" << "\r\n";
	html << "		}" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		@keyframes revealingIn {" << "\r\n";
	html << "" << "\r\n";
	html << "		0% {" << "\r\n";
	html << "			width: 0;" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		100% {" << "\r\n";
	html << "			width: 100%;" << "\r\n";
	html << "		}" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		@keyframes revealingOut {" << "\r\n";
	html << "" << "\r\n";
	html << "		0% {" << "\r\n";
	html << "			transform: translateX(0);" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		100% {" << "\r\n";
	html << "			transform: translateX(100%);" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		}" << "\r\n";
	html << "" << "\r\n";
	html << "		.abs-site-link {" << "\r\n";
	html << "		position: fixed;" << "\r\n";
	html << "		bottom: 20px;" << "\r\n";
	html << "		left: 20px;" << "\r\n";
	html << "		color: hsla(0, 0%, 0%, .6);" << "\r\n";
	html << "		font-size: 16px;" << "\r\n";
	html << "		}" << "\r\n";
	html << "	</style>" << "\r\n";
	html << "" << "\r\n";
	html << "	<meta charset=\"UTF-8\">" << "\r\n";
	html << "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << "\r\n";
	html << "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << "\r\n";
	html << "	<title>Document</title>" << "\r\n";
	html << "</head>" << "\r\n";
	html << "<body>" << "\r\n";
	html << "	<h1 class=\"block-effect\" style=\"--td: " << total_duration << "s\">" << "\r\n";
	html << "		<div class=\"block-reveal\" style=\"--bc: " << top_color << "; --d: " << top_duration << "s\">" << top_string << "</div>" << "\r\n";
	html << "		<div class=\"block-reveal\" style=\"--bc: " << bot_color << "; --d: " << bot_duration << "s\">" << bot_string << "</div>" << "\r\n";
	html << "	</h1>" << "\r\n";
	html << "" << "\r\n";
	html << "	<div class=\"info\">" << "\r\n";

	std::string	str;
	while (in >> str)
		html << "		<p>" << str << "</p>" << "\r\n";

	html << "	</div>" << "\r\n";
	html << "" << "\r\n";
	html << "	<!--<a href=\"https://abubakersaeed.netlify.app/designs/d12-block-revealing-effect\" class=\"abs-site-link\" rel=\"nofollow noreferrer\" target=\"_blank\">abs/designs/d12-block-revealing-effect</a> -->" << "\r\n";
	html << "</body>" << "\r\n";
	html << "</html>" << "\r\n";

	return (html.str());
}

static void
	parseQuery(
		std::stringstream	&ss,
		double				&total_duration,
		double				&top_duration,
		double				&bot_duration,
		std::string			&top_string,
		std::string			&bot_string,
		std::string			&top_color,
		std::string			&bot_color )
{
	std::string	query;

	while (getline(ss, query, '&'))
	{
		std::string::size_type	asg_idx;
		std::string				key;
		std::string				val;

		if ((asg_idx = query.find('=')) == query.npos)
			continue ;
		else if (asg_idx + 1 == query.length())
			continue ;
		key = query.substr(0, asg_idx);
		val = query.substr(asg_idx + 1);
		if (key == "total_duration")
			total_duration = strtod(val.c_str(), NULL);
		else if (key == "top_duration")
			top_duration = strtod(val.c_str(), NULL);
		else if (key == "bot_duration")
			bot_duration = strtod(val.c_str(), NULL);
		else if (key == "top_string")
			top_string = val;
		else if (key == "bot_string")
			bot_string = val;
		else if (key == "top_color")
			top_color = val;
		else if (key == "bot_color")
			bot_color = val;
	}
}

static std::string
	makeErrorHTML(void)
{
	std::stringstream	html;

	html << "<!DOCTYPE html>" << "\r\n";
	html << "<html lang=\"en\">" << "\r\n";
	html << "<head>" << "\r\n";
	html << "	<meta charset=\"UTF-8\">" << "\r\n";
	html << "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << "\r\n";
	html << "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << "\r\n";
	html << "	<title>Error</title>" << "\r\n";
	html << "</head>" << "\r\n";
	html << "<body>" << "\r\n";
	html << "	Something wrong ..." << "\r\n";
	html << "</body>" << "\r\n";
	html << "</html>" << "\r\n";
	return (html.str());
}

static void
	getQueryStream(std::stringstream &query_stream)
{
	char				*method;

	method = getenv("REQUEST_METHOD");
	if (method)
	{
		if (strcmp(method, "GET") == 0)
		{
			if (getenv("QUERY_STRING"))
				query_stream.str(getenv("QUERY_STRING"));
			else
				query_stream.str("");
		}
		else if (strcmp(method, "POST") == 0)
		{
			ssize_t	read_size = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
			ssize_t	read_bytes;
			char	buffer[read_size + 1];

			while ((read_bytes = read(0, buffer, std::min(read_size, MAX_OFFSET))) > 0)
			{
				read_size -= read_bytes;
				buffer[read_bytes] = '\0';
				query_stream << buffer;
			}
		}
		else
			throw makeErrorHTML();
	}
	else
		throw makeErrorHTML();
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (-1);

	std::ofstream	log("./cgi/reveal_cgi.log", std::ios::out | std::ios::app);
	if (log.is_open() == false)
		return (-1);

	printHeader();

	std::stringstream	query_stream;

	double		total_duration	= 1.2;
	double		top_duration	= 0.1;
	double		bot_duration	= 0.5;
	std::string	top_string		= "CGI Tester";
	std::string	bot_string		= "hson seyu webserv";
	std::string	top_color		= "#4040bf";
	std::string	bot_color		= "#bf4060";

	try
	{
		getQueryStream(query_stream);
		log << "Query string = " << query_stream.str() << "\r\n";
	}
	catch (std::string &e)
	{
		printHTML(e);
		log << "ERROR: Cannot get query stream" << "\r\n";
		log.close();
		return (1);
	}

	std::ifstream	in(argv[1]);
	if (in.is_open() == false)
	{
		printHTML(makeErrorHTML());
		log << "ERROR: Cannot open file: " << argv[1] << "\r\n";
		log.close();
		return (1);
	}

	log << "Target file : " << argv[1] << "\r\n";
	log << total_duration << "\r\n";
	log << top_duration << "\r\n";
	log << bot_duration << "\r\n";
	log << top_string << "\r\n";
	log << bot_string << "\r\n";
	log << top_color << "\r\n";
	log << bot_color << "\r\n";
	log << "\r\n";

	parseQuery(query_stream, total_duration, top_duration, bot_duration, top_string, bot_string, top_color, bot_color);
	printHTML(makeHTML(in, total_duration, top_duration, bot_duration, top_string, bot_string, top_color, bot_color));
	in.close();
	log.close();
	return (0);
}
