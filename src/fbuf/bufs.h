/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * A collection of buffers, whether file or other, such as echo line.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef BUFS_H
#define BUFS_H

#include <curses.h>
#include "../ds/dlist.h"
#include "fbuf.h"
#include "fbio.h"
#include "elbuf.h"

// Lengths of the command I/O strings.
// If a string entered into the echo line buffer is longer than 200 bytes
// then it is truncated. Same goes for an oversized return string.
#define CMD_ISTR_LEN 200
#define CMD_OSTR_LEN 500

struct buffers {
	fbufs_t *fbufs;  // List of file buffers.
	elbuf_t elbuf;  // Echo line buffer at bottom of screen for running commands from.
	// Currently active buffer which keys will be sent to (which could be
	// an echo line buffer rather than a standard file buffer).
	fbuf_t *active_buf;  
	// Currently active file buffer (file buffer which will be displayed along with the
	// echo line buffer).
	fbuf_t *active_fbuf;
	// Total number of buffers made, still keeping count of those deleted.
	int nbufs;
	char *cmd_istr;  // Command input string.
	char *cmd_ostr;  // Command output string.
};

typedef struct buffers bufs_t;


/**
 * bufs_init - Initialise a new, empty collection of buffers
 * @fpaths: paths of files to open initially
 * @nfpaths: number of files to open
 *
 * Open an empty, unnamed file if no filepaths are given. Some fields are dynamically allocated.
 * Free with bufs_free.
 */
void bufs_init(bufs_t *b, WINDOW *w, char *fpaths[], int nfpaths);

/**
 * bufs_free - Free buffers allocated with bufs_init
 */
void bufs_free(bufs_t *b);


/**
 * bufs_active_buf_set_elbuf - Set the active buffer to the echo line buffer so the user
 *	can enter and run commands
 */
void bufs_active_buf_set_elbuf(bufs_t *b);

/**
 * bufs_last_fbuf - Get the last file buffer used
 */
fbuf_t *bufs_last_fbuf(bufs_t *b);

/**
 * bufs_active_buf_set_fbuf - Set the active buffer to the most recently active file buffer
 *	so its text can be edited
 */
void bufs_active_buf_set_fbuf(bufs_t *b);


/**
 * bufs_open - Open a file into a new buffer
 *
 * Opens the file into a new file buffer if there is already a file in the buffer.
 * See fbuf_open for info on parameters.
 * Wrapper for fbuf_open. Return 0 on success, -1 on error.
 */
int bufs_open(bufs_t *b, char *fpath, WINDOW *w, int tabsz);

/**
 * bufs_edit - Edit an already opened and existing file
 *
 * Return 0 on success, -1 on error.
 */
int bufs_edit(bufs_t *b, char *fpath);

/**
 * bufs_jump - Jump to buffer with id
 *
 * Return 0 on success, -1 on error.
 */
int bufs_jump(bufs_t *b, int id);

/**
 * bufs_new - Open a new, empty file buffer
 *
 * Link this to a file on disk by writing. Wrapper for fbuf_new. Return 0 on success,
 * -1 on error.
 */
int bufs_new(bufs_t *b, WINDOW *w, int tabsz);

/**
 * bufs_write - Write the active file buffer to its linked file
 *
 * Return number of bytes written, or -1 on error if the buffer isn't linked to a file
 * or there was an error in writing.
 */
int bufs_write(bufs_t *b);

/**
 * bufs_link_write - Link the active file buffer to a file and then write to it
 *
 * Return number of bytes written, or -1 on error.
 */
int bufs_link_write(bufs_t *b, char *fpath);

/**
 * bufs_write_other - Write the active file buffer to another filepath
 */
int bufs_write_other(bufs_t *b, char *fpath, WINDOW *w, int tabsz);

/**
 * bufs_close - Close the currently active file buffer
 *
 * Closes even if there are changes that need to be saved.
 */
void bufs_close(bufs_t *b, WINDOW *w);

#endif
