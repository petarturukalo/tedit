#include <stdlib.h>
#include "clrmap.h"
#include "colour.h"
#include "../log.h"

void clrmap_init(clrmap_t *c, WINDOW *w)
{
	matrix_init(&c->clrmap, getmaxy(w), getmaxx(w), sizeof(clrpair_t));
	c->win = w;
	c->enabled = false;
}

/*
 * Take a snapshot of the current lines in a file buffer that are in view, storing
 * a merged copy of the lines into a single flattened string. Lines in the flat string
 * are delimited by a newline.
 *
 * @s: string to store flattened lines in
 */
static void take_flat_lines_snapshot(fbuf_t *f, str_t *s)
{
	int top_row = f->view.lines_top_row;
	int last_col, first_col = f->view.lines_first_col;
	int bot_row = view_lines_bot_row(&f->view, &f->lines);
	line_t *l;

	for (int i = top_row; i <= bot_row; ++i) {
		l = dlist_get_address(&f->lines, i);
		last_col = view_line_last_col(&f->view, l);
		if (last_col < first_col)
			// The end of the line is left of the start of the view. Need to
			// add a newline so that a next line is moved to when applying regex
			// matches to the colour map.
			str_append(s, '\n');
		else  {
			str_cat_substr(s, l->array, first_col, last_col);
			// If the cut off point of the line didn't have a newline then tag one on
			// so that clrmap_paint() knows to move to a next line. Note that this newline
			// will be out of view: because the colour map dimensions are similar to the view,
			// the call to matrix_set() on this newline character will fail and be skipped.
			if (s->array[s->len-1] != '\n') 
				str_append(s, '\n');
		}
	}
	str_append(s, '\0');
}

/*
 * Resize the width and height of the colour map to match the current size of the curses window.
 */
static void clrmap_resz(clrmap_t *c)
{
	matrix_resz(&c->clrmap, getmaxy(c->win), getmaxx(c->win));
}

/*
 * Return 
 * - a negative number if r1 < r2
 * - 0 if r1 == r2
 * - a positive number if r1 > r2
 */
static int regmatch_data_cmp(regmatch_data_t *r1, regmatch_data_t *r2)
{
	return r1->start - r2->start;
}

/*
 * Update a row and column position to the next cell in a colour map.
 * @row: out-param row to update
 * @col: out-param column to update
 * @c: current char in flat lines snapshot
 */
static void clrmap_next_cell(int *row, int *col, char c)
{
	if (c == '\n') {
		// Move to start of next line.
		(*row)++;
		*col = 0;
	} else {
		// Move to next column.
		(*col)++;
	}
}

/*
 * Populate the cells of the colour map with clrpair_t according to the list of regmatch_data_t.
 * @flat_lines_snapshot: string the matches substrings are in
 * @matches: list of regmatch_data_t
 * @v: view used to take flat lines snapshot
 */
static void clrmap_paint(matrix_t *clrmap, char *flat_lines_snapshot, dlist_t *matches, view_t *v)
{
	regmatch_data_t rdummy, *r, *rnext;
	// Current row, column cell position in the colour map.
	int row = view_display_top_row(v);
	int col = view_display_first_col(v) ;
	int i = 0;  // Current index in the string.
	int j = 0;  // Current index in matches.
	int n = strlen(flat_lines_snapshot);

	matrix_memset(clrmap, COLOUR_DEFAULT);
	// Sort substrings in ascending order of start index so that nested substring matches can 
	// be skipped.
	qsort(matches->array, matches->len, sizeof(regmatch_data_t), (int (*)(const void *, const void *))regmatch_data_cmp);
	rdummy.end = 0;  
	r = &rdummy;

	while (j < matches->len) {
		// Skip all substrings inside the current substring, e.g. a keyword inside a string, such as keyword
		// do inside string "do this".
		do {
			rnext = dlist_get_address(matches, j++);
		} while (j < matches->len && rnext->start < r->end);
		// Couldn't find any more substrings past the most recently applied substring.
		if (j == matches->len && rnext->start < r->end)
			break;
		r = rnext;
		// Move past gap before substring.
		for (; i < n && i < r->start; ++i)
			clrmap_next_cell(&row, &col, flat_lines_snapshot[i]);
		// Fill substring with its colour.
		for (; i < n && i < r->end; ++i)  {
			matrix_set(clrmap, row, col, &r->clrpair);
			clrmap_next_cell(&row, &col, flat_lines_snapshot[i]);
		}
	}
}

void clrmap_syntax_highlight(clrmap_t *c, fbuf_t *f)
{
	syntax_rule_t *rules = find_syntax_rules(fbuf_link_name(f));

	c->enabled = rules;
	
	if (rules) {
		dlist_t matches;
		str_t flat_lines_snapshot;

		dlist_init(&matches, DLIST_MIN_CAP, sizeof(regmatch_data_t));
		str_alloc(&flat_lines_snapshot, DLIST_MIN_CAP);

		clrmap_resz(c);
		take_flat_lines_snapshot(f, &flat_lines_snapshot);
		// Get rid of pseudo spaces so don't have to worry about handling them in regex.
		str_replace_pspaces(&flat_lines_snapshot);
		exec_syntax_rules(flat_lines_snapshot.array, rules, &matches);
		clrmap_paint(&c->clrmap, flat_lines_snapshot.array, &matches, &f->view);

		dlist_free(&matches, NULL);
		dlist_free(&flat_lines_snapshot, NULL);
	} 
}

void clrmap_free(clrmap_t *c)
{
	matrix_free(&c->clrmap);
}
