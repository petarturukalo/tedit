/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "tedit.h"

tedata_t t;  // Global text editor data.

/**
 * thread_input - Wrapper for input_start.
 */
void *thread_input(void *data)
{
	input_start((tedata_t *)data);
}

/**
 * thread_display - Wrapper for display_start.
 */
void *thread_display(void *data)
{
	display_start((tedata_t *)data);
}

/**
 * cleanup - Free the global text editor data.
 */
void cleanup(void)
{
	tedata_free(&t);
}

void tedit_start(char *fpaths[], int nfpaths)
{
	pthread_t tids[2];

	// Interrupt is ignored entirely and termination ignored while initialising
	// data in case it's called. Doing this in the first place feels like this sort
	// of signal guarding should be done everywhere random mallocs and such are done
	// and not just here, to avoid tiny memory leaks on exit. Or that other dynamically 
	// allocated memory could just become static instead.
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	tedata_init(&t, fpaths, nfpaths);

	atexit(cleanup);  // Clean up program on exit.
	signal(SIGTERM, sig_clean_exit);  // Calls exit to clean up.
	// Use own sigcont and sigtstp handlers for job control as the default curses
	// implementations jumble the screen on returning to the foreground by its use of
	// the doupdate function.
	signal(SIGTSTP, sig_handle_tstp);
	signal(SIGCONT, sig_handle_cont);

	// Below is expected to be exited by the user running a quit command or
	// receiving a kill/termination signal.
	pthread_create(&tids[0], NULL, thread_input, (void *)&t);
	pthread_create(&tids[1], NULL, thread_display, (void *)&t);

	pthread_join(tids[0], NULL);
	pthread_join(tids[1], NULL);
}

