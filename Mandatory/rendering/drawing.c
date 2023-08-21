/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   drawing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 16:21:37 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/21 09:51:01 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
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
                // Draw player as a smaller square in the middle of the grid square
                if ((y % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2 && y % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2) &&
                    (x % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2 && x % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2))
                    my_mlx_pixel_put(data, x, y, 0xFF0000); // Player color (red)
                else
                    my_mlx_pixel_put(data, x, y, 0x00FF00); // Background color (green)
            }
            else
                my_mlx_pixel_put(data, x, y, 0x00FF00); // Background color (green)
			x++;
        }
		y++;
    }
}

// void	render_grid(t_data *data, int x, int y)
// {
// 	if (y % CELL_SIZE == 0 || x % CELL_SIZE == 0)
// 		my_mlx_pixel_put(data, x, y, 0);
// }

// void	render_content(t_data *data, t_map *map, int x, int y)
// {
// 	if (map->map[y / CELL_SIZE][x / CELL_SIZE] == '1')
// 		my_mlx_pixel_put(data, x, y, 255);
// 	else if (y / CELL_SIZE == data->player.y && x / CELL_SIZE == data->player.x)
// 	{
// 		if ((y % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2
// 				&& y % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2)
// 			&& (x % CELL_SIZE >= (CELL_SIZE - PLAYER_SIZE) / 2
// 				&& x % CELL_SIZE < (CELL_SIZE + PLAYER_SIZE) / 2))
// 			my_mlx_pixel_put(data, x, y, 0xFF0000);
// 		else
// 			my_mlx_pixel_put(data, x, y, 0x00FF00);
// 	}
// 	else
// 		my_mlx_pixel_put(data, x, y, 0x00FF00);
// }

// void	render_map(t_data *data, t_map *map)
// {
// 	int x;
// 	int	y;

// 	y = -1;
// 	while (++y < map->row_len * CELL_SIZE)
// 	{
// 		x = -1;
// 		while (++x < map->col_len * CELL_SIZE)
// 		{
// 			render_grid(data, x, y);
// 			render_content(data, map, x, y);
// 		}
// 	}
// }

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
		ray_angle = player->direction + (FOV / 2) - ((FOV / 1000) * i);
		cast_ray(data, player, ray_angle);
		i++;
	}
}
