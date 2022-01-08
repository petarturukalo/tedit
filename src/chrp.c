/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "chrp.h"

int chrp_find(char *s, char c, int start, int end)
{
	for (int i = start; i <= end; ++i) {
		if (s[i] == c)
			return i;
	}
	return -1;
}

int chrp_find_reverse(char *s, char c, int start, int end)
{
	for (int i = end; i >= start; --i) {
		if (s[i] == c)
			return i;
	}
	return -1;
}

char *chrpcpy_alloc(char *s)
{
	char *t;
	int n = strlen(s);

	t = malloc((n+1)*sizeof(char));

	if (!t)
		return NULL;

	strncpy(t, s, n);
	t[n] = '\0';

	return t;
}

/*
 * strcount - Get a count of characters in a null-terminated string
 */
static int strcount(char *s, int c)
{
	int cnt, n;

	cnt = 0;
	n = strlen(s);

	for (int i = 0; i < n; ++i) {
		if (s[i] == c)
			++cnt;
	}
	return cnt;
}

/*
 * strlenf - Get the length of a formatted string after substitution
 * 
 * Only handles string and int, and breaks on % characters substituted in.
 */
static int strlenf(char *fmt, va_list args)
{
	char *s;
	int i, n, nr, len = 0;

	// Remove length of chars that get substituted.
	len = strlen(fmt)-2*strcount(fmt, '%');
	n = strlen(fmt);

	// Go through and find all format substitutions, which start
	// at a % character.
	i = 0;
	while (i < n) {
		i = chrp_find(fmt, '%', i, n-1);

		if (i == -1)
			break;
		if (fmt[i+1] == 's') {
			s = va_arg(args, char *);
			len += strlen(s);
		} else if (fmt[i+1] == 'd') {
			nr = va_arg(args, int);
			len += ndigits(nr);
		}
		// Move passed formatter.
		i += 2;
	}
	return len;
}

char *chrpprintf(char *format, ...)
{
	va_list args;
	char *s;
	int len;

	va_start(args, format);
	len = strlenf(format, args);
	va_end(args);

	s = malloc((len+1)*sizeof(char));

	va_start(args, format);
	vsprintf(s, format, args);
	va_end(args);

	return s;
}

int chrp_nmatched(char *s, char c, int start, int end)
{
	int matched = 0;

	for (int i = start; i <= end; ++i) {
		if (s[i] == c)
			matched += 1;
		else
			break;
	}
	return matched;
}

int chrp_nmatched_reverse(char *s, char c, int start, int end)
{
	int matched = 0;

	for (int i = end; i >= start; --i) {
		if (s[i] == c)
			matched += 1;
		else
			break;
	}
	return matched;
}

