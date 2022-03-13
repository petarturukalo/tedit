/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * I/O operations on a file buffer.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef FBIO_H
#define FBIO_H

#include <curses.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include "../lines.h"
#include "../cursor.h"
#include "../tab.h"
#include "../view.h"
#include "../chrp.h"
#include "fbuf.h"

/*
 * fbuf_new - Start a new empty file buffer (doesn't have any lines)
 * @f: out-param file buffer to make a new, empty file out of
 * @w: curses window file buffer will be displayed in
 * @tabsz: see tab.h
 * @id: unique identifier for buffer in case there are multiple empty unnamed buffers
 *
 * Has no file associated with it, and so can't be written. Associate/link a file
 * with it by call to fbuf_link. Free with fbuf_free.
 */
void fbuf_new(fbuf_t *f, WINDOW *w, int tabsz, int id);
/*
 * fbuf_link - Link a file buffer to a new filepath
 *
 * Dynamically allocates and copies the filepath, which is freed in fbuf_free.
 * Return whether the allocation was successful.
 */
bool fbuf_link(fbuf_t *f, char *fpath);

/*
 * fbuf_fork - Copy an entire file buffer to a new file buffer
 * @dest: out-param destination file buffer to copy to
 * @src: source file buffer to copy from
 */
void fbuf_fork(fbuf_t *dest, fbuf_t *src, WINDOW *w, int id);

/*
 * fbuf_open - Open a file into a file buffer.
 * @f: out-param file buffer to open file into
 * @fpath: filepath of file to open
 *
 * Creates the file if it doesn't exist, reading the contents of the file 
 * into the file buffer. Free with fbuf_free.
 * See fbuf_new for descriptions on the rest of the parameters.
 * Return whether could successfully open the file.
 */
bool fbuf_open(fbuf_t *f, char *fpath, WINDOW *w, int tabsz, int id);

/*
 * fbuf_write - Write the whole of the buffer to its linked filepath.
 *
 * Return -1 on error, and number of bytes written otherwise.
 */
int fbuf_write(fbuf_t *f);

#endif
