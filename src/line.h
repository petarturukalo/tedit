/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Line in a file that appears in a list of lines.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef LINE_H
#define LINE_H

#include "tab.h"

typedef str_t line_t;

/*
 * line_alloc - Allocate memory for a line
 *
 * Wrapper for str_alloc (see it for more info).
 */
line_t *line_alloc(int capacity);

/*
 * line_init - Dynamically allocate a line 
 * @s: string contents of line
 * @n: length of string
 * @tabsz: max length of tab, or size of tabstop intervals (see tab.h)
 *
 * Return NULL on error. Must be freed with line_free. Expands tabs to spaces
 * as specified in tab.h
 */
line_t *line_init(char *s, int n, int tabsz);


/*
 * line_append - Append a character to a line.
 * 
 * Wrapper for str_append (see it for more info).
 */
void line_append(line_t *l, char c);

/*
 * line_free_sl - Free a line as part of a static list (list of lines)
 * @line: line cast to void pointer for compatibility with slist_free since
 *	that's how a list of lines is implemented
 */
void line_free_sl(void *line);

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
 * @l: line to split
 * @col: column to split at, with the at and after columns making
 *	up the new line
 * @tabsz: see tab.h
 *
 * Return the new line. Potentially truncates the input line.
 */
line_t *line_split(line_t *l, int col, int tabsz);

#endif
