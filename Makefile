.PHONY: all debug clean fclean re
.DEFAULT_GOAL: all

NAME = webserv

all: $(NAME)

CC = clang++

CPPFLAGS = -Wall -Werror -Wextra $(DEBUGF)
ifeq ($(MAKECMDGOALS), debug)
DEBUGF = -g3 -fsanitize=address

DEBUG_CHECK:
	DEBUG_CHECK=1
else
DEBUG_CHECK:
	DEBUG_CHECK=0
endif

INC_DIR = .

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
INC_MAIN = Webserv.hpp $(INC_SERVERMANAGER)

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

OBJS = ErrorMsgHandler.o \
	Location.o \
	Request.o \
	Response.o \
	Resource.o \
	Re3.o \
	Server.o \
	PortManager.o \
	Client.o \
	ServerManager.o

%.o: %.cpp
	$(CC) $(CFLANGS) -c $< -o $@

ifneq ($(shell $(DEBUG_CHECK)), DEBUG_CHECK)
$(NAME): fclean $(OBJS)
	$(CC) $(CFLAGS) -c $? -o $@
else
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -c $? -o $@
endif

debug: DEBUG_CHECK all

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
