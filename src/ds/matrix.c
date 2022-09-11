/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#include "matrix.h"
#include "../log.h"

bool matrix_init(matrix_t *m, int nrows, int ncols, size_t eltsz)
{
	if (nrows >= 0 && ncols >= 0 && eltsz > 0) {
		dlist_t row;

		m->nrows = nrows;
		m->ncols = ncols;
		m->eltsz = eltsz;

		dlist_init(&m->rows, nrows, sizeof(dlist_t));
		for (int i = 0; i < nrows; ++i) {
			dlist_init(&row, ncols, eltsz);
			for (int j = 0; j < ncols; ++j)
				dlist_append(&row, NULL);
			dlist_append(&m->rows, &row);
		}
		return true;
	} 
	return false;
}

static void matrix_init_row(dlist_t *row, matrix_t *m)
{
	dlist_init(row, m->ncols, m->eltsz);
}

bool matrix_resz(matrix_t *m, int new_nrows, int new_ncols)
{
	// Don't resize if a dimension hasn't changed.
	if (m->nrows == new_nrows && m->ncols == new_ncols)
		return false;
	if (new_nrows >= 0 && new_ncols >= 0) {
		dlist_t *row;

		// Need to set matrix colunm amount before any calls to matrix_init_row() so that newly added
		// rows are filled with the correct amount of columns.
		m->nrows = new_nrows;
		m->ncols = new_ncols;

		dlist_resize_len_init_data(&m->rows, new_nrows, (dlist_elem_data_fn)matrix_init_row, m);
		for (int i = 0; i < new_nrows; ++i) {
			row = dlist_get_address(&m->rows, i);
			dlist_resize_len(row, new_ncols);
		}
		return true;
	}
	return false;
}

/*
 * Get whether a row, column position is in the matrix (not out of bounds).
 */
static bool pos_in_matrix(matrix_t *m, int row, int col)
{
	return (row >= 0 && row < m->nrows) && (col >= 0 && col < m->ncols);
}

void *matrix_get_address(matrix_t *m, int row, int col)
{
	if (pos_in_matrix(m, row, col)) {
		dlist_t *r = dlist_get_address(&m->rows, row);
		return dlist_get_address(r, col);
	}
	return NULL;
}

bool matrix_set(matrix_t *m, int row, int col, void *elem)
{
	void *adrs = matrix_get_address(m, row, col);
	if (adrs)
		memcpy(adrs, elem, m->eltsz);
	return adrs;
}

void matrix_memset(matrix_t *m, unsigned char c)
{
	dlist_t *r;

	for (int i = 0; i < m->nrows; ++i) {
		r = dlist_get_address(&m->rows, i);
		dlist_memset(r, c);
	}
}

void matrix_free(matrix_t *m)
{
	dlist_t *row;

	for (int i = 0; i < m->nrows; ++i) {
		row = dlist_get_address(&m->rows, i);
		dlist_free(row, NULL);
	}
	dlist_free(&m->rows, NULL);
}

