/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Operations on character pointers (strings), named so
 * to differentiate between this and the str data structure.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef CHRP_H
#define CHRP_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "math.h"

/*
 * chrp_find - Find the index of the first occurrence of a character 
 *	in a substring
 * @c: character to find
 * @start: start index (inclusive) of the substring, and position to start left to right
 *	comparisons from
 * @end: end index (inclusive) of the substring
 *
 * Return -1 on character not found.
 */
int chrp_find(char *s, char c, int start, int end);

/*
 * chrp_find_reverse - Find the index of the first occurrence of a character in a substring
 *	(traversing right to left, so first occurrence is rightmost reached from starting point)
 * @c: character to find
 * @start: start index (inclusive) of the substring
 * @end: end index (inclusive) of the substring, and position to start right to left comparisons from
 *
 * Return -1 on character not found.
 */
int chrp_find_reverse(char *s, char c, int start, int end);

/*
 * chrpprintf - Initialise a new string with printf style formatting 
 * @format: format string to pass to sprintf
 *
 * Return dynamically allocated string. Must be freed with free.
 */
char *chrpprintf(char *format, ...);

/*
 * chrp_nmatched - Get the number of consecutive matches of a character along a substring
 * @start: start index (inclusive) of the substring
 * @end: end index (inclusive) of the substring
 */
int chrp_nmatched(char *s, char c, int start, int end);

/*
 * chrp_nmatched_reverse - Get the number of consecutive matches of a character along a 
 *	substring in reverse
 * @start: start index (inclusive) of the substring
 * @end: end index (inclusive) of the substring
 */
int chrp_nmatched_reverse(char *s, char c, int start, int end);


/*
 * Data for strncat bookkeeping. See strncat_start() and strncat_cont() for more info.
 */
typedef struct strncat_data {
	char *dest;  // Destination string that sources are concatenated to.
	uint cur_len;  // Current length of string being continously built.
	uint max_len;  // Max length of string that is being built.
} strncat_data_t;

/*
 * Prepare for building a string with multiple calls to strncat on one string, but only
 * providing the max length of the string once before the first call to strncat_cont(), so that
 * you don't have to recalculate the number of bytes to concat on each subsequent call like you would
 * with strncat().
 *
 * @dest: destination string that will be built
 * @max_len: max length of the destination string
 */
void strncat_start(char *dest, uint max_len, strncat_data_t *scdata);

/*
 * Continue building a string by concatenating source safely onto the destination, without worrying
 * about the destination's remaining length and overrunning its buffer. Requires that strncat_start() have
 * been called on sdata at least once.
 *
 * The wrapping dest string will always become null terminated.
 */
void strncat_cont(const char *src, strncat_data_t *sdata);

/*
 * Like strchr() but return a pointer to the first occurrence in the string
 * s that isn't the character c. Return NULL for no occurrence.
 */
char *strnchr(char *s, int c);
/*
 * Same as strnchr() but find the first occurence starting at the end of the string
 * and moving backwards.
 */
char *strnchr_reverse(char *s, int c);

/*
 * Remove a consecutive sequence of the same character c at the end of a string s.
 * Removing all trailing hyphen '-' from string "foo---", for example, would get the string
 * "foo".
 */
void strip_trailchar(char *s, int c);

#endif
