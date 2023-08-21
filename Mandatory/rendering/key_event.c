/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_event.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 16:39:49 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/20 20:54:07 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

void	rotate_player(t_player *player, double angle)
{
	player->direction += angle;
	while (player->direction < 0)
		player->direction += 2 * M_PI;
	while (player->direction  >= 2 * M_PI)
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
	printf("%d\n", keycode);
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

void free_map(t_map *map)
{
	int	i;

	i = 0;
    while (i < map->row_len)
        free(map->map[i++]);
    free(map->map);
}

