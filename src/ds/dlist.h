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

// TODO could move element free function from dlist_free() to struct member
// so doesn't have to be param and can also be used in delete functions
// without a param
typedef struct dynamic_list {
	char *array;  // Bytes where elements are stored.
	int len;  // Current number of elements in the array.
	int capacity;  // Current number of elements that can fit in the array.
	size_t eltsz;  // Size of an element in the array.
} dlist_t;

/* Function to run on an element of a dlist. */
typedef void (*dlist_elem_fn)(void *);
/* 
 * Function to run on an element of a dlist with some arbitrary data.
 * First param is the element, second is the data.
 */
typedef void (*dlist_elem_data_fn)(void *, void *);
/*
 * Function which takes dlist element as first parameter and either another element or arbitrary
 * data as second. Return whether the first param element is one being looked for.
 */
typedef bool (*dlist_match_fn)(void *, void *);

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
 * @free_elem: function to free an element in the list. If NULL it's
 *	assumed elements don't need freeing.
 */
void dlist_free(dlist_t *s, dlist_elem_fn free_elem);

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
/*
 * Remove all the elements in a list. Resets its length to 0.
 * @free_elem: function to free any existing elements before they get cleared. Pass
 *	NULL if elements don't need to be freed.
 */
void dlist_clear(dlist_t *d, dlist_elem_fn free_elem);
/*
 * Append an element to the end of the list. If elem is NULL then
 * still increases the size of the list.
 */
void dlist_append(dlist_t *d, void *elem);
/*
 * Append a new element only providing a function to initialise	it.
 */
void dlist_append_init(dlist_t *d, dlist_elem_fn init_elem);
void dlist_insert(dlist_t *d, int index, void *elem);

/*
 * Remove the last element in the list and optionally store it in out_elem
 * if out_elem isn't NULL. Return whether an element was popped.
 */
bool dlist_pop(dlist_t *d, void *out_elem);
void dlist_delete_ind(dlist_t *d, int index, dlist_elem_fn free_elem);
/*
 * @elem: element to delete
 *
 * Return whether the element was found and deleted.
 */
bool dlist_delete_elem(dlist_t *d, void *elem, dlist_match_fn mfn, dlist_elem_fn free_elem);

/*
 * @fn: function to run on each element in the list
 */
void dlist_for_each(dlist_t *d, dlist_elem_fn fn);
/*
 * @fn: function to run on each element in the list. 
 * @data: 2nd param passed to fn
 */
void *dlist_lookup_address(dlist_t *d, void *data, dlist_match_fn fn);
void dlist_for_each_data(dlist_t *d, dlist_elem_data_fn fn, void *data);
			 

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
void dlist_copy(dlist_t *dest, dlist_t *src, dlist_elem_fn free_elem);
/*
 * Create a new list from an existing list. Allocates the new list and
 * copies elements to it.
 */
void dlist_copy_new(dlist_t *d, dlist_t *out_d);
/*
 * Make a list a copy of an array of elements. Clears any existing elements.
 * @nelts: number of elements in the source array
 * @free_elem: see dlist_copy()
 */
void dlist_copy_array(dlist_t *d, void *elts, int nelts, dlist_elem_fn free_elem);

/*
 * Resize the number of elements in a list (its length) to new_len.
 * If new_len is < the current length of the list then elements are lost.
 */
void dlist_resize_len(dlist_t *l, int new_len);
/*
 * Same as dlist_resize_len() but initialise new elements with the init_elem function
 * as they are added.
 * @data: data pased to init_elem function as second parameter
 */
void dlist_resize_len_init_data(dlist_t *l, int new_len, dlist_elem_data_fn init_elem, void *data);

/*
 * Fill the allocated memory area of a list with a constant byte c. 
 */
void dlist_memset(dlist_t *l, unsigned char c);

#endif
