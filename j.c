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
#define MINIMAP_SCALE 0.2
#define MINIMAP_CELL_SIZE (CELL_SIZE * MINIMAP_SCALE)
#define MINIMAP_PLAYER_SIZE (PLAYER_SIZE * MINIMAP_SCALE)

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
    int last_mouse_x;
    double rotation_speed;
    int mini_map_scale;
}	t_data;

void move_player(t_player *player, t_map *map, double direction);

enum Colors {
    COLOR_BLACK = 0,
    COLOR_WHITE = 0xFFFFFF,  // Corrected to represent white in RGB
    COLOR_RED = 0xFF0000,
    COLOR_GREEN = 0x00FF00,
    COLOR_BLUE = 0x0000FF
};


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
		if (!str[i + y])
            return resl;
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

void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char *dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}

bool is_player_position(int x, int y, t_data *data)
{
    return y / CELL_SIZE == data->player.y && x / CELL_SIZE == data->player.x;
}


bool is_inside_player_boundary(int x, int y)
{
    return (y % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2 && y % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2) &&
           (x % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2 && x % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2);
}

void cast_ray(t_data *data, t_player *player, double angle);
void draw_player(t_data *data, t_player *player)
{
    double ray_angle;
    for (int i = 0; i < 1000; i++)
    {
        ray_angle = player->direction - (FOV / 2) + ((FOV / 1000) * i);
        cast_ray(data, player, ray_angle);
    }
}

void render_map(t_data *data, t_map *map)
{
    for (int y = 0; y < map->row_len * CELL_SIZE; y++)
    {
        for (int x = 0; x < map->col_len * CELL_SIZE; x++)
        {
            // Drawing grid
            if (y % CELL_SIZE == 0 || x % CELL_SIZE == 0)
            {
                my_mlx_pixel_put(data, x, y, COLOR_BLACK);
                continue;  // We continue to the next loop iteration so that the grid color is not overridden
            }
            
            // Drawing walls and doors
            if (map->map[y / CELL_SIZE][x / CELL_SIZE] == '1')
                my_mlx_pixel_put(data, x, y, COLOR_WHITE);
            if (map->map[y / CELL_SIZE][x / CELL_SIZE] == 'D') // Closed door
                my_mlx_pixel_put(data, x, y, COLOR_RED);
            else if (map->map[y / CELL_SIZE][x / CELL_SIZE] == 'd') // Open door
                my_mlx_pixel_put(data, x, y, COLOR_BLUE);
            // Drawing player
            else if (is_player_position(x, y, data) && is_inside_player_boundary(x, y))
                my_mlx_pixel_put(data, x, y, COLOR_BLACK);
            else
                my_mlx_pixel_put(data, x, y, COLOR_GREEN);  // Background color
        }
    }
}




void rotate_player(t_player *player, double angle)
{
    player->direction = fmod(player->direction + angle, 2 * M_PI);
    if (player->direction < 0)
        player->direction += 2 * M_PI;
}

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

double get_hight(double distance)
{
    const double r_dis = (HIGHT / 2) * tan(0.5);
    return (r_dis * 40) / distance;
}

void set_boundaries(double *from, double *to, double height)
{
    *from = (HIGHT / 2) - height / 2;
    *to = (HIGHT / 2) + height / 2;
    
    if (*from < 0) *from = 0;
    if (*to > HIGHT) *to = HIGHT;
}

