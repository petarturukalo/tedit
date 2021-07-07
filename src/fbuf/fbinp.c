/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "fbinp.h"

/**
 * fbinp_left - Move the cursor of a file buffer left by one
 */
void fbinp_left(fbuf_t *f)
{
	mv_left(&f->fb_cursor, fbaux_cur_line(f));
}

/**
 * fbinp_down - Move the cursor of a file buffer down by one
 */
void fbinp_down(fbuf_t *f)
{
	mv_down(&f->fb_cursor, f->fb_lines);
}

/**
 * fbinp_up - Move the cursor of a file buffer up by one
 */
void fbinp_up(fbuf_t *f)
{
	mv_up(&f->fb_cursor, f->fb_lines);
}

/**
 * fbinp_right - Move the cursor of a file buffer right by one
 */
void fbinp_right(fbuf_t *f)
{
	mv_right(&f->fb_cursor, fbaux_cur_line(f));
}

/**
 * fbinp_home - Move the cursor of a file buffer to the start of its current line
 */
void fbinp_home(fbuf_t *f)
{
	mv_start(&f->fb_cursor);
}

/**
 * fbinp_end - Move the cursor of a file buffer to the end of its current line
 */
void fbinp_end(fbuf_t *f)
{
	mv_end(&f->fb_cursor, fbaux_cur_line(f));
}

/**
 * fbinp_pgup - Move the cursor of a file buffer up a page/view
 */
void fbinp_pgup(fbuf_t *f)
{
	mv_view_pgup(&f->fb_cursor, f->fb_lines, &f->fb_view);
}

/**
 * fbinp_pgdn - Move the cursor of a file buffer down a page/view
 */
void fbinp_pgdn(fbuf_t *f)
{
	mv_view_pgdn(&f->fb_cursor, f->fb_lines, &f->fb_view);
}

/**
 * fbinp_delete - Delete the current character under the cursor in a file buffer
 */
void fbinp_delete(fbuf_t *f)
{
	f->fb_unsaved_edit = true;

	if (lins_delete(fbaux_cur_line(f), fbaux_next_line(f), &f->fb_cursor, f->fb_tabsz))
		// Delete next line since merged with current (cursor stay still so still +1 for next).
		lines_delete(f->fb_lines, fbaux_row(f)+1);  
}

/**
 * fbinp_backspace - Backspace the character immediately before the cursor in a file buffer
 */
void fbinp_backspace(fbuf_t *f)
{
	if (lins_backspace(fbaux_cur_line(f), fbaux_prev_line(f), &f->fb_cursor, f->fb_tabsz))
		// Delete current line since merged with previous (cursor moved up so +1 for "current").
		lines_delete(f->fb_lines, fbaux_row(f)+1);  
}

/**
 * fbinp_enter - Split the current line in two with the current column inclusive and onwards making the
 *	new line
 */
void fbinp_enter(fbuf_t *f)
{
	line_t *nl = lins_split(fbaux_cur_line(f), &f->fb_cursor, f->fb_tabsz);
	// Cursor got moved down by one so inserting on new current line will insert the new line
	// after the line entered from.
	lines_insert(f->fb_lines, fbaux_row(f), nl);
}

/**
 * fbinp_esc - Handle having pressed the escape key by switching to the echo line.
 */
void fbinp_esc(bufs_t *b)
{
	bufs_active_buf_set_elbuf(b);
}

/**
 * fbinp_handle_seq_char - Handle having read a sequence-produced character
 */
void fbinp_handle_seq_char(bufs_t *b, int c)
{
	fbuf_t *f = b->b_active_buf;

	switch (c) {
		case KEY_LEFT:	fbinp_left(f); break;
		case KEY_DOWN:	fbinp_down(f); break;  
		case KEY_UP:	fbinp_up(f); break;  
		case KEY_RIGHT:	fbinp_right(f); break;

		case KEY_DC:	fbinp_delete(f); break; 
		case KEY_HOME:	fbinp_home(f); break;
		case KEY_END:	fbinp_end(f); break;
		case KEY_PPAGE:	fbinp_pgup(f); break;
		case KEY_NPAGE: fbinp_pgdn(f); break;
	}
}

/**
 * fbinp_insert_char - Insert a regular character into a file buffer
 */
void fbinp_insert_char(fbuf_t *f, char c)
{
	lins_insert_char(fbaux_cur_line(f), &f->fb_cursor, c, f->fb_tabsz);
}

/**
 * fbinp_handle_reg_char - Handle having read a regular character (not produced
 *	by a sequence)
 */
void fbinp_handle_reg_char(bufs_t *b, int c)
{
	fbuf_t *f = b->b_active_buf;

	if (c == ASCII_ESC)
		fbinp_esc(b);
	else {
		f->fb_unsaved_edit = true;

		if (c == ASCII_BS)
			fbinp_backspace(f);
		else if (c == ASCII_ENTER)
			fbinp_enter(f);
		else 
			fbinp_insert_char(f, c);
	}
}

void fbinp_handle_char(bufs_t *b, int c)
{
	if (c >= KEY_MIN)
		fbinp_handle_seq_char(b, c);
	else
		fbinp_handle_reg_char(b, c);
}

