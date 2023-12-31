_R			=\e[0m
_RED		=\e[91m
_BLUE		=\e[94m
_CYAN		=\e[96m
_WHITE		=\e[97m

###############################
#           MAKEFILE          #
###############################



NAME        = webserv

CC          = clang++
CFLAGS      = -Werror -Wextra -Wall --std=c++98 #-g3 -fstandalone-debug -fsanitize=address
DEFINE		= -D CGI_PATH=\"$(shell which php-cgi)\"
RM          = rm -rf

DIR_SRCS    = src/
DIR_OBJS    = obj/
DIR_INC     = inc/

F_SRCS      =		main.cpp 			\
                	Server.cpp			\
					Request.cpp			\
					ConfigParser.cpp	\
					WebServer.cpp		\
					Cgi.cpp				\
					utils.cpp			\

SRCS        = $(addprefix $(DIR_SRCS), $(F_SRCS))

OBJS        = $(addprefix $(DIR_OBJS), $(F_SRCS:.cpp=.o))

HEADER      = -I $(DIR_INC)

all:        $(NAME)


$(DIR_OBJS)%.o:		$(DIR_SRCS)%.cpp
		@echo "$(_CYAN)Compilating..$(_R)"
		$(CC) $(CFLAGS) $(HEADER) $(DEFINE) -c $< -o $@

$(NAME):    $(DIR_OBJS) $(OBJS)
		${CC} $(CFLAGS) $(OBJS) -o $(NAME)
		@echo "$(_BLUE)Compilation completed.$(_R)"

$(DIR_OBJS):
	@mkdir $(DIR_OBJS)

clean:
		@echo "$(_RED)clean$(_R)"
		@$(RM) $(DIR_OBJS)

fclean:        clean
		@echo "$(_RED)fclean$(_R)"
		@$(RM) $(NAME)

re:        fclean all

.PHONY:     all re clean fclean
