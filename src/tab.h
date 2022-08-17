/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef TAB_H
#define TAB_H

#include "chrp.h"
#include "ds/str.h"

// Default length of tabstop (max number of characters that a
// tab character takes up).
#define TABSZ 8

/*
 * Identifier for the start of a tab and continuing characters.
 * When a tab is inserted it is replaced with pseudo space characters,
 * first a TAB_START and the rest TAB_CONT, so that two consecutive
 * tabs can be distinguished as separate and not one tab. 
 * This also makes displaying of tab characters easier when shifting the view 
 * of a file horizontally (see view.h), and only requires that there is one
 * cursor tracking the file buffer (what is seen on screen is how it appears in the
 * underlying buffer).
 */
#define TAB_START -2
#define TAB_CONT  -1

/*
 * dist_to_next_tabstop - Get the distance in spaces to fill string length to the
 *	next tabstop
 * @i: index start in imaginary string to get distance to next tab stop from
 * @tabsz: max number of spaces that a tab occupies
 */
int dist_to_next_tabstop(int i, int tabsz);

/*
 * tablen - Get the current length of a tab filled with spaces in a string which
 *	uses space filled tabs
 * @i: index of start of tab character in string
 *
 * O(tabsz) worst case time complexity.
 */
int tablen(str_t *s, int i);

/*
 * str_insert_tab_spaces - Insert a tab as spaces at an index
 * @s: string to insert tab into
 * @i: index in str to insert tab at
 *
 * Return the number of spaces inserted.
 * O(n*tabsz) worst case time complexity where n is the length of the string.
 */
int str_insert_tab_spaces(str_t *s, int i, int tabsz);

/*
 * str_delete_tab_spaces - Delete a tab as spaces at an index
 * @s: string to delete tab from
 * @i: index in str to delete tab at (expected to be a TAB_START character)
 */
void str_delete_tab_spaces(str_t *s, int i);

/*
 * str_contract_tab_spaces - Contract all space-filled tabs back into tabs
 */
void str_contract_tab_spaces(str_t *s, int tabsz);

/*
 * str_expand_tab_spaces - Expand all tabs into spaces
 *
 * O(n*tabsz) worst case time complexity where n is the length of the string.
 */
void str_expand_tab_spaces(str_t *s, int tabsz);

/*
 * str_align_next_tab - Align the first next seen tab to its tab bar in case
 *	it is misaligned
 * @start_ind: index to start looking for a tab at 
 * @tabsz: max number of spaces that a tab occupies
 *
 * O(n*tabsz) worst case time complexity where n is the length of the string.
 * Tab bar description: consider a tab size of 4, so there can be only 1 tab
 * every 4 characters - each span of 4 characters is a tab bar
 */
void str_align_next_tab(str_t *s, int start_ind, int tabsz);

#endif
