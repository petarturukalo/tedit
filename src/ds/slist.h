/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Static list. A C++ vector (or Python list) data structure
 * except that it doesn't grow and has a static maximum length.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef SLIST_H
#define SLIST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct static_list {
	void **array;  // Array used to store user-defined structures.
	int len;  // Current number of elements in the array.
	int capacity;  // Maximum amount of elements that can be in the array.
};

typedef struct static_list slist_t;

/**
 * slist_t - Initialise a new static list
 *
 * Return NULL on error. Must be freed with slist_free.
 */
slist_t *slist_init(int maxlen);
/**
 * slist_free - Free a static list initialised with slist_init
 * free_elem: function to free an element in the list. Pass NULL if
 *	elements don't need freeing.
 */
void slist_free(slist_t *s, void (*free_elem)(void *));

/**
 * slist_append - Append an element to a static list
 *
 * O(1) time complexity. It's up to the user to check boundaries.
 */
void slist_append(slist_t *s, void *elem);
/**
 * slist_insert - Insert an element at index into the list
 */
void slist_insert(slist_t *s, int index, void *elem);

/**
 * slist_lookup - Look up an element in the list
 * @target: element looking up
 * @match_func: function which takes as params an element in the list and the target,
 *	and returns true if the element is the target
 *
 * Time complexity O(n*m) where n is the current number of elements in the list
 * and m is the time complexity of comparison by the match function between two
 * elements.
 */
int slist_lookup(slist_t *s, void *target, bool (*match_func)(void *, void *));

/**
 * slist_delete - Delete an element from a list by index
 * @index: index of element to delete
 *
 * Time complexity O(n) to shuffle elements after index back into position.
 */
void slist_delete_ind(slist_t *s, int index);
/**
 * slist_delete_elem - Delete an element from a list
 * 
 * See slist_lookup for params since this wraps that. Deletes the leftmost occurrence
 * of the element. Return whether the element was found and so deleted.
 */
bool slist_delete_elem(slist_t *s, void *elem, bool (*match_func)(void *, void *));

#endif
