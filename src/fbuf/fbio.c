/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "fbio.h"

/*
 * fbuf_reset - Reset all of a file buffer's struct members to default values except for
 *	its view (and leaves tab size unchanged)
 */
static void fbuf_reset_most(fbuf_t *f)
{
	f->filepath = NULL;
	cursor_reset(&f->cursor);
	f->unsaved_edit = false;
}

/*
 * fbuf_reset - Reset all of a file buffer's struct members to default values
 */
static void fbuf_reset(fbuf_t *f)
{
	fbuf_reset_most(f);
	view_reset(&f->view);
}

/*
 * fbuf_init_most - Initialise the values of a file buffer except for its lines
 * @f: out-param file buffer to initialise
 * @w: curses window file buffer is displayed to
 * @id: see fbuf struct
 */
static void fbuf_init_most(fbuf_t *f, WINDOW *w, int tabsz, int id)
{
	fbuf_reset_most(f);
	view_init(&f->view, w, 0, 1, 0, 0);
	f->tabsz = tabsz;
	f->id = id;
}

void fbuf_new(fbuf_t *f, WINDOW *w, int tabsz, int id)
{
	fbuf_init_most(f, w, tabsz, id);
	lines_alloc(&f->lines);
	// Add a single empty line which the user will start on.
	dlist_append_init(&f->lines, (void (*)(void *))line_alloc);
}

bool fbuf_link(fbuf_t *f, char *fpath)
{
	f->filepath = strdup(fpath);
	return f->filepath;
}

void fbuf_fork(fbuf_t *dest, fbuf_t *src, WINDOW *w, int id)
{
	dest->id = id;
	dest->cursor = src->cursor;
	dest->filepath = src->filepath;
	lines_fork(&src->lines, &dest->lines);
	dest->tabsz = src->tabsz;
	dest->view = src->view;
	dest->unsaved_edit = false;
}

/*
 * Open the file linked to a file buffer and return a file descriptor for it.
 * Assumes the file buffer has been linked to a file with call to fubf_link().
 */
int fbuf_openfd(fbuf_t *f)
{
	return open(f->filepath, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
}

bool fbuf_open(fbuf_t *f, char *fpath, WINDOW *w, int tabsz, int id)
{
	int fd; 
	bool read_success;

	fbuf_init_most(f, w, tabsz, id);
	fbuf_link(f, fpath);
	fd = fbuf_openfd(f);

	if (fd == -1)
		return false;

	read_success = lines_from_fd(&f->lines, fd, tabsz);
	close(fd);

	if (!read_success) 
		fbuf_free(f);
	return read_success;
}

int fbuf_write(fbuf_t *f)
{
	int fd, bytes;

	if (f->filepath) {
		fd = fbuf_openfd(f);
		
		if (fd != -1) {
			bytes = lines_write(&f->lines, f->tabsz, fd);

			if (bytes != -1)
				f->unsaved_edit = false;
			close(fd);
			return bytes;
		}
	}
	return -1;
}

