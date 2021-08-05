/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "display.h"

// Refresh rates in micro seconds.
#define REFRESH_RATE_60_HZ_USEC 16667
#define REFRESH_RATE_120_HZ_USEC 8333 
#define REFRESH_RATE_MAX_USEC 100

// Start and end range (both endpoints inclusive) for
// ASCII characters that can be safely printed.
#define ASCII_PRINT_START_INCL 32
#define ASCII_PRINT_END_INCL  127

/**
 * ascii_printable - Get whether a character is an ASCII printable character
 */
static bool ascii_printable(char c)
{
	return c >= ASCII_PRINT_START_INCL && c <= ASCII_PRINT_END_INCL;
}

/**
 * addnstrtab - Similar to curses addnstr but displays tab characters as specified in
 *	tab.h as spaces
 * @s: string to display
 * @start: index of first character in string to display (inclusive)
 * @end: index of last character in string to display (inclusive)
 */
void addtabsubstr(char *s, int start, int end, bool last_line, WINDOW *w)
{
	int i = start;

	for (; i <= end; ++i)  {
		if (s[i] == TAB_START || s[i] == TAB_CONT)
			waddch(w, ' ');
		else if (s[i] == '\n' || ascii_printable(s[i]))
			waddch(w, s[i]);
		else
			waddch(w, '@');  // Non-printable characters.
	}
}

/**
 * display_fbuf_line - Display the view of a line in the file buffer
 * @l: line to display
 * @i: index row in view (first displayed line has this 0, second has 1, etc., by 0-indexing)
 * @first_col: first column in the line to display
 * @view_width: width of the view
 * @view_disp_top_row: row of top left anchor point of view on window
 * @view_disp_top_col: column of top left anchor point of view on window
 */
void display_fbuf_line(line_t *l, int i, int first_col, int view_width, 
		       int view_disp_top_row, int view_disp_first_col, bool last_line, WINDOW *w)
{
	int end_col = first_col+view_width-1 > line_len_nl(l)-1 ? line_len_nl(l)-1 : first_col+view_width-1;

	wmove(w, view_disp_top_row+i, view_disp_first_col);

	addtabsubstr(l->buf, first_col, end_col, last_line, w);
}

/*
 * display_fbuf_lines - Display the view of a file buffer's lines
 */
void display_fbuf_lines(fbuf_t *f, WINDOW *w)
{
	// Row and column positions over the file buffer's lines (all inclusive).
	int top_row, bot_row, first_col, end_col;
	int i, vheight, vwidth, view_disp_top_row, view_disp_first_col;
	line_t *l;
	view_t *v = &f->view;

	top_row = v->lines_top_row;
	first_col = v->lines_first_col;
	vheight = view_height(v);
	vwidth = view_width(v);

	if (top_row+vheight-1 >= fbaux_nlines(f))
		bot_row = fbaux_nlines(f)-1;
	else
		bot_row = top_row+vheight-1;

	view_disp_top_row = view_display_top_row(v);
	view_disp_first_col = view_display_first_col(v);

	// Move cursor to display anchor point (top left corner of its displayed view), the
	// point to start displaying at.
	wmove(w, view_disp_top_row, view_disp_first_col);

	i = 0;
	for (int lnr = top_row; lnr <= bot_row; ++lnr, ++i) 
		display_fbuf_line(fbaux_line(f, lnr), i, first_col, vwidth, 
				  view_disp_top_row, view_disp_first_col, lnr == bot_row, w);
}

/**
 * display_fbuf_cursor - Display the file buffer cursor in view on the curses standard screen
 */
void display_fbuf_cursor(fbuf_t *f, WINDOW *w)
{
	view_t *v;
	cursor_t *c = &f->cursor;

	v = &f->view;

	wmove(w, view_cursor_display_row(v, c), view_cursor_display_col(v, c));
}

/**
 * display_buffers - Display all file buffers to the curses standard screen
 */
void display_buffers(bufs_t *b, WINDOW *w)
{
	werase(w);
	// Show current file buffer being edited along with echo line buffer where user enters commands.
	display_fbuf_lines(b->active_fbuf, w);
	display_fbuf_lines(&b->elbuf, w);
	// Active buffer might be the echo line buffer and only want to display one cursor.
	display_fbuf_cursor(b->active_buf, w);
	wrefresh(w);
}

/**
 * display_text_editor - Display the text editor to the curses standard screen
 */
void display_text_editor(tedata_t *t)
{
	display_buffers(&t->bufs, t->win);
}

void display_start(tedata_t *t)
{
	for (;;) {
		sem_wait(&t->sem);
		display_text_editor(t);
		sem_post(&t->sem);

		usleep(REFRESH_RATE_MAX_USEC);
	}
}

