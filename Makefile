.SILENT:
NAME = server client
CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRCS = srcs/server.c srcs/client.c
OBJS = $(SRCS:.c=.o)
all: $(NAME)
$(NAME): $(OBJS)
	$(LINK.o) srcs/server.o -o server
	$(LINK.o) srcs/client.o -o client
clean:
	rm -f $(OBJS)
fclean: clean
	rm -f client server
re: fclean all
