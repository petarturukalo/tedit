/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "fbaux.h"

line_t *fbaux_prev_line(fbuf_t *f)
{
	int nr = f->cursor.row-1;

	if (nr < 0)
		return NULL;
	return dlist_get_address(&f->lines, nr);
}

line_t *fbaux_cur_line(fbuf_t *f)
{
	return dlist_get_address(&f->lines, f->cursor.row);
}

line_t *fbaux_next_line(fbuf_t *f)
{
	int nr = f->cursor.row+1;

	if (nr > f->lines.len-1)
		return NULL;
	return dlist_get_address(&f->lines, nr);
}
