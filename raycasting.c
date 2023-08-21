/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysabr <ysabr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 13:25:25 by ysabr             #+#    #+#             */
/*   Updated: 2023/08/10 19:12:05 by ysabr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "cub3d.h"

#include<math.h>
# include <stdint.h>
# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
# include <ctype.h>
# include <fcntl.h>
# include <string.h>
# include <stdio.h>
# include <unistd.h>

// typedef struct	s_data {
// 	void	*img;
// 	char	*addr;
// 	int		bits_per_pixel;
// 	int		line_length;
// 	int		endian;
// }				t_data;

// void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
// {
// 	char	*dst;

// 	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
// 	*(unsigned int*)dst = color;
// }

float A, B, C;
float cubeWidth = 20;
int with = 160, height = 44;
float zBuffer[160 * 44];
char buffer[160 * 44];
int distancefromCam = 100;
int backgroundASCIICode = ' ';
float incementSpeed = 0.6;
float x, y, z;
float ooz;
int xp, yp;
float K1 = 40;
int idx;

float calculateX(int i, int j, int k) {
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) + j * cos(A) * sin(C)
        + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(int i, int j, int k) {
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) - j * sin(A) * sin(B) * sin(C) 
        + k * cos(A) * sin(B) * sin(C) - i * cos(B) * sin(C);
}

float calculateZ(int i, int j, int k) {
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void    calculateForSurface(float cubex, float cubeY, float cubeZ, int ch)
{
    x = calculateX(cubex, cubeY, cubeZ);
    y = calculateY(cubex, cubeY, cubeZ);
    z = calculateZ(cubex, cubeY, cubeZ) + distancefromCam;
    
    ooz = 1/z;
    xp = (int) (with / 2 -2 * cubeWidth + K1 * ooz * x * 2);
    yp = (int)(height / 2 + K1 * ooz * y);

    idx = xp + yp * with;
    if ( idx >= 0 && idx < with * height)
    {
        if (ooz > zBuffer[idx]){
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

int main()
{
    printf("\x1b[2J]");
    while (1)
    {
        memset(buffer, backgroundASCIICode, with * height);
        memset(zBuffer, 0, with * height * 4);
        for ( float cubex = -cubeWidth; cubex < cubeWidth ; cubex += incementSpeed)
        {
            for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incementSpeed)
            {
                calculateForSurface(cubex, cubeY, -cubeWidth, '.');
                calculateForSurface(cubeWidth, cubeY, cubex, '$');
                calculateForSurface(-cubeWidth, cubeY, -cubex, '~');
                calculateForSurface(-cubex , cubeY, cubeWidth, '#');
                calculateForSurface(cubex , -cubeWidth, -cubeY, ';');
                calculateForSurface(cubex , cubeWidth, cubeY, '-');
            }
                
        }
        printf("\x1b[H]");
        for (int k = 0; k < with * height; k++)
        {
            putchar (k % with ? buffer[k] : 10);
        }

        A += 0.005;
        B += 0.005;
        usleep(1000);
    }
    return 0;
}