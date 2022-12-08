/*
 * Day 8 -- Treetop Tree House
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

static void day8_sol_func (FILE*, FILE*, FILE*);
static void write_grid (int*, size_t, size_t, FILE*);
static size_t visible_count (int*, size_t, size_t);
static int is_visible (int*, size_t, size_t, size_t, size_t);
static int scenic_score (int*, size_t, size_t, size_t, size_t);

static struct aoc_sol day8_sol = {
	.name = "day8",
	.sol  = &day8_sol_func
};

void __attribute__((constructor)) day8_init (void)
{
	if (!register_sol (day8_sol))
		fprintf(stderr, "day8 load failed.\n");
}

void day8_sol_func (FILE *in_f, FILE *out_f, FILE *debug_out)
{
	char  *line;
	int    highest_scenic_score;
	int   *heights;
	size_t i, j;
	size_t linelen;
	size_t nlines;
	size_t nread;

	highest_scenic_score = 0;
	heights              = NULL;
	line                 = NULL;
	linelen              = 0;
	nlines               = 0;
	nread                = 0;

	while (getline (&line, &nread, in_f) != -1)
	{
		++nlines;

		rtrim (line);
		if (!linelen)
			linelen = strlen (line);

		heights = reallocarray (heights, nlines * linelen, sizeof (int));

		for (i = 0; i < linelen; ++i)
			heights[(nlines - 1) * linelen + i] = line[i] - '0';
	}
	write_grid (heights, nlines, linelen, debug_out);

	fprintf (out_f, "%ld\n", visible_count (heights, nlines, linelen));

	for (i = 0; i < nlines; ++i)
	{
		for (j = 0; j < linelen; ++j)
		{
			if (scenic_score (heights, nlines, linelen, i, j) > highest_scenic_score)
				highest_scenic_score = scenic_score (heights, nlines, linelen, i, j);
		}
	}
	fprintf (out_f, "%d\n", highest_scenic_score);
	free (line);
	free (heights);
}

void write_grid (int *grid, size_t nlines, size_t linelen, FILE *f)
{
	size_t i, j;

	for (i = 0; i < nlines; ++i)
	{
		for (j = 0; j < linelen; ++j)
			fprintf (f, "%d ", grid[i * linelen + j]);
		fputc ('\n', f);
	}
}

size_t visible_count (int *grid, size_t nlines, size_t linelen)
{
	size_t visible;
	size_t i, j;

	visible = 2 * linelen + 2 * (nlines - 2);

	for (i = 1; i < nlines - 1; ++i)
	{
		for (j = 1; j < linelen - 1; ++j)
			visible += is_visible (grid, nlines, linelen, i, j);
	}

	return visible;
}

int is_visible (int *grid, size_t nlines, size_t linelen, size_t y, size_t x)
{
	bool   visible;
	int    check_val;
	size_t i;

	visible   = false;
	check_val = grid[y * linelen + x];

	/* Left */
	for (i = 0; i < x; ++i)
	{
		if (grid[y * linelen + i] >= check_val)
			break;
	}
	visible |= i == x;

	/* Right */
	for (i = linelen - 1; i > x; --i)
		if (grid[y * linelen + i] >= check_val)
			break;
	visible |= i == x;

	/* Top */
	for (i = 0; i < y; ++i)
		if (grid[i * linelen + x] >= check_val)
			break;
	visible |= i == y;

	/* Bottom */
	for (i = nlines - 1; i > y; --i)
		if (grid[i * linelen + x] >= check_val)
			break;
	visible |= i == y;

	return visible ? 1 : 0;
}

int scenic_score (int *grid, size_t nlines, size_t linelen, size_t y, size_t x)
{
	int check_val;
	int dist;
	int i;
	int score;

	check_val = grid[y * linelen + x];
	score     = 1;

	/* Left */
	dist = 0;
	for (i = x - 1; i >= 0; --i)
	{
		++dist;
		if (grid[y * linelen + i] >= check_val)
			break;
	}
	score *= dist;

	/* Right */
	dist = 0;
	for (i = x + 1; i < (int)linelen; ++i)
	{
		++dist;
		if (grid[y * linelen + i] >= check_val)
			break;
	}
	score *= dist;

	/* Top */
	dist = 0;
	for (i = y - 1; i >= 0; --i) 
	{
		++dist;
		if (grid[i * linelen + x] >= check_val)
			break;
	}
	score *= dist;

	/* Bottom */
	dist = 0;
	for (i = y + 1; i < (int)nlines; ++i) 
	{
		++dist;
		if (grid[i * linelen + x] >= check_val)
			break;
	}
	score *= dist;

	return score;
}

