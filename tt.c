
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

typedef struct
{
	double x;
	double y;
	double direction;
} t_player;

typedef struct
{
	int row_len;
	int col_len;
	char **map;
} t_map;


int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	if (!str[i])
		return (i);
	while (str[i])
		i++;
	return (i);
}

void move_player(t_player *player, t_map *map, double direction);

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
}	t_data;

void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

typedef struct
{
	int dx;
	int dy;
	int sx;
	int sy;
	int err;
} t_line_params;

t_line_params initialize_line(int x0, int y0, int x1, int y1)
{
	t_line_params params;
	params.dx = abs(x1 - x0);
	params.dy = abs(y1 - y0);
		
	// Determine sx value
	if (x0 < x1)
		params.sx = 1;
	else
		params.sx = -1;
	// Determine sy value
	if (y0 < y1)
		params.sy = 1;
	else
		params.sy = -1;
	// Determine initial error value
	if (params.dx > params.dy)
		params.err = params.dx / 2;
	else
		params.err = -params.dy / 2;

	return params;
}

void draw_points(t_data *data, int *x0, int *y0, int x1, int y1, int color, t_line_params *params)
{
	int e2;

	while (1) {
		my_mlx_pixel_put(data, *x0, *y0, color);
		if (*x0 == x1 && *y0 == y1)
			break;
		e2 = params->err;
		// Adjust x0 and error value if needed
		if (e2 > -params->dx)
		{
			params->err -= params->dy;
			*x0 += params->sx;
		}
		// Adjust y0 and error value if needed
		if (e2 < params->dy)
		{
			params->err += params->dx;
			*y0 += params->sy;
		}
	}
}

typedef struct
{
	double ray_x;
	double ray_y;
	double ray_dx;
	double ray_dy;
	double distance;
} t_ray;

void draw_line(t_data *data, int x0, int y0, int x1, int y1, int color)
{
	t_line_params params = initialize_line(x0, y0, x1, y1);

	draw_points(data, &x0, &y0, x1, y1, color, &params);
}

// void cast_ray(t_data *data, t_player *player, double angle);
// double cast_ray(t_data *data, t_player *player, double angle);

#define FOV (M_PI / 3)

#define SCREEN_WIDTH  (40 * 25)
#define SCREEN_HEIGHT (40 * 13)

void init_ray(t_ray *ray, t_player *player, double angle)
{
	ray->ray_x = player->x;
	ray->ray_y = player->y;
	ray->ray_dx = cos(angle);
	ray->ray_dy = sin(angle);
}

bool is_ray_colliding(t_map *map, t_ray *ray)
{
	int map_x = (int)(ray->ray_x / CELL_SIZE);
	int map_y = (int)(ray->ray_y / CELL_SIZE);

	if (map_x < 0 || map_x >= map->col_len || map_y < 0 || map_y >= map->row_len)
		return true;
	return (map->map[map_y][map_x] == '1');
}
void cast_ray(t_data *data, t_player *player, double angle, int column)
{
	t_ray ray;

	init_ray(&ray, player, angle);
	while (!is_ray_colliding(&data->map, &ray))
	{
		ray.ray_x += ray.ray_dx;
		ray.ray_y -= ray.ray_dy;
	}
	(void)column;
	// ray.distance = sqrt((ray.ray_x - player->x) * (ray.ray_x - player->x) + 
	// 					(ray.ray_y - player->y) * (ray.ray_y - player->y));

	// // Correct the fisheye distortion by multiplying by the cosine of the difference
    // // between the ray's angle and the player's direction
	// ray.distance *= cos(player->direction - angle);

	// // Calculate the height of the wall slice on the screen
	// int line_height = (int)(SCREEN_HEIGHT / ray.distance);

	// // Calculate the starting and ending positions of the wall slice
	// int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
	// if (draw_start < 0) draw_start = 0;
	// int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
	// if (draw_end >= SCREEN_HEIGHT) draw_end = SCREEN_HEIGHT - 1;

	// // Draw the ceiling, wall, and floor for the column
	// for (int y = 0; y < SCREEN_HEIGHT; y++)
	// {
	// 	if (y < draw_start)
	// 	{
	// 		// Draw ceiling
	// 		my_mlx_pixel_put(data, column, y, 0x888888);  // A gray color for the ceiling
	// 	}
	// 	else if (y >= draw_start && y <= draw_end)
	// 	{
	// 		// Draw wall
	// 		my_mlx_pixel_put(data, column, y, 0xFFFFFF);  // White color for the wall
	// 	}
	// 	else
	// 	{
	// 		// Draw floor
	// 		my_mlx_pixel_put(data, column, y, 0x555555);  // A darker gray for the floor
	// 	}
	// }
}

// void draw_player(t_data *data, t_player *player)
// {
// 	for (int x = 0; x < SCREEN_WIDTH; x++)
// 	{
// 		double ray_angle = player->direction - (FOV / 2) + ((FOV / SCREEN_WIDTH) * x);
// 		cast_ray(data, player, ray_angle, x);
// 	}
// }

