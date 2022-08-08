/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "tedata.h"

void setup_curses(void)
{
	cbreak();
	noecho();
	keypad(stdscr, true);
	set_escdelay(20);  // (Milliseconds.)
}

bool tedata_init(tedata_t *t, char *fpaths[])
{
	if (sem_init(&t->sem, 0, 1) == -1) {
		fprintf(stderr, "failed to init semaphore\n");
		return false;
	}

	// The default init screen window is only used to get characters with getch. This is
	// because a call to wgetch refreshes the window it's being called with, and getch
	// refreshes the stdscr window (the window from initscr), which makes the display glitch
	// out if the display window is the same as the get character input window.
	if (!initscr()) {
		fprintf(stderr, "failed to init curses\n");
		sem_destroy(&t->sem);
		return false;
	}
	refresh();  // Refresh once so that the first call to getch doesn't refresh.
	t->win = newwin(0, 0, 0, 0);  // Display window (separate to get character stdscr window).

	if (!t->win) {
		fprintf(stderr, "failed to create new curses window\n");
		sem_destroy(&t->sem);
		endwin();
		return false;
	}

	setup_curses();
	bufs_init(&t->bufs, t->win, fpaths);
	cmds_init(&t->cmds);

	return true;
}

void tedata_free(tedata_t *t)
{
	delwin(t->win);
	endwin();
	sem_destroy(&t->sem);
	bufs_free(&t->bufs);
	cmds_free(&t->cmds);
}

