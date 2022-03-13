#include "fbuf.h"

/*
 * Get whether two file buffers are equal.
 * Only uses unique ID for comparison.
 */
static bool fbuf_eq(fbuf_t *f, fbuf_t *g)
{
	return f->id == g->id;
}

void fbuf_free(fbuf_t *f)
{
	if (f->filepath)
		free(f->filepath);
	lines_free(&f->lines);
}

void fbufs_free(fbufs_t *fs)
{
	dlist_free(fs, (void (*)(void *))fbuf_free);
}

bool fbufs_delete_fbuf(fbufs_t *fs, fbuf_t *f)
{
	return dlist_delete_elem(fs, f, (bool (*)(void *, void *))fbuf_eq, 
				 (void (*)(void *))fbuf_free);
}
