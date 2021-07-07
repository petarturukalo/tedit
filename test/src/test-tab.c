#include "test-tab.h"

#define TEST_TABSZ 4

void assert_tab_dist(int i, int sln_dist)
{
	assert(dist_to_next_tabstop(i, TEST_TABSZ) == sln_dist);
}

void test_tab_dist(void)
{
	assert_tab_dist(3, 1);

	assert_tab_dist(4, 4);
	assert_tab_dist(5, 3);
	assert_tab_dist(6, 2);
	assert_tab_dist(7, 1);

	assert_tab_dist(8, 4);

	// ...

	assert_tab_dist(121, 3);
}

void test_tab_entry(void)
{
	test_tab_dist();
}

