/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef LINES_H
#define LINES_H

#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include "ds/dlist.h"
#include "ds/str.h"
#include "line.h"

// A dynamic list of lines.
typedef dlist_t lines_t;


/**
 * lines_init - Initialise a new lines data structure
 *
 * Return NULL on error.
 */
lines_t *lines_init(void);

/**
 * lines_from_fd - Read a file into lines from a file descriptor
 * fd: file descriptor of opened file
 * @tabsz: see tab.h
 */
lines_t *lines_from_fd(int fd, int tabsz);

/**
 * lines_from_file - Read a file into lines from a filepath
 */
lines_t *lines_from_file(char *filepath, int tabsz);

/**
 * lines_free - Free the lines data structure
 */
void lines_free(lines_t *l);

/**
 * lines_append - Append a line to a list of lines
 */
void lines_append(lines_t *ls, line_t *l);

/**
 * lines_for_each - Run a function on each line of a lines data structure
 * @line_func: function to run on each line
 * @data: data to pass to function
 */
void lines_for_each(lines_t *l, void (*line_func)(line_t *, void *), void *data);

/**
 * lines_write - Write lines to a file
 *
 * Assumes tabs are replaced with spaces, so temporarily reverts back to regular tabs
 * before writing. Return the total number of bytes written.
 */
int lines_write(lines_t *ls, int tabsz, int fd);

/**
 * lines_get - Get a line from a list of lines
 * @nr: number of line to get (0-indexed)
 */
line_t *lines_get(lines_t *l, int nr);

/**
 * lines_delete - Delete a line from the list of lines and free it
 * @nr: 0-indexed line number
 */
void lines_delete(lines_t *l, int nr);

/**
 * lines_insert - Insert a line into a list of lines
 * @ls: list of lines
 * @index: index to insert new line at
 * @l: new line to insert into lines
 */
void lines_insert(lines_t *ls, int index, line_t *l);

/**
 * lines_len - Get the number of lines in a list of lines
 */
int lines_len(lines_t *ls);

/**
 * lines_fork - Create a new copy of lines from existing lines
 */
lines_t *lines_fork(lines_t *ls);

#endif
