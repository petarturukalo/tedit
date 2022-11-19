/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "fbio.h"

void fbuf_new(fbuf_t *f, WINDOW *w, int tabsz, int id)
{
	fbuf_init_most(f, w, tabsz, id);
	lines_alloc(&f->lines);
	// Add a single empty line which the user will start on.
	dlist_append_init(&f->lines, (dlist_elem_fn)line_alloc);
}

void fbuf_fork(fbuf_t *dest, fbuf_t *src, WINDOW *w, int id)
{
	fbuf_reset_most(dest);
	dest->id = id;
	dest->cursor = src->cursor;
	dest->filepath = src->filepath;
	lines_fork(&src->lines, &dest->lines);
	dest->tabsz = src->tabsz;
	dest->view = src->view;
}

/*
 * Open the file linked to a file buffer and return a file descriptor for it.
 * Assumes the file buffer has been linked to a file with call to fbuf_link().
 *
 * @flags: flags passed to open()
 */
int fbuf_openfd(fbuf_t *f, int flags)
{
	return open(f->filepath, flags, 0644);
}

bool fbuf_open(fbuf_t *f, char *fpath, WINDOW *w, int tabsz, int id)
{
	int fd; 
	bool read_success;

	fbuf_init_most(f, w, tabsz, id);
	fbuf_link(f, fpath);
	fd = fbuf_openfd(f, O_RDONLY);

	if (fd == -1) {
		// When there's no file on disk start an empty file buffer with it linked to
		// the filepath for the next write to create the underlying file.
		if (errno == ENOENT) {
			lines_alloc_empty(&f->lines);
			return true;
		}
		fbuf_unlink(f);
		return false;
	}
	read_success = lines_from_file(&f->lines, fd, tabsz);
	close(fd);

	if (!read_success) 
		fbuf_free(f);
	return read_success;
}

int fbuf_write(fbuf_t *f)
{
	int fd, bytes;

	if (fbuf_linked(f)) {
		fd = fbuf_openfd(f, O_WRONLY|O_CREAT);
		
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

