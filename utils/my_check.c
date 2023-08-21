// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   my_check.c                                         :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2023/08/01 22:39:34 by ysabr             #+#    #+#             */
// /*   Updated: 2023/08/19 21:27:05 by ysabr            ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "../includes/cub3d.h"

// void	ft_memset(void *pointer, int c, size_t len)
// {
// 	size_t			i;
// 	unsigned char	*temporary;

// 	temporary = (unsigned char *)pointer;
// 	i = 0;
// 	while (temporary && i < len)
// 	{
// 		temporary[i] = c;
// 		i++;
// 	}
// }

// void	*ft_memcpy(void *destination, void *source, size_t len)
// {
// 	size_t			i;
// 	unsigned char	*pointer1;
// 	unsigned char	*pointer2;

// 	if (!destination)
// 		return (source);
// 	if (!source)
// 		return (destination);
// 	pointer1 = (unsigned char *)destination;
// 	pointer2 = (unsigned char *)source;
// 	i = 0;
// 	while (i < len)
// 	{
// 		pointer1[i] = pointer2[i];
// 		i++;
// 	}
// 	return (destination);
// }

// void	*malloc(size_t size)
// {
// 	void		*new;
// 	uintptr_t	*ptr;

// 	new = malloc(size);
// 	if (!new)
// 		return (NULL);
// 	ft_memset(new, 0, size);
// 	if (g_lob.ptr == NULL)
// 	{
// 		g_lob.ptr = malloc(10 * sizeof(void *));
// 		if (!g_lob.ptr)
// 			return (NULL);
// 		g_lob.len = 10;
// 	}
// 	g_lob.ptr[g_lob.pos++] = (uintptr_t) new;
// 	if (g_lob.pos + 4 >= g_lob.len)
// 	{
// 		ptr = malloc(g_lob.len * 2 * sizeof(uintptr_t));
// 		ft_memcpy(ptr, g_lob.ptr, g_lob.len * sizeof(uintptr_t));
// 		free(g_lob.ptr);
// 		g_lob.ptr = ptr;
// 		g_lob.len *= 2;
// 	}
// 	return (new);
// }
