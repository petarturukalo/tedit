/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef CURSOR_H
#define CURSOR_H

#include "line.h"
#include "lines.h"
#include "tab.h"

/*
 * A row, column position over a list of lines.
 * Both row and column are 0-indexed.
 */
struct cursor {
	int row;
	int col;
	// Last column manually moved to, where manual refers to
	// having been moved by the user pressing a keyboard key. 
	int prev_manual_mv_col;
};

typedef struct cursor cursor_t;


/*
 * cursor_set_row - Set the row position of a cursor
 * @ls: lines of file buffer cursor is on
 *
 * O(tabsz) worst case time complexity.
 */
void cursor_set_row(cursor_t *c, int row, lines_t *ls);

/*
 * cursor_add_row - Set the row position of a cursor manually by the user pressing a 
 *	keyboard key
 * @offset: offset to add to current cursor row
 *
 * O(tabsz) worst case time complexity.
 */
void cursor_add_row(cursor_t *c, int offset, lines_t *ls);

/*
 * cursor_set_col_manual - Set the column position of a cursor manually by the user
 *	pressing a keyboard key
 * @col: column to set cursor column to
 *
 * O(1) worst case time complexity.
 */
void cursor_set_col_manual(cursor_t *c, int col);

/*
 * cursor_add_col_manual - Set the column position of a cursor manually by the user
 *	pressing a keyboard key
 * @offset: offset to add to current cursor column
 *
 * O(1) worst case time complexity.
 */
void cursor_add_col_manual(cursor_t *c, int offset);

/*
 * cursor_reset - Reset the cursor to default row and column positions.
 */
void cursor_reset(cursor_t *c);

#endif
