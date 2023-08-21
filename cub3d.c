
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


typedef struct s_plit
{
	int		i;
	int		j;
	int		y;
	char	**resl;
}			t_split;

//libft
int			ft_strlen(char *str);



//drawing
void    my_mlx_pixel_put(t_data *data, int x, int y, int color);
void	draw_wall_with_color(t_data *data, double from, double to, int j, int color);
void clear_window(t_data *data);
void render_map(t_data *data, t_map *map);
void draw_player(t_data *data, t_player *player);
void	draw_wall(t_data *data, double from, double to, int j);

//ray_casting
void cast_ray(t_data *data, t_player *player, double angle);
double	get_hight(double dis);
void	from_to(double *from, double *to, double hight);
bool is_ray_colliding(t_map *map, t_ray *ray);

//key_hook
void	rotate_player(t_player *player, double angle);
int		key_hook(int keycode, void *param);
bool	is_valid_position(t_map *map, int x, int y);
void	move_player(t_player *player, t_map *map, double direction);
void	free_map(t_map *map);

//main function and utils
void handle_error(char *message);
char **ft_split(char *str, int row_len, int col_len);
void calculate_map_size(char *buffer, int bytes_read, int *row_len, int *col_len);
void	read_map(char *filename, t_map *map);
void init_ray(t_ray *ray, t_player *player, double angle);

void initialize_game(t_data *img, char *map_file)
{
	img->j = 0;
	read_map(map_file, &img->map);
	img->player = (t_player){2 * 40 + 20, 2 * 40 + 20, M_PI / 4};
}

void	init_ray(t_ray *ray, t_player *player, double angel)
{
	ray->ray_x = player->x;
	ray->ray_y = player->y;
	ray->ray_dx = cos(angel);
	ray->ray_dy = sin(angel);
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		printf("Error\n");
		return 1;
	}
	t_data img;
	initialize_game(&img, av[1]);

	img.mlx = mlx_init();
	img.mlx_win = mlx_new_window()
}
// void	init_ray(t_ray *ray, t_player *player, double angel)
// {
// 	ray->ray_x = player->x;
// 	ray->ray_y = player->y;
// 	ray->ray_dx = cos(angel);
// 	ray->ray_dy = sin(angel);
// }

// int	main(int ac, char **av)
// {
// 	if (ac != 2)
// 	{
// 		printf("Error\n");
// 		return 1;
// 	}
// 	t_data img;
// 	initialize_game(&img, av[1]);

// 	img.mlx = mlx_init();
// 	img.mlx_win = mlx_new_window(img.mlx, 900, 990, "hello");
// 	img.img = mlx_new_image(img.mlx, 900, 990);
// 	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
// 	mlx_put_image_to_window(img.mlx, img.mlx_win, img.img, 0, 0);
// 	mlx_hook(img.mlx_win, 2, 1, key_hook, &img);
// 	mlx_put_image_to_window(img.mlx, img.mlx_win, img.img, 0, 0);
// 	mlx_loop(img.mlx);
// }