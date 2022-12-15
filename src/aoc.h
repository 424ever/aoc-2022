#include <stdbool.h>
#include <stdio.h>

#ifndef _AOC_H_
#define _AOC_H_

typedef void (*sol_func)(FILE *in_f, FILE *out_f, FILE *debug_out);

struct aoc_sol
{
	char    *name;
	sol_func sol;
};

extern bool register_sol (struct aoc_sol);
extern bool is_test_mode (void);
char *rtrim (char*);

#endif /* _AOC_H_ */

