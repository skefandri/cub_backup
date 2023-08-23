
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
#define TURN_ANGLE (M_PI / 90)

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

void draw_line(t_data *data, int x0, int y0, int x1, int y1, int color) 
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int steps;
	(void)data;
	(void)color;

	if (abs(dx) > abs(dy))
		steps = abs(dx);
	else 
		steps = abs(dy);
		
	if (steps == 0)
		return; // Avoid division by zero

	float xInc = dx / (float)steps;
	float yInc = dy / (float)steps;
	float x = x0;
	float y = y0;

	int i = 0;
	while (i <= steps) 
	{
		my_mlx_pixel_put(data, round(x), round(y), color);
		x += xInc;
		y += yInc;
		i++;
	}
}

void cast_ray(t_data *data, t_player *player, double angle);

#define FOV (M_PI / 3)
void draw_player(t_data *data, t_player *player)
{
	int player_size = PLAYER_SIZE;
	int y = player->y - player_size / 2;
	while (y <= player->y + player_size / 2)
	{
		int x = player->x - player_size / 2;
		while (x <= player->x + player_size / 2)
		{
			my_mlx_pixel_put(data, x, y, 0xFF0000);
			x++;
		}
		y++;
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



// #define TURN_ANGLE (M_PI / 90)


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

	// while (player->direction < 0)
	// 	player->direction += 2 * M_PI;
	// while (player->direction >= 2 * M_PI)
	// 	player->direction -= 2 * M_PI;
}

typedef struct
{
	double ray_x;
	double ray_y;
	double ray_dx;
	double ray_dy;
} t_ray;

void init_ray(t_ray *ray, t_player *player, double angle)
{
	ray->ray_x = player->x;
	ray->ray_y = player->y;
	ray->ray_dx = cos(angle);
	ray->ray_dy = sin(angle);
}

bool	is_ray_colliding(t_map *map, t_ray *ray)
{
	int map_x = (int)(ray->ray_x / CELL_SIZE);
	int map_y = (int)(ray->ray_y / CELL_SIZE);

	if (map_x < 0 || map_x >= map->col_len || map_y < 0 || map_y >= map->row_len)
		return true;
	return (map->map[map_y][map_x] == '1');
}

void render_rays(t_data *data, t_player *player)
{
	int i = 0;
	double ray_angle;
	while(i < 1000)
	{
		ray_angle = player->direction - (FOV / 2) + ((FOV / 1000) * i);
		cast_ray(data, player, ray_angle);
		i++;
	}
}

#include <limits.h>


typedef struct s_intersection {
	double x;
	double y;
	double distance;
	int hit;
} t_intersection;


void	initialize_intersections(t_intersection *horizontal, t_intersection *vertical)
{
	horizontal->x = 0;
	horizontal->y = 0;
	horizontal->distance = INT_MAX;
	horizontal->hit = 0;

	vertical->x = 0;
	vertical->y = 0;
	vertical->distance = INT_MAX;
	vertical->hit = 0;
}

void find_horizontal_intersection(t_data *data, t_ray ray, double angle, t_intersection *horizontal)
{
	// (void)horizontal;
	double xinter, yinter;
	double stepx;
	double stepy;

	stepy = CELL_SIZE;
	stepx = (CELL_SIZE / tan(angle));
	yinter = (int)(ray.ray_y / CELL_SIZE) * CELL_SIZE;
	if (sin(angle) > 0)
		stepy *= -1;
	else
	{
		stepx *= -1;
		yinter += CELL_SIZE;
	}
	xinter = ray.ray_x + (ray.ray_y - yinter) / tan(angle);
	while(1)
	{
		// my_mlx_pixel_put(data, xinter, yinter, 0xffffff);
		if (xinter < 0 || yinter < 0 || xinter >= data->map.col_len * CELL_SIZE || yinter >= data->map.row_len * CELL_SIZE)
			break ;
		if (stepy < 0 && data->map.map[(int)yinter/ CELL_SIZE - 1][(int)xinter / CELL_SIZE] == '1')
			break ;
		else if (data->map.map[(int)yinter/ CELL_SIZE][(int)xinter / CELL_SIZE] == '1')
			break ;
		xinter += stepx;
		yinter += stepy;
	}
	horizontal->x = xinter;
	horizontal->y = yinter;
	horizontal->distance = sqrt((ray.ray_x - horizontal->x) * (ray.ray_x - horizontal->x) +
							(ray.ray_y - horizontal->y) * (ray.ray_y - horizontal->y)); 
}


void	find_vertical_intersection(t_data *data, t_ray ray, double angle, t_intersection *vertical)
{
    double xinter, yinter;
    double stepx;
    double stepy;

    xinter = (int)(ray.ray_x / CELL_SIZE) * CELL_SIZE;
    stepx = CELL_SIZE;
    stepy = CELL_SIZE * tan(angle);
    
    if (cos(angle) < 0)
    {
        stepx *= -1;
    }
    else
	{
        stepy *= -1;
        xinter += CELL_SIZE;
	}
    yinter = ray.ray_y + (ray.ray_x - xinter) * tan(angle);
    while (1)
    {
		if (xinter < 0 || yinter < 0 || xinter >= data->map.col_len * CELL_SIZE || yinter >= data->map.row_len * CELL_SIZE)
			break ;
        // my_mlx_pixel_put(data, xinter, yinter, 0xff0000);
		if (stepx < 0 && data->map.map[(int)yinter / CELL_SIZE][(int)xinter / CELL_SIZE - 1] == '1')
			break ;
		else if (data->map.map[(int)yinter/ CELL_SIZE][(int)xinter / CELL_SIZE] == '1')
			break;

        xinter += stepx;
        yinter += stepy;
    }
	vertical->x = xinter;
	vertical->y = yinter;
    vertical->distance = sqrt((ray.ray_x - vertical->x) * (ray.ray_x - vertical->x) +
                              (ray.ray_y - vertical->y) * (ray.ray_y - vertical->y)); // Corrected this line
}

void cast_ray(t_data *data, t_player *player, double angle)
{
	t_ray ray;
	t_intersection horizontal, vertical;

	angle = remainder(angle, M_PI * 2);
	if (angle < 0)
		angle += 2 * M_PI;
	init_ray(&ray, player, angle);
	initialize_intersections(&horizontal, &vertical);
	find_horizontal_intersection(data, ray, angle, &horizontal);
	find_vertical_intersection(data, ray, angle, &vertical);
	if (horizontal.distance < vertical.distance)
		draw_line(data, ray.ray_x, ray.ray_y, horizontal.x, horizontal.y, 0xf00);
	else
		draw_line(data, ray.ray_x, ray.ray_y, vertical.x, vertical.y, 0xf00);
}

// void find_vertical_intersection(t_data *data, t_ray ray, double angle, t_intersection *vertical)
// {
//     double xstep_Vert, ystep_Vert, xintercept, yintercept;

//     double is_ray_facing_down, is_ray_facing_up, is_ray_facing_right, is_ray_facing_left;

//     if (angle < M_PI) {
//         is_ray_facing_down = 1;
//         is_ray_facing_up = 0;
//     } else {
//         is_ray_facing_down = 0;
//         is_ray_facing_up = 1;
//     }

//     if (angle < (M_PI / 2) || angle > (1.5 * M_PI)) {
//         is_ray_facing_right = 1;
//         is_ray_facing_left = 0;
//     } else {
//         is_ray_facing_right = 0;
//         is_ray_facing_left = 1;
//     }

//     xintercept = ((int)(ray.ray_x / CELL_SIZE)) * CELL_SIZE;
//     if (is_ray_facing_right) {
//         xintercept += CELL_SIZE;
//     }

//     yintercept = ray.ray_y + ((xintercept - ray.ray_x) * tan(angle));

//     xstep_Vert = CELL_SIZE;
//     if (is_ray_facing_left) {
//         xstep_Vert *= -1;
//     }
//     ystep_Vert = CELL_SIZE * tan(angle);

//     if (is_ray_facing_up && ystep_Vert > 0) {
//         ystep_Vert *= -1;
//     } else if (is_ray_facing_down && ystep_Vert < 0) {
//         ystep_Vert *= -1;
//     }
// 	while (1)
// 	{
// 		int mapX = (int)(xintercept / CELL_SIZE);
// 		int mapY = (int)(yintercept / CELL_SIZE);

// 		if (mapX < 0 || mapX >= data->map.col_len || mapY < 0 || mapY >= data->map.row_len)
// 			break;

// 		if (data->map.map[mapY][mapX] == '1')
// 		{
// 			vertical->hit = 1;
// 			vertical->x = xintercept;
// 			vertical->y = yintercept;
// 			break;
// 		}
// 		else
// 		{
// 			xintercept += xstep_Vert;
// 			yintercept += ystep_Vert;
// 		}
// 	}
// 	vertical->distance = sqrt((ray.ray_x - vertical->x) * (ray.ray_x - vertical->x) +
// 							  (ray.ray_y - vertical->y) * (ray.ray_y - vertical->y));
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
		rotate_player(&data->player, +TURN_ANGLE);
	render_map(data, &data->map);
	draw_player(data, &data->player);
	// cast_ray(data, &data->player, data->player.direction);
	render_rays(data, &data->player);
	// draw_line(data, data->player.x , data->player.y, data->player.x + 20 * cos(data->player.direction), data->player.y - 20 * sin(data->player.direction) , 255);
	mlx_put_image_to_window(data->mlx, data->mlx_win, data->img, 0, 0);
	return 0;
}

