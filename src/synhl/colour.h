/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Screen colours.
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#ifndef COLOUR_H
#define COLOUR_H

#include <curses.h>

/*
 * Identifier for a foreground, background colour pair usable by COLOR_PAIR(). 
 * Only use these pairs after call to init_pairs().
 *
 * Because the syntax highlighting implementation only needs to change foreground colours, 
 * the background colour is kept at a default across all enum values: the colour in the name
 * of the enum value identifies the foreground colour.
 */
enum colour_pair {
	// Colour pairs must start at 1.
	COLOUR_DEFAULT = 1,
	COLOUR_BLACK,
	COLOUR_RED,
	COLOUR_GREEN,
	COLOUR_YELLOW,
	COLOUR_BLUE,
	COLOUR_MAGENTA,
	COLOUR_CYAN,
	COLOUR_WHITE
};

/*
 * Byte size identifier of enum colour_pair. Only exists so that an enum value can 
 * be stored in a byte rather than a larger data type.
 */
typedef char clrpair_t;

/*
 * Init all colour pairs in enum colour_pair so that they are usable.
 */
void init_pairs(void);

#endif
