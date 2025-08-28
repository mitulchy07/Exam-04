NAME = argo
SRC  = main.c argo.c argo_utils.c
CFLAGS = -Wall -Wextra -Werror -std=c99

all: $(NAME)
$(NAME): $(SRC)
	$(CC) $(CFLAGS) -o $(NAME) $(SRC)

clean:
	rm -f $(NAME)

re: clean all
