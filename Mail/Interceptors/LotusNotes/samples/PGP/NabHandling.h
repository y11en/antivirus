/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: NabHandling.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::

--- revision history --------
9/12/99 Version 1.01: Paul Ryan
+ name-resolution functionality

12/2/98 Version 1.0: Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_NabHandling_h	/* [ */
#define Included_NabHandling_h


//system includes
#include <stdlib.h>		//for free(), etc.
#include <crtdbg.h>

//Notes API includes
#include <global.h>
#include <pool.h>		//for BLOCKID
#include <osmem.h>		//for OSLockObject(), etc.
#include <lookup.h>		//for NAMELocateItem(), etc.
#include <textlist.h>	//for ListGetNumEntries()
#include <regerr.h>		//for ERR_REG_NOTUNIQUE

#include "LibItem.h"
#include "LibString.h"


typedef struct {
	char * pc_name;
	WORD  us_position;
} GroupInfo;

typedef struct _GroupInfoNode	{
	GroupInfo  t;
	struct _GroupInfoNode * pt_next;
} GroupInfoNode;

typedef struct {
	char * pc_container;
	StringNode * pt_headMember;
} GroupNestInfo;

typedef struct _GroupNestNode	{
	GroupNestInfo  t;
	struct _GroupNestNode * pt_next;
} GroupNestNode;

typedef struct	{
	char * pc_nm;
	BOOL  f_found;
}  NameFoundInfo;

typedef struct _NameFoundNode	{
	NameFoundInfo * pt_name;
	struct _NameFoundNode * pt_next;
}  NameFoundNode;


typedef enum	{
	i_MULTI_PURPOSE = '0', 
	i_MAIL_ONLY = '1', 
	i_ACL_ONLY = '2', 
	i_DENYLIST_ONLY = '3'
}  GroupType;

extern const STATUS  eus_SUCCESS, eus_ERR_INVLD_ARG;
extern char  epc_NULL[];
extern int  ei_SAME;


STATUS eus_CreateNameMatchTextItemCopy( const WORD, void *const, 
															char * *const);
BOOL ef_ListContainsName( const char[], const NameFoundNode *);
BOOL ef_AddNameFoundNodeFifo( char *const, const BOOL, const BOOL, 
													NameFoundNode * *const);
void e_FreeNameFoundList( NameFoundNode * *const);

static BOOL f_AddGroupInfoNodeFifo( char *const, const WORD, 
													GroupInfoNode * *const);
static BOOL f_TackOnNestings( const char[], GroupNestNode *const, 
									StringNode * *const, StringNode * *const);


#endif /* ] Included_NabHandling_h */
