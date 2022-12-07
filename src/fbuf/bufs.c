/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "bufs.h"
#include "../misc.h"
#include "../log.h"

static const int FBUF_ID_START = 1;

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

static bool fbuf_id_eq_id(fbuf_t *f, int *id)
{
	return f->id == *id;
}

/*
 * Get the previous file buffer accessed.
 */
static fbuf_t *bufs_prev_fbuf(bufs_t *b)
{
	int id = 0;
	fbuf_t *f = NULL;

	while (stack_pop(&b->recent_fbufs, &id)) {
		f = dlist_lookup_address(&b->fbufs, &id, (dlist_match_fn)fbuf_id_eq_id);
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
 * Open a new, unlinked file buffer with its contents the data piped into 
 * stdin from the command line. Return whether successful.
 */
static bool bufs_new_piped_stdin(bufs_t *b, WINDOW *w, int tabsz)
{
	fbuf_t f;
	bool success;

	success = fbuf_new_piped_stdin(&f, w, tabsz, bufs_next_id(b));
	if (success) 
		append_fbuf_set_active(b, &f);
	else 
		tlog("failed to create file buffer from data piped to stdin");
	return success;
}

/*
 * Try to open a file buffer for each file in fpaths. A list of files that couldn't be opened
 * is flushed to the echo line buffer.
 */
static void bufs_open_files(bufs_t *b, WINDOW *w, char *fpaths[])
{
	char **s;
	strncat_data_t sdata;
	uint nfiles_fail_open = 0;

	strncat_start(b->cmd_ostr, sizeof(b->cmd_ostr), &sdata);

	for (s = fpaths; *s; ++s) {
		if (bufs_open(b, *s, w, TABSZ) == -1) {
			// Build error string.
			if (nfiles_fail_open++ == 0) 
				strncat_cont("couldn't open files ", &sdata);
			else
				strncat_cont(", ", &sdata);
			strncat_printf_cont(&sdata, "'%s' (%s)", *s, strerror(errno));
		}
	}
	// Flush error string to echo line buffer.
	if (b->cmd_ostr[0]) {
		elbuf_set(&b->elbuf, b->cmd_ostr);
		b->cmd_ostr[0] = '\0';
	}
}

static bool reopen_stdin(void)
{
	if (close(STDIN_FILENO) == -1)
		return false;
	if (open("/dev/tty", O_RDONLY) == -1)
		return false;
	return true;
}

static bool data_piped_to_stdin(void)
{
	// Will also return true if a file is redirected to stdin.
	return !isatty(STDIN_FILENO) && errno == ENOTTY;
}

/*
 * Create a new, unlinked file buffer out of user data piped to stdin, if there is any.
 */
static void bufs_handle_piped_stdin(bufs_t *b, WINDOW *w)
{
	if (data_piped_to_stdin()) {
		bufs_new_piped_stdin(b, w, TABSZ);
		// Need to reopen stdin after data is piped to stdin otherwise curses
		// glitches out and the program becomes unusable.
		if (!reopen_stdin())
			tlog("failed to reopen stdin");
	}
}

void bufs_init(bufs_t *b, WINDOW *w, char *fpaths[])
{
	dlist_init(&b->fbufs, DLIST_MIN_CAP, sizeof(fbuf_t));
	b->active_buf = NULL;
	elbuf_init(&b->elbuf, w);
	b->nbufs = FBUF_ID_START;
	b->cmd_istr[0] = '\0';
	b->cmd_ostr[0] = '\0';
	stack_init(&b->recent_fbufs, sizeof(int));

	bufs_open_files(b, w, fpaths);
	bufs_handle_piped_stdin(b, w);
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

static bool fbuf_fpath_eq_fpath(fbuf_t *f, char *fpath)
{
	return f->filepath && strcmp(fpath, f->filepath) == 0;
}

int bufs_edit(bufs_t *b, char *fpath)
{
	fbuf_t *f = dlist_lookup_address(&b->fbufs, fpath, (dlist_match_fn)fbuf_fpath_eq_fpath);

	if (f) {
		set_active_fbuf(b, f);
		return 0;
	}
	return -1;
}

int bufs_jump(bufs_t *b, int id)
{
	fbuf_t *f = dlist_lookup_address(&b->fbufs, &id, (dlist_match_fn)fbuf_id_eq_id);

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

		if (fbuf_linked(f))
			return fbuf_write(f);
	}
	return -1;
}

int bufs_link_write(bufs_t *b, char *fpath)
{
	int bytes;
	fbuf_t *f = b->active_fbuf;

	fbuf_link(f, fpath);
	bytes = fbuf_write(f);

	if (bytes == -1)
		fbuf_unlink(f);
	return bytes;
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

	// Set command input string.
	strncpy(b->cmd_istr, s, max_len);
	if (n >= max_len)
		b->cmd_istr[max_len-1] = '\0';
	else
		b->cmd_istr[n] = '\0';
	// Reset command output string.
	b->cmd_ostr[0] = '\0';
}

