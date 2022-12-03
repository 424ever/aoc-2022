/*
 * Day 3 -- Rucksack Reorganization
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

static void day3_sol_func (FILE*, FILE*, FILE*);
static bool char_in (char*, char);

static struct aoc_sol day3_sol = {
	.name = "day3",
	.sol  = &day3_sol_func
};

void __attribute__((constructor)) day3_init (void)
{
	if (!register_sol (day3_sol))
		fprintf (stderr, "day3 load failed.\n");
}

void day3_sol_func (in_f, out_f, debug_out)
FILE *in_f;
FILE *out_f;
FILE *debug_out;
{
	char    common_char;
	char   *line;
	char   *lines[3];
	int     prio;
	int     prio_sum;
	size_t  i, j;
	size_t  len;
	size_t  nread;

	line     = NULL;
	nread    = 0;
	prio_sum = 0;

	memset (lines, 0, sizeof (lines));

	while (getline (&line, &nread, in_f) != -1)
	{
		len = strlen (line) - 1;

		for (i = 0; i < len / 2; ++i)
		{
			for (j = len / 2; j < len; ++j)
			{
				if (line[i] == line[j])
				{
					common_char = line[i];
				}
			}
		}

		prio = common_char - 'a' + 1;
		if (prio < 0)
			prio += 'A' - 7;

		fprintf (debug_out, "common: %c prio: %d\n", common_char, prio);
		prio_sum += prio;
	}
	fprintf (out_f, "%d\n", prio_sum);
	
	fseek (in_f, 0L, SEEK_SET);
	prio_sum = 0;

	for (;;)
	{
		getline (lines, &nread, in_f);
		getline (lines + 1, &nread, in_f);
		getline (lines + 2, &nread, in_f);

		if (feof (in_f))
			break;

		for (i = 0; i < strlen (lines[0]) - 1; ++i)
		{
			if (char_in (lines[1], lines[0][i]) &&
			    char_in (lines[2], lines[0][i]))
			{
				common_char = lines[0][i];
				break;
			}
		}
		fprintf (debug_out, "common: %c\n", common_char);
		prio = common_char - 'a' + 1;
		if (prio < 0)
			prio += 'A' - 7;
		prio_sum += prio;
	}
	fprintf (out_f, "%d\n", prio_sum);
}

bool char_in (str, c)
char *str;
char  c;
{
	size_t i;

	for (i = 0; i < strlen (str) - 1; ++i)
	{
		if (str[i] == c)
			return true;
	}
	return false;
}

