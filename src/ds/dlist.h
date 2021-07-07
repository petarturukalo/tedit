/* 
 * SPDX-License-Identifier: GPL-2.0
 *
 * A dynamic list which increases in size as more elements are added.
 * It's the same as a static list except functions which add elements
 * to the list may resize it by allocating a new list.
 * Dlist functions must be used for adding elements to the list. As for other 
 * operations, slist functions from slist.h can be used.
 * See slist interface for documentation.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef DLIST_H
#define DLIST_H

#include <math.h>
#include <string.h>
#include <assert.h>
#include "slist.h"
#include "../math.h"

#define DLIST_MIN_CAP 16

typedef slist_t dlist_t;

/**
 * dlist_init - Initialise a new dynamic list
 * @capacity: max number of elements that can initially be held in the list
 *
 * Capacity of underlying list may be initialised to larger than input parameter.
 */
dlist_t *dlist_init(int capacity);

void dlist_append(dlist_t *d, void *elem);
void dlist_insert(dlist_t *d, int index, void *elem);

void dlist_delete_ind(dlist_t *d, int index);
bool dlist_delete_elem(dlist_t *d, void *elem, bool (*match_func)(void *, void *));

#endif