double calculate_distance(t_ray ray, t_player *player, double angle)
{
    return sqrt((ray.ray_x - player->x) * (ray.ray_x - player->x) + 
                (ray.ray_y - player->y) * (ray.ray_y - player->y)) * 
                cos(player->direction - angle);
}
void	draw_wall(t_data *data, double from, double to, int j)
{
	int	i = 0;
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
void clear_window(t_data *data)
{
    int x, y;
    for (y = 0; y < HIGHT; y++)
    {
        for (x = 0; x < WIDTH; x++)
        {
            my_mlx_pixel_put(data, x, y, 0x000000);
        }
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

void	draw_wall_with_color(t_data *data, double from, double to, int j, int color)
{
	int	i = 0;
	if (data->j >= WIDTH) 
    	data->j = 0;
	while (i < from)
	{
		my_mlx_pixel_put(data, j, i, 0xEFDEAB);
		i++;
	}
	while (i < to)
	{
		my_mlx_pixel_put(data, j, i, color);  // Using the provided color
		i++;
	}
	while (i < HIGHT)
	{
		my_mlx_pixel_put(data, j, i, 0x0F0FF0);
		i++;
	}
}


void cast_ray(t_data *data, t_player *player, double angle)
{
    t_ray ray;
    double from, to;
    int wall_color = 0xFFEEEE; // Default wall color

    init_ray(&ray, player, angle);
    while (!is_ray_colliding(&data->map, &ray))
    {
        if (data->map.map[(int)(ray.ray_y / CELL_SIZE)][(int)(ray.ray_x / CELL_SIZE)] == 'D') 
        {
            // Closed door detected
            wall_color = COLOR_RED; // Change wall color to red for door
            break;
        }
        ray.ray_x += ray.ray_dx;
        ray.ray_y -= ray.ray_dy;
    }

    double distance = calculate_distance(ray, player, angle);
    double height = get_hight(distance);

    set_boundaries(&from, &to, height);
    draw_wall_with_color(data, from, to, data->j, wall_color);  // Modify the draw_wall function to accept color
    data->j++;
    if (data->j >= WIDTH)
        data->j = 0;
}

void draw_minimap(t_data *data);
int key_hook(int keycode, void *param)
{
	t_data *data = (t_data *)param;
	printf("%d\n", keycode);

	if (keycode == 65307) // Esc key
	{
		free_map(&data->map);
		exit(0);
	}
	else if (keycode == 115)
		move_player(&data->player, &data->map, data->player.direction + M_PI);
	else if (keycode == 119)
		move_player(&data->player, &data->map, data->player.direction);
	else if (keycode == 100)
		move_player(&data->player, &data->map, data->player.direction - M_PI/2);
	else if (keycode == 97)
		move_player(&data->player, &data->map, data->player.direction + M_PI/2);
	else if (keycode == 65363)
		rotate_player(&data->player, TURN_ANGLE);
	else if (keycode == 65361)
		rotate_player(&data->player, -TURN_ANGLE);
	clear_window(data);
	draw_player(data, &data->player);
	draw_minimap(data);
	mlx_put_image_to_window(data->mlx, data->mlx_win, data->img, 0, 0);
	return 0;
}

bool is_walkable_cell(int x, int y, t_map *map)
{
    int map_x = x / CELL_SIZE;
    int map_y = y / CELL_SIZE;

    if (map_x < 0 || map_x >= map->col_len || map_y < 0 || map_y >= map->row_len)
        return false;
    return (map->map[map_y][map_x] != '1');
}

void move_player(t_player *player, t_map *map, double direction)
{
    // Calculate new potential position based on the direction
    double new_x = player->x + cos(direction) * MOVE_SPEED;
    double new_y = player->y - sin(direction) * MOVE_SPEED;

    if (is_walkable_cell(new_x, new_y, map))
    {
        player->x = new_x;
        player->y = new_y;
    }
}



// void mlx_draw_line(void *mlx, void *win, int x0, int y0, int x1, int y1, int color) {
//     int dx = abs(x1 - x0);
//     int dy = abs(y1 - y0);
//     int sx = (x0 < x1) ? 1 : -1;
//     int sy = (y0 < y1) ? 1 : -1;
//     int err = (dx > dy ? dx : -dy) / 2;
//     int e2;

//     while (true) {
//         // Instead of the previous pixel_put, use this to directly interface with MinilibX
//         mlx_pixel_put(mlx, win, x0, y0, color);
//         if (x0 == x1 && y0 == y1) break;
//         e2 = err;
//         if (e2 > -dx) { err -= dy; x0 += sx; }
//         if (e2 < dy) { err += dx; y0 += sy; }
//     }
// }

// bool is_door(int x, int y, t_map *map)
// {
//     int map_x = x / CELL_SIZE;
//     int map_y = y / CELL_SIZE;

//     if (map->map[map_y][map_x] == 'D')
//         return true;
//     return false;
// }

// void toggle_door(int x, int y, t_map *map)
// {
//     int map_x = x / CELL_SIZE;
//     int map_y = y / CELL_SIZE;

//     if (map->map[map_y][map_x] == 'D')
//         map->map[map_y][map_x] = 'd';
//     else if (map->map[map_y][map_x] == 'd')
//         map->map[map_y][map_x] = 'D';
// }


void draw_minimap(t_data *data)
{
    for (int y = 0; y < data->map.row_len * CELL_SIZE; y += data->mini_map_scale)
    {
        for (int x = 0; x < data->map.col_len * CELL_SIZE; x += data->mini_map_scale)
        {
            int color = COLOR_GREEN;

            if (data->map.map[y / CELL_SIZE][x / CELL_SIZE] == '1')
                color = COLOR_WHITE;
            else if (data->map.map[y / CELL_SIZE][x / CELL_SIZE] == 'D')
                color = COLOR_RED;
            else if (data->map.map[y / CELL_SIZE][x / CELL_SIZE] == 'd')
                color = COLOR_BLUE;

            // Check if the current position is near the player to draw the player
            if (fabs(x - data->player.x) <= PLAYER_SIZE/2 && fabs(y - data->player.y) <= PLAYER_SIZE/2) {
                color = COLOR_BLACK;
            }

            // Draw the pixel on the minimap
            my_mlx_pixel_put(data, x / data->mini_map_scale, y / data->mini_map_scale, color);
        }
    }
}

int mouse_move(int x, int y, void *param)
{
    (void)y;
    t_data *data = (t_data *)param;

    // Calculate difference in mouse movement
    int mouse_diff = x - data->last_mouse_x;
    
    // Adjust the player's direction based on mouse movement
    rotate_player(&data->player, mouse_diff * data->rotation_speed);

    // Update the last_mouse_x for future calculations
    data->last_mouse_x = x;

    // Instead of clearing the window every time, you will just redraw the necessary parts
    draw_player(data, &data->player);
    draw_minimap(data);
    mlx_put_image_to_window(data->mlx, data->mlx_win, data->img, 0, 0);

    return 0;
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
	img.j = 0;
    img.mini_map_scale = 5;
    img.last_mouse_x = WIDTH / 2;
    img.rotation_speed = 0.005;
	read_map(argv[1], &map);
	t_player player = {2 * 40 + 20, 2 * 40 + 20, M_PI / 4};
	img.map = map;
	img.player = player;

	mlx = mlx_init();
	mlx_win = mlx_new_window(mlx, HIGHT, WIDTH, "Hello World!");
	img.mlx = mlx;
	img.mlx_win = mlx_win;
	img.img = mlx_new_image(mlx, HIGHT, WIDTH);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0,0);
	mlx_hook(mlx_win, 2, 1, key_hook, &img);
    mlx_hook(img.mlx_win, 6, (1L<<6), mouse_move, &img);
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0, 0);
	mlx_loop(mlx);
	free_map(&map);

	return (EXIT_SUCCESS);
}
