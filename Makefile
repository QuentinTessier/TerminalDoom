NAME 		=	TDoom

CC			= 	gcc

INCLUDE		=	-Iinclude

LIB			=	-lncurses -lm

CFLAGS		=	-Wall -Wextra $(INCLUDE) $(LIB)

SRC_PATH 	=	./src

SRC			=	$(SRC_PATH)/main.c			\
				$(SRC_PATH)/map.c			\
				$(SRC_PATH)/camera.c		\
				$(SRC_PATH)/dbuffer.c

OBJ			=	$(SRC:.c=.o)

all:			$(NAME)

$(NAME):		$(OBJ)
				$(CC) -o $(NAME) $(OBJ) $(CFLAGS)

clean:
				rm -f $(OBJ)

fclean: 		clean
				rm -f $(NAME)

re:				fclean all

debug:			CFLAGS += -g
debug:			re

.PHONY:			all clean fclean re debug