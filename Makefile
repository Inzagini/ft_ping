CC:= cc
NAME:= ft_ping
CFLAGS:= -g -Wall -Werror -Wextra
INCLUDES:= include
SRC_DIR:= src
SRCS:= main.c 
OBJS = $(SRCS:%.c=$(SRC_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -I. -I$(INCLUDES) -o $(NAME)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@

clean:
	@rm -f $(OBJS) && echo "Cleaned *.o files"

fclean: clean
	@rm -f $(NAME) && echo "Cleaned $(NAME)"

re: fclean all

.PHONY: clean fclean re all
