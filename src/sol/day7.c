/*
 * Day 7 -- No Space Left On Device
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

#define MAX_NAME_LEN 20

struct dirent
{
	bool            dir;
	char            name[MAX_NAME_LEN + 1];
	size_t          size;
	struct dirent  *parent;
	size_t          nchild;
	struct dirent **children;
};

static void day7_sol_func (FILE*, FILE*, FILE*);
static void write_tree (struct dirent*, FILE*, size_t);
static struct dirent *dirent_create (bool, char*, struct dirent*);
static void dirent_move_child (struct dirent*, struct dirent*);
static void dirent_free (struct dirent*);
static void dirent_remove_child (struct dirent *, struct dirent *);
static struct dirent *dirent_find (struct dirent *, char*);
static size_t dirent_tot_size (struct dirent *);
static void sum_dirs_below (struct dirent *, size_t, size_t*);
static void smallest_enough_space_freed (struct dirent *, size_t, size_t, size_t *);

static struct aoc_sol day7_sol = {
	.name = "day7",
	.sol  = &day7_sol_func
};

void __attribute__((constructor)) day7_init (void)
{
	if (!register_sol (day7_sol))
		fprintf (stderr, "day7 load failed.\n");
}

void day7_sol_func (FILE *in_f, FILE *out_f, FILE *debug_out)
{
	char           cmd[3];
	char           fname[MAX_NAME_LEN + 1];
	char          *line;
	size_t         nread;
	size_t         part1_sum;
	size_t         part2;
	struct dirent *child;
	struct dirent *cwd;
	struct dirent *root;

	line      = NULL;
	nread     = 0;
	part1_sum = 0;

	root = dirent_create (true, "(root)", NULL);
	cwd  = root;

	while (getline (&line, &nread, in_f) != -1)
	{
		if (line[0] == '$')
		{
			sscanf(line, "$ %s %s\n", cmd, fname);
			if (strcmp ("ls", cmd) == 0)
				continue;
			if (strcmp ("cd", cmd) == 0)
			{
				if (strcmp("/", fname) == 0)
				{
					cwd = root;
					continue;
				}
				if (fname[0] == '.' &&
				    fname[1] == '.')
				{
					cwd = cwd->parent;
					continue;
				}
				child = dirent_find (cwd, fname);
				if (!child)
					child = dirent_create (true, fname, cwd);
				cwd = child;
			}
		} else
		{
			if (line[0] == 'd' &&
			    line[1] == 'i' &&
			    line[2] == 'r')
			{
				child = dirent_create (true, "", cwd);
				sscanf (line, "dir %s\n", child->name);
			}
			else
			{
				child = dirent_create (false, "", cwd);
				sscanf (line, "%lu %s\n", &child->size, child->name);
			}
		}
	}
	write_tree (root, debug_out, 0);

	sum_dirs_below (root, 100000, &part1_sum);
	smallest_enough_space_freed (root, 70000000 - dirent_tot_size (root), 30000000, &part2);

	fprintf (out_f, "%ld\n%ld\n", part1_sum, part2);
	dirent_free (root);
	free (line);
}

void write_tree (struct dirent *ent, FILE *f, size_t indent)
{
	size_t i;

	for (i = 0; i < indent; ++i)
		fputc(' ', f);

	if (ent->dir)
	{
		fprintf (f, "%s/ (%ld, %ld)\n", ent->name, dirent_tot_size (ent), ent->nchild);
		for (i = 0; i < ent->nchild; ++i)
			write_tree (ent->children[i], f, indent + 2);
	} else
	{
		fprintf (f, "%s (%ld)\n", ent->name, dirent_tot_size (ent));
	}
}

struct dirent *dirent_create (bool dir, char *name, struct dirent *parent)
{
	struct dirent *ent;

	ent = calloc (1, sizeof (struct dirent));
	ent->dir = dir;
	strncpy (ent->name, name, MAX_NAME_LEN);

	if (parent)
		dirent_move_child (ent, parent);

	return ent;
}

void dirent_move_child (struct dirent *child, struct dirent *parent)
{
	dirent_remove_child (child, parent);
	parent->nchild++;
	parent->children = reallocarray (parent->children, parent->nchild, sizeof (struct dirent*));
	parent->children[parent->nchild - 1] = child;
	child->parent = parent;
}

void dirent_remove_child (struct dirent *child, struct dirent *parent)
{
	size_t i;

	for (i = 0; i < parent->nchild; ++i)
	{
		if (parent->children[i] == child)
		{
			if (i != parent->nchild - 1)
				memmove(parent->children + i, parent->children + i + 1, parent->nchild - 1 - i);
			parent->children = reallocarray (parent->children, parent->nchild - 1, sizeof (struct dirent*));
			--parent->nchild;
			break;
		}
	}
}

void dirent_free (struct dirent *ent)
{
	size_t i;

	for (i = 0; i < ent->nchild; ++i)
		dirent_free (ent->children[i]);
	free (ent->children);
	free (ent);
}

struct dirent *dirent_find (struct dirent *ent, char *cname)
{
	size_t i;

	for (i = 0; i < ent->nchild; ++i)
	{
		if (strcmp (ent->children[i]->name, cname) == 0)
			return ent->children[i];
	}
	return NULL;
}

size_t dirent_tot_size (struct dirent *ent)
{
	size_t i, size;

	if (!ent->dir)
		size = ent->size;
	else
	{
		size = 0;
		for (i = 0; i < ent->nchild; ++i)
		{
			size += dirent_tot_size (ent->children[i]);
		}
	}

	return size;
}

void sum_dirs_below (struct dirent *ent, size_t limit, size_t *sum)
{
	size_t i;

	for (i = 0; i < ent->nchild; ++i)
	{
		if (!ent->children[i]->dir)
			continue;
		sum_dirs_below (ent->children[i], limit, sum);
	}

	if (ent->dir && dirent_tot_size (ent) <= limit)
		*sum += dirent_tot_size (ent);

}

void smallest_enough_space_freed (struct dirent *ent, size_t free, size_t needed, size_t *candidate)
{
	size_t i;

	if (ent->dir && needed <= free + dirent_tot_size (ent) && dirent_tot_size (ent) < *candidate)
		*candidate = dirent_tot_size (ent);
	for (i = 0; i < ent->nchild; ++i)
		smallest_enough_space_freed (ent->children[i], free, needed, candidate);
}
