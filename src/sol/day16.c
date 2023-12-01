/*
 * Day 16 -- Proboscidea Volcanium
 */

#include <inttypes.h>
#include <search.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

#define MAX_TUNNELS 20
#define STATE_LEN   101
#define BUFFER_SIZE 90000000

struct valve
{
	char	 name[2];
	int	 flow_rate;
	char	 tunnels[MAX_TUNNELS][2];
	size_t	 ntunnels;
	uint64_t mask; /* has a single bit set, different for each valve */
};

struct buffered_max
{
	int	 filled;
	int	 curr;
	int	 remaining;
	uint64_t open;
	uint8_t	 others;
	int	 max;
};

static int max(int, int);
static int max_pressure_release(struct valve *, size_t, int start, int curr,
				int maxtime, int remaining, uint64_t open,
				uint8_t others);
static int find_by_name(struct valve *, size_t, char[2]);
static uint64_t hash(int curr, int remaining, uint64_t open, uint8_t others);
static bool	find_in_buffer(int curr, int remaining, uint64_t open,
			       uint8_t others, int *maxp);
static void	day16_sol_func(FILE *, FILE *, FILE *);
static void	dump_valves(FILE *, struct valve *, size_t);
static void	parse_valves(FILE *, struct valve **, size_t *);
static void	put_buffer(int curr, int remaining, uint64_t open, int max,
			   uint8_t others);

static struct aoc_sol day16_sol = {.name = "day16", .sol = &day16_sol_func};
static struct buffered_max *buffer;

void __attribute__((constructor)) day16_init(void)
{
	if (!register_sol(day16_sol))
		fprintf(stderr, "day16 load failed.\n");
}

void day16_sol_func(FILE *in_f, FILE *out_f, FILE *debug_out)
{
	char	      startname[2];
	struct valve *valves;
	size_t	      nvalves;
	uint64_t      open;
	int	      start;

	nvalves	     = 0;
	startname[0] = 'A';
	startname[1] = 'A';
	valves	     = NULL;

	parse_valves(in_f, &valves, &nvalves);
	dump_valves(debug_out, valves, nvalves);
	open   = 0;
	buffer = calloc(BUFFER_SIZE, sizeof(*buffer));

	start = find_by_name(valves, nvalves, startname);

	fprintf(out_f, "%d\n",
		max_pressure_release(valves, nvalves, start, start, 30, 30,
				     open, 0));
	fprintf(out_f, "%d\n",
		max_pressure_release(valves, nvalves, start, start, 26, 26,
				     open, 1));
}

int max_pressure_release(struct valve *v, size_t n, int start, int curr,
			 int maxtime, int remaining_mins, uint64_t open,
			 uint8_t others)
{
	int    ans;
	size_t i;
	int new;
	int bufmax;

	ans = 0;

	if (remaining_mins == 0)
		return others == 0
			   ? 0
			   : max_pressure_release(v, n, start, start, maxtime,
						  maxtime, open, others - 1);

	if (find_in_buffer(curr, remaining_mins, open, others, &bufmax))
		return bufmax;

	if (!(open & v[curr].mask) && v[curr].flow_rate > 0)
	{
		ans = max(ans, v[curr].flow_rate * (remaining_mins - 1) +
				   max_pressure_release(
				       v, n, start, curr, maxtime,
				       remaining_mins - 1,
				       (open | v[curr].mask), others));
	}

	for (i = 0; i < v[curr].ntunnels; ++i)
	{
		new = find_by_name(v, n, v[curr].tunnels[i]);
		if (new < 0)
			abort();
		ans = max(ans, max_pressure_release(v, n, start, new, maxtime,
						    remaining_mins - 1, open,
						    others));
	}

	put_buffer(curr, remaining_mins, open, ans, others);

	return ans;
}

int max(int a, int b)
{
	return a > b ? a : b;
}

void parse_valves(FILE *f, struct valve **arrptr, size_t *nptr)
{
	char	     *line;
	char	     *tmp;
	uint64_t      mask;
	size_t	      nread;
	struct valve *v;

	line  = NULL;
	nread = 0;
	mask  = 1;

	while (getline(&line, &nread, f) != -1)
	{
		rtrim(line);
		++*nptr;
		*arrptr = reallocarray(*arrptr, *nptr, sizeof(**arrptr));
		v	= *arrptr + *nptr - 1;

		sscanf(line, "Valve %c%c has flow rate=%d", &v->name[0],
		       &v->name[1], &v->flow_rate);
		v->ntunnels = 0;
		v->mask	    = mask;
		mask	    = mask << 1;

		tmp = strstr(line, "valves");

		if (tmp)
		{
			tmp += 5;
			while (tmp)
			{
				tmp += 2;
				v->tunnels[v->ntunnels][0] = tmp[0];
				v->tunnels[v->ntunnels][1] = tmp[1];
				++v->ntunnels;
				tmp = strstr(tmp, ",");
			}
		}
		else
		{
			v->ntunnels	 = 1;
			tmp		 = strstr(line, "valve") + 6;
			v->tunnels[0][0] = tmp[0];
			v->tunnels[0][1] = tmp[1];
		}
	}

	if (*nptr > 64)
	{
		fprintf(stderr, "Too many valves for mask\n");
		abort();
	}
}

void dump_valves(FILE *f, struct valve *v, size_t n)
{
	size_t i, j;

	for (i = 0; i < n; ++i)
	{
		fprintf(f,
			"%ld: valve %c%c flow_rate: %d, maks: %" PRIu64
			", tunnels: ",
			i, v[i].name[0], v[i].name[1], v[i].flow_rate,
			v[i].mask);

		for (j = 0; j < v[i].ntunnels; ++j)
			fprintf(f, "%c%c ", v[i].tunnels[j][0],
				v[i].tunnels[j][1]);
		fputc('\n', f);
	}
}

int find_by_name(struct valve *v, size_t n, char name[2])
{
	size_t i;

	for (i = 0; i < n; ++i)
	{
		if (!memcmp(v[i].name, name, 2))
			return i;
	}
	fprintf(stderr, "Not found: '%c%c'\n", name[0], name[1]);
	return -1;
}

uint64_t hash(int curr, int remaining, uint64_t open, uint8_t others)
{
	return ((uint64_t) curr << 58) | ((uint64_t) remaining << 53) |
	       (open << 1) | others;
}

bool find_in_buffer(int curr, int remaining, uint64_t open, uint8_t others,
		    int *maxp)
{
	uint64_t idx;

	idx = hash(curr, remaining, open, others) % BUFFER_SIZE;
	if (buffer[idx].filled && buffer[idx].curr == curr &&
	    buffer[idx].remaining == remaining && buffer[idx].open == open &&
	    buffer[idx].others == others)
	{
		if (maxp)
			*maxp = buffer[idx].max;
		return true;
	}
	return false;
}

void put_buffer(int curr, int remaining, uint64_t open, int max, uint8_t others)
{
	uint64_t idx;

	idx		   = hash(curr, remaining, open, others) % BUFFER_SIZE;
	buffer[idx].filled = true;
	buffer[idx].curr   = curr;
	buffer[idx].remaining = remaining;
	buffer[idx].open      = open;
	buffer[idx].others    = others;
	buffer[idx].max	      = max;
}
