/*
 * Day 1 -- Calorie Counting
 */
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

#define MAX_NUM 30

struct day1_cals
{
	size_t count;
	size_t cals[MAX_NUM];
};

static void day1_sol_func (FILE*, FILE*, FILE*);
static size_t cals_sum (struct day1_cals*);
static int cals_sum_compar (const void*, const void*);

static struct aoc_sol day1_sol = {
	.name = "day1",
	.sol  = &day1_sol_func
};

void __attribute__((constructor)) day1_init (void)
{
	if (not register_sol (day1_sol))
		fprintf (stderr, "day1 load failed.\n");
}

void day1_sol_func (in_f, out_f, debug_out)
FILE *in_f;
FILE *out_f;
FILE *debug_out;
{
	char             *line;
	size_t            i;
	size_t            j;
	size_t            calidx;
	size_t            nread;
	size_t            max_sum;
	size_t            top_3_sums;
	struct day1_cals *cals_arr;
	
	line     = NULL;
	nread    = 0;

	calidx   = 0;
	cals_arr = calloc (1, sizeof (struct day1_cals));

	while (getline (&line, &nread, in_f) not_eq -1)
	{
		if (line[0] == '\n')
		{
			fprintf (debug_out, "new, calidx = %ld\n", calidx);
			++calidx;
			cals_arr = reallocarray (cals_arr, calidx + 1, sizeof (struct day1_cals));
			memset (cals_arr + calidx, 0, sizeof (struct day1_cals));
			continue;
		}
		cals_arr[calidx].cals[cals_arr[calidx].count] = atoll (line);
		++cals_arr[calidx].count;
		if (cals_arr[calidx].count >= MAX_NUM)
			fprintf (stderr, "MAX_NUM not big enough\n");
	}

	qsort (cals_arr, calidx + 1, sizeof (struct day1_cals), &cals_sum_compar);

	fprintf (debug_out, "calidx = %ld\n", calidx);
	for (i = 0; i < calidx + 1; ++i)
	{
		fprintf (debug_out, "cals[%ld] = {\n"
				    "  .count = %ld\n"
				    "  .cals  = [\n", i, cals_arr[i].count);
		for (j = 0; j < cals_arr[i].count; ++j)
			fprintf (debug_out, "    %ld,\n", cals_arr[i].cals[j]);
		fprintf (debug_out, "  ]\n"
				    " sum = %ld\n}\n", cals_sum (cals_arr + i));
	}

	max_sum = cals_sum (cals_arr + calidx);

	top_3_sums = 0;
	for (i = 0; i <= 2; ++i)
		top_3_sums += cals_sum (cals_arr + calidx - i);


	fprintf (out_f, "%ld\n%ld", max_sum, top_3_sums);

	free (line);
	free (cals_arr);
}

static size_t cals_sum (cals)
struct day1_cals *cals;
{
	size_t i;
	size_t sum;

	sum = 0;

	for (i = 0; i < cals->count; ++i)
		sum += cals->cals[i];

	return sum;
}

static int cals_sum_compar (i1, i2)
const void *i1;
const void *i2;
{
	return cals_sum ((struct day1_cals*) i1) -
	       cals_sum ((struct day1_cals*) i2);
}

