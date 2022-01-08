/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Dynamic list. A C++ vector (or Python list) data structure.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef DLIST_H
#define DLIST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define DLIST_MIN_CAP 16

struct dynamic_list {
	void *array;
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
/**
 * Free the members of a dynamic list initialised with dlist_init.
 * free_elem: function to free an element in the list. If NULL it's
 *	assumed elements don't need freeing.
 */
void dlist_free(dlist_t *s, void (*free_elem)(void *));


void dlist_append(dlist_t *d, void *elem);
void dlist_insert(dlist_t *d, int index, void *elem);

void dlist_delete_ind(dlist_t *d, int index);
/*
 * @elem: element to delete
 * @match_func: function which returns true if two elements are the equal
 *
 * Return whether the element was found and deleted.
 */
bool dlist_delete_elem(dlist_t *d, void *elem, bool (*match_func)(void *, void *));

#endif
