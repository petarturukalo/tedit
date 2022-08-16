#include "test-ds.h"

static void dlist_init_int(dlist_t *d)
{
	dlist_init(d, DLIST_MIN_CAP, sizeof(int));
}

static void dlist_init_int_array(dlist_t *d, int *a, int n)
{
	dlist_init_array(d, DLIST_MIN_CAP, sizeof(int), a, n);
}

/*
 * Assert that the elements in a dynamic list match the elements in an int array.
 * @len: length of the int array
 */
static void assert_dlist_eq_int_array(dlist_t *d, int *a, int len)
{
	int n;

	assert(d->len == len);
	for (int i = 0; i < d->len; ++i) {
		dlist_get(d, i, &n);
		assert(n == a[i]);
	}
}

static void test_dlist_get(void)
{
	dlist_t d;
	int n;

	dlist_init_int_array(&d, (int[]){ 1, 2, 3 }, 3);

	for (int i = 0; i < 3; ++i) {
		dlist_get(&d, i, &n);
		assert(n == i+1);
	}
	dlist_free(&d, NULL);
}

/*
 * Test dynamic list growing and shrinking when adding/removing elements.
 */
static void test_dlist_resize(void)
{
	dlist_t d;
	int n;

	dlist_init_int(&d);
	n = 5;

	for (int i = 0; i < DLIST_MIN_CAP; ++i) {
		dlist_append(&d, &i);
		assert(d.len == i+1);
		assert(d.capacity == DLIST_MIN_CAP);
	}

	assert(d.len == DLIST_MIN_CAP);
	assert(d.capacity == DLIST_MIN_CAP);
	dlist_append(&d, &n);
	assert(d.len == DLIST_MIN_CAP+1);
	assert(d.capacity == 2*DLIST_MIN_CAP);

	for (int i = 0; i < DLIST_MIN_CAP-1; ++i)
		dlist_append(&d, &i);

	assert(d.len == 2*DLIST_MIN_CAP);
	assert(d.capacity == 2*DLIST_MIN_CAP);
	dlist_append(&d, &n);
	assert(d.len == 2*DLIST_MIN_CAP+1);
	assert(d.capacity == 4*DLIST_MIN_CAP);

	dlist_pop(&d, NULL);
	assert(d.len == 2*DLIST_MIN_CAP);
	assert(d.capacity == 2*DLIST_MIN_CAP);

	for (int i = 0; i < DLIST_MIN_CAP-1; ++i)
		dlist_pop(&d, NULL);

	assert(d.len == DLIST_MIN_CAP+1);
	assert(d.capacity == 2*DLIST_MIN_CAP);
	dlist_pop(&d, NULL);
	assert(d.len == DLIST_MIN_CAP);
	assert(d.capacity == DLIST_MIN_CAP);

	dlist_free(&d, NULL);
}

/*
 * Test appending and popping elements.
 */
static void test_dlist_append_pop(void)
{
	dlist_t d;
	int n;

	dlist_init_int(&d);

	for (int i = 0; i <= 10000; ++i) {
		dlist_append(&d, &i);
		dlist_get(&d, i, &n);
		assert(n == i);
		assert(d.len == i+1);
	}
	for (int i = 10000; i >= 0; --i) {
		dlist_pop(&d, &n);
		assert(n == i);
		assert(d.len == i);
	}
	dlist_free(&d, NULL);
}

/*
 * Test setting an element. Also tests [get_]byte_address.
 */
static void test_dlist_set(void)
{
	dlist_t d;
	int n, m;

	dlist_init_int(&d);

	for (int i = 0; i < 3; ++i) {
		dlist_append(&d, &i);
		n = 10*i;
		dlist_set(&d, i, &n);
		dlist_get(&d, i, &m);
		assert(n == m);
	}
	dlist_free(&d, NULL);
}

/*
 * Test inserting at an index and deleting from an index.
 */
static void test_dlist_insert_delete(void)
{
	dlist_t d;
	int *n, m;

	dlist_init_int(&d);

	for (int i = 0; i < 8; ++i) 
		dlist_append(&d, &i);
	for (int i = 0; i < 9; ++i) 
		dlist_insert(&d, 0, &i);

	assert(d.len == DLIST_MIN_CAP+1);
	assert(d.capacity == 2*DLIST_MIN_CAP);

	assert_dlist_eq_int_array(&d, (int[]){ 8, 7, 6, 5, 4, 3, 2, 1, 0, 
					       0, 1, 2, 3, 4, 5, 6, 7 }, 17);

	// Insert and delete off end.
	m = 5;
	dlist_insert(&d, d.len, &m);
	n = dlist_get_address(&d, d.len-1);
	assert(*n == 5);
	dlist_delete_ind(&d, d.len-1, NULL);
	// Delete off end again to halve the capacity.
	dlist_delete_ind(&d, d.len-1, NULL);
	assert(d.len == DLIST_MIN_CAP);
	assert(d.capacity == DLIST_MIN_CAP);

	dlist_free(&d, NULL);
}