bool is_valid_position(t_map *map, int x, int y)
{
	if (x < 0 || x >= map->col_len * CELL_SIZE || y < 0
		|| y >= map->row_len * CELL_SIZE)
		return (false);
	if (map->map[y / CELL_SIZE][x / CELL_SIZE] == '1')
		return (false);
	return (true);
}

void move_player(t_player *player, t_map *map, double direction)
{
	double	new_x;
	double	new_y;

	new_x = player->x + cos(direction) * MOVE_SPEED;
	new_y = player->y - sin(direction) * MOVE_SPEED;
	
	if (is_valid_position(map, new_x, new_y)
		&& is_valid_position(map, new_x, player->y)
		&& is_valid_position(map, player->x, new_y))
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
	t_player player = {2 * CELL_SIZE + 20, 2 * CELL_SIZE + 20, M_PI / 4};
	img.map = map;
	img.player = player;

	mlx = mlx_init();
	mlx_win = mlx_new_window(mlx, CELL_SIZE * map.col_len, CELL_SIZE * map.row_len, "Hello World!");
	img.mlx = mlx;
	img.mlx_win = mlx_win;
	img.img = mlx_new_image(mlx, CELL_SIZE * map.col_len, CELL_SIZE * map.row_len);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	render_map(&img, &map);
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0,0);
	mlx_hook(mlx_win, 2, 1L << 0, key_hook, &img);
	render_map(&img, &map);
	draw_player(&img, &player);
	// cast_ray(&img, &img.player, img.player.direction);
	render_rays(&img, &img.player);
	draw_line(&img, img.player.x , img.player.y, img.player.x + 20 * cos(img.player.direction), img.player.y - 20 * sin(img.player.direction) , 255);
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0, 0);
	mlx_loop(mlx);
	return (EXIT_SUCCESS);
}
