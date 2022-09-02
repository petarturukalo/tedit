/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "bufs.h"

static int FBUF_ID_START = 1;

void bufs_active_buf_set_elbuf(bufs_t *b)
{
	b->active_buf = &b->elbuf;
}

/*
 * Get the ID of the next buffer that will be added to the list of buffers.
 */
int bufs_next_id(bufs_t *b)
{
	return b->nbufs++;
}

static bool id_eq_fbuf_id(int *id, fbuf_t *f)
{
	return *id == f->id;
}

/*
 * Get the previous file buffer accessed.
 */
static fbuf_t *bufs_prev_fbuf(bufs_t *b)
{
	int id = 0;
	fbuf_t *f = NULL;

	while (stack_pop(&b->recent_fbufs, &id)) {
		f = dlist_lookup_address(&b->fbufs, &id, (bool (*)(void *, void *))id_eq_fbuf_id);
		// Lookup will return NULL if the file has been closed. This check skips closed files, e.g.
		// you open A, B, C, then edit B: the stack will be [A,B,C,B] *B. Close current file B you get 
		// [A,B,C] *C, with C now the current file. B is already closed so when you close C, B gets skipped 
		// and the stack becomes [A] *A, with A the current file.
		if (f)
			break;
	}
	return f;
}

void bufs_active_buf_set_fbuf(bufs_t *b)
{
	b->active_buf = b->active_fbuf;
}

/*
 * Set the active file buffer in b to the file buffer f.
 * Always use this instead of setting the active_fbuf struct member directly
 * because it does bookkeeping used by bufs_prev_fbuf().
 */
static void set_active_fbuf(bufs_t *b, fbuf_t *f)
{
	int *id = stack_peek(&b->recent_fbufs);

	if (f && (!id || *id != f->id)) 
		stack_push(&b->recent_fbufs, &f->id);
	b->active_fbuf = f;
}

/*
 * Append a file buffer to the list of file buffers and set it as the active buffer.
 */
static void append_fbuf_set_active(bufs_t *b, fbuf_t *f)
{
	fbufs_t *fs = &b->fbufs;

	dlist_append(fs, f);
	set_active_fbuf(b, dlist_get_address(fs, fs->len-1));
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

/*
 * Build a string to indicate to the user a list of files that failed to be opened.
 *
 * @fpath: filepath of file that couldn't be opened
 * @i: index of filepath in list of filepaths (0-indexed)
 */
static void build_files_fail_open_str(strncat_data_t *sdata, char *fpath, int i)
{			
	if (i == 0) 
		strncat_cont("couldn't open files ", sdata);
	else
		strncat_cont(", ", sdata);
	strncat_printf_cont(sdata, "'%s' (%s)", fpath, strerror(errno));
}

void bufs_init(bufs_t *b, WINDOW *w, char *fpaths[])
{
	char **s;
	strncat_data_t sdata;
	uint nfiles_fail_open = 0;

	dlist_init(&b->fbufs, DLIST_MIN_CAP, sizeof(fbuf_t));
	b->active_buf = NULL;
	elbuf_init(&b->elbuf, w);
	b->nbufs = FBUF_ID_START;
	b->cmd_istr[0] = '\0';
	b->cmd_ostr[0] = '\0';
	strncat_start(b->cmd_ostr, sizeof(b->cmd_ostr), &sdata);
	stack_init(&b->recent_fbufs, sizeof(int));

	for (s = fpaths; *s; s++) {
		if (bufs_open(b, *s, w, TABSZ) == -1)  
			build_files_fail_open_str(&sdata, *s, nfiles_fail_open++);
	}
	// Flush list of files that couldn't be opened to the echo line buffer so that it
	// can be viewed by the user.
	if (b->cmd_ostr[0]) {
		elbuf_set(&b->elbuf, b->cmd_ostr);
		b->cmd_ostr[0] = '\0';
	}
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
	dlist_free(&b->recent_fbufs, NULL);
}

static bool fpath_eq_fbuf_fpath(char *fpath, fbuf_t *f)
{
	return f->filepath && strcmp(fpath, f->filepath) == 0;
}

int bufs_edit(bufs_t *b, char *fpath)
{
	fbuf_t *f = dlist_lookup_address(&b->fbufs, fpath, (bool (*)(void *, void*))fpath_eq_fbuf_fpath);

	if (f) {
		set_active_fbuf(b, f);
		return 0;
	}
	return -1;
}

int bufs_jump(bufs_t *b, int id)
{
	fbuf_t *f = dlist_lookup_address(&b->fbufs, &id, (bool (*)(void *, void *))id_eq_fbuf_id);

	if (f) {
		set_active_fbuf(b, f);
		return 0;
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

	if (bytes == -1)
		fbuf_free(&f);
	else
		append_fbuf_set_active(b, &f);
	return bytes;
}

void bufs_close(bufs_t *b, WINDOW *w)
{
	fbuf_t *f = b->active_fbuf;

	fbufs_delete_fbuf(&b->fbufs, f);
	// Remove the ID that was pushed when this buffer was opened.
	stack_pop(&b->recent_fbufs, NULL);
	set_active_fbuf(b, bufs_prev_fbuf(b));

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

