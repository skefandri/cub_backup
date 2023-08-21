/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 20:31:49 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/12 00:11:09 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

// char	**ft_malloc(t_split *split, t_info *data)
// {
// 	char	**resl;

// 	split->j = 0;
// 	split->i = 0;
// 	resl = malloc(sizeof(char *) * (data->row_len + 1));
// 	if (!resl)
// 		errmsg("malloc failed !");
// 	resl[data->row_len] = NULL;
// 	return (resl);
// }

// char	**ft_split(char *str, t_info *data)
// {
// 	t_split	split;

// 	split.resl = ft_malloc(&split, data);
// 	data->col_len = ft_sttrlen(str, '\n');
// 	while (split.resl && str[split.i])
// 	{
// 		split.y = 0;
// 		split.resl[split.j] = malloc(sizeof(char) * (data->col_len + 1));
// 		if (!split.resl[split.j])
// 			errmsg("malloc failed !");
// 		while (str[split.i + split.y] && str[split.i + split.y] != '\n')
// 		{
// 			split.resl[split.j][split.y] = str[split.i + split.y];
// 			split.y++;
// 		}
// 		split.resl[split.j][split.y] = '\0';
// 		if (!str[split.i + split.y])
// 			return (split.resl);
// 		split.i += split.y;
// 		split.j++;
// 		split.i++;
// 		if (str[split.i] && data->col_len != ft_sttrlen(&str[split.i], '\n'))
// 			errmsg ("The lines in the map are not compatible !");
// 	}
// 	return (split.resl);
// }
static int	count_word(const char *s, int c)
{
	int	count;
	int	i;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != c && (s[i + 1] == c || s[i + 1] == '\0'))
			count++;
		i++;
	}
	return (count);
}

static char	**allocate_word(const char *str, char c)
{
	int		count;
	char	**subword;

	if (!str)
		return (NULL);
	count = count_word(str, c);
	subword = (char **)ft_calloc((count + 1), sizeof(char *));
	if (!subword)
		return (NULL);
	return (subword);
}

char	**ft_split(char const *s, char c)
{
	int		size;
	char	**split;
	int		i;
	int		j;

	i = -1;
	split = allocate_word(s, c);
	if (!split)
		return (NULL);
	j = 0;
	while (++i < count_word(s, c))
	{
		size = 0;
		while (s[j] == c)
			j++;
		while (s[j + size] != c && s[j + size])
			size++;
		split[i] = (char *)ft_calloc(size + 1, sizeof(char));
		ft_strlcpy(split[i], &s[j], size + 1);
		j += size;
	}
	return (split);
}
