/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Miscellaneous functions.
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#ifndef MISC_H
#define MISC_H

#include <sys/types.h>
#include <stdbool.h>

/* Get the number of elements in an array where its size is known. */
#define ARRAY_LEN(a)	(sizeof(a)/sizeof(a[0]))

/*
 * Get the length of an array where its size isn't known but its element size is,
 * and the array ends in a zeroed element.
 */
int array_len(char *a, size_t eltsz);

/*
 * For two pointers p and q where p < q, make on offset from q relative to p.
 */
int ptr_relative_off(char *p, char *q, off_t off);

/*
 * Free pointer and set it to NULL if it's not NULL.
 * @ptr: address of pointer
 */
void freep(void **ptr);
/* Char pointer version of freep(). */
void freecp(char **ptr);

/*
 * Get whether a file exists.
 */
bool file_exists(const char *fpath);

#endif
