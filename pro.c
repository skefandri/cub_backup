
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
void	my_mlx_pixel_put(t_data *data, int x, int y, int color);
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


void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

void	render_grid(t_data *data, int x, int y)
{
	if (y % CELL_SIZE == 0 || x % CELL_SIZE == 0)
		my_mlx_pixel_put(data, x, y, 0);
}

void	render_content(t_data *data, t_map *map, int x, int y)
{
	if (map->map[y / CELL_SIZE][x / CELL_SIZE] == '1')
		my_mlx_pixel_put(data, x, y, 255);
	else if (y / CELL_SIZE == data->player.y && x / CELL_SIZE == data->player.x)
	{
		if ((y % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2
				&& y % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2)
			&& (x % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2
				&& x % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2))
			my_mlx_pixel_put(data, x, y, 0xFF0000);
		else
			my_mlx_pixel_put(data, x, y, 0x00FF00);
	}
	else
		my_mlx_pixel_put(data, x, y, 0x00FF00);
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

void	draw_wall(t_data *data, double from, double to, int j)
{
	int	i;

	i = 0;
	if (data->j >= WIDTH)
		data->j = 0;
	while (i < from)
	{
		my_mlx_pixel_put(data, j, i, 0xEFDEAB);
		i++;
	}
	while (i < to)
	{
		my_mlx_pixel_put(data, j, i, 0xFFEEEE);
		i++;
	}
	while (i < HIGHT)
	{
		my_mlx_pixel_put(data, j, i, 0x0F0FF0);
		i++;
	}
}

void	clear_window(t_data *data)
{
	int	x;
	int	y;
	
	y = -1;
	while (++y < HIGHT)
	{
		x = -1;
		while (++x < WIDTH)
			my_mlx_pixel_put(data, x, y, 0x000000);
	}
}

void	draw_player(t_data *data, t_player *player)
{
	int	player_size;
	int	y;
	int	x;

	player_size = 5;
	y = player->y - player_size;
	while (y <= player->y + player_size)
	{
		x = player->x - player_size;
		while (x <= player->x + player_size)
			x++;
		y++;
	}
	int i = 0;
	double ray_angle;
	while(i < 1000)
	{
		ray_angle = player->direction - (FOV / 2) + ((FOV / 1000) * i);
		cast_ray(data, player, ray_angle);
		i++;
	}
}

void	rotate_player(t_player *player, double angle)
{
	player->direction += angle;
	while (player->direction < 0)
		player->direction += 2 * M_PI;
	while (player->direction >= 2 * M_PI)
		player->direction -= 2 * M_PI;
}

int key_hook(int keycode, void *param)
{
	t_data *data = (t_data *)param;
	if (keycode == 65307)
	{
		free_map(&data->map);
		exit(0);
	}
	if (keycode == 115)
		 move_player(&data->player, &data->map, data->player.direction + M_PI);
	if (keycode == 119)
		move_player(&data->player, &data->map, data->player.direction);
	if (keycode == 100)
		move_player(&data->player, &data->map, data->player.direction - M_PI/2);
	if (keycode == 97)
		move_player(&data->player, &data->map, data->player.direction + M_PI/2);
	if (keycode == 65363)
		rotate_player(&data->player, TURN_ANGLE);
	if (keycode == 65361)
		rotate_player(&data->player, -TURN_ANGLE);
	clear_window(data);
	draw_player(data, &data->player);
	mlx_put_image_to_window(data->mlx, data->mlx_win, data->img, 0, 0);
	return 0;
}

bool is_valid_position(t_map *map, int x, int y)
{
	if (x < 0 || x >= map->col_len * CELL_SIZE || y < 0 || y >= map->row_len * CELL_SIZE)
		return (false);
	if (map->map[y / CELL_SIZE][x / CELL_SIZE] == '1')
		return (false);
	return (true);
}


void move_player(t_player *player, t_map *map, double direction)
{
	double new_x = player->x + cos(direction) * MOVE_SPEED;
	double new_y = player->y - sin(direction) * MOVE_SPEED;

	if (is_valid_position(map, new_x, new_y))
	{
		player->x = new_x;
		player->y = new_y;
	}
}

void free_map(t_map *map)
{
	int	i;

	i = 0;
	while (i < map->row_len)
		free(map->map[i++]);
	free(map->map);
}

void cast_ray(t_data *data, t_player *player, double angle)
{
	t_ray	ray;
	double	from;
	double	to;
	double	hight;

	init_ray(&ray, player, angle);
	while (!is_ray_colliding(&data->map, &ray))
	{
		ray.ray_x += ray.ray_dx;
		ray.ray_y -= ray.ray_dy;
	}
	ray.distance = sqrt((ray.ray_x - player->x) * (ray.ray_x - player->x) + 
					 (ray.ray_y - player->y) * (ray.ray_y - player->y)) 
					 * cos(player->direction - angle);
	hight = get_hight(ray.distance);
	from_to(&from, &to, hight);
	draw_wall(data, from, to, data->j);
	data->j++;
}

double	get_hight(double dis)
{
	double r_dis = (HIGHT / 2) * tan(0.5);
	double hight = (r_dis * 40) / dis;
	return (hight); 
}

void	from_to(double *from, double *to, double hight)
{
	*from = (HIGHT / 2) - hight / 2;
	*to = (HIGHT / 2) + hight / 2;
	if (*from < 0)
		*from = 0;
	if (*to > HIGHT)
		*to = HIGHT;
}

bool is_ray_colliding(t_map *map, t_ray *ray)
{
	int map_x = (int)(ray->ray_x / CELL_SIZE);
	int map_y = (int)(ray->ray_y / CELL_SIZE);

	if (map_x < 0 || map_x >= map->col_len || map_y < 0 || map_y >= map->row_len)
		return true;
	return (map->map[map_y][map_x] == '1');
}

int	ft_strlen(char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
		i++;
	return (i);
}




void init_ray(t_ray *ray, t_player *player, double angle)
{
	ray->ray_x = player->x;
	ray->ray_y = player->y;
	ray->ray_dx = cos(angle);
	ray->ray_dy = sin(angle);
}

void initialize_game(t_data *img, char *map_file)
{
	img->j = 0;
	read_map(map_file, &img->map);
	img->player = (t_player){2 * 40 + 20, 2 * 40 + 20, M_PI / 4};
}

int initialize_graphics(t_data *img)
{
	img->mlx = mlx_init();
	if (!img->mlx)
		return (0);
	img->mlx_win = mlx_new_window(img->mlx, HIGHT, WIDTH, "Hello World!");
	if (!img->mlx_win)
		return (0);
	img->img = mlx_new_image(img->mlx, HIGHT, WIDTH);
	if (!img->img)
		return (0);
	img->addr = mlx_get_data_addr(img->img, &img->bits_per_pixel, &img->line_length, &img->endian);
	if (!img->addr)
		return (0);
	return (1);
}

int main(int argc, char *argv[])
{
	t_data  img;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <map_file>\n", argv[0]);
		return (EXIT_FAILURE);
	}
	initialize_game(&img, argv[1]);
	if (!initialize_graphics(&img))
	{
		fprintf(stderr, "Error initializing graphics.\n");
		return (EXIT_FAILURE);
	}
	mlx_put_image_to_window(img.mlx, img.mlx_win, img.img, 0, 0);
	mlx_hook(img.mlx_win, 2, 1, key_hook, &img);
	mlx_put_image_to_window(img.mlx, img.mlx_win, img.img, 0, 0);
	mlx_loop(img.mlx);
	free_map(&img.map);
	return (EXIT_SUCCESS);
}
