/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Auxiliary file buffer functions (file buffer from fbuf.h).
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef FBAUX_H
#define FBAUX_H

#include "fbuf.h"
#include "../lines.h"

/*
 * fbaux_prev_line - Get the line before/above the line the file buffer's cursor is currently on
 *
 * Return NULL for no such line.
 */
line_t *fbaux_prev_line(fbuf_t *f);

/*
 * fbaux_cur_line - Get the current line the file buffer's cursor is on.
 */
line_t *fbaux_cur_line(fbuf_t *f);

/*
 * fbaux_next_line - Get the line after/below the line the file buffer's cursor is currently on
 *
 * Return NULL for no such line.
 */
line_t *fbaux_next_line(fbuf_t *f);


#endif
