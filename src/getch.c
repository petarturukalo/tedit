/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "getch.h"
/*
 * TODO backspace doesn't work in the xterm terminal emulator,
 * but home and end do when using xterm terminal emulator with
 * a xterm TERM env
 */

// Maximum number of characters in the escape sequences currently
// being handled.
static const int ESC_SEQ_MAX_LEN = 4;

// Escape sequences produced instead of a single char under some terminals
// that have the TERM environment variable set to xterm[-256color].
static const char *XTERM_HOME = "\33[1~";
static const char *XTERM_END = "\33[4~";

/*
 * Get whether the TERM environment variable is xterm[-256color].
 */
static bool term_is_xterm(void)
{
	char *term = getenv("TERM");
	return term && strncmp(term, "xterm", 5) == 0;
}

/*
 * Fallback handle the user pressing a key which produced an escape sequence (multiple characters)
 * by converting it into a single char. Return a single char.
 */
static int mygetch_fallback(void)
{
	char s[16];
	int i, n, c, d;
	
	nodelay(stdscr, false);

	i = 0;
	n = sizeof(s);
	bzero(s, n);
	// Hang on to int version of char since might not be ASCII and would lose
	// its actual value by overflow if only used char version stored in buffer.
	c = getch();  // Block for a character.
	s[i++] = c;  
	
	nodelay(stdscr, true);

	// Nonblockingly get characters to fallback handle escape
	// sequenced characters that curses doesn't handle properly.
	while (i < n && (d = getch()) != ERR) 
		s[i++] = d;
	if (i == ESC_SEQ_MAX_LEN) {
		if (strncmp(s, XTERM_HOME, i) == 0)
			return KEY_HOME;
		if (strncmp(s, XTERM_END, i) == 0)
			return KEY_END;
	}
	return c;
}

int mygetch(void)
{
	static int fallback = -1;
	
	if (fallback == -1)
		fallback = term_is_xterm();
	if (fallback)
		return mygetch_fallback();
	return getch();
}
