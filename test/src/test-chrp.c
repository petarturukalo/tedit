#include <stdlib.h>
#include "test-chrp.h"

static void assert_strnchr(char *s, int c, int expected_offset)
{
	assert(strnchr(s, c) == s+expected_offset);
}

static void test_strnchr(void)
{
	assert(strnchr("", 'a') == NULL);
	assert(strnchr("a", 'a') == NULL);
	assert_strnchr("a", 'b', 0);
	assert_strnchr("abc", 'c', 0);
	assert_strnchr("cca", 'c', 2);
	assert_strnchr("aaaabbbbbdddddd", 'c', 0);
	assert_strnchr("aaabbccdddd", 'a', 3);
}

static void assert_strnchr_reverse(char *s, int c, int expected_offset)
{
	assert(strnchr_reverse(s, c) == s+expected_offset);
}

static void test_strnchr_reverse(void)
{
	assert(strnchr_reverse("", 'a') == NULL);
	assert(strnchr_reverse("a", 'a') == NULL);
	assert_strnchr_reverse("b", 'a', 0);
	assert_strnchr_reverse("abc", 'a', 2);
	assert_strnchr_reverse("baaa", 'a', 0);
	assert_strnchr_reverse("bbbaaaaaaa", 'a', 2);
	assert(strnchr_reverse("aaaaaaa", 'a') == NULL);
}

static void assert_strip_trailchar(char *s, int c, char *expected_str)
{
	char *t = strdup(s);
	strip_trailchar(t, c);
	assert(strcmp(t, expected_str) == 0);
	free(t);
}

static void test_strip_trailchar(void)
{
	assert_strip_trailchar("", ' ', "");
	assert_strip_trailchar(" ", ' ', "");
	assert_strip_trailchar("a", ' ', "a");
	assert_strip_trailchar("a ", ' ', "a");
	assert_strip_trailchar("aaaa   ", ' ', "aaaa");
	assert_strip_trailchar("  aaa    ", ' ', "  aaa");
	assert_strip_trailchar(" a b ", 'b', " a b ");
	assert_strip_trailchar(" a bb", 'b', " a ");
}

void test_chrp(void)
{
	test_strnchr();
	test_strnchr_reverse();
	test_strip_trailchar();
}
