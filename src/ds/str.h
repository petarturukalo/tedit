/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * A string that grows and shrinks dynamically as characters are added and removed,
 * like a string in C++ or Python.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef STR_H
#define STR_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../math.h"
#include "dlist.h"

typedef dlist_t str_t;


/*
 * Allocate memory for a string. 
 * @capacity: starting capacity of the string (see dlist_t for more info)
 *
 * Return should be freed with dlist_free.
 */
void str_alloc(str_t *out_s, int capacity);
/*
 * Initialise a new string using a null-terminated string. 
 */
void str_init(str_t *out_s, char *str);
/*
 * Initialise a new string with an optionally null-terminated string 
 * of some length. 
 */
void str_ninit(str_t *out_s, char *str, int n);

void str_append(str_t *s, char c);
void str_insert(str_t *s, char c, int i);

/*
 * Copy the characters in a char * string to a list string.
 * Clears any of the list string's existing characters.
 */
void str_copy(str_t *s, char *t);

#endif
