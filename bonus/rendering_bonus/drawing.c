/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   drawing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 14:54:22 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/19 17:51:46 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void    my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char *dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
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
		my_mlx_pixel_put(data, j, i, color);
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

void draw_player(t_data *data, t_player *player)
{
    double ray_angle;
    for (int i = 0; i < 1000; i++)
    {
        ray_angle = player->direction - (FOV / 2) + ((FOV / 1000) * i);
        cast_ray(data, player, ray_angle);
    }
}
