/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "lines.h"

// Number of bytes to read per read operation.
#define READSZ 4096

void lines_alloc(lines_t *ls)
{
	dlist_init(ls, 64, sizeof(line_t));
}

void lines_free(lines_t *ls)
{
	dlist_free(ls, (dlist_elem_fn)line_free);
}

static void lines_expand_tab_spaces(lines_t *ls, int tabsz);

bool lines_from_file(lines_t *ls, int fd, int tabsz)
{
	int i, bread;  // Bytes read.
	line_t l;
	static char readbuf[READSZ];

	if (flock(fd, LOCK_EX|LOCK_NB) == -1)
		return false;

	lines_alloc(ls);
	line_alloc(&l);

	while ((bread = read(fd, readbuf, READSZ)) > 0) {
		for (i = 0; i < bread;) {
			for (; i < bread && readbuf[i] != '\n'; ++i)
				str_append(&l, readbuf[i]);
			if (i < bread && readbuf[i] == '\n') {
				// End current line working on and start a new one.
				str_append(&l, '\n');
				dlist_append(ls, &l);
				line_alloc(&l);
				++i;
			}
		}
	}
	flock(fd, LOCK_UN);

	if (bread == 0) {
		dlist_append(ls, &l);
		lines_expand_tab_spaces(ls, tabsz);
		return true;
	} 
	lines_free(ls);
	return false;
}

void lines_alloc_empty(lines_t *ls)
{
	lines_alloc(ls);
	dlist_append_init(ls, (dlist_elem_fn)line_alloc);
}

static void line_expand_tab_spaces(line_t *l, int *tabsz)
{
	str_expand_tab_spaces(l, *tabsz);
}

/*
 * Expand all tabs in a list of lines into pseudo spaces
 */
static void lines_expand_tab_spaces(lines_t *ls, int tabsz)
{
	dlist_for_each_data(ls, (dlist_elem_data_fn)line_expand_tab_spaces, (void *)&tabsz);
}

static void line_contract_tab_spaces(line_t *l, int *tabsz)
{
	str_contract_tab_spaces(l, *tabsz);
}

/*
 * Contract all spaces there were tabs back into tabs
 */
static void lines_contract_tab_spaces(lines_t *ls, int tabsz)
{
	dlist_for_each_data(ls, (dlist_elem_data_fn)line_contract_tab_spaces, (void *)&tabsz);
}

int lines_write(lines_t *ls, int tabsz, int fd)
{
	line_t *l;
	int bytes, ttl_bytes = 0;

	if (flock(fd, LOCK_EX|LOCK_NB) == -1)
		return -1;

	lines_contract_tab_spaces(ls, tabsz);

	ftruncate(fd, 0);
	lseek(fd, 0, SEEK_SET);

	for (int i = 0; i < ls->len; ++i) {
		l = dlist_get_address(ls, i);
		
		bytes = write(fd, l->array, l->len);
		
		if (bytes == -1) {
			ttl_bytes = -1;
			break;
		}
		ttl_bytes += bytes;
	}
	fsync(fd);
	flock(fd, LOCK_UN);

	lines_expand_tab_spaces(ls, tabsz);
	return ttl_bytes;
}

void lines_delete(lines_t *ls, int nr)
{
	dlist_delete_ind(ls, nr, (dlist_elem_fn)line_free);
}

static void lines_append_forked_line(line_t *line, lines_t *out_lines)
{
	line_t l;
	dlist_copy_new(line, &l);
	dlist_append(out_lines, &l);
}

void lines_fork(lines_t *src, lines_t *dest)
{
	lines_alloc(dest);
	dlist_for_each_data(src, (dlist_elem_data_fn)lines_append_forked_line, (void *)dest);
}

int lines_add_row(lines_t *ls, int row, off_t off)
{
	int new_row = row+off;

	if (new_row < 0)
		new_row = 0;
	else if (new_row >= ls->len)
		new_row = ls->len-1;
	return new_row;
}
