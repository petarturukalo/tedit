/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "slist.h"

slist_t *slist_init(int capacity)
{
	slist_t *s = malloc(sizeof(slist_t));

	if (!s) 
		return NULL;

	s->array = calloc(capacity, sizeof(void *));

	if (!s->array) {
		free(s);
		return NULL;
	}
		
	s->len = 0;
	s->capacity = capacity;

	return s;
}

void slist_for_each(slist_t *s, void (*func)(void *))
{
	for (int i = 0; i < s->len; ++i)
		func(s->array[i]);
}

void slist_free(slist_t *s, void (*free_elem)(void *))
{
	if (free_elem)
		slist_for_each(s, free_elem);
	free(s->array);
	free(s);
}

void slist_append(slist_t *s, void *elem)
{
	s->array[s->len++] = elem;
}

void slist_insert(slist_t *s, int index, void *elem)
{
	for (int i = s->len; i > index; --i)
		s->array[i] = s->array[i-1];
	s->array[index] = elem;
	++s->len;
}

void slist_delete_ind(slist_t *s, int index)
{
	--s->len;

	for (; index < s->len; ++index) 
		s->array[index] = s->array[index+1];
}

int slist_lookup(slist_t *s, void *target, bool (*match_func)(void *, void *))
{
	for (int i = 0; i < s->len; ++i) {
		if (match_func(target, s->array[i]))
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

