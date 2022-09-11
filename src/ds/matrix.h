/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Matrix/table data structure, a 2D array of elements.
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#ifndef MATRIX_H
#define MATRIX_H

#include "dlist.h"

typedef struct matrix {
	dlist_t rows;
	int nrows, ncols;  // Current number of rows and columns.
	size_t eltsz;  // Size of an element in bytes.
} matrix_t;

/*
 * Initialise a nrows*ncols matrix where each element is of size eltsz in bytes.
 * Return whether initialisation was successful.
 */
bool matrix_init(matrix_t *m, int nrows, int ncols, size_t eltsz);

/*
 * Resize an existing matrix to a new number of rows (height) and columns (width).
 * Elements that are truncated as of height/width being decreased are lost.
 *
 * Return whether resizing was successful.
 */
bool matrix_resz(matrix_t *m, int new_nrows, int new_ncols);

/*
 * Get the address of the element at a row, column position in the matrix (0-indexed), 
 * or NULL for no such element, i.e. the position is out of bounds of the matrix.
 */
void *matrix_get_address(matrix_t *m, int row, int col);

/*
 * Copy elem to the element at a row, column position in the matrix (0-indexed).
 * Return whether the element was set.
 */
bool matrix_set(matrix_t *m, int row, int col, void *elem);
/*
 * Fill the allocated memory area of a matrix with a constant byte c. 
 */
void matrix_memset(matrix_t *m, unsigned char c);

/*
 * Free a matrix initialised with matrix_init(). Elements are lost after freeing.
 */
void matrix_free(matrix_t *m);

#endif
