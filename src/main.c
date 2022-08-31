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
#include "input.h"
#include "tab.h"
#include "fbuf/fbuf.h"
#include "fbuf/fbio.h"

static tedata_t t = { 0 };  // Global text editor data.

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

	// Interrupt is ignored entirely and termination ignored while initialising
	// data to pedantically avoid memory leaks in this one spot.
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (!tedata_init(&t, argv+1)) {
		fprintf(stderr, "failed to init text editor data\n");
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
