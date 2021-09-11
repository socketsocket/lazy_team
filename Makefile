.PHONY: all debug clean fclean re simple
.DEFAULT_GOAL: simple

NAME = webserv

all: $(NAME)

CC = clang++

INC_DIR = ./include/
OBJ_DIR = ./obj/
SRC_DIR = ./src/

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 $(DEBUGF)
INCFLAG = -I$(INC_DIR)
ifeq ($(MAKECMDGOALS), debug)
DEBUGF = -g3 -fsanitize=address
DEBUG_CHECK = 1
else ifeq ($(MAKECMDGOALS), redebug)
DEBUGF = -g3 -fsanitize=address
DEBUG_CHECK = 1
else
DEBUG_CHECK = 0
endif

# header files dependencies
INC_TERMPRINTER = $(INC_DIR)TermPrinter.hpp
INC_LOCATION = $(INC_DIR)Location.hpp
INC_REQUEST = $(INC_DIR)Request.hpp $(INC_LOCATION)
INC_RESPONSE = $(INC_DIR)Response.hpp
INC_RESOURCE = $(INC_DIR)Resource.hpp
INC_RE3 = $(INC_DIR)Re3.hpp $(INC_REQUEST) $(INC_RESPONSE) $(INC_RESOURCE)
INC_CGICONNECTOR = $(INC_DIR)CgiConnector.hpp
INC_SERVER = $(INC_DIR)Server.hpp $(INC_RE3) $(INC_CGICONNECTOR)
INC_PORTMANGER = $(INC_DIR)PortManager.hpp $(INC_SERVER)
INC_CLIENT = $(INC_DIR)Client.hpp $(INC_PORTMANGER)
INC_SERVERMANAGER = $(INC_DIR)ServerManager.hpp $(INC_CLIENT) $(INC_ERRMSGHANDLER)
INC_CONFIGPARSER = $(INC_DIR)Webserv.hpp $(INC_SERVERMANAGER)
INC_MAIN = $(INC_CONFIGPARSER)

$(OBJ_DIR)TermPrinter.o : $(SRC_DIR)TermPrinter.cpp $(INC_ERRMSGHANDLER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)Location.o: $(SRC_DIR)Location.cpp $(INC_LOCATION)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)Request.o: $(SRC_DIR)Request.cpp $(INC_REQUEST)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)Response.o: $(SRC_DIR)Response.cpp $(INC_RESPONSE)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)Resource.o: $(SRC_DIR)Resource.cpp $(INC_RESOURCE)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)Re3.o: $(SRC_DIR)Re3.cpp $(INC_RE3)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)CgiConnector.o: $(SRC_DIR)CgiConnector.cpp $(INC_CGICONNECTOR)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)Server.o: $(SRC_DIR)Server.cpp $(INC_SERVER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)PortManager.o: $(SRC_DIR)PortManager.cpp $(INC_PORTMANGER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)Client.o: $(SRC_DIR)Client.cpp $(INC_CLIENT)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)ServerManager.o: $(SRC_DIR)ServerManager.cpp $(INC_SERVERMANAGER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@
$(OBJ_DIR)ConfigParser.o: $(SRC_DIR)ConfigParser.cpp $(INC_CONFIGPARSER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@

OBJ = TermPrinter.o \
	Location.o \
	Request.o \
	Response.o \
	Resource.o \
	Re3.o \
	CgiConnector.o \
	Server.o \
	PortManager.o \
	Client.o \
	ServerManager.o \
	ConfigParser.o \
	main.o
OBJS = $(addprefix $(OBJ_DIR),$(OBJ))

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $< -o $@

# if debug_check is not set, just compile.
ifeq ($(shell cat .make.tmp), $(DEBUG_CHECK))
$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(INCFLAG) $(OBJS) -o $@
	echo $(DEBUG_CHECK) > .make.tmp
# if the former make command is same as before, just compile.
else ifeq ($(shell echo $debug_check), $(DEBUG_CHECK))
$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(INCFLAG) $(OBJS) -o $@
	echo $(DEBUG_CHECK) > .make.tmp
# if the former make command is equal to the current one, delete files and recompile.
else
$(NAME): fclean $(OBJS)
	$(CC) $(CPPFLAGS) $(INCFLAG) $(OBJS) -o $@
	echo $(DEBUG_CHECK) > .make.tmp
endif

debug: all

redebug: fclean debug

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
