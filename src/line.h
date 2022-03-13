/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Line in a file that appears in a list of lines. 
 * These functions are wrappers for str functions. 
 * See str.h for more info.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef LINE_H
#define LINE_H

#include "tab.h"

typedef str_t line_t;

void line_alloc(line_t *l);
/*
 * @tabsz: max length of tab, or size of tabstop intervals (see tab.h)
 *
 * Expands tabs to spaces as specified in tab.h
 */
void line_init(line_t *l, char *s, int n, int tabsz);
void line_free(line_t *l);

/*
 * line_len - Get the length of a line
 *
 * Doesn't include the end newline character to keep lengths synchronised
 * across all lines since the end line doesn't have a newline character.
 */
int line_len(line_t *l);
/*
 * line_len_nl - Get the length of a line including the end newline
 */
int line_len_nl(line_t *l);

/*
 * line_split - Split a line into two
 * @l: line to split, becomes the left part of the split
 * @col: column to split at, with the at and after columns making
 *	up the new line
 * @tabsz: see tab.h
 * @newline: out-param new line right part of split that gets put onto a new line
 *
 * Return the new line. Potentially truncates the input line.
 */
void line_split(line_t *l, int col, int tabsz, line_t *newline);

#endif
