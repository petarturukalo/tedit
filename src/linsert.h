/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Line insert operations on a file that also affect the cursor.
 * This makes line insert operations and the cursor tightly coupled.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef LINSERT_H
#define LINSERT_H

#include <stdbool.h>
#include "chrp.h"
#include "ds/str.h"
#include "line.h"
#include "tab.h"
#include "cursor.h"

/**
 * lins_insert_char - Insert a character into a line, moving the cursor
 *	with it
 * @l: line cursor is on
 * @crs: cursor
 * @c: character to insert
 * @tabsz: see tab.h
 */
void lins_insert_char(line_t *l, cursor_t *crs, char c, int tabsz);

/**
 * lins_delete - Delete a character under the cursor on a line
 * @cur: current line (line to delete character from)
 * @next: line below current line in a list of lines
 * @c: cursor on current line
 *
 * Return true if a newline was deleted and so next line was concatenated onto end of
 * current line. Expects the calling function removes the "next" line from the list 
 * of lines if a newline is deleted from the current line. 
 * 
 * O(n*tabsz + m) worst case time complexity where n is the length of the current line
 * and m is the length of the next line.
 */
bool lins_delete(line_t *cur, line_t *next, cursor_t *c, int tabsz);

/**
 * lins_backspace - Backspace a character before the cursor on a line
 * @cur: current line to backspace from
 * @prev: line above current line in list of lines
 * @c: cursor on current line
 *
 * Return true if the start of a line was backspaced and so was concatenated onto 
 * the end of the previous line, which would then expect the calling function to 
 * delete the current line. 
 *
 * O((n+m)*tabsz) worst case time complexity where n is the length of the current line
 * and m is the length of the previous line.
 */
bool lins_backspace(line_t *cur, line_t *prev, cursor_t *c, int tabsz);

/**
 * lins_split - Split a line at the current cursor position (the new right line includes 
 *	the current character under the cursor)
 *
 * Return the new right line made by the split. Expects the calling function inserts the new line 
 * into the list of lines. 
 *
 * O(n) worst case time complexity where n is the length of the line.
 */
line_t *lins_split(line_t *l, cursor_t *c, int tabsz);

#endif
