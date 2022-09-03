/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Auxiliary commands.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "cmd.h"

/*
 * lsstr - Get a string of a list of the open files in the file buffers
 * @sdata: out-param string to build list of files in
 */
void lsstr(fbufs_t *fs, fbuf_t *active_fbuf, strncat_data_t *sdata)
{
	fbuf_t *f;

	for (int i = 0; i < fs->len; ++i) {
		f = dlist_get_address(fs, i);

		strncat_printf_cont(sdata, "'%s' [%d", f->filepath ? f->filepath : "unnamed", f->id);

		if (active_fbuf == f)
			strncat_cont("*", sdata);
		strncat_cont("]", sdata);

		if (i < fs->len-1)
			strncat_cont(", ", sdata);
	}
}

void acmd_list_handler(char *s, bufs_t *b, WINDOW *w)
{
	strncat_data_t sdata;
	strncat_start(b->cmd_ostr, sizeof(b->cmd_ostr), &sdata);
	lsstr(&b->fbufs, b->active_fbuf, &sdata);
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

