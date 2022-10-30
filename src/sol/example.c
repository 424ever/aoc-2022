/*
 * example -- Example problem
 */
#include <iso646.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "aoc.h"

static void example_sol_func (FILE*, FILE*, FILE*);

static struct aoc_sol example_sol = {
	.name = "example",
	.sol  = &example_sol_func
};

void __attribute__((constructor)) example_init (void)
{
	if (not register_sol (example_sol))
		fprintf (stderr, "example load failed.\n");
}

void example_sol_func (in_f, out_f, debug_out)
FILE *in_f;
FILE *out_f;
FILE *debug_out;
{
	char  *line;
	size_t nread;
	size_t num;
	size_t sum;

	line  = NULL;
	nread = 0;
	sum   = 0;

	while (getline (&line, &nread, in_f) not_eq -1)
	{
		num = atoll (line);
		sum += num;
		fprintf (debug_out, "sum: %ld, num: %ld\n", sum, num);
	}

	fprintf (out_f, "%ld", sum);

	free (line);
}

