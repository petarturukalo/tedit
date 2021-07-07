#include "test-ds.h"

#define BUFSZ 64

/**
 * test_dlist - Test the dynamic list data structure (also implicitly
 *	tests the static list data structure)
 *
 * Mostly just tests the growing and shrinking of a list.
 */
static void test_dlist(void)
{
	int i, buf[BUFSZ];
	dlist_t *l = dlist_init(0);

	// Fill buffer with dummy values.
	for (i = 0; i < BUFSZ; ++i)
		buf[i] = i;

	assert(l->sl_capacity == DLIST_MIN_CAP);

	for (i = 0; i < 16; ++i)
		dlist_append(l, (void *)(buf+i));

	assert(l->sl_capacity == DLIST_MIN_CAP);
	dlist_append(l, (void *)(buf+i++));
	assert(l->sl_capacity == 2*DLIST_MIN_CAP);

	for (; i < 32; ++i)
		dlist_insert(l, i, (void *)(buf+i));  // Insert instead of append.

	assert(l->sl_capacity == 2*DLIST_MIN_CAP);
	dlist_append(l, (void *)(buf+i++));
	assert(l->sl_capacity == 4*DLIST_MIN_CAP);

	for (; i < BUFSZ; ++i)
		dlist_append(l, (void *)(buf+i));
	
	assert(l->sl_capacity == 4*DLIST_MIN_CAP);
	dlist_append(l, (void *)(buf+i++));
	assert(l->sl_capacity == 8*DLIST_MIN_CAP);

	dlist_delete_ind(l, 0);
	assert(l->sl_capacity == 4*DLIST_MIN_CAP);

	for (i = 0; i < 31; ++i)
		dlist_delete_ind(l, 0);

	assert(l->sl_capacity == 4*DLIST_MIN_CAP);
	dlist_delete_ind(l, 0);
	assert(l->sl_capacity == 2*DLIST_MIN_CAP);

	for (i = 0; i < 15; ++i)
		dlist_delete_ind(l, 0);

	assert(l->sl_capacity == 2*DLIST_MIN_CAP);
	dlist_delete_ind(l, 0);
	assert(l->sl_capacity == DLIST_MIN_CAP);

	for (i = 0; i < 16; ++i) {
		dlist_delete_ind(l, 0);
		assert(l->sl_capacity == DLIST_MIN_CAP);
	}

	slist_free(l, NULL);
}

void test_ds_entry(void)
{
	test_dlist();
	// (String data structure not tested since it's implemented
	// the same as a dynamic list.)
}

