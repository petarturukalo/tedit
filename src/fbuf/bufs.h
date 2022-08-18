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
#include "../ds/stack.h"
#include "../chrp.h"
#include "fbuf.h"
#include "fbio.h"
#include "elbuf.h"

struct buffers {
	fbufs_t fbufs;  // List of file buffers.
	// A stack containing the int IDs of the most recently accessed file buffers. This is to keep 
	// track of which file buffers were swapped to or opened, and in which order.
	// ID is used instead of address because the addresses shift as buffers are created and deleted.
	mystack_t recent_fbufs;
	elbuf_t elbuf;  // Echo line buffer at bottom of screen for running commands from.
	// Currently active buffer which keys will be sent to (which could be
	// an echo line buffer rather than a standard file buffer).
	fbuf_t *active_buf;  
	// Currently active file buffer (file buffer which will be displayed along with the
	// echo line buffer).
	fbuf_t *active_fbuf;
	// Total number of buffers made, still keeping count of those deleted.
	int nbufs;
	char cmd_istr[256];  // Command input string.
	char cmd_ostr[512];  // Command output string.
};

typedef struct buffers bufs_t;


/*
 * bufs_init - Initialise a new, empty collection of buffers
 * @fpaths: paths of files to open initially. This array is NULL pointer terminated.
 *
 * Open an empty, unnamed file if no filepaths are given. Some fields are dynamically allocated.
 * Free with bufs_free.
 */
void bufs_init(bufs_t *b, WINDOW *w, char *fpaths[]);

/*
 * bufs_free - Free buffers allocated with bufs_init
 */
void bufs_free(bufs_t *b);


/*
 * bufs_active_buf_set_elbuf - Set the active buffer to the echo line buffer so the user
 *	can enter and run commands
 */
void bufs_active_buf_set_elbuf(bufs_t *b);

/*
 * Get the previous file buffer accessed.
 */
fbuf_t *bufs_prev_fbuf(bufs_t *b);

/*
 * bufs_active_buf_set_fbuf - Set the active buffer to the most recently active file buffer
 *	so its text can be edited
 */
void bufs_active_buf_set_fbuf(bufs_t *b);


/*
 * bufs_open - Open a file into a new buffer
 *
 * Opens the file into a new file buffer if there is already a file in the buffer.
 * See fbuf_open for info on parameters.
 * Wrapper for fbuf_open. Return 0 on success, -1 on error.
 */
int bufs_open(bufs_t *b, char *fpath, WINDOW *w, int tabsz);

/*
 * bufs_edit - Edit an already opened and existing file
 *
 * Return 0 on success, -1 on error.
 */
int bufs_edit(bufs_t *b, char *fpath);

/*
 * bufs_jump - Jump to buffer with id
 *
 * Return 0 on success, -1 on error.
 */
int bufs_jump(bufs_t *b, int id);

/*
 * bufs_new - Open a new, empty file buffer
 *
 * Link this to a file on disk by writing. Wrapper for fbuf_new. 
 */
void bufs_new(bufs_t *b, WINDOW *w, int tabsz);

/*
 * bufs_write - Write the active file buffer to its linked file
 *
 * Return number of bytes written, or -1 on error if the buffer isn't linked to a file
 * or there was an error in writing.
 */
int bufs_write(bufs_t *b);

/*
 * bufs_link_write - Link the active file buffer to a file and then write to it
 *
 * Return number of bytes written, or -1 on error.
 */
int bufs_link_write(bufs_t *b, char *fpath);

/*
 * bufs_write_other - Write the active file buffer to another filepath
 */
int bufs_write_other(bufs_t *b, char *fpath, WINDOW *w, int tabsz);

/*
 * bufs_close - Close the currently active file buffer
 *
 * Closes even if there are changes that need to be saved.
 */
void bufs_close(bufs_t *b, WINDOW *w);

/*
 * Safely set the input string of the buffer.
 * @s: string to set input string to
 * @n: length of string
 *
 * Sets the command output string to a length 0 null-terminated string.
 */
void bufs_reset_cmd_strs(bufs_t *b, char *s, int n);

#endif
