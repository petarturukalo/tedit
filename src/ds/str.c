/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "str.h"

str_t *str_alloc(int capacity)
{
	int new_cap;
	str_t *s = malloc(sizeof(str_t));

	if (!s)
		return NULL;

	new_cap = round_up_pow2(capacity);

	if (new_cap < STR_MIN_CAP)
		new_cap = STR_MIN_CAP;

	s->len = 0;
	s->capacity = new_cap;
	s->buf = malloc(new_cap*sizeof(char));

	if (!s->buf) {
		free(s);
		return NULL;
	}

	return s;
}

str_t *str_ninit(char *str, int n)
{
	str_t *s;

	s = str_alloc(n);
	strncpy(s->buf, str, n);
	s->len = n;

	return s;
}

str_t *str_init(char *str)
{
	return str_ninit(str, strlen(str));
}

void str_free(str_t *s)
{
	free(s->buf);
	free(s);
}

/**
 * str_resize - Resize the underlying capacity of a string 
 *
 * Return 0 on success, -1 on error.
 */
int str_resize(str_t *s, int new_cap)
{
	void *new_buf = realloc(s->buf, new_cap*sizeof(void *));

	if (!new_buf)
		return -1;

	s->buf = new_buf;
	s->capacity = new_cap;

	return 0;
}

/**
 * try_grow - Try to grow a string dynamically
 *
 * Doubles its capacity if the string is at full capacity.
 */
void str_try_grow(str_t *s)
{
	if (s->len == s->capacity) {
		int new_cap = s->capacity<<1;

		if (str_resize(s, new_cap) == -1)
			fprintf(stderr, "str grow malloc\n");
	}
}

void str_insert_aux(str_t *s, char c, int index)
{
	for (int i = s->len; i > index; --i) 
		s->buf[i] = s->buf[i-1];
	s->buf[index] = c;
	++s->len;
}

void str_insert(str_t *s, char c, int index)
{
	str_try_grow(s);
	str_insert_aux(s, c, index);
}


void str_append(str_t *s, char c)
{
	str_try_grow(s);
	s->buf[s->len++] = c;
}


void str_delete_aux(str_t *s, int index)
{
	--s->len;

	for (int i = index; i < s->len; ++i)
		s->buf[i] = s->buf[i+1];
}

void str_try_shrink(str_t *s)
{
	int new_cap = round_up_pow2(s->len);
	
	if (new_cap >= STR_MIN_CAP && new_cap < s->capacity) {
		if (str_resize(s, new_cap) == -1)
			fprintf(stderr, "str shrink malloc\n");
	}
}

void str_delete(str_t *s, int index)
{
	str_delete_aux(s, index);
	str_try_shrink(s);
}

char str_pop(str_t *s)
{
	char c = s->buf[--s->len];
	str_try_shrink(s);
	return c;
}

/**
 * str_copy - Copy a part of a string to another part of a different string
 * @src: string to copy from
 * @sstart: index to start copying from
 * @send: index to end copying from (inclusive)
 * @dstart: index to start copying to
 * @dend: index to end copying to (inclusive)
 *
 * Assumes the length between sstart and send and dstart and dend is the same.
 * Doesn't change destination length.
 */
void str_copy(str_t *src, int sstart, int send, str_t *dst, int dstart, int dend)
{
	int i = sstart;
	int j = dstart;

	for (; i <= send && j <= dend; ++i, ++j)
		dst->buf[j] = src->buf[i];
}

str_t *str_split(str_t *s, int index)
{
	str_t *t;
	int newlen = s->len-index;

	t = str_alloc(newlen);
	
	// Copy substring spanning from index to end of string to 
	// the new string.
	str_copy(s, index, s->len-1, t, 0, newlen-1);
	t->len = newlen;

	// Truncate source string.
	s->len = index;
	str_try_shrink(s);

	return t;
}

str_t *str_cat(str_t *dest, str_t *src)
{
	for (int i = 0; i < src->len; ++i)
		str_append(dest, src->buf[i]);
}

void str_set(str_t *s, char *t)
{
	int n;

	s->len = 0;
	str_try_shrink(s);

	if (t) {
		n = strlen(t);

		for (int i = 0; i < n; ++i)
			str_append(s, t[i]);
	}
}

str_t *str_cpy(str_t *s)
{
	return str_ninit(s->buf, s->len);
}
