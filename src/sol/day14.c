/*
 * Day 14 -- Regolith Reservoir
 */
#include <search.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_X 1000
#define MAX_Y 1000

#include "aoc.h"

static void day14_sol_func (FILE*, FILE*, FILE*);
static int max (int, int);
static int min (int, int);
static void print_field (FILE*, int, int, int, int);

static struct aoc_sol day14_sol = {
	.name = "day14",
	.sol  = &day14_sol_func
};

static char field[MAX_X][MAX_Y];

void __attribute__((constructor)) day14_init (void)
{
	if (!register_sol (day14_sol))
		fprintf (stderr, "day14 load failed.\n");
}

void day14_sol_func (FILE *in_f, FILE *out_f, FILE *debug_out)
{
	char  *line;
	char  *tmp;
	int    i, j;
	int    maxy;
	int    prevx, prevy;
	int    x, y;
	size_t drops;
	size_t nread;

	drops = 0;
	line  = NULL;
	maxy  = -1;
	prevx = -1;
	prevy = -1;

	for (i = 0; i < MAX_X; ++i)
	{
		for (j = 0; j < MAX_Y; ++j)
			field[i][j] = '.';
	}

	while (getline (&line, &nread, in_f) != -1)
	{
		prevx = -1;
		prevy = -1;
		rtrim (line);
		tmp = line;

		while (tmp)
		{
			sscanf(tmp, "%d,%d", &x, &y);

			if (prevx > 0)
			{
				if (y == prevy)
				{
					for (i = min(x, prevx); i <= max(x, prevx); ++i)
						field[i][y] = '#';
				} else if (x == prevx)
				{
					for (i = min(y, prevy); i <= max(y, prevy); ++i)
						field[x][i] = '#';
				} else
					abort();
			}

			prevx = x;
			prevy = y;
			maxy = max(y, maxy);

			tmp = strstr (tmp, "-> ");

			if (tmp)
				tmp += 3;
		}
	}
	free (line);

	print_field (debug_out, 490, 20, 0, 15);

	x = 500;
	y = 0;
	for (;;)
	{
		if (y == maxy)
			break;
		else if (field[x][y + 1] == '.')
			++y;
		else if (x > 0 && field[x - 1][y + 1] == '.')
		{
			--x;
			++y;
		} else if (x < (MAX_X - 1) && field[x + 1][y + 1] == '.')
		{
			++x;
			++y;
		} else 
		{
			field[x][y] = 'o';
			++drops;
			x = 500;
			y = 0;
		}

	}

	fprintf (out_f, "%ld\n", drops);
	for (i = 0; i < MAX_X; ++i)
	{
		for (j = 0; j < MAX_Y; ++j)
			if (field[i][j] == 'o')
				field[i][j] = '.';
	}

	for (i = 0; i < MAX_X; ++i)
	{
		field[i][maxy + 2] = '=';
	}

	drops = 0;
	x = 500;
	y = 0;
	for (;;)
	{
		if (field[x][y + 1] == '.')
			++y;
		else if (x > 0 && field[x - 1][y + 1] == '.')
		{
			--x;
			++y;
		} else if (x < (MAX_X - 1) && field[x + 1][y + 1] == '.')
		{
			++x;
			++y;
		} else 
		{
			if (x == 500 && y == 0)
				break;
			field[x][y] = 'o';
			++drops;
			x = 500;
			y = 0;
		}

	}

	print_field (debug_out, 494, 10, 0, 10);

	fprintf (out_f, "%ld\n", drops + 1);
}

void print_field (FILE *f, int startx, int nx, int starty, int ny)
{
	int i, j;

	fprintf (f, "    %3d\n", startx);
	for (i = 0; i < ny; ++i)
	{
		fprintf (f, "%-3d ", starty + i);
		for (j = 0; j < nx; ++j)
		{
			fputc (field[j + startx][i + starty], f);
		}
		fputc ('\n', f);
	}
}

int min (int a, int b)
{
	return a < b ? a : b;
}

int max (int a, int b)
{
	return a > b ? a : b;
}

