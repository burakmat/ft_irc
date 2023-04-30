NAME = ircserv
CC = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I lib/
SRCS = src/main.cpp \
		src/Server.cpp \
		src/Socket.cpp \
		src/User.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME) test

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

test:
	gcc test/client.c -o client
	mv client test

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)
	rm -rf test/client

re: fclean all

.PHONY: all clean fclean re test
