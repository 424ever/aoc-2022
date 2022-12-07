/*
 * Day 6 -- Tuning Trouble
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

static void day6_sol_func (FILE*, FILE*, FILE*);
static int find_first_unique_seq (FILE*, int);
static bool has_dups (char*, int);
static void push_back (char*, int, char);

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
	(void) debug_out;

	fprintf (out_f, "%d\n", find_first_unique_seq (in_f, 4));
	fseek (in_f, 0, SEEK_SET);
	fprintf (out_f, "%d\n", find_first_unique_seq (in_f, 14));
}

int find_first_unique_seq (f, n)
FILE *f;
int   n;
{
	int   i;
	char  c;
	char *buf;

	i   = 0;
	buf = calloc (n, 1);

	while ((c = fgetc (f)) != EOF)
	{
		++i;
		push_back (buf, n, c);
		
		if (i <= n)
			continue;

		if (!has_dups (buf, n))
			goto exit;
	}

exit:
	free (buf);
	return i;
}

bool has_dups (buf, n)
char *buf;
int   n;
{
	int i, j;

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

void push_back (char *buf, int n, char c)
{
	memmove (buf, buf + 1, n - 1);
	buf[n - 1] = c;
}

