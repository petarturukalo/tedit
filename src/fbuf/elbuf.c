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
	e->fb_lines = lines_init();
	lines_insert(e->fb_lines, 0, str_alloc(0));
}

void elbuf_init(elbuf_t *e, WINDOW *w)
{
	cursor_reset(&e->fb_cursor);
	view_init(&e->fb_view, w, -1, 0, 0, 0);
	elbuf_init_lines(e);
}

void elbuf_free(elbuf_t *e)
{
	lines_free(e->fb_lines);
}

line_t *elbuf_line(elbuf_t *e)
{
	return fbaux_line(e, 0);
}

char *elbuf_str(elbuf_t *e)
{
	char *t;
	str_t *s = fbaux_line(e, 0);

	// Calloc for implicit null termination character.
	t = calloc(s->s_len+1, sizeof(char));
	strncpy(t, s->s_buf, s->s_len);

	return t;
}

void elbuf_set(elbuf_t *e, char *s)
{
	line_t *l = elbuf_line(e);

	str_set(l, s);
	e->fb_cursor.c_col = l->s_len;
}
