/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "elbuf.h"

/**
 * elbuf_init_lines - Initialise the echo line buffer's lines
 *
 * Only ever has one line.
 */
void elbuf_init_lines(elbuf_t *e)
{
	e->lines = lines_init();
	lines_insert(e->lines, 0, str_alloc(0));
}

void elbuf_init(elbuf_t *e, WINDOW *w)
{
	cursor_reset(&e->cursor);
	view_init(&e->view, w, -1, 0, 0, 0);
	elbuf_init_lines(e);
}

void elbuf_free(elbuf_t *e)
{
	lines_free(e->lines);
}

line_t *elbuf_line(elbuf_t *e)
{
	return fbaux_line(e, 0);
}

char *elbuf_str(elbuf_t *e)
{
	return fbaux_line(e, 0)->buf;
}

int elbuf_strlen(elbuf_t *e)
{
	return fbaux_line(e, 0)->len;
}

void elbuf_set(elbuf_t *e, char *s)
{
	line_t *l = elbuf_line(e);

	str_set(l, s);
	e->cursor.col = l->len;
}
