#include <ctype.h>
#include <iso646.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "aoc.h"

#define MAX_SOLS 99
#define OUTPUT_SIZE 1024 /* Allow for 1k of solution output */

static struct aoc_sol sols[MAX_SOLS];
static size_t         n_reg_sols = 0;
static char           output[OUTPUT_SIZE];
static char           expected_output[OUTPUT_SIZE];

bool  register_sol (struct aoc_sol);
void *rtrim (char*);

int main (argc, argv)
int   argc;
char *argv[];
{
	FILE  *in_f;
	FILE  *out_f;
	FILE  *test_in_f;
	bool   found_sol;
	bool   test_mode;
	char   in_fname[PATH_MAX];
	char   test_in_fname[PATH_MAX];
	char  *problem;
	int    option;
	size_t i;

	test_in_f = NULL;

	while ((option = getopt (argc, argv, "t")) not_eq -1)
	{
		switch (option)
		{
			case 't':
				test_mode = true;
				break;
			case '?':
				fprintf (stderr, "unknown option: %c\n",
						optopt);
				break;
		}
	}

	if (optind == argc)
	{
		fprintf (stderr, "Usage: %s [-t] <problem>\n",
				argv[0]);
		return EXIT_FAILURE;
	}

	problem = argv[optind];

	if (test_mode)
	{
		snprintf (in_fname, PATH_MAX, "inputs/%s.test", problem);
		snprintf (test_in_fname, PATH_MAX, "test-outputs/%s", problem);

		if (not (test_in_f = fopen (test_in_fname, "r")))
		{
			perror (test_in_fname);
			return EXIT_FAILURE;
		}
		fread (expected_output, OUTPUT_SIZE, 1, test_in_f);
		rtrim (expected_output);
	} else
		snprintf (in_fname, PATH_MAX, "inputs/%s", problem);

	if (not (in_f = fopen (in_fname, "r")))
	{
		perror (in_fname);
		return EXIT_FAILURE;
	}

	if (not (out_f = fmemopen (output, OUTPUT_SIZE, "w")))
	{
		perror ("fmemopen");
		return EXIT_FAILURE;
	}
	
	for (i = 0; i < n_reg_sols; ++i)
	{
		if (strcmp (sols[i].name, problem) == 0)
		{
			sols[i].sol (in_f, out_f);
			found_sol = true;
			fflush (out_f);
			break;
		}
	}

	if (not found_sol)
	{
		fprintf (stderr, "No solution found!\n");
		return EXIT_FAILURE;
	}

	if (test_mode)
	{
		if (strcmp (output, expected_output) not_eq 0)
			printf ("Solution didn't pass test case\n");
		else
			printf ("Solution passed test case! YAY!\n");
		printf ("E: '%s'\n", expected_output);
		printf ("G: '%s'\n", output);
	} else
	{
		printf ("%s\n", output);
	}

	if (fclose (in_f) not_eq 0)
		perror ("fclose");
	if (test_in_f and fclose (test_in_f) not_eq 0)
		perror ("fclose");
	fclose (out_f);

	return EXIT_SUCCESS;
}

bool register_sol (sol)
struct aoc_sol sol;
{
	size_t i;

	if (n_reg_sols == MAX_SOLS)
		return false;

	for (i = 0; i < n_reg_sols; ++i)
	{
		if (strcmp (sols[i].name, sol.name) == 0)
			return false;
	}

	sols[n_reg_sols++] = sol;

	return true;
}

void *rtrim (s)
char *s;
{
	char *back;
	
	back = s + strlen (s);
	while (isspace (*--back));
	*(back + 1) = '\0';
	return s;
}

