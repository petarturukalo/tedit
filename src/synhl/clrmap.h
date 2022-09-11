/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Colour map. Map colours to positions on the screen for displaying 
 * text in colour (syntax highlighting).
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#ifndef CLRMAP_H
#define CLRMAP_H

#include <curses.h>
#include "../ds/matrix.h"
#include "../ds/str.h"
#include "../fbuf/fbuf.h"
#include "rule.h"

typedef struct colour_map {
	// Matrix of clrpair_t. Each cell stores the colour of the position on 
	// the curses window matching the row, column indices of the cell.
	matrix_t clrmap;
	WINDOW *win;
	// Whether the colour map should be used for colouring the screen.
	bool enabled;
} clrmap_t;

/*
 * Initialise a colour map for syntax highlighting.
 */
void clrmap_init(clrmap_t *c, WINDOW *w);

/*
 * Repaint the colour map with syntax highlighting for a file buffer.
 * Language is chosen from the file type identified by the file's extension.
 */
void clrmap_syntax_highlight(clrmap_t *c, fbuf_t *f);

/*
 * Free a colour map initialised with clrmap_init().
 */
void clrmap_free(clrmap_t *c);

#endif
