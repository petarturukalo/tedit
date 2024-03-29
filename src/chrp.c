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

/*
 * Get the number of times a character c appears in a string s.
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

void strncat_start(char *dest, uint max_len, strncat_data_t *sdata)
{
	*dest = '\0';
	sdata->cur_len = 0;
	sdata->dest = dest;
	sdata->max_len = max_len;
}

void strncat_cont(const char *src, strncat_data_t *sdata)
{
	int n = strlen(src);
	int remaining_bytes = sdata->max_len - sdata->cur_len;
	
	// Leave last remaining char as NULL term.
	if (remaining_bytes > 1 && n) {  
		if (n >= remaining_bytes) 
			n = remaining_bytes-1;  // Sub 1 for NULL term.
		strncat(sdata->dest, src, n);
		sdata->cur_len += n;
	}
}

void strncat_printf_cont(strncat_data_t *sdata, const char *fmt, ...)
{
	static char printbuf[128];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(printbuf, sizeof(printbuf), fmt, ap);
	strncat_cont(printbuf, sdata);
	va_end(ap);
}

char *strnchr(char *s, int c)
{
	for (; *s == c; ++s)
		;
	return *s ? s : NULL;
}

char *strnchr_reverse(char *s, int c)
{
	int i = strlen(s)-1;
	for (; i >= 0; --i) {
		if (s[i] != c)
			return s+i;
	}
	return NULL;
}

void strip_trailchar(char *s, int c)
{
	char *t = strnchr_reverse(s, c);
	if (t)
		*(t+1) = '\0';
	else
		// No different char found means the whole string is the character
		// that should be removed.
		*s = '\0';
}

bool strcmp_suffix(char *s, char *suffix)
{
	int i, j, n, m;

	n = strlen(s);
	m = strlen(suffix);
	
	if (m > n)
		return false;
	i = n-m;
	j = 0;
	for (; i < n && j < m; ++i, ++j) {
		if (s[i] != suffix[j])
			return false;
	}
	return j == m;
}

void strreplace(char *s, char c, char d)
{
	int n = strlen(s);

	for (int i = 0; i < n; ++i) {
		if (s[i] == c)
			s[i] = d;
	}
}
