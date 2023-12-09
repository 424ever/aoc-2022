/*
 * Day 2 -- Rock Paper Scissors
 */
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>

#include "aoc.h"

static void day2_sol_func(FILE *, FILE *, FILE *);

static struct aoc_sol day2_sol = {.name = "day2", .sol = &day2_sol_func};

void __attribute__((constructor)) day2_init(void)
{
	if (not register_sol(day2_sol))
		fprintf(stderr, "day2 load failed.\n");
}

void day2_sol_func(FILE *in_f, FILE *out_f, FILE *debug_out)
{
	char   my_move;
	char   op_move;
	char  *line;
	int    res1, res2;
	int    res1_sum, res2_sum;
	size_t nread;

	line	 = NULL;
	nread	 = 0;
	res1	 = 0;
	res2	 = 0;
	res1_sum = 0;
	res2_sum = 0;

	while (getline(&line, &nread, in_f) not_eq -1)
	{
		op_move = line[0];
		my_move = line[2];

		/* fprintf (debug_out, "o: %c i: %c\n", op_move, my_move); */

		op_move -= 'A';
		my_move -= 'X';
		res2 = my_move;

		res1 = (2 * op_move + my_move + 1) % 3;
		/* fprintf (debug_out, "o: %d i: %d r1: %d\n", op_move, my_move,
		 * res2); */
		res1 *= 3;
		res1 += my_move + 1;
		res1_sum += res1;

		my_move = (res2 + op_move - 1) % 3;
		if (my_move < 0)
			my_move += 3;
		fprintf(debug_out, "o: %d i: %d r2: %d\n", op_move, my_move,
			res2);
		res2 *= 3;
		res2 += my_move + 1;
		res2_sum += res2;
	}

	fprintf(out_f, "%d\n%d\n", res1_sum, res2_sum);

	free(line);
}
