/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "fbuf.h"

int fbufs_len(fbufs_t *fs)
{
	return fs->len;
}

fbuf_t *fbufs_get(fbufs_t *fs, int i)
{
	return fs->array[i];
}

int fbufs_append(fbufs_t *fs, fbuf_t *f)
{
	dlist_append(fs, (void *)f);
}

/**
 * fbuf_match - Get whether two file buffer pointers cast to void pointer point to the 
 *	same file buffer
 *
 * For use in dlist_delete_elem.
 */
bool fbuf_match_vdata(void *f1, void *f2)
{
	return (fbuf_t *)f1 == (fbuf_t *)f2;
}

bool fbufs_delete_fbuf(fbufs_t *fs, fbuf_t *f)
{
	dlist_delete_elem(fs, (void *)f, fbuf_match_vdata);
}

