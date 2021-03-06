/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "bufs.h"

void bufs_active_buf_set_elbuf(bufs_t *b)
{
	b->active_buf = &b->elbuf;
}

/*
 * bufs_next_id - Get the id of the next buffer that will be added to the list of
 *	buffers
 */
int bufs_next_id(bufs_t *b)
{
	return b->nbufs++;
}


fbuf_t *bufs_last_fbuf(bufs_t *b)
{
	// TODO get the most recently accessed buffer instead of the first
	return b->fbufs.len ? dlist_get_address(&b->fbufs, 0) : NULL;
}

void bufs_active_buf_set_fbuf(bufs_t *b)
{
	b->active_buf = b->active_fbuf;
}

/*
 * Append a file buffer to the list of file buffers and set it as the active buffer.
 */
static void append_fbuf_set_active(bufs_t *b, fbuf_t *f)
{
	fbufs_t *fs = &b->fbufs;

	dlist_append(fs, f);
	b->active_fbuf = dlist_get_address(fs, fs->len-1);
}

int bufs_open(bufs_t *b, char *fpath, WINDOW *w, int tabsz)
{
	fbuf_t f;

	if (fpath) {
		if (fbuf_open(&f, fpath, w, tabsz, bufs_next_id(b))) {
			append_fbuf_set_active(b, &f);
			return 0;
		}
	}
	return -1;
}

void bufs_new(bufs_t *b, WINDOW *w, int tabsz)
{
	fbuf_t f;

	fbuf_new(&f, w, tabsz, bufs_next_id(b));
	append_fbuf_set_active(b, &f);
}

void bufs_init(bufs_t *b, WINDOW *w, char *fpaths[])
{
	char **s;

	dlist_init(&b->fbufs, DLIST_MIN_CAP, sizeof(fbuf_t));
	b->active_buf = NULL;
	elbuf_init(&b->elbuf, w);
	b->nbufs = 1;
	b->cmd_istr[0] = '\0';
	b->cmd_ostr[0] = '\0';

	for (s = fpaths; *s; s++) 
		bufs_open(b, *s, w, TABSZ);
	// Handles both cases where there are no filepaths given and 
	// when there are filepaths but none could be opened.
	if (b->fbufs.len == 0)
		bufs_new(b, w, TABSZ);

	b->active_buf = b->active_fbuf;
}

void bufs_free(bufs_t *b)
{
	fbufs_free(&b->fbufs);
	elbuf_free(&b->elbuf);
}

int bufs_edit(bufs_t *b, char *fpath)
{
	fbuf_t *f;
	int n = b->fbufs.len;

	for (int i = 0; i < n; ++i) {
		f = dlist_get_address(&b->fbufs, i);
		
		if (f->filepath && strcmp(fpath, f->filepath) == 0) {
			b->active_fbuf = f;
			return 0;
		}
	}
	return -1;
}

int bufs_jump(bufs_t *b, int id)
{
	fbuf_t *f;
	int n = b->fbufs.len;

	for (int i = 0; i < n; ++i) {
		f = dlist_get_address(&b->fbufs, i);

		if (f->id == id) {
			b->active_fbuf = f;
			return 0;
		}
	}
	return -1;
}

int bufs_write(bufs_t *b)
{
	fbuf_t *f;

	if (b->active_fbuf) {
		f = b->active_fbuf;

		if (f->filepath)
			return fbuf_write(f);
	}
	return -1;
}

int bufs_link_write(bufs_t *b, char *fpath)
{
	fbuf_t *f = b->active_fbuf;

	fbuf_link(f, fpath);
	return fbuf_write(f);
}

int bufs_write_other(bufs_t *b, char *fpath, WINDOW *w, int tabsz)
{
	int bytes;
	fbuf_t f;

	fbuf_fork(&f, b->active_fbuf, w, bufs_next_id(b));
	fbuf_link(&f, fpath);
	bytes = fbuf_write(&f);

	if (bytes != -1) 
		append_fbuf_set_active(b, &f);
	return bytes;
}

void bufs_close(bufs_t *b, WINDOW *w)
{
	fbuf_t *f = b->active_fbuf;

	fbufs_delete_fbuf(&b->fbufs, f);
	b->active_fbuf = bufs_last_fbuf(b);

	// Resort to a new empty buffer if all become closed.
	if (b->fbufs.len == 0) 
		bufs_new(b, w, TABSZ);
}

void bufs_reset_cmd_strs(bufs_t *b, char *s, int n)
{
	int max_len = sizeof(b->cmd_istr);

	// Echo line string doesn't have tabs to worry about so can use its buf directly.
	strncpy(b->cmd_istr, s, max_len);
	if (n >= max_len)
		b->cmd_istr[max_len-1] = '\0';
	else
		b->cmd_istr[n] = '\0';
	b->cmd_ostr[0] = '\0';
}

