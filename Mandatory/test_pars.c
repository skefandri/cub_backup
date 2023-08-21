/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_pars.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 17:45:29 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/19 17:45:43 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

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
