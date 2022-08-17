/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "cursor.h"

void cursor_set_col_manual(cursor_t *c, int col)
{
	c->col = col;
	c->prev_manual_mv_col = col;
}

void cursor_set_row(cursor_t *c, int row, lines_t *ls)
{
	line_t *l = dlist_get_address(ls, row);

	c->row = row;
	c->col = c->prev_manual_mv_col;

	// Push cursor left onto end of line, but don't overwrite the manually
	// moved to column position so that the previous column position can be returned
	// to if another row is immediately moved to.
	if (line_len(l) < c->col)
		c->col = line_len(l);
	// Don't let the cursor sit on the space-filled (imaginary) part of a tab.
	while (l->array[c->col] == TAB_CONT)
		++c->col;
}

void cursor_add_row(cursor_t *c, int offset, lines_t *ls)
{
	return cursor_set_row(c, c->row+offset, ls);
}

void cursor_add_col_manual(cursor_t *c, int offset)
{
	cursor_set_col_manual(c, c->col+offset);
}

void cursor_reset(cursor_t *c)
{
	c->row = 0;
	cursor_set_col_manual(c, 0);
}
