/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "lines.h"

// Number of bytes to read per read operation.
#define READSZ 4096

lines_t *lines_init(void)
{
	return dlist_init(64);
}

/**
 * lines_free - Free a dynamic list of strings representing lines.
 */
void lines_free(lines_t *l)
{
	slist_free(l, line_free_sl);
}

void lines_append(lines_t *ls, line_t *l)
{
	dlist_append(ls, (void *)l);
}

void lines_expand_tab_spaces(lines_t *ls, int tabsz);

/**
 * lines_from_file_aux - Read all lines from a file into a list of lines
 */
int lines_from_file_aux(int fd, lines_t *ls, int tabsz)
{
	int i, bread;  // Bytes read.
	char *buf;
	line_t *l;

	buf = malloc(READSZ*sizeof(char));
	l = line_alloc(0);

	flock(fd, LOCK_EX);

	while ((bread = read(fd, buf, READSZ)) > 0) {
		for (i = 0; i < bread;) {
			for (; i < bread && buf[i] != '\n'; ++i)
				line_append(l, buf[i]);
			if (i < bread && buf[i] == '\n') {
				// End current line working on and start a new one.
				line_append(l, '\n');
				lines_append(ls, l);
				l = line_alloc(0);
				++i;
			}
		}
	}

	flock(fd, LOCK_UN);

	free(buf);

	if (bread == 0)  {
		lines_append(ls, l);  // Save the last line.
		lines_expand_tab_spaces(ls, tabsz);
		return 0;
	} else
		return -1;
}

lines_t *lines_from_fd(int fd, int tabsz)
{
	lines_t *ls = lines_init();

	if (lines_from_file_aux(fd, ls, tabsz) == -1) {
		lines_free(ls);
		return NULL;
	}
	return ls;
}

lines_t *lines_from_file(char *filepath, int tabsz)
{
	lines_t *ls;
	int fd = open(filepath, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);
	
	if (fd == -1)
		return NULL;

	ls = lines_from_fd(fd, tabsz);
	close(fd);
	return ls;
}

void lines_for_each(lines_t *l, void (*line_func)(line_t *, void *), void *data)
{
	for (int i = 0; i < l->len; ++i) 
		line_func(l->array[i], data);
}

/**
 * line_expand_tab_spaces - Expand all tabs in a line into spaces
 * @tabsz: void pointer to tab size so this function can be passed to lines_for_each
 */
void line_expand_tab_spaces(line_t *l, void *tabsz)
{
	int tsz = *(int *)tabsz;
	str_expand_tab_spaces(l, tsz);
}

/**
 * lines_expand_tab_spaces - Expand all tabs in a list of lines into pseudo spaces
 */
void lines_expand_tab_spaces(lines_t *ls, int tabsz)
{
	lines_for_each(ls, line_expand_tab_spaces, (void *)&tabsz);
}

/**
 * line_contract_tab_spaces - Contract all spaces in a line that were tabs back into tabs
 * @tabsz: void pointer to tab size so this function can be passed to lines_for_each
 */
void line_contract_tab_spaces(line_t *l, void *tabsz)
{
	int tsz = *(int *)tabsz;
	str_contract_tab_spaces(l, tsz);
}

/**
 * lines_contract_tab_spaces - Contract all spaces there were tabs back into tabs
 */
void lines_contract_tab_spaces(lines_t *ls, int tabsz)
{
	lines_for_each(ls, line_contract_tab_spaces, (void *)&tabsz);
}

int lines_write(lines_t *ls, int tabsz, int fd)
{
	line_t *l;
	int bytes, ttl_bytes = 0;

	lines_contract_tab_spaces(ls, tabsz);

	flock(fd, LOCK_EX);

	ftruncate(fd, 0);
	lseek(fd, 0, SEEK_SET);

	for (int i = 0; i < ls->len; ++i) {
		l = ls->array[i];
		
		bytes = write(fd, l->buf, l->len);
		
		if (bytes == -1)
			return -1;
		ttl_bytes += bytes;
	}
	fsync(fd);
	flock(fd, LOCK_UN);

	lines_expand_tab_spaces(ls, tabsz);
	return ttl_bytes;
}

line_t *lines_get(lines_t *l, int nr)
{
	return l->array[nr];
}

void lines_delete(lines_t *l, int nr)
{
	str_free(l->array[nr]);
	dlist_delete_ind(l, nr);
}

void lines_insert(lines_t *ls, int index, line_t *l)
{
	dlist_insert(ls, index, (void *)l);
}

int lines_len(lines_t *ls)
{
	return ls->len;
}

lines_t *lines_fork(lines_t *ls)
{
	line_t *l;
	lines_t *new = lines_init();

	for (int i = 0; i < lines_len(ls); ++i) {
		l = str_cpy(lines_get(ls, i));
		lines_append(new, l);
	}
	return new;
}
