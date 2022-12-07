/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include "misc.h"

/*
 * Get whether a memory area is all zeroes.
 * @sz: size of the memory area
 */
static bool zeroed(char *a, size_t sz)
{
	for (int i = 0; i < sz; ++i) {
		if (a[i] != 0)
			return false;
	}
	return true;
}

int array_len(char *a, size_t eltsz)
{
	int len = 0;

	while (!zeroed(a, eltsz)) {
		++len;
		a += eltsz;
	}
	return len;
}

void freep(void **ptr)
{
	if (*ptr) {
		free(*ptr);
		*ptr = NULL;
	}
}

void freecp(char **ptr)
{
	return freep((void **)ptr);
}

bool file_exists(const char *fpath)
{
	struct stat s;
	return !(stat(fpath, &s) == -1 && errno == ENOENT);
}
