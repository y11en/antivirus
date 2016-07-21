/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.
	

	$Id: LibNab.h 12258 2003-02-13 23:57:46Z sdas $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::

--- revision history --------
9/12/99 Version 1.0: Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_LibNab_h	/* [ */
#define Included_LibNab_h


#include "LibString.h"


//Rather than incorporate it directly into NameFoundNode, the following 
//	structure is included here explicitly in order to aid extensibility by 
//	implementers of this library such that the NameFoundNode facilities 
//	provided by the library may still be used. For an example of how the 
//	structure may be extended, see the NameFoundInfoEx structure in nPGPNts.h.
typedef struct	{		
	char * pc_nm;
	BOOL  f_found;
}  NameFoundInfo;

typedef struct _NameFoundNode	{
	NameFoundInfo * pt_name;
	struct _NameFoundNode * pt_next;
}  NameFoundNode;


typedef enum	{
	i_MAIL_ONLY = '1', 
	i_ACL_ONLY = '2', 
	i_DENYLIST_ONLY = '3'
}  GroupType;

#define mpc_TKN_CANONICAL_COMMON_NM  "CN="
static const char  mpc_TKN_NOTES_CANONICAL[] = mpc_TKN_CANONICAL_COMMON_NM;
static const UINT  mui_LEN_TKN_NOTES_CANONICAL = sizeof( 
												mpc_TKN_NOTES_CANONICAL) - 1;


STATUS eus_CreateNameMatchTextItemCopy( const WORD, void *const, 
															char * *const);
STATUS eus_CompileUsersAclGroupList( const char[], char[], 
														StringNode * *const);
char * epc_NotesNmAbbreviate( const char[], char *const);
void e_FreeNameFoundList( NameFoundNode * *const);
BOOL ef_AddNameFoundNodeFifo( char *const, const BOOL, const BOOL, 
													NameFoundNode * *const);
STATUS eus_ResolveMailAddresses( NameFoundNode *const, const BOOL, char[], 
									StringNode * *const, StringNode * *const, 
									UINT *const, BOOL *const);
BOOL ef_ListContainsName( const char[], const NameFoundNode *);


#endif /* ] Included_LibNab_h */
