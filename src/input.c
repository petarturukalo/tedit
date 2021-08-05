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

		sem_wait(&t->sem);

		// Have the character affect the buffer and then synchronise the view of the buffer
		// before handing back over to the display to show the current view of the buffer.
		f = t->bufs.active_buf;
		
		if (f == &t->bufs.elbuf)
			elinp_handle_char(&t->bufs, c, t->cmds, t->win);
		else 
			fbinp_handle_char(&t->bufs, c);
		view_sync_cursor(&f->view, &f->cursor, f->lines);

		sem_post(&t->sem);
	}
}

