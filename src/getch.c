/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "getch.h"

// Maximum number of characters in an escape sequence.
static const int ESC_SEQ_MAX_LEN = 4;

// VTE (gnome terminal) escape sequences.
static char *VTE_HOME = "\33[1~";
static char *VTE_END = "\33[4~";

// TODO backspace doesn't work in xterm
int mygetch(void)
{
	char s[16];
	int i, c, d;
	
	nodelay(stdscr, false);

	i = 0;
	bzero(s, sizeof(s));
	// Hang on to int version of char since might not be ASCII and would lose
	// its actual value by overflow if only used char version stored in buffer.
	c = getch();  // Block for a character.
	s[i++] = c;  
	
	nodelay(stdscr, true);

	// Nonblockingly get characters to fallback handle escape
	// sequenced characters that curses doesn't handle properly.
	while ((d = getch()) != -1) 
		s[i++] = d;
	if (i == ESC_SEQ_MAX_LEN) {
		// This fixed the home and end keys when using a tty terminal (not pseudo terminal).
		if (strncmp(s, VTE_HOME, i) == 0)
			return KEY_HOME;
		if (strncmp(s, VTE_END, i) == 0)
			return KEY_END;
	}
	return c;
}
