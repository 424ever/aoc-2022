/*
 * Day 12 -- Hill Climbing Algorithm
 */
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

struct dijkstra_node
{
	bool	 visited;
	int	 x, y;
	int	 height;
	uint32_t tenative_dist;
};

static void		     day12_sol_func(FILE *, FILE *, FILE *);
static int		     val_at(int *, int, int, int);
static struct dijkstra_node *node_at(struct dijkstra_node *, int, int, int);
static void		     print_heightmap(int *, int, int, FILE *);
static uint32_t		     dijkstra(int *, int, int, int, int, int, int);
struct dijkstra_node	    *min_unvisited(struct dijkstra_node *, int);
uint32_t		     min(uint32_t, uint32_t);

static struct aoc_sol day12_sol = {.name = "day12", .sol = &day12_sol_func};

void __attribute__((constructor)) day12_init(void)
{
	if (!register_sol(day12_sol))
		fprintf(stderr, "day12 load failed.\n");
}

void day12_sol_func(FILE *in_f, FILE *out_f, FILE *debug_out)
{
	char	*line;
	int	 endx, endy;
	int	 h;
	int	 i;
	int	 startx, starty;
	int	 w;
	int	 x, y;
	int	*heightmap;
	size_t	 nread;
	uint32_t mindist;

	line	  = NULL;
	heightmap = NULL;
	w	  = 0;
	h	  = 0;
	startx = starty = endx = endy = 0;
	mindist			      = UINT32_MAX;

	while (getline(&line, &nread, in_f) != -1)
	{
		rtrim(line);
		++h;
		w = strlen(line);
		for (i = 0; i < w; ++i)
		{
			heightmap =
			    realloc(heightmap, w * h * sizeof(*heightmap));
			if (line[i] == 'S')
			{
				startx	= i;
				starty	= h - 1;
				line[i] = 'a';
			}
			if (line[i] == 'E')
			{
				endx	= i;
				endy	= h - 1;
				line[i] = 'z';
			}
			heightmap[(h - 1) * w + i] = line[i] - 'a';
		}
	}

	fprintf(debug_out, "h %d w %d\n", h, w);
	print_heightmap(heightmap, h, w, debug_out);

	for (y = 0; y < h; ++y)
	{
		for (x = 0; x < w; ++x)
		{
			if (heightmap[y * w + x] == 0)
			{
				fprintf(debug_out, "Staring search @%d %d\n", x,
					y);
				mindist =
				    min(mindist, dijkstra(heightmap, h, w, x, y,
							  endx, endy));
			}
		}
	}

	fprintf(out_f, "%" PRIu32 "\n%" PRIu32 "\n",
		dijkstra(heightmap, h, w, startx, starty, endx, endy), mindist);
	free(heightmap);
	free(line);
}

uint32_t dijkstra(int *map, int h, int w, int startx, int starty, int endx,
		  int endy)
{
	int		      nnodes;
	int		      x, y;
	struct dijkstra_node *curr;
	struct dijkstra_node *end;
	struct dijkstra_node *neighbor;
	struct dijkstra_node *nodes;
	uint32_t	      dist;
	uint32_t	      newdist;

	nnodes = h * w + 1;
	nodes  = calloc(nnodes, sizeof(*nodes));

	for (y = 0; y < h; ++y)
	{
		for (x = 0; x < w; ++x)
		{
			nodes[y * w + x].visited       = false;
			nodes[y * w + x].x	       = x;
			nodes[y * w + x].y	       = y;
			nodes[y * w + x].height	       = map[y * w + x];
			nodes[y * w + x].tenative_dist = UINT32_MAX;
		}
	}

	curr		    = node_at(nodes, nnodes, startx, starty);
	curr->tenative_dist = 0;

	end = node_at(nodes, nnodes, endx, endy);

	for (;;)
	{
		/* Up */
		neighbor = node_at(nodes, nnodes, curr->x, curr->y - 1);
		if (neighbor && !neighbor->visited)
		{
			newdist = UINT32_MAX;
			if (neighbor->height <= curr->height + 1)
				newdist = curr->tenative_dist + 1;
			neighbor->tenative_dist =
			    neighbor->tenative_dist > newdist
				? newdist
				: neighbor->tenative_dist;
		}

		/* Down */
		neighbor = node_at(nodes, nnodes, curr->x, curr->y + 1);
		if (neighbor && !neighbor->visited)
		{
			newdist = UINT32_MAX;
			if (neighbor->height <= curr->height + 1)
				newdist = curr->tenative_dist + 1;
			neighbor->tenative_dist =
			    neighbor->tenative_dist > newdist
				? newdist
				: neighbor->tenative_dist;
		}

		/* Left */
		neighbor = node_at(nodes, nnodes, curr->x - 1, curr->y);
		if (neighbor && !neighbor->visited)
		{
			newdist = UINT32_MAX;
			if (neighbor->height <= curr->height + 1)
				newdist = curr->tenative_dist + 1;
			neighbor->tenative_dist =
			    neighbor->tenative_dist > newdist
				? newdist
				: neighbor->tenative_dist;
		}

		/* Right */
		neighbor = node_at(nodes, nnodes, curr->x + 1, curr->y);
		if (neighbor && !neighbor->visited)
		{
			newdist = UINT32_MAX;
			if (neighbor->height <= curr->height + 1)
				newdist = curr->tenative_dist + 1;
			neighbor->tenative_dist =
			    neighbor->tenative_dist > newdist
				? newdist
				: neighbor->tenative_dist;
		}

		curr->visited = true;
		if (end->visited)
			break;

		curr = min_unvisited(nodes, nnodes);
	}

	dist = end->tenative_dist;
	free(nodes);

	return dist;
}

struct dijkstra_node *node_at(struct dijkstra_node *nodes, int nnodes, int x,
			      int y)
{
	int i;

	for (i = 0; i < nnodes; ++i)
	{
		if (nodes[i].x == x && nodes[i].y == y)
			return nodes + i;
	}
	return NULL;
}

struct dijkstra_node *min_unvisited(struct dijkstra_node *nodes, int nnodes)
{
	int		      i;
	uint32_t	      mindist;
	struct dijkstra_node *min_node;

	mindist	 = UINT32_MAX;
	min_node = NULL;

	for (i = 0; i < nnodes; ++i)
	{
		if (nodes[i].tenative_dist <= mindist && !nodes[i].visited)
		{
			mindist	 = nodes[i].tenative_dist;
			min_node = nodes + i;
		}
	}

	return min_node;
}

int val_at(int *arr, int w, int i, int j)
{
	return arr[i * w + j];
}

void print_heightmap(int *map, int h, int w, FILE *f)
{
	int i, j;

	for (i = 0; i < h; ++i)
	{
		for (j = 0; j < w; ++j)
		{
			fprintf(f, "%2d ", val_at(map, w, i, j));
		}
		fputc('\n', f);
	}
}

uint32_t min(uint32_t a, uint32_t b)
{
	return a < b ? a : b;
}
