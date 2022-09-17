/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Syntax highlighting rules using regex.
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#ifndef RULE_H
#define RULE_H

#include <regex.h>
#include "colour.h"
#include "../chrp.h"
#include "../ds/dlist.h"

// Number of file type extensions that a set of syntax rules can be applied to
// (minus 1 because the array should be NULL pointer terminated).
#define NFILE_EXT 16

/*
 * Data identifying a substring matched under a syntax element.
 * @start: start index (inclusive) of substring matched relative to original string
 * @end: end index (exclusive) of substring matched relative to original string
 * @clrpair: colour that the substring should be "highlighted"
 */
typedef struct regex_match_data {
	int start, end;
	clrpair_t clrpair;
} regmatch_data_t;

/*
 * A rule describing a syntax element and its associated colour.
 * @type: the type of syntax element, such as "keyword"
 * @clrpair: colour pair associated with this syntax element. Strings matching the pattern
 *	will be coloured this. See colour_pair for more info.
 * @cflags: extra cflags specific to this syntax rule passed to regcomp()
 * @regex_pattern: regex pattern describing all the strings that fall under
 *	this syntax element, e.g. for the "keyword" syntax element, matching
 *	strings could be loop constructs "for", "while", conditional "if", etc.
 * @compiled: whether the syntax rule was successfully compiled with call to regcomp()
 */
typedef struct syntax_rule {
	char *type;
	clrpair_t clrpair;
	int cflags;
	char *regex_pattern;
	regex_t regex;
	bool compiled;
} syntax_rule_t;

/*
 * Association between file types and the set of syntax rules used to highlight
 * its syntax.
 * @file_extensions: list of file type extensions the rules will be applied to,
 *	e.g. ".c", ".h". This should be NULL pointer terminated.
 */
typedef struct file_syntax_rules {
	syntax_rule_t *rules;
	char *file_extensions[NFILE_EXT];  
} file_syntax_rules_t;


/*
 * Compile the regex backing all syntax rules. A single call to this must be made before any subsequent
 * calls to exec_syntax_rules().
 */
void compile_syntax_rules(void);
/* Free the memory allocated from compile_syntax_rules(). */
void free_syntax_rules(void);


/*
 * Find the syntax rules for a file by checking its type, or NULL if there are no syntax rules 
 * supported for its file type.
 */
syntax_rule_t *find_syntax_rules(char *file);

/*
 * Process the text as per the syntax rules for a list of regmatch_data_t. 
 */
void exec_syntax_rules(char *text, syntax_rule_t *rules, dlist_t *out_matches);

#endif
