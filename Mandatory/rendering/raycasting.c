/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 16:31:54 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/20 20:58:26 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

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
