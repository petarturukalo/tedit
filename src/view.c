/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "view.h"

void view_reset(view_t *v)
{
	v->lines_top_row = 0;
	v->lines_first_col = 0;
	v->pgmv = false;
}

void view_init(view_t *v, WINDOW *w, int top_row_offset, int bot_row_offset, 
	       int first_col_offset, int last_col_offset)
{
	view_reset(v);
	v->win = w;
	v->win_top_row_off = top_row_offset;
	v->win_bot_row_off = bot_row_offset;
	v->win_first_col_off = first_col_offset;
	v->win_last_col_off = last_col_offset;
}

/*
 * len_start_ind - Restrict a length by adding an offset from the start
 *
 * See struct view struct members for descriptions of offset params.
 */
int len_start_ind(int len, int start_offset)
{
	return start_offset >= 0 ? start_offset : len+start_offset;
}

/*
 * len_end_ind - Restrict a length by adding an offset from the end
 *
 * See struct view struct members for descriptions of offset params.
 */
int len_end_ind(int len, int end_offset)
{
	return end_offset >= 0 ? len-1-end_offset : -1-end_offset;
}

/*
 * len_shrink - Shrink a length to a smaller length by applying two border offsets
 *
 * See struct view struct member for descriptions of offset params.
 */
int len_shrink(int len, int start_offset, int end_offset)
{
	// Start and end of range over len that makes new len (both ends inclusive).
	int start, end;

	start = len_start_ind(len, start_offset);
	end = len_end_ind(len, end_offset);

	// Get the length of the range.
	return end-start+1;
}

int view_height(view_t *v)
{
	return len_shrink(getmaxy(v->win), v->win_top_row_off, v->win_bot_row_off);
}

int view_width(view_t *v)
{
	return len_shrink(getmaxx(v->win), v->win_first_col_off, v->win_last_col_off);
}

int view_display_top_row(view_t *v)
{
	return len_start_ind(getmaxy(v->win), v->win_top_row_off);
}

int view_display_first_col(view_t *v)
{
	return len_start_ind(getmaxx(v->win), v->win_first_col_off);
}

/*
 * view_lines_bot_row - Get the last row on the lines underlying a file which the
 *	view covers (inclusive)
 *
 * This is aligned with the cursor on the lines.
 */
int view_lines_bot_row(view_t *v)
{
	return v->lines_top_row+view_height(v)-1;
}

/*
 * view_lines_last_col - Get the last column on the lines underlying a file which the
 *	view covers (inclusive)
 *
 * This is aligned with the cursor on the lines.
 */
int view_lines_last_col(view_t *v)
{
	return v->lines_first_col+view_width(v)-1;
}

/*
 * view_crs_above - Get whether the cursor is above and out of the view
 */
bool view_crs_above(view_t *v, cursor_t *c)
{
	return c->row < v->lines_top_row;
}

/*
 * view_crs_above_dist - Get the distance in lines the cursor is above the top of the view
 */
int view_crs_above_dist(view_t *v, cursor_t *c)
{
	return v->lines_top_row - c->row;
}

/*
 * view_crs_below - Get whether the cursor is below and out of the view
 */
bool view_crs_below(view_t *v, cursor_t *c)
{
	return c->row > view_lines_bot_row(v);
}

/*
 * view_crs_below_dist - Get the distance in lines the cursor is below the bottom of the view
 */
int view_crs_below_dist(view_t *v, cursor_t *c)
{
	return c->row - view_lines_bot_row(v);
}

/*
 * view_crs_leftward - Get whether the cursor is left and out of the view
 */
bool view_crs_leftward(view_t *v, cursor_t *c)
{
	return c->col < v->lines_first_col;
}

/*
 * view_crs_leftward_dist - Get the distance in columns the cursor is left and out of the view
 */
int view_crs_leftward_dist(view_t *v, cursor_t *c)
{
	return v->lines_first_col - c->col;
}

/*
 * view_crs_rightward - Get whether the cursor is right and out of the view
 */
bool view_crs_rightward(view_t *v, cursor_t *c)
{
	return c->col > view_lines_last_col(v);
}

/*
 * view_crs_rightward_dist - Get the distance in columns the cursor is right and out of the view
 */
int view_crs_rightward_dist(view_t *v, cursor_t *c)
{
	return c->col - view_lines_last_col(v);
}

/*
 * view_sync_crs_row_above - Synchronise the view with the cursor being above the view
 */
void view_sync_crs_row_above(view_t *v, cursor_t *c, int nlines)
{
	if (v->pgmv) {
		v->lines_top_row -= view_height(v);

		if (v->lines_top_row < 0)
			v->lines_top_row = 0;
	} else
		v->lines_top_row -= 1;
}

/*
 * view_sync_crs_row_below - Synchronise the view with the cursor being below the view
 */
void view_sync_crs_row_below(view_t *v, cursor_t *c, int nlines)
{
	if (v->pgmv) 
		v->lines_top_row += view_height(v);
	else 
		v->lines_top_row += 1;
}

/*
 * view_sync_crs_row - Sync the view with the cursor in case the cursor row is out of view by
 *	being outside the top or bottom of the view
 */
void view_sync_crs_row(view_t *v, cursor_t *c, int nlines)
{
	if (view_crs_above(v, c)) 
		view_sync_crs_row_above(v, c, nlines);
	else if (view_crs_below(v, c)) 
		view_sync_crs_row_below(v, c, nlines);
	v->pgmv = false;  // Always force off in case tried to pgup/dn when in view.
}

/*
 * view_sync_crs_col_leftward - Put the cursor back in view if it's outside and leftward of the view
 */
void view_sync_crs_col_leftward(view_t *v, cursor_t *c, int linelen)
{
	int dist = view_crs_leftward_dist(v, c);
	// Fit cursor by make leftmost part of view include the column cursor is at.
	v->lines_first_col -= dist;
}

/*
 * view_sync_crs_col_rightward - Put the cursor back in view if it's outside and rightward of the view
 */
void view_sync_crs_col_rightward(view_t *v, cursor_t *c, int linelen)
{
	int dist = view_crs_rightward_dist(v, c);
	// Fit cursor by make rightmost part of view include the column cursor is at.
	v->lines_first_col += dist;  
}

/*
 * view_sync_crs_col - Sync the view with the cursor in case the cursor column is out of view by
 *	being outside the left or right of the view
 * @linelen: length of the current line the cursor is on
 */
void view_sync_crs_col(view_t *v, cursor_t *c, int linelen)
{
	if (view_crs_leftward(v, c)) 
		view_sync_crs_col_leftward(v, c, linelen);
	else if (view_crs_rightward(v, c)) 
		view_sync_crs_col_rightward(v, c, linelen);
}

void view_sync_cursor(view_t *v, cursor_t *c, lines_t *ls)
{
	line_t *cur = dlist_get_address(ls, c->row);

	view_sync_crs_row(v, c, ls->len);
	view_sync_crs_col(v, c, line_len(cur));
}

int view_cursor_display_row(view_t *v, cursor_t *c)
{
	return view_display_top_row(v)+(c->row-v->lines_top_row);
}

int view_cursor_display_col(view_t *v, cursor_t *c)
{
	return view_display_first_col(v)+(c->col-v->lines_first_col);
}
