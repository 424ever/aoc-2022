/*
 * Day 11 -- Monkey in the Middle
 */
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

#define MAX_ITEMS 100
#define OP_OLD	  -1

enum optype
{
	ADD,
	MUL,
	UKN,
};

struct operation
{
	int64_t	    lhs, rhs;
	enum optype type;
};

struct monkey
{
	int64_t		 items[MAX_ITEMS];
	int		 nitems;
	struct operation op;
	int		 test;
	int		 actrue, acfalse;
};

static void day11_sol_func(FILE *, FILE *, FILE *);
static void parse_init_state(FILE *, struct monkey **, int *);
static void dump_state(FILE *, struct monkey *, int, bool);
static void perform_round(struct monkey *, int, size_t *, bool, FILE *, bool);
static int64_t perform_op(struct monkey, int);
static void    throw_item(struct monkey *, int, int);
static int     size_t_comp(const void *, const void *);
static int     get_lcm(struct monkey *, int);

static struct aoc_sol day11_sol = {.name = "day11", .sol = &day11_sol_func};

void __attribute__((constructor)) day11_init(void)
{
	if (!register_sol(day11_sol))
		fprintf(stderr, "day11 load failed.\n");
}

void day11_sol_func(FILE *in_f, FILE *out_f, FILE *debug_out)
{
	int	       i;
	int	       nmonkeys;
	int	       round;
	size_t	      *throwcounts1;
	size_t	      *throwcounts2;
	struct monkey *monkeys1;
	struct monkey *monkeys2;

	throwcounts1 = NULL;

	parse_init_state(in_f, &monkeys1, &nmonkeys);

	monkeys2 = malloc(nmonkeys * sizeof(*monkeys2));
	memcpy(monkeys2, monkeys1, nmonkeys * sizeof(*monkeys2));

	throwcounts1 = malloc(nmonkeys * sizeof(*throwcounts1));
	throwcounts2 = malloc(nmonkeys * sizeof(*throwcounts2));
	memset(throwcounts1, 0, nmonkeys * sizeof(*throwcounts1));
	memset(throwcounts2, 0, nmonkeys * sizeof(*throwcounts2));

	dump_state(debug_out, monkeys1, nmonkeys, true);

	for (round = 1; round <= 20; ++round)
	{
		perform_round(monkeys1, nmonkeys, throwcounts1, true, debug_out,
			      round == 1);

		fprintf(debug_out, "After round %d:\n", round);
		dump_state(debug_out, monkeys1, nmonkeys, false);
	}

	for (round = 1; round <= 10000; ++round)
	{
		perform_round(monkeys2, nmonkeys, throwcounts2, false,
			      debug_out, round == 2);

		if (round == 1 || round == 2 || round == 20 ||
		    round % 1000 == 0)
		{
			fprintf(debug_out, "== After round %d ==\n", round);
			for (i = 0; i < nmonkeys; ++i)
				fprintf(
				    debug_out,
				    "Monkey %d inspected items %ld times.\n", i,
				    throwcounts2[i]);
		}
	}

	qsort(throwcounts1, nmonkeys, sizeof(*throwcounts1), &size_t_comp);
	qsort(throwcounts2, nmonkeys, sizeof(*throwcounts2), &size_t_comp);

	fprintf(out_f, "%ld\n",
		throwcounts1[nmonkeys - 1] * throwcounts1[nmonkeys - 2]);
	fprintf(out_f, "%ld\n",
		throwcounts2[nmonkeys - 1] * throwcounts2[nmonkeys - 2]);
	free(monkeys1);
	free(monkeys2);
	free(throwcounts1);
	free(throwcounts2);
}

void parse_init_state(FILE *f, struct monkey **mptr, int *nptr)
{
	char	      *line;
	char	      *tmp;
	size_t	       nread;
	struct monkey *m;

	line  = NULL;
	nread = 0;

	*mptr = NULL;
	*nptr = 0;

	for (;;)
	{
		++*nptr;
		*mptr = reallocarray(*mptr, *nptr, sizeof(**mptr));
		memset(*mptr + *nptr - 1, 0, sizeof(**mptr));
		m = *mptr + *nptr - 1;

		/* Header line */
		if (getline(&line, &nread, f) == -1)
			break;

		/* Items */
		if (getline(&line, &nread, f) == -1)
			break;
		rtrim(line);

		tmp = strstr(line, ":") + 1;
		for (;;)
		{
			while (isspace(*tmp) || *tmp == ',')
				++tmp;
			if (*tmp == '\0')
				break;

			m->items[m->nitems++] = (int) strtol(tmp, &tmp, 0);
			if (m->nitems > MAX_ITEMS)
			{
				fprintf(stderr,
					"Not enough space for items of %d\n",
					*nptr - 1);
				abort();
			}
		}

		/* Operation */
		if (getline(&line, &nread, f) == -1)
			break;
		rtrim(line);

		tmp = strstr(line, "=") + 2;
		if (memcmp(tmp, "old", 3) != 0)
			sscanf(tmp, "%" SCNi64, &m->op.lhs);
		else
			m->op.lhs = OP_OLD;

		tmp = strstr(tmp, " ") + 1;
		if (*tmp == '+')
			m->op.type = ADD;
		else if (*tmp == '*')
			m->op.type = MUL;
		else
			m->op.type = UKN;

		tmp = strstr(tmp, " ") + 1;
		if (memcmp(tmp, "old", 3) != 0)
			sscanf(tmp, "%" SCNi64, &m->op.rhs);
		else
			m->op.rhs = OP_OLD;

		/* Test */
		if (getline(&line, &nread, f) == -1)
			break;
		rtrim(line);
		sscanf(line, " Test: divisible by %d", &m->test);

		/* true */
		if (getline(&line, &nread, f) == -1)
			break;
		rtrim(line);
		sscanf(line, " If true: throw to monkey %d", &m->actrue);

		/* false */
		if (getline(&line, &nread, f) == -1)
			break;
		rtrim(line);
		sscanf(line, " If false: throw to monkey %d", &m->acfalse);

		/* Skip empty line */
		if (getline(&line, &nread, f) == -1)
			break;

		if (feof(f))
			break;
	}
	free(line);
}

