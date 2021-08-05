/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "line.h"

line_t *line_alloc(int capacity)
{
	return str_alloc(capacity);
}

void line_append(line_t *l, char c)
{
	str_append(l, c);
}

line_t *line_init(char *str, int len, int tabsz)
{
	str_t *s = str_ninit(str, len);

	if (!s)
		return NULL;

	str_expand_tab_spaces(s, tabsz);

	return s;
}

int line_len(line_t *l)
{
	if (l->len > 0 && l->buf[l->len-1] == '\n')
		return l->len-1;  // Don't include newline.
	return l->len;
}

int line_len_nl(line_t *l)
{
	return l->len;
}

void line_free_sl(void *line)
{
	str_free((str_t *)line);
}

line_t *line_split(line_t *l, int col, int tabsz)
{
	str_t *s = str_split(l, col);
	str_append(l, '\n');
	return s;
}

