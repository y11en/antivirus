/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: StringHandling.c 12963 2003-03-15 00:18:21Z sdas $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Provides various string-handling services.

--- revision history --------
3/14/03 Version 1.2.1 Paul Ryan
+ added epc_strInBuf() and various character constants

7/2/01 Version 1.2 Paul Ryan
+ extended module to handle version comparison
+ documentation enhancement

8/26/99 Version 1.1 Paul Ryan
+ integrated standard LibLinkList functionality
+ documentation adjustment

1/9/99 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "StringHandling.h"


char  epc_ALPHALWR[] = "abcdefghijklmnopqrstuvwxyz", 
		epc_ALPHAUPR[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 
		epc_NUM10[] = "0123456789", epc_NUMHEX[] = "0123456789abcdefABCDEF", 
		epc_ALPHANUMLWR[] = "abcdefghijklmnopqrstuvwxyz0123456789", 
		epc_ALPHANUMUPR[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 
		epc_ALPHANUM[] = "abcdefghijklmnopqrstuvwxyz"
							"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 
		ec_DOT = '.', ec_QUOTES = '\"', epc_QUOTES[] = "\"", ec_HYPH = '-', 
		ec_SPACE = ' ', ec_SEMICLN = ';', ec_TILDE = '~', epc_TILDE[] = "~";

#define mi_MAXLEN_CMP  64


/** ei_verCmp( ***
Compares typical dot-notation version strings to determine their relative 
order, in a manner similar to the standard strcmp() function. Comparisons are 
not sensitive to case. Version strings are expected to be in the format 
#[A][.#[A]][.#[A]]... where #'s are decimal numerals and A's are alphabetic 
characters. Tolerance for invalidly formatted strings is strict, though not 
perfectly strict when a clear comparison decision is available. (What is 
"clear" the developer must discern for herself by trial & error.)

--- parameters & return ----
pc_CMP: Version string to be compared with the model version string.
pc_MDL: The model version string to be compared against.
pi: Address of integer variable in which the comparison result will be stored. 
	Result is less than zero if pc_CMP < pc_MDL, zero if pc_CMP = pc_MDL, 
	greater than zero if pc_CMP > pc_MDL. A null string involved in the 
	comparison is considered to be lexicographically the least possible.
RETURN:
	eus_ERR_INVLD_ARG if a parameter is obviously invalid
	eus_SUCCESS if comparison succeeded

--- revision history -------
7/2/01 PR: created			*/
int ei_verCmp( const char  pc_CMP[],
				const char  pc_MDL[], 
				int *const  pi)	{
	//static for speed
	static const char  pc_ZERO[] = "0";

	//static for speed
	static char  pc_okChrs[ sizeof( epc_ALPHANUMLWR) + 1], 
					pc_2Dots[ sizeof( ec_DOT) * 2 + 1];

	unsigned int  ui_lenCmp, ui_lenMdl;
	char  pc_mdl[ mi_MAXLEN_CMP], pc_cmp[ mi_MAXLEN_CMP], * pc_c, * pc_m, 
			* pc_cnx = NULL, * pc_mnx = NULL, * pc_ca, * pc_ma;
	int  i;

	//initialize the result to indicate that the compare version is equivalent 
	//	to the model version
	*pi = ei_SAME;

	//the first time thru, initialize utility strings
	if (!*pc_okChrs)	{
		strcpy( pc_okChrs, epc_ALPHANUMLWR);
		pc_okChrs[ sizeof( epc_ALPHANUMLWR) - 1] = ec_DOT;
		memset( pc_2Dots, ec_DOT, sizeof( ec_DOT) * 2);
	}

	//if any input is obviously invalid, short-circuit with failure
	if (!( pi && pc_CMP && pc_MDL && (ui_lenCmp = strlen( pc_CMP)) < 
								mi_MAXLEN_CMP && !strstr( pc_CMP, pc_2Dots) && 
								strspn( strlwr( strcpy( pc_cmp, pc_CMP)), 
								pc_okChrs) == ui_lenCmp && (ui_lenMdl = 
								strlen( pc_MDL)) < mi_MAXLEN_CMP && !strstr( 
								pc_MDL, pc_2Dots) && strspn( strlwr( strcpy( 
								pc_mdl, pc_MDL)), pc_okChrs) == ui_lenMdl))
		return eus_ERR_INVLD_ARG;

	//if the first component of the compare version string is not null...
	if (*pc_cmp)	{
		//if the component is obviously invalid, short-circuit with failure
		if (!strchr( epc_NUM10, *pc_cmp) || *pc_cmp == '0' && strchr( 
													epc_NUM10, *(pc_cmp + 1)))
			return eus_ERR_INVLD_ARG;
		if (pc_cnx = strchr( pc_cmp, ec_DOT))
			*pc_cnx = (char) NULL;
		if ((pc_ca = _strspnp( pc_cmp, epc_NUM10)) && _strspnp( pc_ca + 1, 
																epc_ALPHALWR))
			return eus_ERR_INVLD_ARG;

		//get rid of any trailing dot in the string
		if (*(pc_cmp + ui_lenCmp - 1) == ec_DOT)
			*(pc_cmp + --ui_lenCmp) = (char) NULL;
	} //if (*pc_cmp)

	//if the first counterpart component is not null...
	if (*pc_mdl)	{
		//if the component is obviously invalid, short-circuit with failure
		if (!strchr( epc_NUM10, *pc_mdl) || *pc_mdl == '0' && strchr( 
													epc_NUM10, *(pc_mdl + 1)))
			return eus_ERR_INVLD_ARG;
		if (pc_mnx = strchr( pc_mdl, ec_DOT))
			*pc_mnx = (char) NULL;
		if ((pc_ma = _strspnp( pc_mdl, epc_NUM10)) && _strspnp( pc_ma + 1, 
																epc_ALPHALWR))
			return eus_ERR_INVLD_ARG;

		//get rid of any trailing dot in the string
		if (*(pc_mdl + ui_lenMdl - 1) == ec_DOT)
			*(pc_mdl + --ui_lenMdl) = (char) NULL;
	} //if (*pc_mdl)

	//if the entire version strings are the same, declare so and short-circuit
	if (ui_lenCmp == ui_lenMdl && memcmp( pc_cmp, pc_mdl, ui_lenCmp) == ei_SAME)
		return eus_SUCCESS;

	//work through the component pairs within the version strings, from major 
	//	to minor...
	pc_m = *pc_mdl ? pc_mdl : NULL;
	pc_c = *pc_cmp ? pc_cmp : NULL;
	do	{
		//if both version strings have been fully analyzed with no effective 
		//	difference found, declare them equivalent and stop analyzing
		if (!( pc_c || pc_m))
			break;

		//if the compare version string has been fully analyzed...
		if (!pc_c)	{
			//if the model version string component is not zero, declare the 
			//	compare version string to be less than the model string and 
			//	stop analyzing
			if (strcmp( pc_m, pc_ZERO) != ei_SAME)	{
				*pi = ei_SAME - 1;
				break;
			}
		//else if the model version string has been fully analyzed...
		}else if (!pc_m)	{
			//if the compare version string component is not zero, declare the 
			//	compare version string to be greater than the model string and 
			//	stop analyzing
			if (strcmp( pc_c, pc_ZERO) != ei_SAME)	{
				*pi = ei_SAME + 1;
				break;
			}
		//Else our mode must still be one-to-one comparison. So if the compare 
		//	component is not equivalent to its counterpart, relay its greater- 
		//	or less-than status and stop analyzing.
		}else if ((i = strcmp( pc_c, pc_m)) != ei_SAME)	{
			*pi = i;
			break;
		} //if (!pc_c)

		//if the model version string has a further compare component to 
		//	analyze...
		if (pc_cnx)	{
			//move to it
			pc_c = pc_cnx + 1;

			//if the component is invalid, short-circuit with failure
			if (!strchr( epc_NUM10, *pc_c) || *pc_c == '0' && strchr( 
														epc_NUM10, *(pc_c + 1)))
				return eus_ERR_INVLD_ARG;
			if (pc_cnx = strchr( pc_c, ec_DOT))
				*pc_cnx = (char) NULL;
			if ((pc_ca = _strspnp( pc_c, epc_NUM10)) && _strspnp( pc_ca + 
															1, epc_ALPHALWR))
				return eus_ERR_INVLD_ARG;
		//else note that we've reached the end of the compare version string
		}else
			pc_c = NULL;

		//if the model version string has a further minor component to 
		//	analyze...
		if (pc_mnx)	{
			//move to it
			pc_m = pc_mnx + 1;

			//if the component is invalid, short-circuit with failure
			if (!strchr( epc_NUM10, *pc_m) || *pc_m == '0' && strchr( 
														epc_NUM10, *(pc_m + 1)))
				return eus_ERR_INVLD_ARG;
			if (pc_mnx = strchr( pc_m, ec_DOT))
				*pc_mnx = (char) NULL;
			if ((pc_ma = _strspnp( pc_m, epc_NUM10)) && _strspnp( pc_ma + 1, 
																epc_ALPHALWR))
				return eus_ERR_INVLD_ARG;
		//else note that we've reached the end of the model version string
		}else
			pc_m = NULL;
	} while (TRUE);

	return eus_SUCCESS;
} //ei_verCmp(


/** epc_strInBuf( ***
Finds a given string within a memory buffer irrespective of any null characters 
that may be in that buffer.

--- parameters ----------
PC: address of null-terminated string to search for
puc: address of memory buffer to search
ui: length of memory buffer to search within
ui_LEN_STR_: Optional. Length of the string to search for. If null, procedure 
	will compute this length itself.
RETURN: address of the first position in memory buffer where matching string 
	resides; null if not found or if any input is obviously invalid

--- revision history ----
3/14/03 PR: created		*/
char * epc_strInBuf( const char *const  PC, 
						const BYTE * puc,
						size_t  ui, 
						const size_t  ui_LEN_STR_)	{
	const size_t  ui_LEN_STR = ui_LEN_STR_ ? ui_LEN_STR_ : PC ? strlen( PC) : 
																(size_t) NULL;

	const BYTE * puc_;

	if (!( PC && *PC && puc && ui >= ui_LEN_STR))
		return NULL;

	while (memcmp( puc, PC, ui_LEN_STR) != ei_SAME)	{
		puc_ = puc;
		if (!( puc = memchr( puc + 1, *PC, ui - 1)))
			break;
		if (( ui -= puc_ - puc) < ui_LEN_STR)
			return NULL;
	}

	return (char *) puc;
} //epc_strInBuf(


/** e_FreeStringArray( ***


--- parameters ----------


--- revision history ----
8/26/99 PR: documentation adjustment
1/5/99 PR: created		*/
//DOC!!
void e_FreeStringArray( char * * *const  pppc, 
						const unsigned long  ul_ELEMENTS)	{
	char * * ppc = *pppc;

	if (!pppc)
		return;

	e_FreeStringArrayElements( ppc, ul_ELEMENTS);

	//free the array pointer itself
	free( ppc);

	//nullify the array pointer to assist the caller in not using it anymore
	*pppc = NULL;
} //e_FreeStringArray(


/** e_FreeStringArrayElements( ***

--- parameters ---------

--- revision history ---
1/5/99 PR: created		*/
//DOC!!
void e_FreeStringArrayElements( char * *const  ppc, 
								const unsigned long  ul_ELEMENTS)	{
	unsigned long  ul;

	if (!ppc)
		return;

	//free each string within the array
	for (ul = 0; ul < ul_ELEMENTS; ul++)
		if (ppc[ ul])	{
			free( ppc[ ul]);
			ppc[ ul] = NULL;
		}
} //e_FreeStringArrayElements(


/** ef_AddStringNode( ***

--- parameters & return ----

RETURN: TRUE if no error occurred; FALSE otherwise

--- revision history -------
3/14/03 PR
+ added FIFO/LIFO parameter, and support of change to ef_AddListNodeFifo() 
  function, renamed to ef_AddListNode()
+ listing format adjustment

8/26/99 PR: integration of standard LibLinkList functionality
1/11/99 PR: created			*/
//DOC!!
BOOL ef_AddStringNode( char *const  pc_orig, 
						const BOOL  f_COPY, 
						const BOOL  f_FIFO, 
						StringNode * *const  ppt_nd)	{
	char * pc;

	if (!( pc_orig && ppt_nd))
		return FALSE;

	//according to the caller's request, either transfer control of the string 
	//	to the just-created node or allocate new space for the string and copy 
	//	it in
	pc = f_COPY ? malloc( strlen( pc_orig) + 1) : pc_orig;
	if (!ef_AddListNode( pc, f_FIFO ? TRUE : FALSE, ppt_nd))	{
		if (f_COPY)
			free( pc);
		return FALSE;
	}

	if (f_COPY)
		strcpy( (*ppt_nd)->pc, pc_orig);

	return TRUE;
} //ef_AddStringNode(


/** ef_AddStringNodeFifoUnique( ***
Purpose is to append a given string to the end of a given list, provided that 
that string is not already in the list.

--- parameters & return -----
pc: pointer to the string to append to the list if it's not already there
f_COPY: flag telling whether a copy of the string (pc) should be stored, 
	instead of the string itself
f_CASE_SENSITIVE: flag telling whether uniqueness should be determined on a 
	case-sensitive or -insensitive basis
ppt: Input & Output. Pointer to the _head_ node pointer in the list to be 
	appended to. If a node is added, the node pointer returned will point to 
	the added node, else the node pointer will be null to indicate that the 
	string was not unique to the list.
RETURN: TRUE if no error occurred; FALSE if memory is insufficient to add the 
	string node

--- suggested enhancement ---
8/26/99 PR: extend procedure so it can be used on a case-insensitive basis

--- revision history --------
8/26/99 PR: created			*/
BOOL ef_AddStringNodeFifoUnique( char *const  pc, 
									const BOOL  f_COPY, 
									const BOOL  f_CASE_SENSITIVE, 
									StringNode * *const  ppt)	{
	StringNode * pt_nd, * pt;

	if (!( pc && ppt))
		return FALSE;

	pt_nd = *ppt;
	*ppt = NULL;

	if (ef_ListContainsString( pc, pt_nd, f_CASE_SENSITIVE))
		return TRUE;

	//according to the caller's request, either transfer control of the 
	//	string to the just-created node or allocate new space for the string 
	//	and copy it in
	pt = pt_nd;
	if (!ef_AddStringNode( pc, f_COPY, TRUE, &pt))
		return FALSE;
	if (!pt_nd)
		pt_nd = pt;

	*ppt = pt;

	return TRUE;
} //ef_AddStringNodeFifoUnique(


/** epc_strtokStrict( ***
Behaves as the standard strtok() function does, except that leading 
delimiters are not skipped but instead return a null string. Thus this 
procedure behaves more along the lines of a "split" function.

--- parameters & return ----
pc_refresh: Pointer to the source string to be searched for the next 
	delimiter. If provided, string will be preserved statically and ensuing 
	tokens obtained by specifying this parameter as NULL.
pc_DELIM: pointer to the string of one-character delimiters to use in parsing 
	the source string
RETURN: Pointer to the next token in the source string. NULL if no further 
	tokens are available in the source string or if no source string has been 
	statically stored by the procedure.

--- revision history -------
8/26/99 PR: created			*/
const char * epc_strtokStrict( char *const  pc_refresh, 
								const char  pc_DELIM[])	{
	static char * pc_src;

	const char * pc, * pc_ret = NULL;
	char * pc_end;

	if (!( pc_DELIM && (pc_refresh || pc_src)))
		return NULL;

	if (pc_refresh)
		pc_src = pc_refresh;

	pc = pc_DELIM;
	do	{
		if (!(pc_end = strchr( pc_src, *pc)))
			continue;

		*pc_end = (char) NULL;
		pc_ret = pc_src;
		pc_src = pc_end + 1;
		break;
	} while (*++pc);

	if (!*pc)	{
		pc_ret = pc_src;
		pc_src = NULL;
	}

	return pc_ret;
} //epc_strtokStrict(


/** ef_ListContainsString( ***
Returns whether a string list includes a given string.

--- parameters & return ----
PC: pointer to the string to be sought for within the list
pt: pointer to the head node of the list to be searched
f_CASE_SENSITIVE: flag telling whether uniqueness should be determined on a 
	case-sensitive or -insensitive basis
RETURN: TRUE if list contains string; FALSE otherwise

--- revision history -------
8/26/99 PR
+ extended procedure to accommodate case-insensitive comparisons
+ completed documentation

1/9/99 PR: created			*/
BOOL ef_ListContainsString( const char  PC[], 
							const StringNode * pt, 
							const BOOL  f_CASE_SENSITIVE)	{
	if (!( PC && pt))
		return FALSE;

	do
		if (pt->pc)
			if ( (f_CASE_SENSITIVE ? strcmp( PC, pt->pc) : 
											stricmp( PC, pt->pc)) == ei_SAME)
				return TRUE;
	while (pt = pt->pt_next);

	return FALSE;
} //ef_ListContainsString(


