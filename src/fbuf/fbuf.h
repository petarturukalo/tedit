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

void fbuf_free(fbuf_t *f);
void fbufs_free(fbufs_t *fs);
bool fbufs_delete_fbuf(fbufs_t *fs, fbuf_t *f);

#endif
