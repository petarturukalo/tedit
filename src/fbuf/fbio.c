/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "fbio.h"

/**
 * fbuf_reset - Reset all of a file buffer's fields to default values except for
 *	its view (and leaves tab size unchanged)
 */
void fbuf_reset_most(fbuf_t *f)
{
	f->filepath = NULL;
	f->lines = NULL;
	cursor_reset(&f->cursor);
	f->unsaved_edit = false;
}

/**
 * fbuf_reset - Reset all of a file buffer's fields to default values
 */
void fbuf_reset(fbuf_t *f)
{
	fbuf_reset_most(f);
	view_reset(&f->view);
}

/**
 * fbuf_init - Initialise a new, empty file buffer
 * @w: curses window file buffer is displayed to
 * @id: see fbuf struct
 *
 * Return NULL on error. Must be freed with fbuf_free.
 */
fbuf_t *fbuf_init(WINDOW *w, int tabsz, int id)
{
	fbuf_t *f = malloc(sizeof(fbuf_t));

	if (!f)
		return NULL;

	fbuf_reset_most(f);
	view_init(&f->view, w, 0, 1, 0, 0);
	f->tabsz = tabsz;
	f->id = id;

	return f;
}

fbuf_t *fbuf_new(WINDOW *w, int tabsz, int id)
{
	fbuf_t *f;
	lines_t *l = lines_init();

	if (!l) 
		return NULL;
	
	f = fbuf_init(w, tabsz, id);

	if (!f) {
		lines_free(l);
		return NULL;
	}
	lines_append(l, str_alloc(0));
	f->lines = l;
	return f;
}

int fbuf_link(fbuf_t *f, char *fpath)
{
	f->filepath = chrpcpy_alloc(fpath);

	if (!f->filepath)
		return -1;
	return 0;
}

fbuf_t *fbuf_fork(fbuf_t *f, WINDOW *w, int id)
{
	fbuf_t *new = malloc(sizeof(fbuf_t));

	if (!new)
		return NULL;

	new->id = id;
	new->cursor = f->cursor;
	new->filepath = f->filepath;
	new->lines = lines_fork(f->lines);
	new->tabsz = f->tabsz;
	new->view = f->view;
	new->unsaved_edit = false;

	return new;
}

/**
 * fbuf_openfd - Open the file associated with a file buffer as a file descriptor
 *
 * Return -1 on error. Assumes the file buffer has an associated file.
 */
int fbuf_openfd(fbuf_t *f)
{
	return open(f->filepath, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);
}

fbuf_t *fbuf_open(char *fpath, WINDOW *w, int tabsz, int id)
{
	int fd; 
	fbuf_t *f = fbuf_init(w, tabsz, id);

	fbuf_link(f, fpath);
	fd = fbuf_openfd(f);

	if (fd == -1)
		return NULL;

	f->lines = lines_from_fd(fd, tabsz);
	close(fd);

	if (!f->lines) {
		fbuf_free(f);
		return NULL;
	}
	return f;
}

int fbuf_write(fbuf_t *f)
{
	int fd, bytes;

	if (f->filepath) {
		fd = fbuf_openfd(f);
		
		if (fd != -1) {
			bytes = lines_write(f->lines, f->tabsz, fd);

			if (bytes != -1)
				f->unsaved_edit = false;
			close(fd);
			return bytes;
		}
	}
	return -1;
}

void fbuf_free(fbuf_t *f)
{
	if (f->filepath)
		free(f->filepath);
	if (f->lines) 
		lines_free(f->lines);
	free(f);
}

/**
 * fbuf_free_vdata - Wrapper for fbuf_free which frees a file buffer cast to void pointer
 * @f: file buffer cast to void pointer
 */
void fbuf_free_vdata(void *f)
{
	fbuf_free((fbuf_t *)f);
}

void fbufs_free(fbufs_t *fs)
{
	slist_free(fs, fbuf_free_vdata);
}
