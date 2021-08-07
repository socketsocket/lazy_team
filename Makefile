.PHONY: all debug clean fclean re simple
.DEFAULT_GOAL: simple

NAME = webserv

all: $(NAME)

CC = clang++

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 $(DEBUGF)
ifeq ($(MAKECMDGOALS), debug)
DEBUGF = -g3 -fsanitize=address
DEBUG_CHECK = 1
else ifeq ($(MAKECMDGOALS), redebug)
DEBUGF = -g3 -fsanitize=address
DEBUG_CHECK = 1
else
DEBUG_CHECK = 0
endif

INC_DIR = .

# header files dependencies
INC_TERMPRINTER = TermPrinter.hpp
INC_LOCATION = Location.hpp
INC_REQUEST = Request.hpp $(INC_LOCATION)
INC_RESPONSE = Response.hpp
INC_RESOURCE = Resource.hpp
INC_RE3 = Re3.hpp $(INC_REQUEST) $(INC_RESPONSE) $(INC_RESOURCE)
INC_SERVER = Server.hpp $(INC_RE3)
INC_PORTMANGER = PortManager.hpp $(INC_SERVER)
INC_CLIENT = Client.hpp $(INC_PORTMANGER)
INC_SERVERMANAGER = ServerManager.hpp $(INC_CLIENT) $(INC_ERRMSGHANDLER)
INC_CONFIGPARSER = Webserv.hpp $(INC_SERVERMANAGER)
INC_MAIN = $(INC_CONFIGPARSER)

TermPrinter.o: %.o : %.cpp $(INC_ERRMSGHANDLER)
	$(CC) $(CPPFLAGS) -c $< -o $@
Location.o: %.o : %.cpp $(INC_LOCATION)
	$(CC) $(CPPFLAGS) -c $< -o $@
Request.o: %.o : %.cpp $(INC_REQUEST)
	$(CC) $(CPPFLAGS) -c $< -o $@
Response.o: %.o : %.cpp $(INC_RESPONSE)
	$(CC) $(CPPFLAGS) -c $< -o $@
Resource.o: %.o : %.cpp $(INC_RESOURCE)
	$(CC) $(CPPFLAGS) -c $< -o $@
Re3.o: %.o : %.cpp $(INC_RE3)
	$(CC) $(CPPFLAGS) -c $< -o $@
Server.o: %.o : %.cpp $(INC_SERVER)
	$(CC) $(CPPFLAGS) -c $< -o $@
PortManager.o: %.o : %.cpp $(INC_PORTMANGER)
	$(CC) $(CPPFLAGS) -c $< -o $@
Client.o: %.o : %.cpp $(INC_CLIENT)
	$(CC) $(CPPFLAGS) -c $< -o $@
ServerManager.o: %.o : %.cpp $(INC_SERVERMANAGER)
	$(CC) $(CPPFLAGS) -c $< -o $@
ConfigParser.o: %.o : %.cpp $(INC_CONFIGPARSER)
	$(CC) $(CPPFLAGS) -c $< -o $@

OBJS = TermPrinter.o \
	Location.o \
	Request.o \
	Response.o \
	Resource.o \
	Re3.o \
	Server.o \
	PortManager.o \
	Client.o \
	ServerManager.o \
	ConfigParser.o \
	main.o

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

# if debug_check is not set, just compile.
ifeq ($(shell cat .make.tmp), $(DEBUG_CHECK))
$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@
	echo $(DEBUG_CHECK) > .make.tmp
# if the former make command is same as before, just compile.
else ifeq ($(shell echo $debug_check), $(DEBUG_CHECK))
$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@
	echo $(DEBUG_CHECK) > .make.tmp
# if the former make command is equal to the current one, delete files and recompile.
else
$(NAME): fclean $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@
	echo $(DEBUG_CHECK) > .make.tmp
endif

debug: all

redebug: fclean debug

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
