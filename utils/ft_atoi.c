/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 10:12:15 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/11 10:12:41 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	ft_sf(const char *str, int *sign)
{
	int	i;

	i = 0;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\r' || str[i] == '\n'
		|| str[i] == '\f' || str[i] == '\v')
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			*sign = -1;
		i++;
	}
	return (i);
}

int	ft_atoi(const char *str)
{
	int				i;
	int				sign;
	unsigned long	result;

	sign = 1;
	result = 0;
	i = ft_sf(str, &sign);
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (result > __LONG_MAX__)
			break ;
		result = (result * 10) + (str[i] - 48);
		i++;
	}
	if (result > __LONG_MAX__)
	{
		if (sign < 0)
			return (0);
		return (-1);
	}
	return (result * sign);
}
