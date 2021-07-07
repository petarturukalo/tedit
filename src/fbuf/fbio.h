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

/**
 * fbuf_new - Start a new empty file buffer (doesn't have any lines)
 * @f: file buffer to make a new, empty file out of
 * @w: curses window file buffer will be displayed in
 * @tabsz: see tab.h
 * @id: unique identifier for buffer in case there are multiple empty unnamed buffers
 *
 * Has no file associated with it, and so can't be written. Associate/link a file
 * with it by call to fbuf_link.
 */
fbuf_t *fbuf_new(WINDOW *w, int tabsz, int id);
/**
 * fbuf_link - Link a file buffer to a new filepath
 *
 * Return -1 on error and 0 on success. Dynamically allocates and copies the filepath, which
 * can be freed in fbuf_free.
 */
int fbuf_link(fbuf_t *f, char *fpath);

/**
 * fbuf_fork - Copy an entire file buffer to a new file buffer
 */
fbuf_t *fbuf_fork(fbuf_t *f, WINDOW *w, int id);

/**
 * fbuf_open - Open a file into a file buffer.
 * @fpath: filepath of file to open
 *
 * Creates the file if it doesn't exist. 
 * Uses dynamically allocated memory and opens a file. 
 * Free this memory and also close the associated file with fbuf_close. 
 * See fbuf_new for descriptions on the rest of the parameters.
 */
fbuf_t *fbuf_open(char *fpath, WINDOW *w, int tabsz, int id);

/**
 * fbuf_write - Write the whole of the buffer to its linked filepath.
 *
 * Return -1 on error, and number of bytes written otherwise.
 */
int fbuf_write(fbuf_t *f);

/**
 * fbuf_free - Free a file buffer
 */
void fbuf_free(fbuf_t *f);

/**
 * fbufs_free - Free a list of file buffers
 */
void fbufs_free(fbufs_t *fs);

#endif
