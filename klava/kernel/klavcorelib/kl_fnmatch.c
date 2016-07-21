// kl_fnmatch.c
//

#include <klava/kl_fnmatch.h>

/*
 * robust glob pattern matcher
 * ozan s. yigit/dec 1994
 * public domain
 *
 * glob patterns:
 *	*	matches zero or more characters
 *	?	matches any single character
 *	[set]	matches any character in the set
 *	[^set]	matches any character NOT in the set
 *		where a set is a group of characters or ranges. a range
 *		is written as two characters seperated with a hyphen: a-z denotes
 *		all characters between a to z inclusive.
 *	[-set]	set matches a literal hypen and any character in the set
 *	[]set]	matches a literal close bracket and any character in the set
 *
 *	char	matches itself except where char is '*' or '?' or '['
 *	\char	matches char, including any pattern character
 *
 * examples:
 *	a*c		ac abc abbc ...
 *	a?c		acc abc aXc ...
 *	a[a-z]c		aac abc acc ...
 *	a[-a-z]c	a-c aac abc ...
 *
 */

#ifndef NEGATE
#define NEGATE	'^'			/* std cset negation char */
#endif

#define TRUE    1
#define FALSE   0

int kl_fnmatch (const char *str, const char *p, unsigned int flags)
{
	int negate;
	int match;
	int c;

	while (*p) {
		if (!*str && *p != '*')
			return FALSE;

		switch (c = *p++) {

		case '*':
			while (*p == '*')
				p++;

			if (!*p)
				return TRUE;

			if (*p != '?' && *p != '[' && *p != '\\')
				while (*str && *p != *str)
					str++;

			while (*str) {
				if (kl_fnmatch (str, p, flags))
					return TRUE;
				str++;
			}
			return FALSE;

		case '?':
			if (*str)
				break;
			return FALSE;
/*
 * set specification is inclusive, that is [a-z] is a, z and
 * everything in between. this means [z-a] may be interpreted
 * as a set that contains z, a and nothing in between.
 */
		case '[':
			if (*p != NEGATE)
				negate = FALSE;
			else {
				negate = TRUE;
				p++;
			}

			match = FALSE;

			while (!match && (c = *p++)) {
				if (!*p)
					return FALSE;
				if (*p == '-') {	/* c-c */
					if (!*++p)
						return FALSE;
					if (*p != ']') {
						if (*str == c || *str == *p ||
						    (*str > c && *str < *p))
							match = TRUE;
					}
					else {		/* c-] */
						if (*str >= c)
							match = TRUE;
						break;
					}
				}
				else {			/* cc or c] */
					if (c == *str)
						match = TRUE;
					if (*p != ']') {
						if (*p == *str)
							match = TRUE;
					}
					else
						break;
				}
			}

			if (negate == match)
				return FALSE;
/*
 * if there is a match, skip past the cset and continue on
 */
			while (*p && *p != ']')
				p++;
			if (!*p++)	/* oops! */
				return FALSE;
			break;

		case '\\':
			if ((flags & KL_FNMATCH_NO_ESCAPE) == 0)
			{
				if (*p)
					c = *p++;
			}
		default:
			if (c != *str)
				return FALSE;
			break;

		}
		str++;
	}

	return !*str;
}




