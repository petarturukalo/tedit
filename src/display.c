/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include <unistd.h>
#include "display.h"
#include "log.h"

// Start and end range (both endpoints inclusive) for
// ASCII characters that can be safely printed.
static const int ASCII_PRINT_START_INCL = 32;
static const int ASCII_PRINT_END_INCL = 127;

/*
 * ascii_printable - Get whether a character is an ASCII printable character
 */
static bool ascii_printable(char c)
{
	return c >= ASCII_PRINT_START_INCL && c <= ASCII_PRINT_END_INCL;
}

/*
 * addnstrtab - Similar to curses addnstr but displays tab characters as specified in
 *	tab.h as spaces
 * @s: string to display
 * @start: index of first character in string to display (inclusive)
 * @end: index of last character in string to display (inclusive)
 */
void addtabsubstr(char *s, int start, int end, WINDOW *w)
{
	int i = start;

	for (; i <= end; ++i) {
		if (s[i] == TAB_START || s[i] == TAB_CONT)
			waddch(w, ' ');
		else if (s[i] == '\n' || ascii_printable(s[i]))
			waddch(w, s[i]);
		else
			waddch(w, '@');  // Non-printable characters.
	}
}

/*
 * display_fbuf_lines - Display the view of a file buffer's lines
 */
void display_fbuf_lines(fbuf_t *f, WINDOW *w)
{
	int top_row, bot_row, first_col, end_col;
	int i, view_disp_top_row, view_disp_first_col;
	line_t *l;
	view_t *v = &f->view;

	top_row = v->lines_top_row;
	first_col = v->lines_first_col;
	bot_row = view_lines_bot_row(v, &f->lines);

	view_disp_top_row = view_display_top_row(v);
	view_disp_first_col = view_display_first_col(v);

	// Move cursor to display anchor point (top left corner of its displayed view), the
	// point to start displaying at.
	wmove(w, view_disp_top_row, view_disp_first_col);

	i = 0;
	for (int lnr = top_row; lnr <= bot_row; ++lnr, ++i)  {
		l = dlist_get_address(&f->lines, lnr);
		end_col = view_line_last_col(v, l);
		wmove(w, view_disp_top_row+i, view_disp_first_col);  // Move to start of line.
		addtabsubstr(l->array, first_col, end_col, w);  // Display whole line.
	}
}

/*
 * display_fbuf_cursor - Display the file buffer cursor in view on the curses standard screen
 */
void display_fbuf_cursor(fbuf_t *f, WINDOW *w)
{
	view_t *v;
	cursor_t *c;

	v = &f->view;
	c = &f->cursor;

	wmove(w, view_cursor_display_row(v, c), view_cursor_display_col(v, c));
}

/*
 * Set the colours in a window to the colours in a colour map.
 */
static void display_clrmap(clrmap_t *c, WINDOW *w)
{
	int nrows = c->clrmap.nrows;
	int ncols = c->clrmap.ncols;
	clrpair_t *clrpair;

	for (int i = 0; i < nrows; ++i) {
		for (int j = 0; j < ncols; ++j) {
			clrpair = matrix_get_address(&c->clrmap, i, j);
			mvwchgat(w, i, j, 1, 0, *clrpair, NULL);
		}
	}
}

/*
 * Display syntax highlighting for a file buffer to the curses standard screen.
 * Assumes the file buffer has already been displayed to the screen.
 */
static void display_syntax_highlighting(clrmap_t *c, fbuf_t *f, WINDOW *w)
{
	if (has_colors()) {
		if (clrmap_syntax_highlight(c, f))
			display_clrmap(c, w);
	}
}

void display_text_editor(tedata_t *t)
{
	bufs_t *b = &t->bufs;
	WINDOW *w = t->win;

	werase(w);
	// Show current file buffer being edited along with echo line buffer where user enters commands.
	display_fbuf_lines(b->active_fbuf, w);
	display_fbuf_lines(&b->elbuf, w);
	display_syntax_highlighting(&t->clrmap, b->active_fbuf, w);
	// Active buffer might be the echo line buffer and only want to display one cursor.
	display_fbuf_cursor(b->active_buf, w);
	wrefresh(w);
}
