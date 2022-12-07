/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * File buffer data structure. Lines of a file awaiting continuous edit
 * and save/write operations.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef FBUF_H
#define FBUF_H

#include "../ds/dlist.h"
#include "../lines.h"
#include "../cursor.h"
#include "../view.h"

struct file_buffer {
	// Unique identifier for the file buffer in case there are multiple 
	// unlinked files or buffers linked to the same filepath.
	int id;
	cursor_t cursor;
	char *filepath;  // Path to currently open file.
	lines_t lines;  // Buffer of open file.
	int tabsz;
	view_t view;  // What the user sees.
	// Whether the file has been edited since last writing.
	bool unsaved_edit;
};

typedef struct file_buffer fbuf_t;
typedef dlist_t fbufs_t;

/*
 * Reset a file buffer's struct members to default values except for
 * its view and tab size.
 */
void fbuf_reset_most(fbuf_t *f);
/*
 * fbuf_reset - Reset all of a file buffer's struct members to default values
 */
void fbuf_reset(fbuf_t *f);
/*
 * fbuf_init_most - Initialise the values of a file buffer except for its lines
 * @f: out-param file buffer to initialise
 * @w: curses window file buffer is displayed to
 * @id: see fbuf struct
 */
void fbuf_init_most(fbuf_t *f, WINDOW *w, int tabsz, int id);

/*
 * fbuf_link - Link a file buffer to a new filepath
 *
 * Dynamically allocates and copies the filepath, which is freed in fbuf_free.
 * Return whether the allocation was successful.
 */
bool fbuf_link(fbuf_t *f, char *fpath);
/*
 * Unlink a file buffer from its current filepath.
 */
void fbuf_unlink(fbuf_t *f);
/* 
 * Get whether a file buffer has been linked to a filepath. 
 */
bool fbuf_linked(fbuf_t *f);
/*
 * Get the name of the filepath that the file buffer is linked to, or "unlinked" 
 * if it is unlinked.
 */
char *fbuf_link_name(fbuf_t *f);

void fbuf_free(fbuf_t *f);
void fbufs_free(fbufs_t *fs);
bool fbufs_delete_fbuf(fbufs_t *fs, fbuf_t *f);

/*
 * fbuf_prev_line - Get the line before/above the line the file buffer's cursor is currently on
 *
 * Return NULL for no such line.
 */
line_t *fbuf_prev_line(fbuf_t *f);

/*
 * fbuf_cur_line - Get the current line the file buffer's cursor is on.
 */
line_t *fbuf_cur_line(fbuf_t *f);

/*
 * fbuf_next_line - Get the line after/below the line the file buffer's cursor is currently on
 *
 * Return NULL for no such line.
 */
line_t *fbuf_next_line(fbuf_t *f);

/*
 * fbuf_new - Start a new empty file buffer (doesn't have any lines)
 * @f: out-param file buffer to make a new, empty file out of
 * @w: curses window file buffer will be displayed in
 * @tabsz: see tab.h
 * @id: unique identifier for buffer in case there are multiple empty unlinked buffers
 *
 * Has no file associated with it, and so can't be written. Associate/link a file
 * with it by call to fbuf_link. Free with fbuf_free.
 */
void fbuf_new(fbuf_t *f, WINDOW *w, int tabsz, int id);
/*
 * Same as fbuf_new() but fill the contents of the buffer with data piped to stdin.
 * Only call this if stdin isn't a tty.
 */
bool fbuf_new_piped_stdin(fbuf_t *f, WINDOW *w, int tabsz, int id);

/*
 * Copy an entire file buffer to a new file buffer. The new file buffer
 * remains unlinked, not inheriting the source's link.
 *
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
