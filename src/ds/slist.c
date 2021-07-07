/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "slist.h"

slist_t *slist_init(int capacity)
{
	slist_t *s = malloc(sizeof(slist_t));

	if (!s) {
		fprintf(stderr, "can't alloc slist\n");
		return NULL;
	}

	s->sl_array = calloc(capacity, sizeof(void *));

	if (!s->sl_array) {
		fprintf(stderr, "can't alloc slist array\n");
		free(s);
		return NULL;
	}
		
	s->sl_len = 0;
	s->sl_capacity = capacity;

	return s;
}

void slist_for_each(slist_t *s, void (*func)(void *))
{
	for (int i = 0; i < s->sl_len; ++i)
		func(s->sl_array[i]);
}

void slist_free(slist_t *s, void (*free_elem)(void *))
{
	if (free_elem)
		slist_for_each(s, free_elem);
	free(s->sl_array);
	free(s);
}

void slist_append(slist_t *s, void *elem)
{
	s->sl_array[s->sl_len++] = elem;
}

void slist_insert(slist_t *s, int index, void *elem)
{
	for (int i = s->sl_len; i > index; --i)
		s->sl_array[i] = s->sl_array[i-1];
	s->sl_array[index] = elem;
	++s->sl_len;
}

void slist_delete_ind(slist_t *s, int index)
{
	--s->sl_len;

	for (; index < s->sl_len; ++index) 
		s->sl_array[index] = s->sl_array[index+1];
}

int slist_lookup(slist_t *s, void *target, bool (*match_func)(void *, void *))
{
	for (int i = 0; i < s->sl_len; ++i) {
		if (match_func(target, s->sl_array[i]))
			return i;
	}
	return -1;
}

bool slist_delete_elem(slist_t *s, void *elem, bool (*match_func)(void *, void *))
{
	int i = slist_lookup(s, elem, match_func);

	if (i != -1) {
		slist_delete_ind(s, i);
		return true;
	}
	return false;
}

