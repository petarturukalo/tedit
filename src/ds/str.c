/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "str.h"

void str_alloc(str_t *out_s, int capacity)
{
	dlist_init(out_s, capacity, sizeof(char));
}

void str_ninit(str_t *out_s, char *str, int n)
{
	str_alloc(out_s, n);
	strncpy(out_s->array, str, n);
	out_s->len = n;
}

void str_init(str_t *out_s, char *str)
{
	str_ninit(out_s, str, strlen(str));
}

void str_append(str_t *s, char c)
{
	dlist_append(s, &c);
}

void str_insert(str_t *s, char c, int i)
{
	dlist_insert(s, i, &c);
}

void str_copy(str_t *s, char *t)
{
	dlist_copy_array(s, t, strlen(t), NULL);
}
