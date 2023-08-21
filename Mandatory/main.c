/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 08:36:38 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/20 11:53:59 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

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
