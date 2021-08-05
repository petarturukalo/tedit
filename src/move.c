/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "move.h"

void mv_left(cursor_t *crs, line_t *l)
{
	char c;
	int tab_ind, i = crs->col;

	if (i > 0) {
		// Character moving over.
		c = l->buf[i-1];

		if (c == TAB_CONT) {
			// Move all the way to the start of a tab.
			tab_ind = chrp_find_reverse(l->buf, TAB_START, 0, i-2);
			cursor_set_col_manual(crs, tab_ind);
		} else  
			cursor_add_col_manual(crs, -1);
	}
}

void mv_down(cursor_t *c, lines_t *ls)
{
	if (c->row < ls->len-1)
		cursor_add_row(c, 1, ls);
}

void mv_up(cursor_t *c, lines_t *ls)
{
	if (c->row > 0)
		cursor_add_row(c, -1, ls);
}

void mv_right(cursor_t *crs, line_t *l)
{
	char c;
	int tlen, i = crs->col;

	if (i == line_len(l)-1) 
		// Go right by one regardless since moving to either a newline or there's nothing 
		// there on the last line (both act as an append point).
		cursor_add_col_manual(crs, 1);
	else if (i < line_len(l)-1) {
		c = l->buf[i];

		if (c == TAB_START) {
			tlen = tablen(l, i);
			cursor_add_col_manual(crs, tlen);
		} else 
			cursor_add_col_manual(crs, 1);
	}
}

void mv_start(cursor_t *c)
{
	cursor_set_col_manual(c, 0);
}

void mv_end(cursor_t *c, line_t *l)
{
	cursor_set_col_manual(c, line_len(l));
}

void mv_view_pgup(cursor_t *c, lines_t *ls, view_t *v)
{
	int prev_row, target_row = c->row - view_height(v);

	prev_row = c->row;

	// Prevent cursor going passed top of file.
	if (target_row < 0)
		target_row = 0;
	if (target_row != prev_row) {
		// Only flag a page move if moved so don't keep moving up accidentally.
		cursor_set_row(c, target_row, ls);
		v->pgmv = true;
	}
}

void mv_view_pgdn(cursor_t *c, lines_t *ls, view_t *v)
{
	int prev_row, target_row = c->row+view_height(v);

	prev_row = c->row;

	// Prevent falling off bottom of file.
	if (target_row >= ls->len)
		target_row = ls->len-1;
	if (target_row != prev_row) {
		cursor_set_row(c, target_row, ls);
		v->pgmv = true;
	}
}

