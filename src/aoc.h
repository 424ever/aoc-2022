#include <stdbool.h>
#include <stdio.h>

#ifndef _AOC_H_
#define _AOC_H_

typedef void (*sol_func)(FILE*, FILE*);

struct aoc_sol
{
	char    *name;
	sol_func sol;
};

extern bool register_sol (struct aoc_sol);

#endif /* _AOC_H_ */

