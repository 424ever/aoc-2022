/*
 * Day 10 -- Cathode-Ray Tube
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

#define LINELEN 20

static void day10_sol_func(FILE *, FILE *, FILE *);

static struct aoc_sol day10_sol = {.name = "day10", .sol = &day10_sol_func};

void __attribute__((constructor)) day10_init(void)
{
	if (!register_sol(day10_sol))
		fprintf(stderr, "day10 load failed.\n");
}

void day10_sol_func(FILE *in_f, FILE *out_f, FILE *debug_out)
{
	char line[LINELEN];
	char crt[240];
	int  crtpos;
	int  crtx;
	int  cycle;
	int  i, j;
	int  strength_sum;
	int  x;

	cycle	     = 0;
	crtpos	     = 0;
	strength_sum = 0;
	x	     = 1;

	while (fgets(line, LINELEN, in_f))
	{
		rtrim(line);
		++cycle;
		crtx	      = crtpos % 40;
		crt[crtpos++] = abs(crtx - x) <= 1 ? '#' : '.';

		if ((cycle - 20) % 40 == 0)
		{
			fprintf(debug_out, "add on cycle %d x=%d\n", cycle, x);
			strength_sum += cycle * x;
		}

		if (strstr(line, "addx"))
		{
			++cycle;
			crtx	      = crtpos % 40;
			crt[crtpos++] = abs(crtx - x) <= 1 ? '#' : '.';

			if ((cycle - 20) % 40 == 0)
			{
				fprintf(debug_out,
					"add on cycle %d x=%d (after add)\n",
					cycle, x);
				strength_sum += cycle * x;
			}

			x += atoi(line + 5);
		}
	}

	fprintf(out_f, "%d\n", strength_sum);

	for (i = 0; i < 6; ++i)
	{
		for (j = 0; j < 40; ++j)
			fputc(crt[i * 40 + j], out_f);
		fputc('\n', out_f);
	}
	fputc('\n', out_f);
}
