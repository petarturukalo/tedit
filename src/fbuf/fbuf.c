#include "fbuf.h"

/*
 * Get whether two file buffers are equal.
 * Only uses unique ID for comparison.
 */
static bool fbuf_eq(fbuf_t *f, fbuf_t *g)
{
	return f->id == g->id;
}

void fbuf_reset_most(fbuf_t *f)
{
	f->filepath = NULL;
	cursor_reset(&f->cursor);
	f->unsaved_edit = false;
}

void fbuf_reset(fbuf_t *f)
{
	fbuf_reset_most(f);
	view_reset(&f->view);
}

void fbuf_init_most(fbuf_t *f, WINDOW *w, int tabsz, int id)
{
	fbuf_reset_most(f);
	view_init(&f->view, w, 0, 1, 0, 0);
	f->tabsz = tabsz;
	f->id = id;
}

bool fbuf_link(fbuf_t *f, char *fpath)
{
	f->filepath = strdup(fpath);
	return f->filepath;
}

void fbuf_unlink(fbuf_t *f)
{
	freecp(&f->filepath);
}

void fbuf_free(fbuf_t *f)
{
	fbuf_unlink(f);
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
