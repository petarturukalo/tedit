/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "elinp.h"

/**
 * elinp_esc - Handle having pressed the escape character from an echo line buffer
 *	by swapping back to the most recent file buffer
 */
void elinp_esc(bufs_t *b)
{
	bufs_active_buf_set_fbuf(b);
	elbuf_set(&b->elbuf, "");
}

/**
 * elinp_enter - Handle the enter key having been pressed in an echo line buffer
 *
 * Tries to parse and run a command.
 */
void elinp_enter(bufs_t *b, cmds_t *cs, WINDOW *w)
{
	char *line_str, *echo_str;
	elbuf_t *e = &b->elbuf;

	line_str = elbuf_str(e);
	echo_str = cmds_parse(line_str, cs, b, w);

	elbuf_set(e, echo_str);

	// Jump back to file buffer if no string is echoed back.
	if ((echo_str && strlen(echo_str) == 0) || !echo_str)
		elinp_esc(b);

	free(echo_str);
	free(line_str);
}

/**
 * elinp_handle_seq_char - Handle having pressed a key character produced by a sequence
 */
void elinp_handle_seq_char(bufs_t *b, int c)
{
	switch (c) {
		// Restrict the set of available sequence keys: don't include any vertical movement keys
		// such as up, down, page up and page down.
		case KEY_LEFT:
		case KEY_RIGHT:
		case KEY_DL:
		case KEY_HOME: 
		case KEY_END:  
			fbinp_handle_char(b, c);
			break;
	}
}

/**
 * elinp_handle_reg_char - Handle having pressed a key not produced by a sequence 
 *	(a regular character)
 * @c: character from pressed key
 */
void elinp_handle_reg_char(bufs_t *b, int c, cmds_t *cs, WINDOW *w)
{
	// Only handle regular characters that aren't tab along with enter to run a command,
	// and escape to switch back to the file buffer.
	if (c == ASCII_ENTER)
		elinp_enter(b, cs, w);
	else if (c == ASCII_ESC)
		elinp_esc(b);
	else if (c != '\t')
		fbinp_handle_char(b, c);
}

void elinp_handle_char(bufs_t *b, int c, cmds_t *cs, WINDOW *w)
{
	if (c >= KEY_MIN)
		elinp_handle_seq_char(b, c);
	else 
		elinp_handle_reg_char(b, c, cs, w);
}

