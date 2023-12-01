/*
 * Day 5 -- Supply Stacks
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

#define MAX_STACK 50

struct stack
{
	size_t count;
	char   crates[MAX_STACK];
};

struct proc
{
	unsigned from; /* zero indexed */
	unsigned to;
	unsigned count;
};

static void day5_sol_func(FILE *, FILE *, FILE *);
static void apply_proc_1(struct stack *, struct proc);
static void apply_proc_2(struct stack *, struct proc);
static void stack_add_bottom(struct stack *, char);
static char stack_peek(struct stack *);
static void stack_push(struct stack *, char);
static char stack_pop(struct stack *);
static void write_stacks(FILE *, struct stack *, size_t);
static void write_procs(FILE *, struct proc *, size_t);

static struct aoc_sol day5_sol = {.name = "day5", .sol = &day5_sol_func};

void __attribute__((constructor)) day5_init(void)
{
	if (!register_sol(day5_sol))
		fprintf(stderr, "day5 load failed.\n");
}

void  day5_sol_func(in_f, out_f, debug_out) FILE *in_f;
FILE *out_f;
FILE *debug_out;
{
	char	     *crate;
	char	     *line;
	size_t	      i;
	size_t	      nprocs;
	size_t	      nread;
	size_t	      nstacks;
	struct proc  *procs;
	struct stack *stacks;
	struct stack *stacks_1;
	struct stack *stacks_2;

	line	= NULL;
	nread	= 0;
	nstacks = 0;
	stacks	= NULL;
	nprocs	= 0;
	procs	= calloc(1, sizeof(struct proc));

	/* Parse initial stacks */
	while (getline(&line, &nread, in_f) > 1)
	{
		line[strlen(line) - 1] = '\0';
		if (!nstacks)
		{
			nstacks = (strlen(line) + 1) / 4;
			stacks	= calloc(nstacks, sizeof(struct stack));
			fprintf(debug_out, "nstacks: %ld\n", nstacks);
		}

		fprintf(debug_out, "line: '%s'\n", line);
		crate = line;
		while ((crate = strchr(crate, '[')) != NULL)
		{
			i = (crate - line) / 4;
			crate++;
			stack_add_bottom(stacks + i, *crate);
		}
	}
	write_stacks(debug_out, stacks, nstacks);
	stacks_1 = calloc(nstacks, sizeof(struct stack));
	stacks_2 = calloc(nstacks, sizeof(struct stack));

	memcpy(stacks_1, stacks, nstacks * sizeof(struct stack));
	memcpy(stacks_2, stacks, nstacks * sizeof(struct stack));

	/* Parse procedures */
	while (getline(&line, &nread, in_f) != -1)
	{
		procs = reallocarray(procs, nprocs + 1, sizeof(struct proc));
		sscanf(line, "move %u from %u to %u\n", &procs[nprocs].count,
		       &procs[nprocs].from, &procs[nprocs].to);
		--procs[nprocs].from;
		--procs[nprocs].to;
		++nprocs;
	}
	write_procs(debug_out, procs, nprocs);

	for (i = 0; i < nprocs; ++i)
	{
		fprintf(debug_out,
			"--------------- applying [%ld] ------------\n", i);
		apply_proc_1(stacks_1, procs[i]);
		apply_proc_2(stacks_2, procs[i]);
	}

	for (i = 0; i < nstacks; ++i)
		fputc(stack_peek(stacks_1 + i), out_f);
	fputc('\n', out_f);
	for (i = 0; i < nstacks; ++i)
		fputc(stack_peek(stacks_2 + i), out_f);

	free(stacks);
	free(stacks_1);
	free(stacks_2);
	free(procs);
	free(line);
}

void	    apply_proc_1(s, p) struct stack *s;
struct proc p;
{
	char   c;
	size_t i;

	for (i = 0; i < p.count; ++i)
	{
		c = stack_pop(s + p.from);
		stack_push(s + p.to, c);
	}
}

void	    apply_proc_2(s, p) struct stack *s;
struct proc p;
{
	char   *buf;
	ssize_t i;

	buf = malloc(p.count);

	for (i = 0; i < p.count; ++i)
		buf[i] = stack_pop(s + p.from);

	for (i = i - 1; i >= 0; --i)
	{
		stack_push(s + p.to, buf[i]);
	}

	free(buf);
}

void stack_add_bottom(struct stack *s, char c)
{
	++s->count;
	if (s->count >= MAX_STACK)
	{
		fprintf(stderr, "stack grew too large");
		abort();
	}
	s->crates[s->count - 1] = c;
}

char	      stack_peek(s)
struct stack *s;
{
	if (s->count == 0)
	{
		fprintf(stderr, "attempted to peek on empty stack\n");
		abort();
	}
	return s->crates[0];
}

void stack_push(struct stack *s, char c)
{
	++s->count;
	if (s->count >= MAX_STACK)
	{
		fprintf(stderr, "stack grew too large\n");
		abort();
	}

	memmove(s->crates + 1, s->crates, s->count - 1);
	s->crates[0] = c;
}

char	      stack_pop(s)
struct stack *s;
{
	char c;

	if (s->count == 0)
	{
		fprintf(stderr, "attempted to pop from empty stack\n");
		abort();
	}
	--s->count;
	c = s->crates[0];

	memmove(s->crates, s->crates + 1, s->count);

	return c;
}

void	      write_stacks(f, s, n) FILE *f;
struct stack *s;
size_t	      n;
{
	size_t i, j;

	for (i = 0; i < n; ++i)
	{
		fprintf(f, "%ld: ", i);
		for (j = 0; j < s[i].count; ++j)
		{
			fputc(s[i].crates[j], f);
			fputc(' ', f);
		}
		fputc('\n', f);
	}
}

void	     write_procs(f, p, n) FILE *f;
struct proc *p;
size_t	     n;
{
	size_t i;

	for (i = 0; i < n; ++i)
		fprintf(f, "[%ld]: (f %d t %d c %d)\n", i, p[i].from, p[i].to,
			p[i].count);
}
