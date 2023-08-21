#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <mlx.h>
#include <math.h>

#define CELL_SIZE 40
#define PLAYER_SIZE 10
#define MOVE_SPEED 10
#define TURN_ANGLE (M_PI / 60)
#define HIGHT 1000
#define WIDTH 1000
#define FOV (M_PI / 3)

typedef struct
{
	double	x;
	double	y;
	double	direction;
} t_player;


typedef struct
{
	double ray_x;
	double ray_y;
	double ray_dx;
	double ray_dy;
	double distance;
} t_ray;

typedef struct
{
	int	row_len;
	int	col_len;
	char	**map;
} t_map;

typedef struct s_data
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int 	endian;
	t_map	map;
	t_player player;
	void	*mlx;
	void	*mlx_win;
	int		j;
}	t_data;
void handle_error(char *message)
{
	write(2, message, ft_strlen(message));
	exit(EXIT_FAILURE);
}

char **ft_split(char *str, int row_len, int col_len)
{
	char **resl = malloc(sizeof(char *) * (row_len + 1));
	if (!resl)
		handle_error("malloc failed !");
	resl[row_len] = NULL;
	int i = 0, j = 0, y = 0;
	while (resl && str[i]) {
		y = 0;
		resl[j] = malloc(sizeof(char) * (col_len + 1));
		if (!resl[j])
			handle_error("malloc failed !");
		while (str[i + y] && str[i + y] != '\n') {
			resl[j][y] = str[i + y];
			y++;
		}
		resl[j][y] = '\0';
		if (!str[i + y]) return resl;
		i += y;
		j++;
		i++;
	}
	return resl;
}

void calculate_map_size(char *buffer, int bytes_read, int *row_len, int *col_len)
{
	int i = 0;
	*row_len = 0;
	*col_len = 0;
	while (i < bytes_read)
	{
		if (buffer[i] == '\n')
		{
			(*row_len)++;
			if (*col_len == 0) *col_len = i;
		}
		i++;
	}
	if (buffer[bytes_read - 1] != '\n')
		(*row_len)++;
}

void	read_map(char *filename, t_map *map)
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1)
		handle_error("Error reading map");
	char buffer[4096];
	int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
	if (bytes_read <= 0)
		handle_error("Error reading map file");
	int row_len, col_len;
	calculate_map_size(buffer, bytes_read, &row_len, &col_len);
	map->row_len = row_len;
	map->col_len = col_len;
	map->map = ft_split(buffer, row_len, col_len);
}

void    my_mlx_put_pixel(t_data *data, int x, int y, int color)
{
    char    *dst;

    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned *)dst = color;
}


void    render_grid(t_data *data, int x, int y)
{
    if (x % CELL_SIZE == 0 || y % CELL_SIZE == 0)
        my_mlx_put_pixel(data, x, y, 0);
}

void    render_content(t_data *data, t_map *map, int x, int y)
{
    if (map->map[y / CELL_SIZE][x / CELL_SIZE] == '1')
        my_mlx_put_pixel(data, x, y, 255);
}

void	render_map(t_data *data, t_map *map)
{
	int x;
	int	y;

	y = -1;
	while (++y < map->row_len * CELL_SIZE)
	{
		x = -1;
		while (++x < map->col_len * CELL_SIZE)
		{
			render_grid(data, x, y);
			render_content(data, map, x, y);
		}
	}
}

int main(int ac, char **av)
{
    t_data  img;

    if (ac != 2)
    {
        printf("Error");
        return (1);
    }
    
    img.mlx = mlx_init();
    img.mlx_win = mlx_new_window(img.mlx, 900, 900, "Hello");
    img.img = mlx_new_image(img.mlx, 900, 900);
    img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
    mlx_put_image_to_window(img.mlx, img.mlx_win, img.img, 0, 0);
    render_map(&img, &img.map);
    // mlx_hook(img.mlx, 2, 1, key_hook, &img);
    // mlx_put_image_to_window(img.mlx, img.mlx_win, img.img, 0, 0);
    mlx_loop(img.mlx);
}