void draw_player(t_data *data, t_player *player)
{
	int player_size = 5;
	int y = player->y - player_size;
	while (y <= player->y + player_size)
	{
		int x = player->x - player_size;
		while (x <= player->x + player_size)
		{
			my_mlx_pixel_put(data, x, y, 0xFF0000);
			x++;
		}
		y++;
	}
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		double ray_angle = player->direction - (FOV / 2) + ((FOV / SCREEN_WIDTH) * x);
		cast_ray(data, player, ray_angle, x);
	}
}


void render_map(t_data *data, t_map *map)
{
	int x = 0, y = 0;
	while (y < map->row_len * CELL_SIZE)
	{
		x = 0;
		while (x < map->col_len * CELL_SIZE)
		{
			if (y % CELL_SIZE == 0 || x % CELL_SIZE == 0)
				my_mlx_pixel_put(data, x, y, 0);
		   	else if (map->map[y / CELL_SIZE][x / CELL_SIZE] == '1')
				my_mlx_pixel_put(data, x, y, 255);
			else if (y / CELL_SIZE == data->player.y && x / CELL_SIZE == data->player.x)
			{
				if ((y % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2 && y % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2) &&
					(x % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2 && x % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2))
					my_mlx_pixel_put(data, x, y, 0xFF0000);
				else
					my_mlx_pixel_put(data, x, y, 0x00FF00);
			}
			else
				my_mlx_pixel_put(data, x, y, 0x00FF00);
			x++;
		}
		y++;
	}
}



#define TURN_ANGLE (M_PI / 60)


void draw_direction_line(t_data *data, t_player *player)
{
	int line_length = 10;
	int x1 = player->x + cos(player->direction) * line_length;
	int y1 = player->y - sin(player->direction) * line_length;
	draw_line(data, player->x, player->y, x1, y1, 0x0000FF);
}

void rotate_player(t_player *player, double angle)
{
	player->direction += angle;

	while (player->direction < 0)
		player->direction += 2 * M_PI;
	while (player->direction >= 2 * M_PI)
		player->direction -= 2 * M_PI;
}

// void init_ray(t_ray *ray, t_player *player, double angle)
// {
// 	ray->ray_x = player->x;
// 	ray->ray_y = player->y;
// 	ray->ray_dx = cos(angle);
// 	ray->ray_dy = sin(angle);
// }

// bool is_ray_colliding(t_map *map, t_ray *ray)
// {
// 	int map_x = (int)(ray->ray_x / CELL_SIZE);
// 	int map_y = (int)(ray->ray_y / CELL_SIZE);

// 	if (map_x < 0 || map_x >= map->col_len || map_y < 0 || map_y >= map->row_len)
// 		return true;
// 	return (map->map[map_y][map_x] == '1');
// }




// void cast_ray(t_data *data, t_player *player, double angle)
// {
// 	t_ray ray;

// 	init_ray(&ray, player, angle);
// 	while (!is_ray_colliding(&data->map, &ray))
// 	{
// 		ray.ray_x += ray.ray_dx;
// 		ray.ray_y -= ray.ray_dy;
// 	}
// 	ray.distance = sqrt((ray.ray_x - player->x) * (ray.ray_x - player->x) + 
// 						(ray.ray_y - player->y) * (ray.ray_y - player->y));
// }

int key_hook(int keycode, void *param)
{
	t_data *data = (t_data *)param;
	printf("%d\n", keycode);
	if (keycode == 65307)
		exit(0);
	if (keycode == 115)
		 move_player(&data->player, &data->map, data->player.direction + M_PI);
	if (keycode == 119)
		move_player(&data->player, &data->map, data->player.direction);
	if (keycode == 100)
		move_player(&data->player, &data->map, data->player.direction - M_PI/2);
	if (keycode == 97)
		move_player(&data->player, &data->map, data->player.direction + M_PI/2);
	if (keycode == 65363)
		rotate_player(&data->player, -TURN_ANGLE);
	if (keycode == 65361)
		rotate_player(&data->player, TURN_ANGLE);
	render_map(data, &data->map);
	draw_player(data, &data->player);
	mlx_put_image_to_window(data->mlx, data->mlx_win, data->img, 0, 0);
	return 0;
}


bool is_valid_position(t_map *map, int x, int y)
{
	if (x < 0 || x >= map->col_len * CELL_SIZE || y < 0 || y >= map->row_len * CELL_SIZE)
		return false;
	if (map->map[y / CELL_SIZE][x / CELL_SIZE] == '1')
		return false;
	return true;
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


int main(int argc, char *argv[])
{
	void	*mlx;
	void	*mlx_win;
	t_data	img;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <map_file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	t_map map;
	read_map(argv[1], &map);
	t_player player = {2 * 40 + 20, 2 * 40 + 20, M_PI / 4};
	img.map = map;
	img.player = player;

	mlx = mlx_init();
	mlx_win = mlx_new_window(mlx, 40 * map.col_len, 40 * map.row_len, "Hello World!");
	img.mlx = mlx;
	img.mlx_win = mlx_win;
	img.img = mlx_new_image(mlx, 40 * map.col_len, 40 * map.row_len);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	render_map(&img, &map);
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0,0);
	mlx_hook(mlx_win, 2, 1L << 0, key_hook, &img);
	render_map(&img, &map);
	draw_player(&img, &player);
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0, 0);
	mlx_loop(mlx);
	return (EXIT_SUCCESS);
}
