/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Functions related to parts of a file that are in "view".
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef VIEW_H
#define VIEW_H

#include <curses.h>
#include "cursor.h"
#include "lines.h"

struct view {
	// First row and column displayed on the screen (inclusive), or in "view".
	int lines_top_row;
	int lines_first_col;
	// Offsets from borders of curses windows. An offset is the distance from a border of the curses
	// window that a file starts/finishes getting displayed.
	// For the top row offset, for example, an offset of 0 means the file starts getting displayed on the
	// top row (the 0th index row), and an offset of 1 the row with index 1. A negative index can also be
	// supplied, wrapping around to the other side of the dimension: a -1 top row offset has the top row
	// start at the last line (there is a distance of 1 moving upwards and wrapping around to the bottom row).
	int win_top_row_off;
	int win_bot_row_off;  // Bottom row.
	int win_first_col_off;
	int win_last_col_off;
	WINDOW *win;
	bool pgmv;  // Whether moved by a pgup/dn rather than arrow key.
};

typedef struct view view_t;

/**
 * view_init - Set default values for a view and set up the structure with fields
 *
 * See struct view fields for descriptions of params.
 */
void view_init(view_t *v, WINDOW *w, int top_row_offset, int bot_row_offset, 
	       int first_col_offset, int last_col_offset);

/**
 * view_reset - Reset a view to default values
 */
void view_reset(view_t *v);

/**
 * view_height - Get the height of the view
 *
 * This view is the view as it appears restricted by the init time offsets.
 */
int view_height(view_t *v);

/**
 * view_width - Get the width of the view
 *
 * This view is the view as it appears restricted by the init time offsets.
 */
int view_width(view_t *v);

/**
 * view_display_top_row - Get the index of the line on the screen to start displaying the
 *	file at (inclusive)
 */
int view_display_top_row(view_t *v);

/**
 * view_display_first_col - Get the index of the column on the screen to starts displaying the
 *	file at (inclusive)
 */
int view_display_first_col(view_t *v);

/**
 * view_sync_cursor - Sync the view with the cursor in case the cursor is out of view
 *
 * Makes the cursor in view and can be seen when displayed.
 */
void view_sync_cursor(view_t *v, cursor_t *c, lines_t *ls);

/**
 * view_cursor_display_row - Get the row of a cursor as it would appear in a view (so it can be displayed)
 */
int view_cursor_display_row(view_t *v, cursor_t *c);

/**
 * view_cursor_display_col - Get the column of a cursor as it would appear in a view (so it can be displayed)
 */
int view_cursor_display_col(view_t *v, cursor_t *c);

#endif
