/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "dlist.h"

dlist_t *dlist_init(int capacity)
{
	// Round up to nearest power of 2 that is greater than
	// or equal to the input capacity, so that the list when
	// grown/shrunk can be bit shifted to double and half its
	// size.
	int new_cap = round_up_pow2(capacity);

	if (new_cap < DLIST_MIN_CAP)
		new_cap = DLIST_MIN_CAP;
	return slist_init(new_cap);
}

/**
 * dlist_resize - Resize the underlying capacity of a dynamic list
 *
 * Return 0 on success, -1 on error.
 */
int dlist_resize(dlist_t *d, int new_cap)
{
	void *new_array = realloc(d->array, new_cap*sizeof(void *));

	if (!new_array)
		return -1;

	d->array = new_array;
	d->capacity = new_cap;

	return 0;
}

/**
 * dlist_try_grow - Try to grow the underlying array 
 *
 * Grows the underlying array, doubling its capacity
 * if the array is full. This should be called by all functions
 * that add an element to a list.
 */
void dlist_try_grow(dlist_t *d)
{
	if (d->len == d->capacity) {
		int new_cap = d->capacity<<1;
		dlist_resize(d, new_cap):
	}
}

/**
 * dlist_try_shrink - Try to shrink the underlying array
 *
 * Shrink the underlying array, halving its capacity if the
 * array is half used. Should be called by functions that remove
 * an element from the list.
 */
void dlist_try_shrink(dlist_t *d)
{
	int new_cap = round_up_pow2(d->len);

	if (new_cap >= DLIST_MIN_CAP && new_cap < d->capacity) 
		dlist_resize(d, new_cap);
}

void dlist_append(dlist_t *d, void *elem)
{
	dlist_try_grow(d);
	slist_append(d, elem);
}

void dlist_insert(dlist_t *d, int index, void *elem)
{
	dlist_try_grow(d);
	slist_insert(d, index, elem);
}

void dlist_delete_ind(dlist_t *d, int index)
{
	slist_delete_ind(d, index);
	dlist_try_shrink(d);
}

bool dlist_delete_elem(dlist_t *d, void *elem, bool (*match_func)(void *, void *))
{
	slist_delete_elem(d, elem, match_func);
	dlist_try_shrink(d);
}

