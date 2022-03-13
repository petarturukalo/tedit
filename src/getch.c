/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "getch.h"

// Maximum number of characters in an escape sequence that will require
// fallback handling.
#define ESC_SEQ_MAX_LEN 4
#define ESC_SEQ_BUF_LEN 16

// VTE (gnome terminal) escape sequences.
#define VTE_HOME "\33[1~"
#define VTE_END  "\33[4~"

int mygetch(void)
{
	char s[ESC_SEQ_BUF_LEN];
	int i, c, d;
	
	nodelay(stdscr, false);

	i = 0;
	// Zero-init in case a garbage character is the same as a target escape sequence.
	bzero(s, ESC_SEQ_BUF_LEN);

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
		if (strncmp(s, VTE_HOME, i) == 0)
			return KEY_HOME;
		if (strncmp(s, VTE_END, i) == 0)
			return KEY_END;
	}
	return c;
}
