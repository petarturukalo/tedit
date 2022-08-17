/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Wrapper for reading a single character from stdin. 
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef GETCH_H
#define GETCH_H

#include <curses.h>
#include <string.h>
#include <stdlib.h>

#define ASCII_ENTER 10
#define ASCII_ESC 27	// Escape.
#define ASCII_BS 127	// Backspace.

/*
 * mygetch - Read a single character from stdin with the stdscr window
 *
 * Also returns escape sequence produced keys as a single character.
 * Use this over getch().
 */
int mygetch(void);

#endif
