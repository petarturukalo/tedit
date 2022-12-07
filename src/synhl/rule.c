/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#include <stddef.h>
#include "rule.h"
#include "../misc.h"
#include "../log.h"

// Unsigned and long integer suffixes.
#define INT_SUF_PAT "([uU][lL]{0,2}|[lL]{0,2}[uU]?)"
#define HEX_PAT "[0-9A-Fa-f]"
// Decimal, binary, octal, and hex integers.
#define INT_PAT "(0|[1-9][0-9]*|0[bB][01]+|0[0-7]+|0[xX]" HEX_PAT "+)" INT_SUF_PAT
#define FLT_PAT "([0-9]+\\.[0-9]+|[0-9]+\\.[0-9]*|[0-9]*\\.[0-9]+)[fF]?"
#define BOOL_PAT "(true|false)"

static syntax_rule_t c_syntax_rules[] = {
	{ "keyword", COLOUR_YELLOW, 0, "\\b(break|case|continue|default|do|else|for|"
					   "goto|if|return|sizeof|switch|while)\\b" },
	// Type specifiers, qualifiers, etc.
	{ "type", COLOUR_GREEN, 0, "\\b(auto|bool|char|const|double|enum|extern|float|int|"
				       "long|register|short|signed|static|struct|typedef|"
				       "union|unsigned|void|volatile)\\b" },
	{ "preproc", COLOUR_MAGENTA, REG_NEWLINE, "^\\s*#\\s*(define|elif|else|endif|error|if|"
							     "ifdef|ifndef|include|pragma|undef)\\b" },
	{ "singleline comment", COLOUR_BLUE, REG_NEWLINE, "//.*$" },
	{ "multiline comment", COLOUR_BLUE, 0, "/\\*(\\*[^/]|[^\\*]/|[^\\*/])*\\*/" },
	{ "string", COLOUR_RED, REG_NEWLINE, "\"(\\\\.|[^\"])*\"" },
	{ "char", COLOUR_RED, 0, "'([^'?\\]|\\\\([abfnrtv\\'?0]|[0-3]?[0-7]{0,2}|x" HEX_PAT "{1,2}))'" },
	{ "number", COLOUR_RED, 0, "\\b(" INT_PAT "|" FLT_PAT "|" BOOL_PAT "|NULL)\\b" },
	{ 0 }
};

static file_syntax_rules_t all_file_syntax_rules[] = {
	{ c_syntax_rules, { ".c", ".h", NULL } }
};


/*
 * Run a function on all syntax rules in the "tree" rooted at all_file_syntax_rules.
 */
static void for_each_syntax_rule(void (*fn)(syntax_rule_t *))
{
	file_syntax_rules_t *frules = all_file_syntax_rules;
	int m, n = ARRAY_LEN(all_file_syntax_rules);
	syntax_rule_t *rule;

	for (int i = 0; i < n; ++i) {
		m = array_len((char *)frules[i].rules, sizeof(syntax_rule_t));
		rule = frules[i].rules;
		for (int j = 0; j < m; ++j, ++rule) 
			fn(rule);
	}
}

static void compile_syntax_rule(syntax_rule_t *rule) 
{
	int errcode = regcomp(&rule->regex, rule->regex_pattern, REG_EXTENDED|rule->cflags);

	rule->compiled = !errcode;

	if (errcode) {
		char errbuf[64];
		regerror(errcode, &rule->regex, errbuf, sizeof(errbuf));
		tlog("failed to compile regex %s, error code %d: %s", rule->type, errcode, errbuf);
	} 
}

void compile_syntax_rules(void)
{
	for_each_syntax_rule(compile_syntax_rule);
}

static void free_syntax_rule(syntax_rule_t *rule)
{
	regfree(&rule->regex);
}

void free_syntax_rules(void)
{
	for_each_syntax_rule(free_syntax_rule);
}


syntax_rule_t *find_syntax_rules(char *file)
{
	file_syntax_rules_t *frules = all_file_syntax_rules;
	int n = ARRAY_LEN(all_file_syntax_rules);
	char **ext;

	for (int i = 0; i < n; ++i) {
		ext = frules[i].file_extensions;
		for (; *ext; ++ext) {
			if (strcmp_suffix(file, *ext))
				return frules[i].rules;
		}
	}
	return NULL;
}

/*
 * For two pointers p and q where p < q, make on offset from q relative to p.
 */
static int ptr_relative_off(char *p, char *q, off_t off)
{
	return (q-p)+off;
}

static void exec_syntax_rule(char *text, syntax_rule_t *rule, dlist_t *out_matches)
{
	regmatch_t match;
	regmatch_data_t mdata;
	char *s = text;

	if (!rule->compiled)
		return;
	while (regexec(&rule->regex, s, 1, &match, 0) != REG_NOMATCH) {
		// The indices in match are relative to s. Make them relative to the original text.
		mdata.start = ptr_relative_off(text, s, match.rm_so);
		mdata.end = ptr_relative_off(text, s, match.rm_eo);
		mdata.clrpair = rule->clrpair;

		dlist_append(out_matches, &mdata);
		// Move to the char immediately after the substring just matched.
		s += match.rm_eo;
	}
}

void exec_syntax_rules(char *text, syntax_rule_t *rules, dlist_t *out_matches)
{
	int n = array_len((char *)rules, sizeof(syntax_rule_t));

	for (int i = 0; i < n; ++i)  
		exec_syntax_rule(text, rules+i, out_matches);
}
