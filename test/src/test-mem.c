#include "../../src/tedata.h"

void test_fbuf(void)
{
	fbuf_t *f = fbuf_open("data/tags", NULL, 8, 1);
	fbuf_free(f);
}

void test_lines(void)
{
	lines_t *ls = lines_from_file("data/tags", 8);
	lines_free(ls);
}

/**
 * test_tedata - Test tedata for memory leaks
 *
 * Results of running this function under valgrind might not be that helpful 
 * because of curses showing still reachable memory.
 */
void test_tedata(void)
{
	tedata_t t;
	char *fpaths[] = { "data/0bytes.txt", "data/story.txt", "data/foo.txt", "data/tags" };

	tedata_init(&t, fpaths, 4);

	tedata_free(&t);
}

int main(void)
{
	// These tests use the list and string data structures so
	// they're also implicitly tested.
	test_lines();
	test_fbuf();
	/*test_tedata();*/
	
	return 0;
}
