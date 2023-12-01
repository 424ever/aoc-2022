/*
 * Day 15 -- Beacon Exclusion Zone
 */
#include <inttypes.h>
#include <limits.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "aoc.h"

struct pos
{
	int x, y;
};

struct sensor
{
	struct pos pos;
	int	   dist_to_closest;
};

static void   day15_sol_func(FILE *, FILE *, FILE *);
static bool   can_see_pos(struct sensor, struct pos);
static int    manhattan_dist(struct pos, struct pos);
static int    max(int, int);
static int    min(int, int);
static size_t count_unavail_y(size_t, struct sensor *, struct pos *, size_t);
static struct sensor *any_can_see_pos(struct sensor *, size_t, struct pos);
static void parse_sensors(FILE *, struct sensor **, struct pos **, size_t *);
static void print_sensors(FILE *, struct sensor *, size_t);

static struct aoc_sol day15_sol = {.name = "day15", .sol = day15_sol_func};

static size_t dist_call_count = 0;

void __attribute__((constructor)) day15_init(void)
{
	if (!register_sol(day15_sol))
		fprintf(stderr, "day15 load failed.\n");
}

void day15_sol_func(FILE *in_f, FILE *out_f, FILE *debug_out)
{
	bool	       found;
	int	       maxdim;
	int	       rem;
	size_t	       i;
	size_t	       nsensors;
	size_t	       unavail;
	struct pos     check_pos;
	struct pos    *beacons;
	struct sensor *sensors;

	beacons	 = NULL;
	maxdim	 = 0;
	nsensors = 0;
	sensors	 = NULL;
	unavail	 = 0;

	setlocale(LC_ALL, "");

	parse_sensors(in_f, &sensors, &beacons, &nsensors);
	print_sensors(debug_out, sensors, nsensors);

	if (is_test_mode())
		unavail = count_unavail_y(10, sensors, beacons, nsensors);
	else
		unavail = count_unavail_y(2000000, sensors, beacons, nsensors);

	fprintf(debug_out, "(part1) manhattan_dist () called %ld times\n",
		dist_call_count);
	dist_call_count = 0;
	fprintf(out_f, "%ld\n", unavail);

	if (is_test_mode())
		maxdim = 20;
	else
		maxdim = 4000000;

	found	    = false;
	check_pos.x = 0;
	check_pos.y = 0;
	for (i = 0; i < nsensors; ++i)
	{
		check_pos.x = sensors[i].pos.x;
		for (check_pos.y =
			 sensors[i].pos.y - sensors[i].dist_to_closest - 1;
		     check_pos.y <
		     sensors[i].pos.y + sensors[i].dist_to_closest + 2;
		     ++check_pos.y)
		{
			rem = sensors[i].dist_to_closest -
			      abs(sensors[i].pos.y - check_pos.y) + 1;

			check_pos.x = sensors[i].pos.x - rem;

			if (check_pos.x >= 0 && check_pos.x <= maxdim &&
			    check_pos.y >= 0 && check_pos.y <= maxdim &&
			    !any_can_see_pos(sensors, nsensors, check_pos))
			{
				found = true;
				break;
			}

			if (rem > 0)
			{
				check_pos.x = sensors[i].pos.x - rem;

				if (check_pos.x >= 0 && check_pos.x <= maxdim &&
				    check_pos.y >= 0 && check_pos.y <= maxdim &&
				    !any_can_see_pos(sensors, nsensors,
						     check_pos))
				{
					found = true;
					break;
				}
			}
		}

		fprintf(debug_out, "check sensor %ld of %ld\n", i + 1,
			nsensors);

		if (found)
			break;
	}

	fprintf(out_f, "%ld\n", (long) check_pos.x * 4000000 + check_pos.y);
	fprintf(debug_out, "(part2) manhattan_dist () called %ld times\n",
		dist_call_count);
	free(sensors);
}

size_t count_unavail_y(size_t y, struct sensor *s, struct pos *b, size_t n)
{
	bool	   pos_is_beacon;
	int	   minx, maxx;
	size_t	   count;
	size_t	   i;
	struct pos check_pos;

	count	    = 0;
	check_pos.y = y;
	maxx	    = INT_MIN;
	minx	    = INT_MAX;

	for (i = 0; i < n; ++i)
	{
		minx = min(minx, s[i].pos.x);
		maxx = max(maxx, s[i].pos.x);
	}

	check_pos.x = minx;
	while (any_can_see_pos(s, n, check_pos))
	{
		--check_pos.x;
		--minx;
	}

	check_pos.x = maxx;
	while (any_can_see_pos(s, n, check_pos))
	{
		++check_pos.x;
		++maxx;
	}

	for (check_pos.x = minx; check_pos.x <= maxx; ++check_pos.x)
	{
		pos_is_beacon = false;
		for (i = 0; i < n; ++i)
		{
			if (check_pos.x == b[i].x && check_pos.y == b[i].y)
				pos_is_beacon = true;
		}
		if (pos_is_beacon)
			continue;

		if (any_can_see_pos(s, n, check_pos))
			++count;
	}

	return count;
}

struct sensor *any_can_see_pos(struct sensor *s, size_t n, struct pos p)
{
	size_t i;

	for (i = 0; i < n; ++i)
	{
		if (can_see_pos(s[i], p))
			return s + i;
	}
	return NULL;
}

bool can_see_pos(struct sensor s, struct pos p)
{
	return manhattan_dist(s.pos, p) <= s.dist_to_closest;
}

int min(int a, int b)
{
	return a < b ? a : b;
}

int max(int a, int b)
{
	return a > b ? a : b;
}

void parse_sensors(FILE *f, struct sensor **s, struct pos **b, size_t *n)
{
	char	      *line;
	size_t	       nread;
	struct pos    *beac;
	struct sensor *sensor;

	line = NULL;

	while (getline(&line, &nread, f) != -1)
	{
		++*n;
		*s     = reallocarray(*s, *n, sizeof(**s));
		*b     = reallocarray(*b, *n, sizeof(**b));
		sensor = *s + *n - 1;
		beac   = *b + *n - 1;

		rtrim(line);
		sscanf(line,
		       "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d",
		       &sensor->pos.x, &sensor->pos.y, &beac->x, &beac->y);

		sensor->dist_to_closest = manhattan_dist(sensor->pos, *beac);
	}

	free(line);
}

int manhattan_dist(struct pos a, struct pos b)
{
	++dist_call_count;
	return abs(a.x - b.x) + abs(a.y - b.y);
}

void print_sensors(FILE *f, struct sensor *s, size_t n)
{
	size_t i;

	for (i = 0; i < n; ++i)
	{
		fprintf(f, "%ld: sensor { x: %d y: %d dist: %d }\n", i,
			s[i].pos.x, s[i].pos.y, s[i].dist_to_closest);
	}
}
