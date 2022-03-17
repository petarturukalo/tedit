/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "fcmd.h"

/*
 * fcmd_write_handle - Handle writing to the currently active file buffer to its
 *	linked file
 */
void fcmd_write_handle(bufs_t *b)
{
	int bytes;

	if ((bytes = bufs_write(b)) == -1) 
		snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "can't write to file%s", 
			 errno == EACCES ? ": permission denied" : "");
	else
		snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "wrote %d bytes", bytes);
}

/*
 * fcmd_write_handle_other - Handle writing the currently active file buffer to a new file
 */
void fcmd_write_handle_other(bufs_t *b, char *fpath, WINDOW *w)
{
	int bytes, tabsz = b->active_fbuf->tabsz;

	if ((bytes = bufs_write_other(b, fpath, w, tabsz)) == -1)  
		snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "can't write to file '%s'%s", fpath, 
			 errno == EACCES ? ": permission denied" : "");
	else
		snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "wrote %d bytes to file '%s'", bytes, fpath);
}

void fcmd_write_handle_new(bufs_t *b, char *fpath)
{
	int bytes = bufs_link_write(b, fpath);

	if (bytes == -1)
		snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "can't write to file '%s'%s", fpath, 
			 errno == EACCES ? ": permission denied" : "");
	else
		snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "wrote %d bytes to file '%s'", bytes, fpath);
}

/*
 * fcmd_write_handler - Handle a write operation
 */
void fcmd_write_handler(char *fpath, bufs_t *b, WINDOW *w)
{
	if (b->active_fbuf->filepath) {
		if (fpath) 
			fcmd_write_handle_other(b, fpath, w);
		else
			fcmd_write_handle(b);
	} else {
		// Buffer is unnamed, so it must have a name to continue.
		if (fpath)
			fcmd_write_handle_new(b, fpath);
		else
			strcpy(b->cmd_ostr, "buf not linked to file: need filepath");
	}
}

/*
 * fcmd_close_handler - Handle closing the currently active file buffer
 *
 * Only closes it if there are no unsaved changes.
 */
void fcmd_close_handler(char *s, bufs_t *b, WINDOW *w)
{
	fbuf_t *f = b->active_fbuf;

	if (f) {
		if (f->unsaved_edit)
			strcpy(b->cmd_ostr, "need to write before close");
		else
			bufs_close(b, w);
	}
}

/*
 * fcmd_fclose_handler - Force close the currently active file buffer, 
 *	losing any unsaved changes
 */
void fcmd_fclose_handler(char *s, bufs_t *b, WINDOW *w)
{
	if (b->active_fbuf)
		bufs_close(b, w);
}

/*
 * fcmd_open_handler - Handle opening of a new file and making it the active file buffer
 */
void fcmd_open_handler(char *fpath, bufs_t *b, WINDOW *w)
{
	int tabsz = b->active_fbuf->tabsz;

	if (!fpath) 
		bufs_new(b, w, tabsz);
	else if (bufs_open(b, fpath, w, tabsz) == -1) 
		snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "can't open file '%s'%s", fpath, 
			 errno == EACCES ? ": permission denied" : "");
}

/*
 * fcmd_edit_handler - Handle editing of an existing file already opened in a file buffer
 */
void fcmd_edit_handler(char *fpath, bufs_t *b, WINDOW *w)
{
	if (fpath) {
		if (bufs_edit(b, fpath) == -1)
			snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "file '%s' not open", fpath);
	} else
		strcpy(b->cmd_ostr, "no filepath");
}

/*
 * fcmd_jump_handler - Handle jumping to buffer with an id
 * @sid: id as string
 */
void fcmd_jump_handler(char *sid, bufs_t *b, WINDOW *w)
{
	int id;

	if (sid) {
		id = atoi(sid);

		if (id > 0 && bufs_jump(b, id) != -1) 
			return;
		snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "buf id %s not in available bufs", sid);
	} else
		snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "no buf id given");
}

cmd_t fcmd_write = { "w", "write", fcmd_write_handler };
cmd_t fcmd_close = { "c", "close", fcmd_close_handler };
cmd_t fcmd_fclose = { "fc", "fclose", fcmd_fclose_handler };
cmd_t fcmd_open = { "o", "open", fcmd_open_handler };
cmd_t fcmd_edit = { "e", "edit", fcmd_edit_handler };
cmd_t fcmd_jump = { "j", "jump", fcmd_jump_handler };

