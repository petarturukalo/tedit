/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "line.h"

void line_alloc(line_t *l)
{
	str_alloc(l, DLIST_MIN_CAP);
}

void line_init(line_t *l, char *s, int n, int tabsz)
{
	str_ninit(l, s, n);
	str_expand_tab_spaces(l, tabsz);
}

int line_len(line_t *l)
{
	if (l->len > 0 && l->array[l->len-1] == '\n')
		return l->len-1;  // Don't include newline.
	return l->len;
}

int line_len_nl(line_t *l)
{
	return l->len;
}

void line_split(line_t *l, int col, int tabsz, line_t *newline)
{
	dlist_split(l, col, newline);
	str_append(l, '\n');
}

void line_free(line_t *l)
{
	dlist_free(l, NULL);
}
