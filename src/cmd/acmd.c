/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "acmd.h"

/**
 *lslen - Get the length of the result of a ls command over a list of file buffers
 */
int lslen(fbufs_t *fs)
{
	fbuf_t *f;
	int len = 0;

	for (int i = 0; i < fbufs_len(fs); ++i) {
		f = fbufs_get(fs, i);

		if (f->fb_filepath)
			len += strlen(f->fb_filepath);
		else
			len += strlen("unnamed");

		// 2 for single quotes wrapping filepath, 1 for intermediary space, 
		// 2 for square brackets around file buffer id, and 2 for trailing spaces.
		len += 7;  
		len += ndigits(f->fb_id);
	}
	// Remove last 2 spaces, add 1 for asterisk identifier of active buf.
	len -= 1;
	return len;
}

/**
 * lsstr - Get a string of a list of the open files in the file buffer
 */
char *lsstr(fbufs_t *fs, fbuf_t *active_fbuf)
{
	fbuf_t *f;
	char *start, *s;
	int n = lslen(fs);

	start = malloc((n+1)*sizeof(char));
	s = start;

	for (int i = 0; i < fbufs_len(fs); ++i) {
		f = fbufs_get(fs, i);

		s += sprintf(s, "'%s' [%d", f->fb_filepath ? f->fb_filepath : "unnamed", f->fb_id);

		if (active_fbuf == f)
			s += sprintf(s, "*");
		s += sprintf(s, "]");

		if (i < fbufs_len(fs)-1)
			s += sprintf(s, ", ");
	}
	start[n] = '\0';
	return start;
}

/**
 * acmd_list_handler - Where to run a command from
 */
char *acmd_list_handler(char *s, bufs_t *b, WINDOW *w)
{
	return lsstr(b->b_fbufs, b->b_active_fbuf);
}

/**
 * acmd_quit_handler - Handle the quit command by attempting to quit the program,
 *	and cleaning up before doing so
 */
char *acmd_quit_handler(char *s, bufs_t *b, WINDOW *w)
{
	fbuf_t *f = b->b_active_fbuf;

	if (f->fb_unsaved_edit)
		return chrpcpy_alloc("need to write before quit");
	exit(EXIT_SUCCESS);
}

/**
 * acmd_fquit_handler - Handle the force quit command by quitting the program and
 *	cleaning up before doing so
 */
char *acmd_fquit_handler(char *s, bufs_t *b, WINDOW *w)
{
	exit(EXIT_SUCCESS);
}

cmd_t acmd_list = { "ls", "list", acmd_list_handler };
cmd_t acmd_quit = { "q", "quit", acmd_quit_handler };
cmd_t acmd_fquit = { "fq", "fquit", acmd_fquit_handler };

