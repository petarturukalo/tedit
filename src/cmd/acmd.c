/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "acmd.h"

/*
 *lslen - Get the length of the result of a ls command over a list of file buffers
 */
int lslen(fbufs_t *fs)
{
	fbuf_t *f;
	int len = 0;

	for (int i = 0; i < fs->len; ++i) {
		f = dlist_get_address(fs, i);

		if (f->filepath)
			len += strlen(f->filepath);
		else
			len += strlen("unnamed");

		// 2 for single quotes wrapping filepath, 1 for intermediary space, 
		// 2 for square brackets around file buffer id, and 2 for trailing spaces.
		len += 7;  
		len += ndigits(f->id);
	}
	// Remove last 2 spaces, add 1 for asterisk identifier of active buf.
	len -= 1;
	return len;
}

/*
 * lsstr - Get a string of a list of the open files in the file buffers
 */
char *lsstr(fbufs_t *fs, fbuf_t *active_fbuf)
{
	fbuf_t *f;
	char *start, *s;
	int n = lslen(fs);

	start = malloc((n+1)*sizeof(char));
	s = start;

	for (int i = 0; i < fs->len; ++i) {
		f = dlist_get_address(fs, i);

		s += sprintf(s, "'%s' [%d", f->filepath ? f->filepath : "unnamed", f->id);

		if (active_fbuf == f)
			s += sprintf(s, "*");
		s += sprintf(s, "]");

		if (i < fs->len-1)
			s += sprintf(s, ", ");
	}
	start[n] = '\0';
	return start;
}

void acmd_list_handler(char *s, bufs_t *b, WINDOW *w)
{
	char *t = lsstr(&b->fbufs, b->active_fbuf);
	snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), "%s", t);
	free(t);
}

/*
 * acmd_quit_handler - Handle the quit command by attempting to quit the program,
 *	and cleaning up before doing so
 */
void acmd_quit_handler(char *s, bufs_t *b, WINDOW *w)
{
	fbuf_t *f;

	for (int i = 0; i < b->fbufs.len; ++i) {
		f = dlist_get_address(&b->fbufs, i);
		if (f->unsaved_edit) {
			snprintf(b->cmd_ostr, sizeof(b->cmd_ostr), 
				 "unsaved edit in buf '%s' [%d]; "
				 "need to write/save before quit", 
				 f->filepath ? f->filepath : "unnamed", f->id);
			return;
		}
	}
	exit(EXIT_SUCCESS);
}

/*
 * acmd_fquit_handler - Handle the force quit command by quitting the program and
 *	cleaning up before doing so
 */
void acmd_fquit_handler(char *s, bufs_t *b, WINDOW *w)
{
	exit(EXIT_SUCCESS);
}

cmd_t acmd_list = { "ls", "list", acmd_list_handler };
cmd_t acmd_quit = { "q", "quit", acmd_quit_handler };
cmd_t acmd_fquit = { "fq", "fquit", acmd_fquit_handler };

