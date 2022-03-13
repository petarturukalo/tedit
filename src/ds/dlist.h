/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Dynamic list, similar to a C++ vector.
 * All elements are of the same size and are expected to be of
 * the same type (although they don't need to be the same type).
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef DLIST_H
#define DLIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../math.h"

#define DLIST_MIN_CAP 16

// TODO could move element free function from dlist_free to struct member
// so doesn't have to be param and can also be used in delete functions
// without a param
struct dynamic_list {
	char *array;  // Bytes where elements are stored.
	int len;  // Current number of elements in the array.
	int capacity;  // Current number of elements that can fit in the array.
	size_t eltsz;  // Size of an element in the array.
};

typedef struct dynamic_list dlist_t;

/*
 * Initialise the members of a dynamic list.
 * @eltsz: size of an element in the list. 
 *
 * See dlist_t for parameters. Free with dlist_free.
 */
void dlist_init(dlist_t *out_d, int capacity, size_t eltsz);
/*
 * Initialise a dynamic list from an array of elements
 * @eltsz: size of an element in the elts array
 * @elts: array of elements to insert into the list
 */
void dlist_init_array(dlist_t *d, int capacity, size_t eltsz, void *elts, int nelts);
/*
 * Free the members of a dynamic list initialised with dlist_init.
 * free_elem: function to free an element in the list. If NULL it's
 *	assumed elements don't need freeing.
 */
void dlist_free(dlist_t *s, void (*free_elem)(void *));

void dlist_try_grow(dlist_t *d);
void dlist_try_shrink(dlist_t *d);

/*
 * Copy the element at index i into out_elem.
 */
void dlist_get(dlist_t *d, int i, void *out_elem);
/*
 * Get the address of the element at index i.
 */
void *dlist_get_address(dlist_t *d, int i);
/*
 * Copy elem to the element at index i.
 */
void dlist_set(dlist_t *d, int i, void *elem);
void dlist_append(dlist_t *d, void *elem);
/*
 * Append a new element only providing a function to initialise	it.
 */
void dlist_append_init(dlist_t *d, void (*init_elem_func)(void *));
void dlist_insert(dlist_t *d, int index, void *elem);

/*
 * Remove the last element in the list and optionally store it in out_elem
 * if out_elem isn't NULL.
 */
void dlist_pop(dlist_t *d, void *out_elem);
void dlist_delete_ind(dlist_t *d, int index, void (*free_elem)(void *));
/*
 * @elem: element to delete
 * @match_func: function which returns true if two elements are equal.
 *	If this is NULL then comparison is done on byte values.
 *
 * Return whether the element was found and deleted.
 */
bool dlist_delete_elem(dlist_t *d, void *elem, bool (*match_func)(void *, void *),
		       void (*free_elem)(void *));

/*
 * @func: function to run on each element in the list
 */
void dlist_for_each(dlist_t *d, void (*func)(void *));
/*
 * @func: function to run on each element in list. First param is element and second
 *	is void data passed into this.
 */
void dlist_for_each_data(dlist_t *d, void (*func)(void *, void *), void *data);
			 

/*
 * dlist_split - Split a list at an index
 *
 * The input list is truncated up to but excluding the index, with all elements
 * at and after the index copied to the newly allocated out_d list.
 */
void dlist_split(dlist_t *d, int index, dlist_t *out_d);
/*
 * dlist_cat - Concatenate a list onto the end of another
 * @dest: list that gets other src added to end of it
 * @src: list to add to end of dest
 */
void dlist_cat(dlist_t *dest, dlist_t *src);

/*
 * Make the destination list a copy of the source list.
 * @free_elem: function to free any existing elements before they get cleared. Pass
 *	NULL if elements don't need to be freed.
 */
void dlist_copy(dlist_t *dest, dlist_t *src, void (*free_elem)(void *));
/*
 * Create a new list from an existing list. Allocates the new list and
 * copies elements to it.
 */
void dlist_copy_new(dlist_t *d, dlist_t *out_d);
/*
 * Make a list a copy of an array of elements. Clears any existing elements.
 * @nelts: number of elements in the source array
 * @free_elem: see dlist_copy
 */
void dlist_copy_array(dlist_t *d, void *elts, int nelts, void (*free_elem)(void *));

#endif
