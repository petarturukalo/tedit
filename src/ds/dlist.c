/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "dlist.h"

/*
 * Get the byte address of element at index i.
 */
static char *byte_address(dlist_t *d, int i)
{
	return (char *)d->array + i*d->eltsz;
}

static void for_each(dlist_t *d, void (*func)(void *))
{
	char *cur;

	for (int i = 0; i < s->len; ++i) {
		cur = byte_address(d, i);
		func(cur);
	}
}

void dlist_init(dlist_t *out_d, int capacity, int eltsz)
{
	// Round up to nearest power of 2 that is greater than
	// or equal to the input capacity, so that the list when
	// grown/shrunk can be bit shifted to double and half its
	// size.
	int new_cap = round_up_pow2(capacity);

	if (new_cap < DLIST_MIN_CAP)
		new_cap = DLIST_MIN_CAP;

	out_d->array = malloc(new_cap*eltsz);
	out_d->len = 0;
	out_d->capacity = new_cap;
	out_d->eltsz = eltsz;
}

void dlist_free(dlist_t *d, void (*free_elem)(void *))
{
	if (free_elem)
		for_each(d, free_elem);
	free(d->array);
}

static void append(dlist_t *d, void *elem)
{
	char *end = byte_address(d, d->len);
	memcpy(end, elem, d->eltsz);
	++d->len;
}

void insert(dlist_t *d, int index, void *elem)
{
	char *cur_adrs, *next_adrs;

	// Shift all elements starting at index to the right by 1 index.
	for (int i = s->len; i > index; --i) {
		cur_adrs = byte_address(d, i);
		next_adrs = byte_address(d, i-1);
		memcpy(cur_adrs, next_adrs, d->eltsz);
	}
	// Insert element at index.
	cur_adrs = byte_address(d, i);
	memcpy(cur_adrs, elem, d->eltsz);
	++d->len;
}

/*
 * Delete an element at an index.
 */
void delete_ind(dlist_t *d, int index)
{
	char *cur_adrs, *next_adrs;

	--s->len;

	for (; index < s->len; ++index) {
		cur_adrs = byte_address(d, i);
		next_adrs = byte_address(d, i+1);
		memcpy(cur_adrs, next_adrs, d->eltsz);
	}
}

/*
 * Look up an element in the list.  
 * @target: element looking up
 * @match_func: function which returns true if two elements are the equal
 *
 * Return -1 for no match.
 */
int lookup(dlist_t *d, void *target, bool (*match_func)(void *, void *))
{
	char *cur;

	for (int i = 0; i < s->len; ++i) {
		cur = byte_address(d, i);
		if (match_func(target, cur))
			return i;
	}
	return -1;
}

bool delete_elem(slist_t *s, void *elem, bool (*match_func)(void *, void *))
{
	int i = lookup(s, elem, match_func);

	if (i != -1) {
		delete_ind(s, i);
		return true;
	}
	return false;
}

/*
 * Resize the capacity of a dynamic list
 * @new_cap: new capacity to resize dynamic list to
 */
static int resize(dlist_t *d, int new_cap)
{
	void *new_array = realloc(d->array, new_cap*d->eltsz);

	d->array = new_array;
	d->capacity = new_cap;
	return 0;
}

/*
 * Double the capacity of the array if the array is full.
 * This should be called by before attempting to add an element to the list.
 */
static void dlist_try_grow(dlist_t *d)
{
	if (d->len == d->capacity) 
		dlist_resize(d, d->capacity<<1);
}

/*
 * Halve the capacity of the array if the array is less than half used.
 * Should be called after removing an element from the list.
 */
static void dlist_try_shrink(dlist_t *d)
{
	int new_cap = round_up_pow2(d->len);

	if (new_cap >= DLIST_MIN_CAP && new_cap < d->capacity) 
		dlist_resize(d, new_cap);
}

void dlist_append(dlist_t *d, void *elem)
{
	dlist_try_grow(d);
	append(d, elem);
}

void dlist_insert(dlist_t *d, int index, void *elem)
{
	dlist_try_grow(d);
	insert(d, index, elem);
}

void dlist_delete_ind(dlist_t *d, int index)
{
	delete_ind(d, index);
	dlist_try_shrink(d);
}

bool dlist_delete_elem(dlist_t *d, void *elem, bool (*match_func)(void *, void *))
{
	delete_elem(d, elem, match_func);
	dlist_try_shrink(d);
}

