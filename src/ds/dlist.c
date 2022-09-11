/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include <strings.h>
#include "dlist.h"
// TODO some copy functions in here can be improved with a single call to memcpy

/*
 * Maximum size of an element in bytes. If an element is greater in size than this
 * then some functions here may not work as expected.
 */
#define MAX_ELTSZ 1024
/* Memory area shared by functions so that each doesn't have to allocate its own. */
static char elembuf[MAX_ELTSZ];

/*
 * Get the byte address of element at index i.
 */
static char *byte_address(dlist_t *d, int i)
{
	return d->array + i*d->eltsz;
}

void dlist_for_each(dlist_t *d, dlist_elem_fn fn)
{
	char *cur;

	for (int i = 0; i < d->len; ++i) {
		cur = byte_address(d, i);
		fn(cur);
	}
}

void dlist_for_each_data(dlist_t *d, dlist_elem_data_fn fn, void *data)
{
	char *cur;

	for (int i = 0; i < d->len; ++i) {
		cur = byte_address(d, i);
		fn(cur, data);
	}
}

void dlist_init(dlist_t *out_d, int capacity, size_t eltsz)
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

/*
 * Concatenate an array of elements to the end of a list.
 */
static void cat_array(dlist_t *d, void *elts, int nelts)
{
	if (elts) {
		for (int i = 0; i < nelts; ++i) {
			dlist_append(d, elts);
			elts += d->eltsz;
		}
	}
}

void dlist_init_array(dlist_t *d, int capacity, size_t eltsz, void *elts, int nelts)
{
	dlist_init(d, capacity, eltsz);
	cat_array(d, elts, nelts);
}

static void dlist_free_elements(dlist_t *d, dlist_elem_fn free_elem)
{
	if (free_elem)
		dlist_for_each(d, free_elem);
}

void dlist_free(dlist_t *d, dlist_elem_fn free_elem)
{
	if (d->array) {
		dlist_free_elements(d, free_elem);
		free(d->array);
		d->array = NULL;
	}
}

void dlist_get(dlist_t *d, int i, void *out_elem)
{
	char *adrs = byte_address(d, i);
	memcpy(out_elem, adrs, d->eltsz);
}

void *dlist_get_address(dlist_t *d, int i)
{
	return byte_address(d, i);
}

void dlist_set(dlist_t *d, int i, void *elem)
{
	char *adrs = byte_address(d, i);
	memcpy(adrs, elem, d->eltsz);
}

static void append(dlist_t *d, void *elem)
{
	if (elem) {
		char *end = byte_address(d, d->len);
		memcpy(end, elem, d->eltsz);
	}
	++d->len;
}

