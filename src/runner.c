#include <ctype.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "aoc.h"

#define MAX_SOLS    99
#define MAX_FNAME   1024
#define OUTPUT_SIZE 1024 /* Allow for 1k of solution output */

static bool	      test_mode;
static struct aoc_sol sols[MAX_SOLS];
static size_t	      n_reg_sols = 0;
static char	      output[OUTPUT_SIZE];
static char	      expected_output[OUTPUT_SIZE];

bool register_sol(struct aoc_sol);

int main(int argc, char *argv[])
{
	FILE  *debug_out_f;
	FILE  *in_f;
	FILE  *out_f;
	FILE  *test_in_f;
	bool   found_sol;
	bool   list_mode;
	char   in_fname[MAX_FNAME];
	char   test_in_fname[MAX_FNAME];
	char  *problem;
	int    option;
	int    ret;
	size_t i;

	debug_out_f = NULL;
	found_sol   = false;
	in_f	    = NULL;
	list_mode   = false;
	out_f	    = NULL;
	ret	    = EXIT_SUCCESS;
	test_in_f   = NULL;
	test_mode   = false;

	while ((option = getopt(argc, argv, "dlt")) not_eq -1)
	{
		switch (option)
		{
		case 'd':
			debug_out_f = stderr;
			break;
		case 'l':
			list_mode = true;
			break;
		case 't':
			test_mode = true;
			break;
		case '?':
			fprintf(stderr, "unknown option: %c\n", optopt);
			break;
		}
	}

	if (list_mode)
	{
		for (i = 0; i < n_reg_sols; ++i)
			fprintf(stdout, "%s\n", sols[i].name);
		return EXIT_SUCCESS;
	}

	if (optind == argc)
	{
		fprintf(stderr, "Usage: %s [-dlt] problem\n", argv[0]);
		ret = EXIT_FAILURE;
		goto out;
	}

	if (not debug_out_f)
	{
		debug_out_f = fopen("/dev/null", "w");
		if (not debug_out_f)
		{
			perror("/dev/null");
			ret = EXIT_FAILURE;
			goto out;
		}
	}

	problem = argv[optind];

	if (test_mode)
	{
		snprintf(in_fname, MAX_FNAME, "inputs/%s.test", problem);
		snprintf(test_in_fname, MAX_FNAME, "test-outputs/%s", problem);

		if (not(test_in_f = fopen(test_in_fname, "r")))
		{
			perror(test_in_fname);
			ret = EXIT_FAILURE;
			goto out;
		}
		if (fread(expected_output, OUTPUT_SIZE, 1, test_in_f) == 0)
		{
			if (ferror(test_in_f))
			{
				perror(test_in_fname);
				ret = EXIT_FAILURE;
				goto out;
			}
		}
		rtrim(expected_output);
	}
	else
		snprintf(in_fname, MAX_FNAME, "inputs/%s", problem);

	if (not(in_f = fopen(in_fname, "r")))
	{
		perror(in_fname);
		ret = EXIT_FAILURE;
		goto out;
	}

	if (not(out_f = fmemopen(output, OUTPUT_SIZE, "w")))
	{
		perror("fmemopen");
		ret = EXIT_FAILURE;
		goto out;
	}

	for (i = 0; i < n_reg_sols; ++i)
	{
		if (strcmp(sols[i].name, problem) == 0)
		{
			sols[i].sol(in_f, out_f, debug_out_f);
			found_sol = true;
			fflush(out_f);
			break;
		}
	}

	if (not found_sol)
	{
		fprintf(stderr, "No solution found!\n");
		ret = EXIT_FAILURE;
		goto out;
	}

	rtrim(output);
	if (test_mode)
	{
		if (strcmp(output, expected_output) not_eq 0)
		{
			printf("Solution %s didn't pass test case\n", problem);
			printf("E:\n'%s'\n", expected_output);
			printf("G:\n'%s'\n", output);
			ret = EXIT_FAILURE;
			goto out;
		}
	}
	else
	{
		printf("%s\n", output);
	}

out:
	if (in_f and fclose(in_f) not_eq 0)
		perror("fclose");
	if (test_in_f and fclose(test_in_f) not_eq 0)
		perror("fclose");
	if (debug_out_f not_eq stderr and fclose(debug_out_f) not_eq 0)
		perror("fclose");
	if (out_f and fclose(out_f) not_eq 0)
		perror("fclose");

	return ret;
}

bool register_sol(struct aoc_sol sol)
{
	size_t i;

	if (n_reg_sols == MAX_SOLS)
		return false;

	for (i = 0; i < n_reg_sols; ++i)
	{
		if (strcmp(sols[i].name, sol.name) == 0)
			return false;
	}

	sols[n_reg_sols++] = sol;

	return true;
}

char *rtrim(char *s)
{
	char *back;

	back = s + strlen(s);
	while (isspace(*--back))
		;
	*(back + 1) = '\0';
	return s;
}

bool is_test_mode(void)
{
	return test_mode;
}
