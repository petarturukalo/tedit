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

/**
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

/**
 * chrp_find_reverse - Find the index of the first occurrence of a character in a substring
 *	(traversing right to left, so first occurrence is rightmost reached from starting point)
 * @c: character to find
 * @start: start index (inclusive) of the substring
 * @end: end index (inclusive) of the substring, and position to start right to left comparisons from
 *
 * Return -1 on character not found.
 */
int chrp_find_reverse(char *s, char c, int start, int end);

/** 
 * chrpcpy_alloc - Copy a string to a newly dynamically allocated string (null-terminated)
 * @s: null-terminated string
 *
 * Return NULL on error.
 */
char *chrpcpy_alloc(char *s);

/**
 * chrpprintf - Initialise a new string with printf style formatting and arrangement
 * @len: length >= that of outputted format string (length of returned string; leave room
 *	for null termination character)
 * @format: format string to pass to sprintf
 *
 * Return dynamically allocated string. Must be freed with free.
 */
char *chrpprintf(char *format, ...);

/**
 * chrp_nmatched - Get the number of consecutive matches of a character along a substring
 */
int chrp_nmatched(char *s, char c, int start, int end);

/**
 * chrp_nmatched_reverse - Get the number of consecutive matches of a character along a 
 *	substring in reverse
 */
int chrp_nmatched_reverse(char *s, char c, int start, int end);

#endif
