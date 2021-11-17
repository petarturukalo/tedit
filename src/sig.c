/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "sig.h"

void sig_clean_exit(int sig)
{
	exit(EXIT_SUCCESS);
}

void sig_handle_tstp(int sig)
{
	// Prepare for returning back to shell.
	reset_shell_mode();
	endwin();
	
	// Put program in background.
	signal(SIGTSTP, SIG_DFL);
	kill(getpid(), SIGTSTP); 
	// (Execution continues in the SIGCONT handler on return to foreground.)
}

void sig_handle_cont(int sig)
{
	// Return back to program.
	// Setup curses again since if this isn't called an escape sequence key such as 
	// an arrow key is interpreted as the escape key if it's the first key pressed back
	// on returning to the program.
	setup_curses();
	reset_prog_mode();
	flushinp();
	signal(SIGTSTP, sig_handle_tstp);
}
