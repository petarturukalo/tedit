/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#include <stdlib.h>
#include "clrmap.h"
#include "colour.h"
#include "../log.h"

/*
 * Number of extra lines above the view to include when syntax highlighting so that 
 * a syntax element such as a multiline comment can be coloured in case its start is
 * above the view, or its end is below the view.
 *
 * If a very long (in height) multiline comment still isn't being coloured then increase this.
 */
static const int EXTRA_LINES = 63;

void clrmap_init(clrmap_t *c, WINDOW *w)
{
	matrix_init(&c->clrmap, getmaxy(w), getmaxx(w), sizeof(clrpair_t));
	c->win = w;
}

/*
 * Take a snapshot of the current lines in a file buffer centered around the file buffer's view. 
 * A merged copy of the lines is stored into a single flattened string. Lines in the flat string
 * are delimited by a newline.
 *
 * @s: string to store flattened lines in
 * @extra_lines: number of extra lines above the view to include in the snapshot. And the same for
 *	below the view.
 */
static void take_flat_lines_snapshot(fbuf_t *f, str_t *s, int extra_lines)
{
	lines_t *ls = &f->lines;
	int top_row = lines_add_row(ls, f->view.lines_top_row, -extra_lines);
	int bot_row = lines_add_row(ls, view_lines_bot_row(&f->view, ls), extra_lines);
	line_t *l;

	for (int i = top_row; i <= bot_row; ++i) {
		l = dlist_get_address(&f->lines, i);
		dlist_cat(s, l);
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
 * Update a cursor's position to the start of the next line/row.
 */
static void next_row(cursor_t *c)
{
	++c->row;
	c->col = 0;
}

/*
 * Update a cursor position to a next cell position dependent on the
 * current character that it is over.
 */
static void next_cell(cursor_t *crs, char c)
{
	if (c == '\n') 
		next_row(crs);
	else 
		++crs->col;
}

/* Bookkeeping data used by clrmap_paint(). */
struct paint_data {
	// The cursor positions are kept separate because the set of lines that the colour 
	// map covers will be a subset of the lines in the flat lines snapshot string, since
	// the snapshot includes extra lines.
	cursor_t cpos;  // Current cell position in the colour map.
	cursor_t spos;  // Current cell position in the lines snapshot string if it weren't flat.
	matrix_t *clrmap;
	view_t *v;
	// The actual amount of extra lines above the view. This can be less than the extra lines passed
	// to take_flat_lines_snapshot() when the top of the view is close to the top of the lines as
	// there aren't enough actual extra lines to include.
	int extra_lines_above;  
	// Current number of consecutive in-view columns found. Gets reset to 0 whenever an
	// out-of-view column is found.
	int nconsec_cols_in_view;  
};

/*
 * Get whether the current cursor position tracking the string is within the view vertically.
 * If true it could still be out of view horizontally.
 */
static bool spos_in_vertical_view(struct paint_data *p)
{
	int row = p->spos.row;
	return row >= p->extra_lines_above && row < p->extra_lines_above+view_height(p->v);
}

/*
 * Get whether the current cursor position tracking the string is in view and
 * over the colour map. 
 */
static bool spos_in_view(struct paint_data *p)
{
	return spos_in_vertical_view(p) && col_in_view(p->v, p->spos.col);
}

/*
 * Set the colour at the colour map cursor position if the string cursor is in view. 
 * Update all cursors to their next cell position.
 */
static void paint(struct paint_data *p, clrpair_t clrpair, char c)
{
	if (spos_in_view(p)) {
		if (clrpair != COLOUR_DEFAULT)
			matrix_set(p->clrmap, p->cpos.row, p->cpos.col, &clrpair);
		next_cell(&p->cpos, c);
		if (c == '\n')
			// Moved to a next line because found a newline character, so
			// make sure don't move to a next next line when get out of view.
			p->nconsec_cols_in_view = 0;
		else
			++p->nconsec_cols_in_view;
	} else if (p->nconsec_cols_in_view) {  
		// If reached here then got out of view but hadn't found a newline: force
		// colour map cursor onto the next line.
		next_row(&p->cpos);
		p->nconsec_cols_in_view = 0;
	} else if (spos_in_vertical_view(p) && p->spos.col < p->v->lines_first_col && c == '\n')
		// Found a newline left of the view: force colour map cursor onto next line.
		next_row(&p->cpos);
	next_cell(&p->spos, c);
}

/*
 * Populate the cells of the colour map with clrpair_t according to the list of regmatch_data_t.
 * @flat_lines_snapshot: string the matches substrings are in
 * @matches: list of regmatch_data_t
 * @v: view used to take flat lines snapshot
 * @extra_lines: see take_flat_lines_snapshot()
 */
static void clrmap_paint(matrix_t *clrmap, char *flat_lines_snapshot, dlist_t *matches, view_t *v,
			 int extra_lines)
{
	regmatch_data_t rdummy, *r, *rnext;
	struct paint_data p = {
		.cpos = { view_display_top_row(v), view_display_first_col(v) },
		.spos = { 0, 0 },
		.clrmap = clrmap,
		.v = v,
		.extra_lines_above = v->lines_top_row < extra_lines ? v->lines_top_row : extra_lines,
		.nconsec_cols_in_view = 0
	};
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
			paint(&p, COLOUR_DEFAULT, flat_lines_snapshot[i]);
		// Fill substring with its colour.
		for (; i < n && i < r->end; ++i)  
			paint(&p, r->clrpair, flat_lines_snapshot[i]);
		
	}
}

bool clrmap_syntax_highlight(clrmap_t *c, fbuf_t *f)
{
	syntax_rule_t *rules = find_syntax_rules(fbuf_link_name(f));

	if (rules) {
		dlist_t matches;
		str_t flat_lines_snapshot;

		dlist_init(&matches, DLIST_MIN_CAP, sizeof(regmatch_data_t));
		str_alloc(&flat_lines_snapshot, DLIST_MIN_CAP);

		clrmap_resz(c);
		take_flat_lines_snapshot(f, &flat_lines_snapshot, EXTRA_LINES);
		// Get rid of pseudo spaces so don't have to worry about handling them in regex.
		str_replace_pspaces(&flat_lines_snapshot);
		exec_syntax_rules(flat_lines_snapshot.array, rules, &matches);
		clrmap_paint(&c->clrmap, flat_lines_snapshot.array, &matches, &f->view, EXTRA_LINES);

		dlist_free(&matches, NULL);
		dlist_free(&flat_lines_snapshot, NULL);
	} 
	return rules;
}

void clrmap_free(clrmap_t *c)
{
	matrix_free(&c->clrmap);
}
