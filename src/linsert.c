/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "linsert.h"

/*
 * lins_insert_reg - Insert a regular character into a line
 *
 * O(n*tabsz) worst case time complexity where n is the length of the line.
 */
void lins_insert_reg(line_t *l, cursor_t *crs, char c, int tabsz)
{
	int i = crs->col;
	
	// Can't equal TAB_CONT since cursor can't be over it, so don't worry
	// about handling it.
	if (i < line_len(l) && l->array[i] == TAB_START) {
		// Replace tab character with insert character.
		l->array[i] = c;

		// Shift start of tab to next index for O(1) insert instead of O(n*tabsz).
		if (i+1 < line_len(l) && l->array[i+1] == TAB_CONT)
			l->array[i+1] = TAB_START;
		else  // Got rid of a whole tab bar, so fill up the whole of the next tab bar.
			str_insert_tab_spaces(l, i+1, tabsz);  // O(n*tabsz).
	} else {
		str_insert(l, c, i);
		str_align_next_tab(l, i+1, tabsz);
	}
	cursor_add_col_manual(crs, 1);
}


/*
 * lins_insert_tab - Insert a tab character into a line
 *
 * O(n*tabsz) worst case time complexity where n is the length of the line.
 */
void lins_insert_tab(line_t *l, cursor_t *c, int tabsz)
{
	int col_add, offset, i = c->col;

	if (l->array[i] == TAB_START)   {
		// If inserted a tab before a tab then tab pushing in front of could get misaligned.
		// Avoid realigning by inserting the tab after the current tab, in the next tab bar, 
		// taking up its own tab bar.
		offset = tablen(l, i);
		str_insert_tab_spaces(l, i+offset, tabsz);
		cursor_add_col_manual(c, offset);  // Jump to tab that got inserted.
	} else  {
		col_add = str_insert_tab_spaces(l, i, tabsz);
		cursor_add_col_manual(c, col_add);
		// Inserted a new tab not immediately in front of another, so try to realign a next tab
		// in case it got misaligned.
		str_align_next_tab(l, c->col, tabsz);
	}
}

void lins_insert_char(line_t *l, cursor_t *crs, char c, int tabsz)
{
	if (c == '\t')
		lins_insert_tab(l, crs, tabsz);
	else
		lins_insert_reg(l, crs, c, tabsz);
}

/*
 * lins_delete_reg - Delete a regular character where the cursor is (a
 *	regular character being a non-tab character)
 *
 * O(n*tabsz) worst case time complexity where n is the length of the line.
 */
void lins_delete_reg(line_t *l, cursor_t *c, int tabsz)
{
	dlist_delete_ind(l, c->col, NULL);
	str_align_next_tab(l, c->col, tabsz);
}

/*
 * lins_delete_tab - Delete a tab character where the cursor is 
 *
 * O(n*tabsz) worst case time complexity where n is the length of the line.
 */
void lins_delete_tab(line_t *l, cursor_t *c, int tabsz)
{
	str_delete_tab_spaces(l, c->col);
	str_align_next_tab(l, c->col, tabsz);
}

/*
 * lins_linecat - Concatenate two lines together
 * @dest: line to concat src line to end of; assumes this has a newline (and so isn't
 *	the last line)
 * @src: line to concat onto end of dest line
 *
 * O(n+m*tabsz) worst case time complexity where n is the length of the source line
 * and m is the length of the destination line.
 */
void lins_linecat(line_t *dest, line_t *src, int tabsz)
{
	int n = line_len(dest);

	// Remove newline before concatenating.
	dlist_delete_ind(dest, n, NULL);
	dlist_cat(dest, src);
	str_align_next_tab(dest, n, tabsz);
}

/*
 * lins_delete_newline - Delete a newline character off the end of the current line.
 */
void lins_delete_newline(line_t *cur, line_t *next, int tabsz)
{
	lins_linecat(cur, next, tabsz);
}

bool lins_delete(line_t *cur, line_t *next, cursor_t *crs, int tabsz)
{
	char c;
	int i = crs->col;

	if (i < line_len(cur)) {
		c = cur->array[i];

		if (c == TAB_START)
			lins_delete_tab(cur, crs, tabsz);
		else
			lins_delete_reg(cur, crs, tabsz);
		return false;
	} else if (next) {
		// Ind is at end of line (cursor is over newline) and there is a next line,
		// meaning that current line is not the last line and guaranteed to have a 
		// newline, so remove it and concat it with the next line.
		lins_delete_newline(cur, next, tabsz);
		return true;
	}
	return false;
}

/*
 * lins_backspace_tab - Backspace a tab character in a line, knowing that the previous
 *	character is part of a tab
 *
 * O(n*tabsz) worst case time complexity where n is the length of the string.
 */
void lins_backspace_tab(line_t *l, cursor_t *crs, int tabsz)
{
	char c;
	int i = crs->col;

	c = l->array[i-1];

	// Tab doesn't have any continuation characters, so deleting its 
	// start deletes the whole thing.
	if (c == TAB_START) {
		dlist_delete_ind(l, i-1, NULL);
		cursor_add_col_manual(crs, -1);
	} else {
		// Find start of tab and then delete it.
		int tab_ind = chrp_find_reverse(l->array, TAB_START, 0, i-2);

		str_delete_tab_spaces(l, tab_ind);
		cursor_set_col_manual(crs, tab_ind);
	}
	str_align_next_tab(l, crs->col, tabsz);
}

/*
 * lins_backspace_reg - Backspace a regular character on the current line before the current
 *	cursor position.
 *
 * O(n*tabsz) worst case time complexity where n is the length of the line.
 */
void lins_backspace_reg(line_t *l, cursor_t *c, int tabsz)
{
	cursor_add_col_manual(c, -1);
	lins_delete_reg(l, c, tabsz);
}

/*
 * lins_backspace_start - Backspace the start of the current line, concatenating it to the end
 *	of the previous line
 *
 * O(n+m*tabsz) worst case time complexity where n is the length of the previous line and m
 * is the length of the current line.
 */
void lins_backspace_start(line_t *cur, line_t *prev, cursor_t *c, int tabsz)
{
	int n = line_len(prev);

	lins_linecat(prev, cur, tabsz);
	// Avoid entry function to set since know will be on a valid character because 
	// currently over that character when backspacing.
	c->row -= 1;  
	cursor_set_col_manual(c, n);
}

bool lins_backspace(line_t *cur, line_t *prev, cursor_t *crs, int tabsz)
{
	char c;
	int i = crs->col;

	if (i > 0) {
		c = cur->array[i-1];

		if (c == TAB_START || c == TAB_CONT)
			lins_backspace_tab(cur, crs, tabsz);
		else
			lins_backspace_reg(cur, crs, tabsz);
		return false;
	} else if (prev) {
		// At start of line and not on the first line, so merge
		// the current line with the previous line.
		lins_backspace_start(cur, prev, crs, tabsz);
		return true;
	}
	return false;
}

void lins_split(line_t *l, cursor_t *c, int tabsz, line_t *newline)
{
	line_split(l, c->col, tabsz, newline);

	// Put cursor at start of next line, avoiding row entry function since first column
	// is always valid, even if it's a tab as of it being the start of a tab and not a continuation
	// of it.
	c->row += 1;
	cursor_set_col_manual(c, 0);
}
