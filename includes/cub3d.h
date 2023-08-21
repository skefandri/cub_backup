/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 13:25:41 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/20 12:16:28 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

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

#endif