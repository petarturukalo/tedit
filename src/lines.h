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
 * Allocate an empty set of lines. The lines will have 1 empty line.
 */
void lines_alloc_empty(lines_t *ls);
/*
 * Read a file into a list of lines.
 * @ls: out-param lines to store read file into
 * @fd: file descriptor of opened file
 * @tabsz: see tab.h
 *
 * Return whether could successfully read lines. 
 */
bool lines_from_file(lines_t *ls, int fd, int tabsz);


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

/*
 * Add an offset to a row index to get a new row index. The new row index is aligned within
 * the bounds of the lines if it were to become out of bounds.
 */
int lines_add_row(lines_t *ls, int row, off_t off);

#endif
