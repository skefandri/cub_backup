NAME = cub3D

SRC_MAN =   ./utils/ft_strlen.c \
		 ./Mandatory/rendering/drawing.c ./Mandatory/rendering/raycasting.c ./Mandatory/main.c ./Mandatory/test_pars.c ./Mandatory/rendering/key_event.c

CFLAGS =  -Wall -Wextra -Werror -fsanitize=address -g3 -I /nfs/homes/ysabr/Desktop/minilibx-linux -lXext -lX11 -lm -lz

all : $(NAME)
$(NAME) : $(SRC_MAN)
		cc  $(CFLAGS) $(SRC_MAN) -o $(NAME) -L /nfs/homes/ysabr/Desktop/minilibx-linux -lmlx_Linux

clean :
		rm -rf $(OBJS)

fclean : clean
		rm -rf $(NAME)

re : fclean all

