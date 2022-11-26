#include "fbuf.h"
#include "../misc.h"

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

bool fbuf_linked(fbuf_t *f)
{
	return f->filepath;
}

void fbuf_unlink(fbuf_t *f)
{
	freecp(&f->filepath);
}

char *fbuf_link_name(fbuf_t *f)
{
	if (fbuf_linked(f))
		return f->filepath;
	return "unlinked";
}

void fbuf_free(fbuf_t *f)
{
	fbuf_unlink(f);
	lines_free(&f->lines);
}

void fbufs_free(fbufs_t *fs)
{
	dlist_free(fs, (dlist_elem_fn)fbuf_free);
}

bool fbufs_delete_fbuf(fbufs_t *fs, fbuf_t *f)
{
	return dlist_delete_elem(fs, f, (dlist_match_fn)fbuf_eq, (dlist_elem_fn)fbuf_free);
}

line_t *fbuf_prev_line(fbuf_t *f)
{
	int nr = f->cursor.row-1;

	if (nr < 0)
		return NULL;
	return dlist_get_address(&f->lines, nr);
}

line_t *fbuf_cur_line(fbuf_t *f)
{
	return dlist_get_address(&f->lines, f->cursor.row);
}

line_t *fbuf_next_line(fbuf_t *f)
{
	int nr = f->cursor.row+1;

	if (nr > f->lines.len-1)
		return NULL;
	return dlist_get_address(&f->lines, nr);
}

void fbuf_new(fbuf_t *f, WINDOW *w, int tabsz, int id)
{
	fbuf_init_most(f, w, tabsz, id);
	lines_alloc(&f->lines);
	// Add a single empty line which the user will start on.
	dlist_append_init(&f->lines, (dlist_elem_fn)line_alloc);
}

void fbuf_fork(fbuf_t *dest, fbuf_t *src, WINDOW *w, int id)
{
	fbuf_reset_most(dest);
	dest->id = id;
	dest->cursor = src->cursor;
	dest->filepath = src->filepath;
	lines_fork(&src->lines, &dest->lines);
	dest->tabsz = src->tabsz;
	dest->view = src->view;
}

/*
 * Open the file linked to a file buffer and return a file descriptor for it.
 * Assumes the file buffer has been linked to a file with call to fbuf_link().
 *
 * @flags: flags passed to open()
 */
int fbuf_openfd(fbuf_t *f, int flags)
{
	return open(f->filepath, flags, 0644);
}

bool fbuf_open(fbuf_t *f, char *fpath, WINDOW *w, int tabsz, int id)
{
	int fd; 
	bool read_success;

	fbuf_init_most(f, w, tabsz, id);
	fbuf_link(f, fpath);
	fd = fbuf_openfd(f, O_RDONLY);

	if (fd == -1) {
		// When there's no file on disk start an empty file buffer with it linked to
		// the filepath for the next write to create the underlying file.
		if (errno == ENOENT) {
			lines_alloc_empty(&f->lines);
			return true;
		}
		fbuf_unlink(f);
		return false;
	}
	read_success = lines_from_file(&f->lines, fd, tabsz);
	close(fd);

	if (!read_success) 
		fbuf_free(f);
	return read_success;
}

int fbuf_write(fbuf_t *f)
{
	int fd, bytes;

	if (fbuf_linked(f)) {
		fd = fbuf_openfd(f, O_WRONLY|O_CREAT);
		
		if (fd != -1) {
			bytes = lines_write(&f->lines, f->tabsz, fd);

			if (bytes != -1)
				f->unsaved_edit = false;
			close(fd);
			return bytes;
		}
	}
	return -1;
}

