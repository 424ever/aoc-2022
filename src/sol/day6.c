/*
 * Day 6 -- Tuning Trouble
 */
#include <stdio.h>
#include <string.h>

#include "aoc.h"

static void day6_sol_func (FILE*, FILE*, FILE*);
static bool has_dups (char*, size_t);
static void push_back (char*, size_t, char);

static struct aoc_sol day6_sol = {
	.name = "day6",
	.sol  = &day6_sol_func
};

void __attribute__((constructor)) day6_init (void)
{
	if (!register_sol (day6_sol))
		fprintf (stderr, "day6 load failed.\n");
}

void day6_sol_func (in_f, out_f, debug_out)
FILE *in_f;
FILE *out_f;
FILE *debug_out;
{
	char buf_1[4];
	char buf_2[14];
	char c;
	int  n;

	n = 0;

	while ((c = fgetc (in_f)) != EOF)
	{
		++n;
		fprintf (debug_out, "buf: [ %d %d %d %d ] c: %d %c\n", buf_1[0], buf_1[1], buf_1[2], buf_1[3], c, c);
		push_back (buf_1, 4, c);

		if (n <= 4)
			continue;

		if (!has_dups (buf_1, 4))
		{
			fprintf (out_f, "%d\n", n);
			break;
		}
	}
	fseek (in_f, 0, SEEK_SET);

	n = 0;
	while ((c = fgetc (in_f)) != EOF)
	{
		++n;
		push_back (buf_2, 14, c);

		if (n <= 14)
			continue;

		if (!has_dups (buf_2, 14))
		{
			fprintf (out_f, "%d\n", n);
			break;
		}
	}

}

bool has_dups (buf, n)
char  *buf;
size_t n;
{
	size_t i, j;

	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < n; ++j)
		{
			if (buf[i] == buf[j] && i != j)
				return true;
		}
	}
	return false;
}

void push_back (buf, n, c)
char  *buf;
size_t n;
char   c;
{
	memmove (buf, buf + 1, n - 1);
	buf[n - 1] = c;
}

