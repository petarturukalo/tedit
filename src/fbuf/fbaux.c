/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "fbaux.h"

int fbaux_row(fbuf_t *f)
{
	return f->cursor.row;
}

int fbaux_col(fbuf_t *f)
{
	return f->cursor.col;
}

int fbaux_nlines(fbuf_t *f)
{
	return lines_len(f->lines);
}

line_t *fbaux_line(fbuf_t *f, int line_nr)
{
	return lines_get(f->lines, line_nr);
}

line_t *fbaux_prev_line(fbuf_t *f)
{
	int nr = fbaux_row(f)-1;

	if (nr < 0)
		return NULL;
	return fbaux_line(f, nr);
}

line_t *fbaux_cur_line(fbuf_t *f)
{
	return fbaux_line(f, f->cursor.row);
}

line_t *fbaux_next_line(fbuf_t *f)
{
	int nr = fbaux_row(f)+1;

	if (nr > fbaux_nlines(f)-1)
		return NULL;
	return fbaux_line(f, nr);
}