void dump_state(FILE *f, struct monkey *m, int n, bool full)
{
	int j;
	int i;

	for (i = 0; i < n; ++i)
	{
		fprintf(f, "Monkey %d: ", i);
		if (full)
		{
			fprintf(f, "  Items: (%d) ", m[i].nitems);
			for (j = 0; j < m[i].nitems; ++j)
				fprintf(f, "%" PRIi64 " ", m[i].items[j]);
			fputc('\n', f);
			fprintf(f, "  Operation: new = ");
			if (m[i].op.lhs == OP_OLD)
				fprintf(f, "%s ", "old");
			else
				fprintf(f, "%" PRIi64 " ", m[i].op.lhs);
			fputc(m[i].op.type == ADD   ? '+'
			      : m[i].op.type == MUL ? '*'
						    : '?',
			      f);
			if (m[i].op.rhs == OP_OLD)
				fprintf(f, " %s\n", "old");
			else
				fprintf(f, " %" PRIi64 "\n", m[i].op.rhs);
			fprintf(f, "  Test: %d\n", m[i].test);
			fprintf(f, "    true: %d\n    false: %d\n", m[i].actrue,
				m[i].acfalse);
		}
		else
		{
			for (j = 0; j < m[i].nitems; ++j)
				fprintf(f, "%" PRIi64 " ", m[i].items[j]);
		}
		fputc('\n', f);
	}
}

void perform_round(struct monkey *m, int n, size_t *tcounts, bool div_value,
		   FILE *debug, bool print_debug)
{
	int i;
	int j;
	int orig_nitems;

	for (i = 0; i < n; ++i)
	{
		orig_nitems = m[i].nitems;

		tcounts[i] += orig_nitems;
		if (print_debug)
			fprintf(debug, "Monkey %d:\n", i);

		for (j = 0; j < orig_nitems; ++j)
		{
			if (print_debug)
				fprintf(debug, "  inspecting val %" PRIi64 "\n",
					m[i].items[0]);

			m[i].items[0] = perform_op(m[i], 0);

			if (print_debug)
				fprintf(debug, "    after op %" PRIi64 "\n",
					m[i].items[0]);

			if (div_value)
			{
				m[i].items[0] /= 3;
				if (print_debug)
					fprintf(debug,
						"    decreased to %" PRIi64
						"\n",
						m[i].items[0]);
			}
			else
				m[i].items[0] %= get_lcm(m, n);

			if (m[i].items[0] % m[i].test == 0)
			{
				if (print_debug)
					fprintf(debug,
						"    passed, threw to %d\n",
						m[i].actrue);

				throw_item(m, i, m[i].actrue);
			}
			else
			{
				if (print_debug)
					fprintf(debug,
						"    not passed, threw to %d\n",
						m[i].acfalse);

				throw_item(m, i, m[i].acfalse);
			}
		}
	}
}

int64_t perform_op(struct monkey m, int i)
{
	int64_t val1, val2;
	int64_t res;

	val1 = m.op.lhs == OP_OLD ? m.items[i] : m.op.lhs;
	val2 = m.op.rhs == OP_OLD ? m.items[i] : m.op.rhs;

	if (m.op.type == ADD)
		res = val1 + val2;
	else
		res = val1 * val2;

	return res;
}

void throw_item(struct monkey *m, int from, int to)
{
	if (m[from].nitems == 0)
	{
		fprintf(stderr, "Attempted to throw from empty\n");
		abort();
	}

	if (m[to].nitems >= MAX_ITEMS)
	{
		fprintf(stderr, "Attempted to throw to full\n");
		abort();
	}

	m[to].items[m[to].nitems] = m[from].items[0];
	memmove(m[from].items, m[from].items + 1,
		sizeof(m[from].items) - sizeof(*m[from].items));

	--m[from].nitems;
	++m[to].nitems;
}

int size_t_comp(const void *a, const void *b)
{
	size_t v1 = *((size_t *) a);
	size_t v2 = *((size_t *) b);

	if (v1 == v2)
		return 0;
	else if (v1 < v2)
		return -1;
	else
		return 1;
}

int get_lcm(struct monkey *m, int n)
{
	int i;
	int lcm;

	lcm = 1;

	for (i = 0; i < n; ++i)
		lcm *= m[i].test;

	return lcm;
}
