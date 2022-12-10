/*
 * Day 9 -- Rope Bridge
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

#define MAX_LINE_LEN 6 /* D NN\n\0 */

struct pos {
	int x;
	int y;
};

static void day9_sol_func (FILE*, FILE*, FILE*);
static inline int max (int, int);
static void set_insert (struct pos**, size_t*, struct pos);
static void print_pos_arr (struct pos*, size_t, FILE*);

static struct aoc_sol day9_sol = {
	.name = "day9",
	.sol  = &day9_sol_func
};

void __attribute__((constructor)) day9_init (void)
{
	if (!register_sol (day9_sol))
		fprintf (stderr, "day9 load failed.\n");
}

void day9_sol_func (FILE *in_f, FILE *out_f, FILE *debug_out)
{
	char        dir;
	int         dist;
	int         n;
	int         orig_n;
	size_t      i;
	size_t      nvis_1;
	size_t      nvis_2;
	struct pos  headpos;
	struct pos  part2_pos[10];
	struct pos  prevheadpos;
	struct pos  tailpos;
	struct pos *visited_1;
	struct pos *visited_2;

	nvis_1    = 0;
	nvis_2    = 0;
	visited_1 = NULL;
	visited_2 = NULL;
	headpos.x = headpos.y = 0;
	tailpos.x = tailpos.y = 0;
	memset (part2_pos, 0, sizeof (part2_pos));

	while (fscanf (in_f, "%c %d\n",
			     &dir,
			     &n) != EOF)
	{
		orig_n = n;
		while (n > 0)
		{
			--n;
			prevheadpos = headpos;
	
			switch (dir)
			{
			case 'U':
				++headpos.y;
				++part2_pos[0].y;
				break;
			case 'D':
				--headpos.y;
				--part2_pos[0].y;
				break;
			case 'L':
				--headpos.x;
				--part2_pos[0].x;
				break;
			case 'R':
				++headpos.x;
				++part2_pos[0].x;
				break;
			default:
				fprintf (stderr, "Unknown direction '%c'\n", dir);
				abort ();
			}
	
			dist = max (abs (headpos.x - tailpos.x), abs (headpos.y - tailpos.y));
			if (dist == 2)
				tailpos = prevheadpos;
			set_insert (&visited_1, &nvis_1, tailpos);

			fprintf (debug_out, "%c %d (%d)\n", dir, orig_n, n);

			for (i = 1; i < 10; ++i)
			{
				if (!memcmp (part2_pos + i, part2_pos + i - 1, sizeof (struct pos)))
					continue;

				if (abs (part2_pos[i].x - part2_pos[i - 1].x) <= 1 &&
				    abs (part2_pos[i].y - part2_pos[i - 1].y) <= 1)
					continue;
				
				if (abs (part2_pos[i].x - part2_pos[i - 1].x) == 2)
					part2_pos[i].x += (part2_pos[i - 1].x - part2_pos[i].x) / 2;
				else
					part2_pos[i].x  = part2_pos[i - 1].x;

				if (abs (part2_pos[i].y - part2_pos[i - 1].y) == 2)
					part2_pos[i].y += (part2_pos[i - 1].y - part2_pos[i].y) / 2;
				else
					part2_pos[i].y  = part2_pos[i - 1].y;

			}
			set_insert (&visited_2, &nvis_2, part2_pos[9]);

			print_pos_arr (part2_pos, 10, debug_out);
		}
	}

	fprintf (out_f, "%ld\n%ld\n", nvis_1, nvis_2);
	free (visited_1);
	free (visited_2);
}

int max (int i1, int i2)
{
	return i1 > i2 ? i1 : i2;
}

void set_insert (struct pos **arrptr, size_t *nptr, struct pos p)
{
	bool   in_set;
	size_t i;

	in_set = false;

	for (i = 0; i < *nptr; ++i)
	{
		if ((*arrptr)[i].x == p.x &&
		    (*arrptr)[i].y == p.y)
		{
			in_set = true;
			break;
		}
	}

	if (!in_set)
	{
		++*nptr;
		*arrptr = reallocarray(*arrptr, *nptr, sizeof (**arrptr));
		(*arrptr)[*nptr - 1] = p;
	}
}

void print_pos_arr (struct pos *arr, size_t n, FILE *f)
{
	size_t i;

	fprintf (f, "IND X  Y\n");
	for (i = 0; i < n; ++i)
		fprintf (f, "%3ld %-2d %-2d\n", i, arr[i].x, arr[i].y);
	fputc('\n', f);
}

