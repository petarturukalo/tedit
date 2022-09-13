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
 * fbuf_reset - Reset all of a file buffer's struct members to default values except for
 *	its view (and leaves tab size unchanged)
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

#endif
