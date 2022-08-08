/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "tedit.h"

tedata_t t = { 0 };  // Global text editor data.

/*
 * cleanup - Free the global text editor data.
 */
void cleanup(void)
{
	tedata_free(&t);
}

void tedit_start(char *fpaths[])
{
	pthread_t tids[2];

	// Interrupt is ignored entirely and termination ignored while initialising
	// data to pedantically avoid memory leaks in this one spot.
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (!tedata_init(&t, fpaths)) {
		fprintf(stderr, "failed to init text editor data\n");
		exit(EXIT_FAILURE);
	}

	atexit(cleanup);  // Clean up program on exit.
	signal(SIGTERM, sig_clean_exit);  // Calls exit to clean up.
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
}

