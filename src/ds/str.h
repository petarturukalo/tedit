/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * A string that grows and shrinks dynamically as characters are added and removed.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef STR_H
#define STR_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../math.h"

// String minimum capacity.
#define STR_MIN_CAP 16

struct string {
	char *buf;
	int len;  // Current number of members in the string.
	// Current capacity of the string, the max amount of members that
	// can be added. Grows and shrinks dynamically.
	int capacity;  
};

typedef struct string str_t;


/**
 * str_alloc - Allocate memory for a string
 * @capacity: initial capacity of the string. Gets rounded up to the
 *	nearest power of 2 that's greater than or equal to capacity.
 * 
 * Return NULL on error. Must be freed with str_free.
 */
str_t *str_alloc(int capacity);
/**
 * str_ninit - Initialise a new string, providing its length.
 * @str: string to fill underlying buffer with, not expected to be
 *	null terminated
 * @n: length of the string, or as much that is to be used
 */
str_t *str_ninit(char *str, int n);
/**
 * str_init - Initialise a new string.
 * @str: string to fill the underlying buffer with, expected to be null
 *	terminated so that its length can be gotten 
 */
str_t *str_init(char *str);
/**
 * str_free - Free a string initialised with str_init.
 */
void str_free(str_t *s);

/**
 * str_try_shrink - Try to dynamically shrink a string
 */
void str_try_shrink(str_t *s);

/**
 * str_insert - Insert a character at an index
 * @c: character to insert
 * @index: index to insert at
 *
 * Pushes all characters back by one. Time complexity O(n)
 * where n is the length of the string, to shuffle characters back.
 */
void str_insert(str_t *s, char c, int index);
/**
 * str_append - Add a character to the end of a string.
 */
void str_append(str_t *s, char c);

/**
 * str_delete - Delete the character at index
 *
 * Pulls all characters after the index forward by one. Time complexity
 * O(n) where n is the length of the string, to shuffle characters forwards.
 */
void str_delete(str_t *s, int index);
/**
 * str_pop - Pop the character off at the end of the string
 *
 * Return the character.
 */
char str_pop(str_t *s);

/**
 * str_split - Split a string at an index.
 *
 * The input string is truncated up to but excluding the index, with all that 
 * comes at and after it allocated and copied to a new string, which is
 * returned, or NULL on error.
 */
str_t *str_split(str_t *s, int index);

/**
 * str_cat - Concatenate one string to the end of the other
 * @dest: string that gets other src added to end of it
 * @src: string to add to end of dest
 *
 * Worst case time complexity O(n) where n is the length of the source string
 * (ignoring potential reallocation).
 */
str_t *str_cat(str_t *dest, str_t *src);

/**
 * str_set - Set the string buffer of s to t
 * @s: string data structure to copy to
 * @t: null terminated string to copy from
 *
 * This is called set rather than copy because it clears existing string contents.
 */
void str_set(str_t *s, char *t);

/**
 * str_cpy - Copy an existing string to a new string
 */
str_t *str_cpy(str_t *s);

#endif