static void test_dlist_delete_elem(void)
{
	dlist_t d;
	int n;

	dlist_init_int_array(&d, (int[]){ 1, 2, 3 }, 3);
	n = 2;
	dlist_delete_elem(&d, &n, NULL, NULL);
	assert_dlist_eq_int_array(&d, (int[]){ 1, 3 }, 2);
}

/*
 * @a: array used to make dynamic list out of and array that gets split
 * @i: index to split array at
 * @n: length of the array being split
 * @left: left side result of the split, exludes the element at the split index
 * @right: right side result of the split, includes the element at the split index
 * @llen: length of left
 * @rlen: length of right
 */
static void assert_dlist_split(int *a, int i, int n, int *left, int *right, int llen, int rlen)
{
	dlist_t in, out;

	dlist_init_int_array(&in, a, n);

	dlist_split(&in, i, &out);
	assert_dlist_eq_int_array(&in, left, llen);
	assert_dlist_eq_int_array(&out, right, rlen);

	dlist_free(&in, NULL);
	dlist_free(&out, NULL);
}

static void test_dlist_split(void)
{
	assert_dlist_split((int[]){ 1, 2 }, 1, 2, (int[]){ 1 }, (int[]){ 2 }, 1, 1);
	assert_dlist_split((int[]){ 1, 2 }, 0, 2, NULL, (int[]){ 1, 2 }, 0, 2);
	assert_dlist_split((int[]){ 1, 2, 3, 4, 5, 6, 7, 8, 9 }, 3, 9, 
			   (int[]){ 1, 2, 3 }, (int[]){ 4, 5, 6, 7, 8, 9 }, 3, 6);
}

static void assert_dlist_cat(int *dest, int n, int *src, int m, int *result, int rlen)
{
	dlist_t d, s;

	dlist_init_int_array(&d, dest, n);
	dlist_init_int_array(&s, src, m);

	dlist_cat(&d, &s);
	assert_dlist_eq_int_array(&d, result, rlen);

	dlist_free(&d, NULL);
	dlist_free(&s, NULL);
}

static void test_dlist_cat(void)
{
	assert_dlist_cat(NULL, 0, (int[]){ 1, 2 }, 2, (int[]){ 1, 2 }, 2);
	assert_dlist_cat((int[]){ 1, 2 }, 2, (int[]){ 3, 4 }, 2, (int[]){ 1, 2, 3, 4 }, 4);
}

static void test_dlist_copy(void)
{
	dlist_t d, s;

	dlist_init_int_array(&d, (int[]){ 1, 2, 3 }, 3);
	dlist_init_int_array(&s, (int[]){ 3, 4, 5, 6 }, 4);
	dlist_copy(&d, &s, NULL);

	assert_dlist_eq_int_array(&d, (int[]){ 3, 4, 5, 6 }, 4);

	dlist_free(&d, NULL);
	dlist_free(&s, NULL);
}

static void test_dlist_copy_new(void)
{
	dlist_t in, out;

	dlist_init_int_array(&in, (int[]){ 1, 2, 3 }, 3);
	dlist_copy_new(&in, &out);
	assert_dlist_eq_int_array(&out, (int[]){ 1, 2, 3 }, 3);

	dlist_free(&in, NULL);
	dlist_free(&out, NULL);

}

static void test_dlist_copy_array(void)
{
	dlist_t d;

	dlist_init_int_array(&d, (int[]){ 4, 3, 2, 1 }, 4);
	dlist_copy_array(&d, (int[]){ 1, 2, 3 }, 3, NULL);
	assert_dlist_eq_int_array(&d, (int[]){ 1, 2, 3 }, 3);

	dlist_free(&d, NULL);
}

/*
 * Test the dynamic list data structure.
 */
static void test_dlist(void)
{
	test_dlist_get();
	test_dlist_resize();
	test_dlist_append_pop();
	test_dlist_set();
	test_dlist_insert_delete();
	test_dlist_delete_elem();
	test_dlist_split();
	test_dlist_cat();
	test_dlist_copy();
	test_dlist_copy_new();
	test_dlist_copy_array();
}

void test_ds(void)
{
	test_dlist();
}

