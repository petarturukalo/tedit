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

/**
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
	return fbufs_len(b->fbufs) > 0 ? fbufs_get(b->fbufs, 0) : NULL;
}

void bufs_active_buf_set_fbuf(bufs_t *b)
{
	b->active_buf = b->active_fbuf;
}

/**
 * bufs_fbufs_append - Append a new file buffer to the list of file buffers
 */
void bufs_fbufs_append(bufs_t *b, fbuf_t *f)
{
	// Set active buffer if don't have one.
	if (fbufs_len(b->fbufs) == 0) 
		b->active_fbuf = f;
	fbufs_append(b->fbufs, f);
}

int bufs_open(bufs_t *b, char *fpath, WINDOW *w, int tabsz)
{
	fbuf_t *f;

	if (fpath) {
		f = fbuf_open(fpath, w, tabsz, bufs_next_id(b));

		if (f) {
			bufs_fbufs_append(b, f);
			b->active_fbuf = f;
			return 0;
		}
	}
	return -1;
}

int bufs_new(bufs_t *b, WINDOW *w, int tabsz)
{
	fbuf_t *f = fbuf_new(w, tabsz, bufs_next_id(b));

	if (f) {
		bufs_fbufs_append(b, f);
		b->active_fbuf = f;
		return 0;
	}
	return -1;
}

void bufs_init(bufs_t *b, WINDOW *w, char *fpaths[], int nfpaths)
{
	b->fbufs = dlist_init(0);
	b->active_buf = NULL;
	elbuf_init(&b->elbuf, w);
	b->nbufs = 1;
	b->cmd_istr = malloc(sizeof(char)*CMD_ISTR_LEN);
	b->cmd_ostr = malloc(sizeof(char)*CMD_OSTR_LEN);

	if (nfpaths > 0) {
		for (int i = 0; i < nfpaths; ++i)
			bufs_open(b, fpaths[i], w, TABSZ);
	} 
	// Handles both cases where there are no filepaths given and 
	// when there are filepaths but none could be opened.
	if (b->fbufs->len == 0)
		bufs_new(b, w, TABSZ);

	b->active_buf = b->active_fbuf;
}

void bufs_free(bufs_t *b)
{
	fbufs_free(b->fbufs);
	elbuf_free(&b->elbuf);
	free(b->cmd_istr);
	free(b->cmd_ostr);
}

int bufs_edit(bufs_t *b, char *fpath)
{
	fbuf_t *f;
	int n = fbufs_len(b->fbufs);

	for (int i = 0; i < n; ++i) {
		f = fbufs_get(b->fbufs, i);
		
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
	int n = fbufs_len(b->fbufs);

	for (int i = 0; i < n; ++i) {
		f = fbufs_get(b->fbufs, i);

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
	fbuf_t *f = fbuf_fork(b->active_fbuf, w, bufs_next_id(b));

	if (f) {
		fbuf_link(f, fpath);

		bytes = fbuf_write(f);

		if (bytes == -1) 
			free(f);
		else {
			fbufs_append(b->fbufs, f);
			b->active_fbuf = f;
			return bytes;
		}
	}
	return -1;
}

void bufs_close(bufs_t *b, WINDOW *w)
{
	fbuf_t *f = b->active_fbuf;

	fbufs_delete_fbuf(b->fbufs, f);
	free(f);

	b->active_fbuf = bufs_last_fbuf(b);

	// Resort to a new empty buffer if all become closed.
	if (fbufs_len(b->fbufs) == 0)  
		bufs_fbufs_append(b, fbuf_new(w, TABSZ, bufs_next_id(b)));
}
