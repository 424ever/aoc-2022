/*
 * Day 13 -- Distress Signal
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "aoc.h"

#define MAX_PACKS 500

struct packet
{
	bool		is_array;
	int		value;
	int		nvalues;
	struct packet **values;
};

static void	      day13_sol_func(FILE *, FILE *, FILE *);
static struct packet *parse_packet(char **);
static int	      compare_packets(struct packet *, struct packet *);
static int	      pack_compar(const void *, const void *);
static void	      print_packet(struct packet *, FILE *);
static void	      packet_free(struct packet *);

static struct aoc_sol day13_sol = {.name = "day13", .sol = &day13_sol_func};

void __attribute__((constructor)) day13_init(void)
{
	if (!register_sol(day13_sol))
		fprintf(stderr, "day13 load failed.\n");
}

void day13_sol_func(FILE *in_f, FILE *out_f, FILE *debug_out)
{
	char	      *line;
	char	      *tmp;
	char	      *divpackstr;
	size_t	       i;
	size_t	       idx_prod;
	size_t	       idx_sum;
	size_t	       npackets;
	size_t	       nread;
	struct packet *div_packs[2];
	struct packet *packets[MAX_PACKS];

	idx_prod = 1;
	idx_sum	 = 0;
	line	 = NULL;
	npackets = 0;

	while (getline(&line, &nread, in_f) != -1)
	{
		rtrim(line);
		tmp		    = line;
		packets[npackets++] = parse_packet(&tmp);

		if (getline(&line, &nread, in_f) == -1)
			break;

		rtrim(line);
		tmp		    = line;
		packets[npackets++] = parse_packet(&tmp);

		if (getline(&line, &nread, in_f) == -1)
			break;
	}

	for (i = 0; i * 2 < npackets; ++i)
	{
		fprintf(debug_out, "== Pair %ld ==\n", i + 1);
		if (compare_packets(packets[i * 2], packets[i * 2 + 1]) < 0)
		{
			fprintf(debug_out, "correct order\n");
			idx_sum += i + 1;
		}
		else
			fprintf(debug_out, "incorrect order\n");
	}

	for (i = 0; i < npackets; ++i)
	{
		print_packet(packets[i], debug_out);
		fputc('\n', debug_out);
	}

	divpackstr   = "[[2]]";
	div_packs[0] = parse_packet(&divpackstr);
	divpackstr   = "[[6]]";
	div_packs[1] = parse_packet(&divpackstr);

	packets[npackets++] = div_packs[0];
	packets[npackets++] = div_packs[1];

	qsort(packets, npackets, sizeof(struct packet *), &pack_compar);

	fprintf(debug_out, "\n\n == After sort ==\n");
	for (i = 0; i < npackets; ++i)
	{
		if (packets[i] == div_packs[0] || packets[i] == div_packs[1])
			idx_prod *= i + 1;
		print_packet(packets[i], debug_out);
		fputc('\n', debug_out);
		packet_free(packets[i]);
	}

	fprintf(out_f, "%ld\n%ld\n", idx_sum, idx_prod);

	free(line);
}

struct packet *parse_packet(char **lineptr)
{
	struct packet *pack;

	pack = calloc(1, sizeof(*pack));

	if (**lineptr == '[')
	{
		pack->is_array = true;
		++*lineptr;

		for (;;)
		{
			if (**lineptr == ']')
				break;

			++pack->nvalues;
			pack->values = reallocarray(pack->values, pack->nvalues,
						    sizeof(struct packet *));

			pack->values[pack->nvalues - 1] = parse_packet(lineptr);
			++*lineptr;
			if (**lineptr == ',')
				++*lineptr;
		}
	}
	else
	{
		pack->is_array = false;
		sscanf(*lineptr, "%d", &pack->value);

		while (**lineptr != ',' && **lineptr != ']' &&
		       **lineptr != '\0')
			++*lineptr;
		--*lineptr;
	}

	return pack;
}

int compare_packets(struct packet *a, struct packet *b)
{
	int cmp;
	int i;

	i   = 0;
	cmp = 0;

	if (!a->is_array && !b->is_array)
		cmp = a->value - b->value;

	else if (a->is_array && b->is_array)
	{
		for (;;)
		{
			if (i == a->nvalues && i == b->nvalues)
			{
				cmp = 0;
				break;
			}

			if (i == a->nvalues)
			{
				cmp = -1;
				break;
			}

			if (i == b->nvalues)
			{
				cmp = 1;
				break;
			}

			cmp = compare_packets(a->values[i], b->values[i]);
			if (cmp != 0)
				break;
			++i;
		}
	}

	else if (a->is_array)
	{
		b->is_array	    = true;
		b->values	    = malloc(sizeof(struct packet *));
		b->values[0]	    = calloc(1, sizeof(struct packet));
		b->nvalues	    = 1;
		b->values[0]->value = b->value;
		cmp		    = compare_packets(a, b);
		free(b->values[0]);
		free(b->values);
		b->is_array = false;

		return cmp;
	}

	else if (b->is_array)
	{
		a->is_array	    = true;
		a->values	    = malloc(sizeof(struct packet *));
		a->values[0]	    = calloc(1, sizeof(struct packet));
		a->nvalues	    = 1;
		a->values[0]->value = a->value;
		cmp		    = compare_packets(a, b);
		free(a->values[0]);
		free(a->values);
		a->is_array = false;

		return cmp;
	}

	return cmp;
}

void print_packet(struct packet *p, FILE *f)
{
	int i;

	if (p->is_array)
	{
		fputc('[', f);
		for (i = 0; i < p->nvalues; ++i)
		{
			print_packet(p->values[i], f);
			fputc(',', f);
		}
		fputc(']', f);
	}
	else
	{
		fprintf(f, "%d", p->value);
	}
}

void packet_free(struct packet *p)
{
	int i;

	if (p->is_array)
	{
		for (i = 0; i < p->nvalues; ++i)
			packet_free(p->values[i]);
		free(p->values);
	}
	free(p);
}

int pack_compar(const void *a, const void *b)
{
	return compare_packets((struct packet *) *((const void **) a),
			       (struct packet *) *((const void **) b));
}
