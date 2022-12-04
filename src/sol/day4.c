/*
 * Day 4 -- Camp Cleanup
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "aoc.h"

struct region
{
	int l, h;
};

static void day4_sol_func (FILE*, FILE*, FILE*);
static bool overlap (struct region r[2]);
static bool full_overlap (struct region r[2]);
static void write_reg (FILE*, struct region);

static struct aoc_sol day4_sol = {
	.name = "day4",
	.sol  = &day4_sol_func
};

void __attribute__((constructor)) day4_init (void)
{
	if (!register_sol (day4_sol))
		fprintf (stderr, "day4 load failed.\n");
}

void day4_sol_func (in_f, out_f, debug_out)
FILE *in_f;
FILE *out_f;
FILE *debug_out;
{
	size_t        full_ol_count;
	size_t        ol_count;
	struct region reg[2];

	full_ol_count = 0;
	ol_count      = 0;

	while (fscanf (in_f, "%d-%d,%d-%d\n",
			      &reg[0].l, &reg[0].h,
			      &reg[1].l, &reg[1].h) > 0)
	{
		write_reg (debug_out, reg[0]);
		write_reg (debug_out, reg[1]);
		fputc ('\n', debug_out);

		full_ol_count += full_overlap (reg);
		ol_count += overlap (reg);
	}
	
	fprintf (out_f, "%ld\n%ld\n", full_ol_count, ol_count);
}

bool overlap (r)
struct region r[2];
{
	return r[0].l <= r[1].h && r[1].l <= r[0].h;
}

bool full_overlap (r)
struct region r[2];
{
	return (r[0].l <= r[1].l &&
		r[0].h >= r[1].h)
	         ||
	       (r[1].l <= r[0].l &&
		r[1].h >= r[0].h);
}

void write_reg (f, r)
FILE         *f;
struct region r;
{
	fprintf (f, "reg(%d %d)\n", r.l, r.h);
}

