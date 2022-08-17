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

typedef dlist_t lines_t;

void lines_alloc(lines_t *ls);
void lines_free(lines_t *ls);

/*
 * lines_from_fd - Read a file into lines from a file descriptor
 * @ls: out-param lines to store read file into
 * @fd: file descriptor of opened file
 * @tabsz: see tab.h
 *
 * Return whether could successfully read lines.
 */
bool lines_from_fd(lines_t *ls, int fd, int tabsz);
/*
 * See lines_from_fd for more info.
 */
bool lines_from_file(lines_t *ls, char *filepath, int tabsz);


/*
 * lines_for_each - Run a function on each line of a lines data structure
 * @line_func: function to run on each line
 * @data: data to pass to function
 */
void lines_for_each(lines_t *l, void (*line_func)(line_t *, void *), void *data);

/*
 * lines_write - Write lines to a file
 *
 * Assumes tabs are replaced with pseudo spaces, so temporarily reverts back to regular tabs
 * before writing. Return the total number of bytes written.
 */
int lines_write(lines_t *ls, int tabsz, int fd);

/*
 * lines_delete - Delete a line from the list of lines and free it
 * @nr: 0-indexed line number
 */
void lines_delete(lines_t *l, int nr);

/*
 * Create a new copy of lines from existing lines
 */
void lines_fork(lines_t *src, lines_t *dest);

#endif
