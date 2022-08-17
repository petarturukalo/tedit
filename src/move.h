/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Cursor movement functions. Map these to keboard presses such
 * as the arrow keys and special keys to move around a file.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef MOVE_H
#define MOVE_H

#include "cursor.h"
#include "chrp.h"
#include "view.h"

/*
 * mv_left - Move the cursor left by one
 * @l: line cursor is on
 *
 * O(1) worst case time complexity.
 */
void mv_left(cursor_t *c, line_t *l);

/*
 * mv_down - Move the cursor down by one
 * @ls: list of lines
 *
 * O(tabsz) worst case time complexity.
 */
void mv_down(cursor_t *c, lines_t *ls);

/*
 * mv_up - Move the cursor up by one
 * @ls: list of lines
 *
 * O(tabsz) worst case time complexity
 */
void mv_up(cursor_t *c, lines_t *ls);

/*
 * mv_right - Move the cursor right by one
 * @l: line cursor is on
 *
 * O(1) worst case time complexity.
 */
void mv_right(cursor_t *c, line_t *l);

/*
 * mv_start - Move the cursor to the start of the current line
 *
 * O(1) worst case time complexity.
 */
void mv_start(cursor_t *c);

/*
 * mv_end - Move the cursor to the end of the current line
 * @l: line cursor is on
 *
 * O(1) worst case time complexity.
 */
void mv_end(cursor_t *c, line_t *l);

/*
 * mv_pgup - Move the cursor up by a view page
 * @ls: lines cursor is over
 *
 * O(tabsz) worst case time complexity. Sets view pgmv flag for the 
 * view to be synchronised with the cursor later on.
 */
void mv_view_pgup(cursor_t *c, lines_t *ls, view_t *v);

/*
 * mv_pgdn - Move the cursor down by a view page
 * @ls: lines cursor is over
 *
 * O(tabsz) worst case time complexity. Sets view pgmv flag for the 
 * view to be synchronised with the cursor later on.
 */
void mv_view_pgdn(cursor_t *c, lines_t *ls, view_t *v);

#endif
