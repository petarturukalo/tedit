/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include <pthread.h>
#include <signal.h>
#include "tedata.h"
#include "sig.h"
#include "display.h"
#include "getch.h"
#include "fbuf/fbinp.h"
#include "fbuf/elinp.h"
#include "log.h"

static tedata_t t = { 0 };  // Global text editor data.

/*
 * Start the user input main loop. Each loop a key is read from the user
 * and that key handled by having it affect the current file buffer.
 */
static void input_start(tedata_t *t)
{
	int c;
	fbuf_t *f;

	for (;;) {
		c = mygetch();

		sem_wait(&t->sem);

		f = t->bufs.active_buf;
		if (f == t->bufs.active_fbuf)
			fbinp_handle_char(&t->bufs, c);
		else 
			elinp_handle_char(&t->bufs, c, &t->cmds, t->win);
		view_sync_cursor(&f->view, &f->cursor, &f->lines);

		sem_post(&t->sem);
	}
}

static void display_start(tedata_t *t)
{
	for (;;) {
		sem_wait(&t->sem);
		display_text_editor(t);
		sem_post(&t->sem);

		usleep(REFRESH_RATE_USE_USEC);
	}
}

/*
 * cleanup - Free the global text editor data.
 */
void cleanup(void)
{
	tedata_free(&t);
}

int main(int argc, char *argv[])
{
	pthread_t tids[2];

	// Interrupt is ignored entirely and termination ignored while initialising data.
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (!tedata_init(&t, argv+1)) {
		tlog("failed to init text editor data");
		exit(EXIT_FAILURE);
	}

	atexit(cleanup);
	signal(SIGTERM, sig_clean_exit);
	// Use own sigcont and sigtstp handlers for job control as the default curses
	// implementations jumble the screen on returning to the foreground by its use of
	// the doupdate function.
	signal(SIGTSTP, sig_handle_tstp);
	signal(SIGCONT, sig_handle_cont);

	// Below is expected to be exited by the user running a quit command or
	// receiving a kill/termination signal.
	pthread_create(&tids[0], NULL, (void *(*)(void *))input_start, (void *)&t);
	pthread_create(&tids[1], NULL, (void *(*)(void *))display_start, (void *)&t);

	pthread_join(tids[0], NULL);
	pthread_join(tids[1], NULL);
	return 0;
}