void insert(dlist_t *d, int index, void *elem)
{
	int i;
	char *cur_adrs, *next_adrs;

	// Shift all elements starting at index to the right by 1 index.
	// Can't do all in one call to memcpy unless it's guaranteed to copy
	// in reverse. The same goes for the copying in delete_ind, except it
	// needs to copy going forward.
	for (i = d->len; i > index; --i) {
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
static void delete_ind(dlist_t *d, int index, dlist_elem_fn free_elem)
{
	char *cur_adrs, *next_adrs;

	if (free_elem)
		free_elem(byte_address(d, index));
	--d->len;

	for (; index < d->len; ++index) {
		cur_adrs = byte_address(d, index);
		next_adrs = byte_address(d, index+1);
		memcpy(cur_adrs, next_adrs, d->eltsz);
	}
}

/*
 * Get whether two elements are equal.
 */
static bool eq(dlist_t *d, void *a, void *b)
{
	return memcmp(a, b, d->eltsz) == 0;
}

/*
 * Get the index of an element in the list. Return -1 for no match.
 *
 * @data: if fn is NULL then this is the element being looked up and
 *	elements are compared by byte values. Else this is the first parameter
 *		passed to fn.
 */
static int dlist_lookup(dlist_t *d, void *data, dlist_match_fn fn)
{
	char *cur;

	for (int i = 0; i < d->len; ++i) {
		cur = byte_address(d, i);
		if (fn ? fn(cur, data) : eq(d, cur, data))
			return i;
	}
	return -1;
}

/*
 * Same as dlist_lookup() but return the address of the element instead of its index.
 * Return NULL for no match.
 */
void *dlist_lookup_address(dlist_t *d, void *data, dlist_match_fn fn)
{
	int i = dlist_lookup(d, data, fn);
	return i == -1 ? NULL : dlist_get_address(d, i);
}

static bool delete_elem(dlist_t *d, void *elem, dlist_match_fn mfn, 
			dlist_elem_fn free_elem)
{
	int i = dlist_lookup(d, elem, mfn);

	if (i != -1) {
		delete_ind(d, i, free_elem);
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
 * Raise the capacity of the array if the list is at or over capacity.
 */
void dlist_try_grow(dlist_t *d)
{
	if (d->len >= d->capacity) {
		// Add 1 to double capacity when length == capacity, since only gets
		// rounded up when the number isn't a power of 2, and capacity is 
		// always a power of 2.
		int new_cap = round_up_pow2(d->len+1);
		resize(d, new_cap);
	}
}

/*
 * Reduce the capacity of the array to just above what's needed to store
 * its current elements.
 */
void dlist_try_shrink(dlist_t *d)
{
	int new_cap = round_up_pow2(d->len);

	if (new_cap >= DLIST_MIN_CAP && new_cap <= d->capacity) 
		resize(d, new_cap);
}

void dlist_clear(dlist_t *d, dlist_elem_fn free_elem)
{
	dlist_free_elements(d, free_elem);
	d->len = 0;
	dlist_try_shrink(d);
}

void dlist_append(dlist_t *d, void *elem)
{
	dlist_try_grow(d);
	append(d, elem);
}

/*
 * Safely zero the first eltsz bytes of the shared elembuf without overruning it. 
 */
static void bzero_elembuf(size_t eltsz)
{
	if (eltsz > sizeof(elembuf))
		eltsz = sizeof(elembuf);
	bzero(elembuf, eltsz);
}

void dlist_append_init(dlist_t *d, dlist_elem_fn init_elem)
{
	bzero_elembuf(d->eltsz);
	init_elem(elembuf);
	dlist_append(d, elembuf);
}

/*
 * Same as dlist_append_init() but the function also takes data as second param.
 */
static void dlist_append_init_data(dlist_t *d, dlist_elem_data_fn init_elem, void *data)
{
	bzero_elembuf(d->eltsz);
	init_elem(elembuf, data);
	dlist_append(d, elembuf);
}

void dlist_insert(dlist_t *d, int index, void *elem)
{
	dlist_try_grow(d);
	insert(d, index, elem);
}

void dlist_delete_ind(dlist_t *d, int index, dlist_elem_fn free_elem)
{
	delete_ind(d, index, free_elem);
	dlist_try_shrink(d);
}

bool dlist_delete_elem(dlist_t *d, void *elem, dlist_match_fn mfn, 
		       dlist_elem_fn free_elem)
{
	bool deleted = delete_elem(d, elem, mfn, free_elem);
	dlist_try_shrink(d);
	return deleted;
}

bool dlist_pop(dlist_t *d, void *out_elem)
{
	if (d->len) {
		if (out_elem)
			dlist_get(d, d->len-1, out_elem);
		--d->len;
		dlist_try_shrink(d);
		return true;
	}
	return false;
}

/*
 * Copy the element at index i in the source list to the element at index j
 * in the destination list.
 */
static void dlist_copy_elt(dlist_t *src, int i, dlist_t *dest, int j)
{
	char *dest_adrs = byte_address(dest, j);
	dlist_get(src, i, dest_adrs);
}

/*
 * dlist_copy_bounds - Copy a sublist in one list to a sublist in another list
 * @src: source list to copy from
 * @sstart: index start (inclusive) of sublist in source list
 * @send: index end (inclusive) of sublist in source list
 * @dest: destination list to copy to
 * @dstart: index start (inclusive) of sublist in destination list
 * @dend: index end (inclusive) of sublist in destination list
 */
static void dlist_copy_bounds(dlist_t *src, int sstart, int send, dlist_t *dest, int dstart, int dend)
{
	int i, j;

	i = sstart;
	j = dstart;

	for (; i <= send && j <= dend; ++i, ++j) 
		dlist_copy_elt(src, i, dest, j);
}

void dlist_split(dlist_t *d, int index, dlist_t *out_d)
{
	int newlen = d->len-index;

	dlist_init(out_d, newlen, d->eltsz);

	// Copy sublist spanning from index to end of list to
	// the new list. Know there's enough space so don't need to 
	// call dlist_try_grow.
	dlist_copy_bounds(d, index, d->len-1, out_d, 0, newlen-1);
	out_d->len = newlen;

	// Truncate source list.
	d->len = index;
	dlist_try_shrink(d);
}

void dlist_cat(dlist_t *dest, dlist_t *src)
{
	int i, j;

	i = 0;
	j = dest->len;
	dest->len += src->len;
	dlist_try_grow(dest);

	for (; i < src->len; ++i, ++j)
		dlist_copy_elt(src, i, dest, j);
}

void dlist_copy(dlist_t *dest, dlist_t *src, dlist_elem_fn free_elem)
{
	dlist_clear(dest, free_elem);
	dlist_cat(dest, src);
}

void dlist_copy_new(dlist_t *d, dlist_t *out_d)
{
	dlist_init(out_d, d->capacity, d->eltsz);
	dlist_cat(out_d, d);
}

void dlist_copy_array(dlist_t *d, void *elts, int nelts, dlist_elem_fn free_elem)
{
	dlist_clear(d, free_elem);
	cat_array(d, elts, nelts);
}

/*
 * Shrink the length of a list to meet a new length.
 */
static void dlist_shrink_len(dlist_t *l, int new_len)
{
	while (l->len > new_len)
		dlist_pop(l, NULL);
}

void dlist_resize_len(dlist_t *l, int new_len)
{
	if (new_len >= 0) {
		while (l->len < new_len)
			dlist_append(l, NULL);
		dlist_shrink_len(l, new_len);
	}
}

void dlist_resize_len_init_data(dlist_t *l, int new_len, dlist_elem_data_fn init_elem, void *data)
{
	if (new_len >= 0) {
		while (l->len < new_len)
			dlist_append_init_data(l, init_elem, data);
		dlist_shrink_len(l, new_len);
	}
}

void dlist_memset(dlist_t *l, unsigned char c)
{
	 memset(l->array, c, l->len);
}
