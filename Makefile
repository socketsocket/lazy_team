.PHONY: all debug clean fclean re simple
.DEFAULT_GOAL: simple

NAME = webserv

all: $(NAME)

CC = clang++

CPPFLAGS = -Wall -Werror -Wextra $(DEBUGF)
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
INC_ERRMSGHANDLER = ErrorMsgHandler.hpp
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

ErrorMsgHandler.o: $(patsubst %.o,%.c,$@) $(INC_ERRMSGHANDLER)
Location.o: $(patsubst %.o,%.c,$@) $(INC_LOCATION)
Request.o: $(patsubst %.o,%.c,$@) $(INC_REQUEST)
Response.o: $(patsubst %.o,%.c,$@) $(INC_RESPONSE)
Resource.o: $(patsubst %.o,%.c,$@) $(INC_RESOURCE)
Re3.o: $(patsubst %.o,%.c,$@) $(INC_RE3)
Server.o: $(patsubst %.o,%.c,$@) $(INC_SERVER)
PortManager.o: $(patsubst %.o,%.c,$@) $(INC_PORTMANGER)
Client.o: $(patsubst %.o,%.c,$@) $(INC_CLIENT)
ServerManager.o: $(patsubst %.o,%.c,$@) $(INC_SERVERMANAGER)
ConfigParser.o: $(patsubst %.o,&c,$@) $(INC_CONFIGPARSER)
main.o: $(patsubst %.o,&c,$@) $(INC_MAIN)

OBJS = ErrorMsgHandler.o \
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
ifneq ($(shell echo ${debug_check+x}), x)
$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@
	debug_check=$$(DEBUG_CHECK)
# if the former make command is same as before, just compile.
else ifeq ($(shell echo $debug_check), $(DEBUG_CHECK))
$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@
	debug_check=$$(DEBUG_CHECK)
# if the former make command is equal to the current one, delete files and recompile.
else
$(NAME): fclean $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@
	debug_check=$$(DEBUG_CHECK)
endif

debug: all

redebug: fclean debug

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
