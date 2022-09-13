/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "fbinp.h"

/*
 * fbinp_left - Move the cursor of a file buffer left by one
 */
static void fbinp_left(fbuf_t *f)
{
	mv_left(&f->cursor, fbuf_cur_line(f));
}

/*
 * fbinp_down - Move the cursor of a file buffer down by one
 */
static void fbinp_down(fbuf_t *f)
{
	mv_down(&f->cursor, &f->lines);
}

/*
 * fbinp_up - Move the cursor of a file buffer up by one
 */
static void fbinp_up(fbuf_t *f)
{
	mv_up(&f->cursor, &f->lines);
}

/*
 * fbinp_right - Move the cursor of a file buffer right by one
 */
static void fbinp_right(fbuf_t *f)
{
	mv_right(&f->cursor, fbuf_cur_line(f));
}

/*
 * fbinp_home - Move the cursor of a file buffer to the start of its current line
 */
static void fbinp_home(fbuf_t *f)
{
	mv_start(&f->cursor);
}

/*
 * fbinp_end - Move the cursor of a file buffer to the end of its current line
 */
static void fbinp_end(fbuf_t *f)
{
	mv_end(&f->cursor, fbuf_cur_line(f));
}

/*
 * fbinp_pgup - Move the cursor of a file buffer up a page/view
 */
static void fbinp_pgup(fbuf_t *f)
{
	mv_view_pgup(&f->cursor, &f->lines, &f->view);
}

/*
 * fbinp_pgdn - Move the cursor of a file buffer down a page/view
 */
static void fbinp_pgdn(fbuf_t *f)
{
	mv_view_pgdn(&f->cursor, &f->lines, &f->view);
}

/*
 * fbinp_delete - Delete the current character under the cursor in a file buffer
 */
static void fbinp_delete(fbuf_t *f)
{
	f->unsaved_edit = true;

	if (lins_delete(fbuf_cur_line(f), fbuf_next_line(f), &f->cursor, f->tabsz))
		// Delete next line since merged with current (cursor stay still so still +1 for next).
		lines_delete(&f->lines, f->cursor.row+1);  
}

/*
 * fbinp_backspace - Backspace the character immediately before the cursor in a file buffer
 */
static void fbinp_backspace(fbuf_t *f)
{
	f->unsaved_edit = true;

	if (lins_backspace(fbuf_cur_line(f), fbuf_prev_line(f), &f->cursor, f->tabsz))
		// Delete current line since merged with previous (cursor moved up so +1 for "current").
		lines_delete(&f->lines, f->cursor.row+1);  
}

/*
 * fbinp_enter - Split the current line in two with the current column inclusive and onwards making the
 *	new line
 */
static void fbinp_enter(fbuf_t *f)
{
	line_t nl;
	lins_split(fbuf_cur_line(f), &f->cursor, f->tabsz, &nl);
	// Cursor got moved down by one so inserting on current line will insert the new line
	// after the line entered from.
	dlist_insert(&f->lines, f->cursor.row, &nl);
}

/*
 * fbinp_esc - Handle having pressed the escape key by switching to the echo line.
 */
static void fbinp_esc(bufs_t *b)
{
	bufs_active_buf_set_elbuf(b);
}

/*
 * fbinp_handle_seq_char - Handle having read a sequence-produced character
 */
static void fbinp_handle_seq_char(bufs_t *b, int c)
{
	fbuf_t *f = b->active_buf;

	switch (c) {
		case KEY_LEFT:	fbinp_left(f); break;
		case KEY_DOWN:	fbinp_down(f); break;  
		case KEY_UP:	fbinp_up(f); break;  
		case KEY_RIGHT:	fbinp_right(f); break;

		case KEY_BACKSPACE:	fbinp_backspace(f); break; 

		case KEY_DC:	fbinp_delete(f); break; 
		case KEY_HOME:	fbinp_home(f); break;
		case KEY_END:	fbinp_end(f); break;
		case KEY_PPAGE:	fbinp_pgup(f); break;
		case KEY_NPAGE: fbinp_pgdn(f); break;
	}
}

/*
 * fbinp_insert_char - Insert a regular character into a file buffer
 */
static void fbinp_insert_char(fbuf_t *f, char c)
{
	lins_insert_char(fbuf_cur_line(f), &f->cursor, c, f->tabsz);
}

/*
 * fbinp_handle_reg_char - Handle having read a regular character (not produced
 *	by a sequence)
 */
static void fbinp_handle_reg_char(bufs_t *b, int c)
{
	fbuf_t *f = b->active_buf;

	if (c == ASCII_ESC)
		fbinp_esc(b);
	else {
		f->unsaved_edit = true;

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

