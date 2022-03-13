/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "tab.h"

int dist_to_next_tabstop(int i, int tabsz)
{
	return tabsz-i%tabsz;
}

int tablen(str_t *s, int i)
{
	int spaces = 0;

	if (s->array[i++] == TAB_START)
		++spaces;
	for (; i < s->len && s->array[i] == TAB_CONT; ++i)
		++spaces;
	return spaces;
}

int str_insert_tab_spaces(str_t *s, int i, int tabsz)
{
	int ret, spaces = dist_to_next_tabstop(i, tabsz);

	ret = spaces;
	str_insert(s, TAB_START, i++);

	while (--spaces)
		str_insert(s, TAB_CONT, i++);
	return ret;
}

void str_delete_tab_spaces(str_t *s, int i)
{
	if (s->array[i] == TAB_START) {
		dlist_delete_ind(s, i, NULL);
		
		while (i < s->len && s->array[i] == TAB_CONT) 
			dlist_delete_ind(s, i, NULL);
	}
}

void str_expand_tab_spaces(str_t *s, int tabsz)
{
	int nspaces, i = 0;
	
	while (i < s->len) {
		if (s->array[i] == '\t') {
			nspaces = dist_to_next_tabstop(i, tabsz);

			dlist_delete_ind(s, i, NULL);  // Remove tab.
			str_insert_tab_spaces(s, i, tabsz);  // Replace with pseudo spaces.

			i += nspaces;
		} else
			++i;
	}
}

/*
 * strcontractlen - Get the length of a tab space expanded string if it were to be contracted
 */
int strcontractlen(str_t *s)
{
	char *t;
	int n, count;

	t = s->array;
	n = s->len;
	count = 0;

	for (int i = 0; i < n; ++i) {
		if (t[i] != TAB_CONT)
			++count;
	}
	return count;
}

void str_contract_tab_spaces(str_t *s, int tabsz)
{
	// Read, write and length.
	int r, w, n;
	char *t = s->array;

	r = w = 0;
	n = s->len;

	for (; r < n; ++r) {
		if (t[r] != TAB_CONT) {
			if (t[r] == TAB_START) 
				t[w] = '\t';
			else
				t[w] = t[r];
			++w;
		}
	}
	s->len = w;
	dlist_try_shrink(s);
}

/*
 * str_align_tab - Align a tab to its tab bar if it's misaligned
 * @i: index of start of tab character in line (since a tab can span
 *	multiple characters)
 *
 * O(n*tabsz) worst case time complexity where n is the length of the string.
 */
void str_align_tab(str_t *s, int i, int tabsz)
{
	// The amount of spaces the tab should take up.
	int cur_spaces, target_spaces = dist_to_next_tabstop(i, tabsz);
	
	cur_spaces = tablen(s, i);

	// Grow tab.
	while (cur_spaces < target_spaces) {
		str_insert(s, TAB_CONT, ++i);
		++cur_spaces;
	}  // Shrink tab.
	while (cur_spaces > target_spaces) {
		dlist_delete_ind(s, i+1, NULL);
		--cur_spaces;
	}
}

void str_align_next_tab(str_t *s, int start_ind, int tabsz)
{
	int tab_ind = chrp_find(s->array, TAB_START, start_ind, s->len-1);

	if (tab_ind != -1)
		str_align_tab(s, tab_ind, tabsz);
}


