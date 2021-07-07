/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "input.h"

void input_start(tedata_t *t)
{
	int c;
	fbuf_t *f;

	for (;;) {
		c = mygetch();

		sem_wait(&t->te_sem);

		// Have the character affect the buffer and then synchronise the view of the buffer
		// before handing back over to the display to show the current view of the buffer.
		f = t->te_bufs.b_active_buf;
		
		if (f == &t->te_bufs.b_elbuf)
			elinp_handle_char(&t->te_bufs, c, t->te_cmds, t->te_win);
		else 
			fbinp_handle_char(&t->te_bufs, c);
		view_sync_cursor(&f->fb_view, &f->fb_cursor, f->fb_lines);

		sem_post(&t->te_sem);
	}
}

