
NAME = cub3D
CC = cc
CFLAGS = -Wall -Wextra -Werror -fsanitize=address -g3 -I /nfs/homes/ysabr/Desktop/minilibx-linux -lXext -lX11 -lm -lz
OBJDIR = object
LIB = -L /nfs/homes/ysabr/Desktop/minilibx-linux -lmlx_Linux

SRC_MAN =  utils/ft_strlen.c \
		   Mandatory/rendering/drawing.c \
		   Mandatory/rendering/raycasting.c \
		   Mandatory/main.c \
		   Mandatory/test_pars.c \
		   Mandatory/rendering/key_event.c

# Convert each source file name to an object file inside the object directory
OBJS = $(addprefix $(OBJDIR)/, $(notdir $(SRC_MAN:.c=.o)))

# Default target
.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LIB)

# General rule for all object files
$(OBJDIR)/%.o: | $(OBJDIR)
	$(CC) $(CFLAGS) -c $(filter %/$*.c, $(SRC_MAN)) -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all
