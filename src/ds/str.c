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

	s->s_len = 0;
	s->s_capacity = new_cap;
	s->s_buf = malloc(new_cap*sizeof(char));

	if (!s->s_buf) {
		free(s);
		return NULL;
	}

	return s;
}

str_t *str_ninit(char *str, int n)
{
	str_t *s;

	s = str_alloc(n);
	strncpy(s->s_buf, str, n);
	s->s_len = n;

	return s;
}

str_t *str_init(char *str)
{
	return str_ninit(str, strlen(str));
}

void str_free(str_t *s)
{
	free(s->s_buf);
	free(s);
}

/**
 * str_resize - Resize the underlying capacity of a string 
 *
 * Return 0 on success, -1 on error.
 */
int str_resize(str_t *s, int new_cap)
{
	void *new_buf = realloc(s->s_buf, new_cap*sizeof(void *));

	if (!new_buf)
		return -1;

	s->s_buf = new_buf;
	s->s_capacity = new_cap;

	return 0;
}

/**
 * try_grow - Try to grow a string dynamically
 *
 * Doubles its capacity if the string is at full capacity.
 */
void str_try_grow(str_t *s)
{
	if (s->s_len == s->s_capacity) {
		int new_cap = s->s_capacity<<1;

		if (str_resize(s, new_cap) == -1)
			fprintf(stderr, "str grow malloc\n");
	}
}

void str_insert_aux(str_t *s, char c, int index)
{
	for (int i = s->s_len; i > index; --i) 
		s->s_buf[i] = s->s_buf[i-1];
	s->s_buf[index] = c;
	++s->s_len;
}

void str_insert(str_t *s, char c, int index)
{
	str_try_grow(s);
	str_insert_aux(s, c, index);
}


void str_append(str_t *s, char c)
{
	str_try_grow(s);
	s->s_buf[s->s_len++] = c;
}


void str_delete_aux(str_t *s, int index)
{
	--s->s_len;

	for (int i = index; i < s->s_len; ++i)
		s->s_buf[i] = s->s_buf[i+1];
}

void str_try_shrink(str_t *s)
{
	int new_cap = round_up_pow2(s->s_len);
	
	if (new_cap >= STR_MIN_CAP && new_cap < s->s_capacity) {
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
	char c = s->s_buf[--s->s_len];
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
		dst->s_buf[j] = src->s_buf[i];
}

str_t *str_split(str_t *s, int index)
{
	str_t *t;
	int newlen = s->s_len-index;

	t = str_alloc(newlen);
	
	// Copy substring spanning from index to end of string to 
	// the new string.
	str_copy(s, index, s->s_len-1, t, 0, newlen-1);
	t->s_len = newlen;

	// Truncate source string.
	s->s_len = index;
	str_try_shrink(s);

	return t;
}

str_t *str_cat(str_t *dest, str_t *src)
{
	for (int i = 0; i < src->s_len; ++i)
		str_append(dest, src->s_buf[i]);
}

void str_set(str_t *s, char *t)
{
	int n;

	s->s_len = 0;
	str_try_shrink(s);

	if (t) {
		n = strlen(t);

		for (int i = 0; i < n; ++i)
			str_append(s, t[i]);
	}
}

str_t *str_cpy(str_t *s)
{
	return str_ninit(s->s_buf, s->s_len);
}
