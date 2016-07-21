/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: NabHandling.c 16469 2003-08-21 17:56:49Z pryan $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Reusable code associated with Notes name handling and the Domino Directory 
(Notes Name & Address Book).

--- suggested enhancement ----
9/12/99 PR
change the GroupInfo and GroupNestInfo linked-list handling to use the 
standard linked-list functionality provided through LibLinkList.h

--- revision history ---------
8/21/03 Version 1.1.2 Paul Ryan
+ safety enhancement to e_FreeNameFoundList()
+ minor documentation adjustment

3/20/00 Version 1.1.1 Paul Ryan
+ documentation, formatting adjustment

9/12/99 Version 1.1 Paul Ryan
+ added name- & group-resolution functionality
+ signature change to eus_CreateTextListEntryCopy()
+ Notes name conversion procedure: canonicalized -> abbreviated
+ token renaming
+ doucmentation adjustment

1/10/99 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "NabHandling.h"


#define mpc_TKN_COMMON_NM  "CN="
static const char  mpc_TKN_CANONICAL[] = mpc_TKN_COMMON_NM;
static const UINT  mui_LEN_TKN_CANONICAL = sizeof( mpc_TKN_CANONICAL) - 1;


/** eus_CompileUsersAclGroupList( ***
Constructs a string list of all the groups to which a particular user belongs. 
Procedure assumes that the first entry in the ListName item of the group 
document is the name of the group; follow-on alias entries are ignored.

--- parameters & return ------

RETURN: !eus_SUCCESS if invalid parameters were passed; eus_SUCCESS if no error 
	occured; the Notes API error code otherwise

--- suggested enhancement ----
9/12/99 PR: for the sake of comprehensiveness, extend procedure so that group 
	"aliases" are added to the returned string list

--- revision history ---------
3/14/03 PR
+ support of change to ef_AddStringNodeFifo() function, renamed to 
  ef_AddStringNode()
+ listing format adjustment, token renaming

9/12/99 PR
+ token renaming
+ logic shortening
+ documentation adjustment

1/11/99 PR: created			*/
//DOC!!
STATUS eus_CompileUsersAclGroupList( const char  pc_USRNM[], 
										char  pc_SVRNM[], 
										StringNode * *const  ppt_membrGrps)	{
	const WORD  us_LOOKUP_VIEWS = 1, us_LOOKUP_ITEMS = 3, 
				us_GROUPNM = 0, us_TYPE = 1, us_MEMBERS = 2;
	char  pc_VIEWNM_GROUPS[] = "$VIMGroups", pc_ITMNM_GROUPNM[] = "ListName", 
			pc_ITMNM_TYPE[] = "GroupType", pc_ITMNM_MEMBERS[] = "Members";

	char * pc, pc_ItmNms[ sizeof( pc_ITMNM_GROUPNM) + sizeof( pc_ITMNM_TYPE) + 
													sizeof( pc_ITMNM_MEMBERS)];
	HANDLE  h = NULL;
	void * pv_results, * pv = NULL, * pv_list = NULL;
	WORD  us, us_type, us_len, us_groups, us_entries, us_ixMatch = 0xFFFF;
	BOOL  f_okType, f_failure = FALSE, f_StartedEntries;
	GroupInfoNode * pt_group = NULL, * pt_headGroup = NULL, * pt_grp;
	BYTE * puc, * puc_members;
	StringNode * pt_membrGrps = NULL, * pt_tailMember = NULL, 
				* pt_member, * pt;
	GroupNestNode * pt_headNesting = NULL, * pt_tailNesting = NULL, * pt_nest;
	GroupNestInfo * pt_nst;
	STATUS us_err;

	if (!( pc_USRNM && pc_SVRNM && ppt_membrGrps))
		return !eus_SUCCESS;

	*ppt_membrGrps = NULL;

	//lookup information about all the groups in the primary NAB
	strcpy( pc = pc_ItmNms, pc_ITMNM_GROUPNM);
	strcpy( pc += sizeof( pc_ITMNM_GROUPNM), pc_ITMNM_TYPE);
	strcpy( pc + sizeof( pc_ITMNM_TYPE), pc_ITMNM_MEMBERS);
	if (us_err = NAMELookup( pc_SVRNM, NAME_LOOKUP_ALL | 
									NAME_LOOKUP_NOSEARCHING, us_LOOKUP_VIEWS, 
									pc_VIEWNM_GROUPS, 1, epc_NULL, 
									us_LOOKUP_ITEMS, pc_ItmNms, &h))
		return us_err;

	//if there aren't any groups in the NAB, short-circuit with success since 
	//	there's nothing to compile
	pv_results = OSLockObject( h);
	pv_list = NAMELocateNextName( pv_results, NULL, &us_groups);
	if (!( pv_list && us_groups))
		goto errJump;

	//construct a dedicated list of existing group names so we can look name 
	//	matches up quickly
	for ( us = 0; us < us_groups; us++)	{
		pv = NAMELocateNextMatch( pv_results, pv_list, pv);

		//if the group can't be used with ACLs, loop for the next group name
		if (us_err = eus_CreateNameMatchTextItemCopy( us_TYPE, pv, &pc))
			goto errJump;
		us_ixMatch++;
		f_okType = !( *pc == i_MULTI_PURPOSE || *pc == i_ACL_ONLY);
		free( pc);
		if (!f_okType)
			continue;

		//add the group name to the dedicated list of existing group names
		if (us_err = eus_CreateNameMatchTextItemCopy( us_GROUPNM, pv, &pc))
			goto errJump;
		if (!pc)
			continue;
		if (f_failure = !f_AddGroupInfoNodeFifo( pc, us_ixMatch, &pt_group))	{
			free( pc);
			goto errJump;
		}
		if (!pt_headGroup)
			pt_headGroup = pt_group;
	} //for ( us = 0; us < us_groups; us++)

	//parse each group for user-membership and group-nesting information
	pt_group = pt_headGroup;
	us_ixMatch = 0;
	pv = NULL;
	while (pt_group)	{
		//scroll through to the information record about the next group
		while (us_ixMatch <= pt_group->t.us_position)	{
			pv = NAMELocateNextMatch( pv_results, pv_list, pv);
			us_ixMatch++;
		}

		//if the Members item isn't present (though it should be), iterate on 
		//	to the next group
		if (!( puc_members = NAMELocateItem( pv, us_MEMBERS, &us_type, 
																&us_len)))	{
			pt_group = pt_group->pt_next;
			continue;
		}

		//determine the number of members in the list
		if (us_type == TYPE_TEXT_LIST)
			us_entries = ListGetNumEntries( puc_members, TRUE);
		else if (us_type == TYPE_TEXT)
			us_entries = 1;
		else
			us_entries = 0;

		//process each name in the group as needed
		f_StartedEntries = FALSE;
		for (us = 0; us < us_entries; us++)	{
			//if the "name" is a null string, loop for the next entry
			if (us_type == TYPE_TEXT_LIST)	{
				if (us_err = ListGetText( puc_members, TRUE, us, &puc, &us_len))
					goto errJump;
			}else	{
				puc = puc_members + sizeof( WORD);
				us_len -= sizeof( WORD);
			}
			if (!us_len)
				continue;

			//if the name matches the user name...
			if (memcmp( puc, pc_USRNM, us_len) == ei_SAME && us_len == 
														strlen( pc_USRNM))	{
				//add the group name to the member-group list
				if (f_failure = !ef_AddStringNode( pt_group->t.pc_name, TRUE, 
														TRUE, &pt_tailMember))
					goto errJump;
				if (!pt_membrGrps)
					pt_membrGrps = pt_tailMember;

				//there's no need to continue processing the group if it 
				//	contains the user, so break out this loop so we can move on 
				//	to the next group
				break;
			} //if (puc && memcmp( puc, pc_USRNM

			//if the name is canonicalized, loop for the next name
			if (memcmp( puc, mpc_TKN_CANONICAL, mui_LEN_TKN_CANONICAL) == 
																	ei_SAME)
				continue;

			//We now assume that the name is a group name. If the name is not 
			//	in the list of _pertinent_ groups, loop for the next name in 
			//	this group...
			pt_grp = pt_headGroup;
			while (pt_grp)	{
				if (memcmp( puc, pt_grp->t.pc_name, us_len) == ei_SAME && 
										us_len == strlen( pt_grp->t.pc_name))
					break;
				pt_grp = pt_grp->pt_next;
			} //while (pt_grp)
			if (!pt_grp)
				continue;

			//if nesting tracking has begun but no entries have been logged 
			//	for _this_ group name...
			if (pt_headNesting && !f_StartedEntries)	{
				//add a node onto the list to contain the new entry
				if (f_failure = !(pt_tailNesting = pt_tailNesting->pt_next = 
											calloc( 1, sizeof( GroupNestNode))))
					goto errJump;
			//else if nesting tracking hasn't begun, start it off
			}else if (!pt_headNesting)
				if (f_failure = !(pt_headNesting = pt_tailNesting = calloc( 1, 
													sizeof( GroupNestNode))))
					goto errJump;

			//if nesting entries have been logged for this group name, string 
			//	on room for another entry
			pt = (pt_nst = &pt_tailNesting->t)->pt_headMember;
			if (pt)	{
				while (pt->pt_next)
					pt = pt->pt_next;
				if (f_failure = !(pt = pt->pt_next = calloc( 1, 
														sizeof( StringNode))))
					goto errJump;
			//else start off the nesting entries list for this group name
			}else	{
				if (f_failure = !(pt = pt_nst->pt_headMember = calloc( 1, 
														sizeof( StringNode))))
					goto errJump;
				f_StartedEntries = TRUE;
			} //if (pt)

			//populate the new nesting entry
			if (f_failure = !(pt->pc = malloc( us_len + 1)))
				goto errJump;
			memcpy( pt->pc, puc, us_len);
			pt->pc[ us_len] = NULL;
			if (pt == pt_nst->pt_headMember)
				pt_nst->pc_container = pt_group->t.pc_name;
		} //for (us = 0; us < us_entries

		pt_group = pt_group->pt_next;
	} //while (pt_group)

	//if applicable, add the names of any higher-order groups containing the 
	//	"first-order" groups just discovered
	if (pt_headNesting && (pt_member = pt_membrGrps))	{
		StringNode * pt_headGreaterGroup = NULL, 
					* pt_tailGreaterGroup = NULL;

		do
			if (f_failure = !f_TackOnNestings( pt_member->pc, pt_headNesting, 
														&pt_tailGreaterGroup, 
														&pt_headGreaterGroup))
				goto errJump;
		while (pt_member = pt_member->pt_next);

		pt_member = pt_membrGrps;
		while (pt_member->pt_next)
			pt_member = pt_member->pt_next;
		pt_member->pt_next = pt_headGreaterGroup;
	} //if (pt_headNesting && (pt_member

	*ppt_membrGrps = pt_membrGrps;

errJump:
	//if an error occurred and some member-group entries were logged, free the 
	//	resources allocated for those entries
	if (( us_err || f_failure) && (pt = pt_membrGrps))	{
		StringNode * pt_next;

		do	{
			pt_next = pt->pt_next;

			free( pt->pc);
			free( pt);
		} while (pt = pt_next);
	} //if (( us_err || f_failure) && 

	//If applicable, free all resources still allocated to the nesting-groups 
	//	table. The container string is owned by the dedicated list of 
	//	existing group names, not by the nesting-groups table.
	if (pt_nest = pt_headNesting)	{
		GroupNestNode * pt_next;

		do	{
			pt_next = pt_nest->pt_next;

			if (pt_nest->t.pt_headMember)
				e_FreeList( &pt_nest->t.pt_headMember, TRUE);
			free( pt_nest);
		} while (pt_nest = pt_next);
	} //if (pt_nest = pt_headNesting)

	//if applicable, free the dedicated list of existing group names
	if (pt_grp = pt_headGroup)	{
		GroupInfoNode * pt_next;

		do	{
			pt_next = pt_grp->pt_next;

			free( pt_grp->t.pc_name);
			free( pt_grp);
		} while (pt_grp = pt_next);
	} //if (pt_grp = pt_headGroup)

	//if applicable, free the buffer returned by NAMELookup(), if any
	if (h)	{
		OSUnlockObject( h);
		OSMemFree( h);
	}

	return us_err + f_failure;
} //eus_CompileUsersAclGroupList(


/** f_TackOnNestings( ***


--- parameters & return ----


--- revision history -------
3/14/03 PR: support of change to ef_AddStringNodeFifo() function, renamed to 
	ef_AddStringNode()
9/12/99 PR: logic shortening
1/11/98 PR: created			*/
//DOC!!
static BOOL f_TackOnNestings( const char  pc_GROUPNM[], 
								GroupNestNode *const  pt_headNest, 
								StringNode * *const  ppt_tailMember, 
								StringNode * *const  ppt_headMember)	{
	GroupNestNode * pt_nest;
	StringNode * pt_headMember = NULL;
	BOOL  f_SetHeadMemberNode = ppt_headMember ? !*ppt_tailMember : FALSE;

	_ASSERTE( pc_GROUPNM && pt_headNest && ppt_tailMember);

	if (ppt_headMember)
		*ppt_headMember = NULL;

	pt_nest = pt_headNest;
	while (pt_nest)	{
		if (ef_ListContainsString( pc_GROUPNM, pt_nest->t.pt_headMember, 
																	FALSE))	{
			//copy the container group name to a new node in the member list
			if (!ef_AddStringNode( pt_nest->t.pc_container, TRUE, TRUE, 
															ppt_tailMember))
				return FALSE;

			if (f_SetHeadMemberNode)	{
				pt_headMember = *ppt_tailMember;
				f_SetHeadMemberNode = FALSE;
			}

			//Free the list of group members contained by the current group. 
			//	I believe this step is necessary to guard against infinite 
			//	group recursion (PR 9/12/99).
			e_FreeList( &pt_nest->t.pt_headMember, TRUE);

			//recursively tack on any groups containing this container 
			//	group, for the user name must be a part of those too
			if (!f_TackOnNestings( (*ppt_tailMember)->pc, pt_headNest, 
														ppt_tailMember, NULL))
				return FALSE;
		} //if (ef_ListContainsString( pc_GROUPNM,

		pt_nest = pt_nest->pt_next;
	} //while (pt_nest)

	if (pt_headMember)
		*ppt_headMember = pt_headMember;

	return TRUE;
} //f_TackOnNestings(


/** f_AddGroupInfoNodeFifo( ***


--- parameters & return ----

RETURN: !eus_SUCCESS if invalid parameters were passed; eus_SUCCESS if no 
	error occured; the Notes API error code otherwise

--- revision history -------
9/12/99 PR: documentation adjustment
1/8/99 PR: created			*/
//DOC!!
static BOOL f_AddGroupInfoNodeFifo( char *const  pc, 
									const WORD  us_POSITION, 
									GroupInfoNode * *const  ppt_tail)	{
	GroupInfoNode * pt_tail;

	_ASSERTE( pc && ppt_tail);

	if (!( pt_tail = *ppt_tail))	{
		if (!(pt_tail = calloc( 1, sizeof( GroupInfoNode))))
			return FALSE;
	//else string on a new entry
	}else
		if (!(pt_tail = pt_tail->pt_next = calloc( 1, 
													sizeof( GroupInfoNode))))
			return FALSE;

	//transfer control of the string to the just-created node, and copy the 
	//	position information
	pt_tail->t.pc_name = pc;
	pt_tail->t.us_position = us_POSITION;

	*ppt_tail = pt_tail;

	return TRUE;
} //f_AddGroupInfoNodeFifo(


/** eus_CreateNameMatchTextItemCopy( ***
Purpose is to return a string copy of a particular text item's content 
furnished via a prior call to the NAMELookup() Notes C API call. If the item 
is a text-list item, only the first list member is copied and returned.

--- parameters & return ----
us_ITEM: The index to the item, within the name entry provided by 
	pv_NAME_MATCH, whose textual contents should be copied. If the item turns 
	out to be a text-list item, only the first list member is copied and 
	returned.
pv_NAME_MATCH: pointer to the name entry, in the NAMELookup() results 
	buffer, that contains the item being sought
ppc: Output. Pointer to the pointer variable this procedure should set to 
	point to the string buffer this procedure will allocate and populate with 
	a copy of the textual content requested by the caller. CALLER IS 
	RESPONSIBLE for freeing the allocated buffer to the operating system. If 
	the textual content is not text, or if the textual content is the null 
	string, or if the procedure is unsuccessful, no buffer will be allocated 
	and the pointer variable is guaranteed to be NULL upon return.
RETURN: !eus_SUCCESS if invalid parameters were passed; eus_SUCCESS if no 
	error occured; the Notes API error code otherwise

--- revision history -------
9/12/99 PR
+ return failure if not a text or text-list item
+ documentation completed

1/8/99 PR: created			*/
STATUS eus_CreateNameMatchTextItemCopy( const WORD  us_ITEM, 
										void *const  pv_NAME_MATCH, 
										char * *const  ppc)	{
	BYTE * puc;
	WORD  us_type, us_len;
	char * pc = NULL;
	STATUS  us_err;

	if (!( pv_NAME_MATCH && ppc))
		return !eus_SUCCESS;

	*ppc = NULL;

	if (!( puc = NAMELocateItem( pv_NAME_MATCH, us_ITEM, &us_type, &us_len)))
		return eus_SUCCESS;

	switch (us_type)	{
		case TYPE_TEXT_LIST:
			if (us_err = eus_CreateTextListEntryCopy( 0, puc, TRUE, &pc))
				return us_err;
			if (!pc)
				us_len = NULL;
			break;

		case TYPE_TEXT:
			puc += sizeof( WORD);
			us_len -= sizeof( WORD);
			break;

		default:
			return !eus_SUCCESS;
	} //switch (us_type)

	//if the entry contains a non-blank resolved name...
	if (us_len)
		//if the name has already been extracted from a string in a 
		//	text-list match...
		if (pc)
			//transfer ownership of the string to the caller's string pointer
			*ppc = pc;
		//else allocate caller-controlled space for the resolved name, then 
		//	copy in the name
		else	{
			pc = malloc( us_len + 1);
			memcpy( pc, puc, us_len);
			pc[ us_len] = NULL;
			*ppc = pc;
		} //if (pc)

	return eus_SUCCESS;
} //eus_CreateNameMatchTextItemCopy(


/** epc_NotesNmAbbreviate( ***
Purpose is to return the abbreviated version of the passed-in Notes 
canonicalized name.

--- parameters & return ----
pc_CANONICALIZED: address of the canonicalized version of the Notes name
pc_abbrev: Output. Address of the buffer in which to store the abbreviated 
	version. The caller may use the same pointer for this parameter as is 
	used in the pc_CANONICALIZED parameter.
RETURN: address of abbreviated name (pc_abbrev) if successful; NULL otherwise

--- revision history -------
3/20/00 PR: documentation, formatting adjustment
9/12/99 PR: created			*/
char * epc_NotesNmAbbreviate( const char  pc_CANONICALIZED[], 
								char *const  pc_abbrev)	{
	//static for speed
	static const char  pc_TKN_LEVEL[] = "/", pc_TKN_CONTENT[] = "=", 
						pc_TKN_ORG[] = "O", pc_TKN_COUNTRY[] = "C";
	static const UINT  ui_LEN_TKN_CONTENT = sizeof( pc_TKN_CONTENT) - 1, 
						ui_LEN_TKN_ORG = sizeof( pc_TKN_ORG) - 1, 
						ui_LEN_TKN_COUNTRY = sizeof( pc_TKN_COUNTRY) - 1, 
						ui_LEN_TKN_COMMON_NM = sizeof( mpc_TKN_COMMON_NM) - 1;

	const char * pc, * pc_src;
	UINT  ui_offset = NULL, ui;
	BOOL  f_orgLocated = FALSE, f_CountryReached = FALSE;

	if (!( pc_CANONICALIZED && pc_abbrev && memcmp( pc_CANONICALIZED, 
										mpc_TKN_CANONICAL, 
										mui_LEN_TKN_CANONICAL) == ei_SAME))
		return NULL;

	pc = pc_src = pc_CANONICALIZED;
	do	{
		if (!( pc_src = strstr( pc_src, pc_TKN_CONTENT)))
			return NULL;
		if (pc = strstr( pc + 1, pc_TKN_LEVEL))	{
			if (pc < pc_src + ui_LEN_TKN_CONTENT)
				return NULL;
		}else
			pc = pc_CANONICALIZED + strlen( pc_CANONICALIZED);

		if (!f_orgLocated)	{
			if (memcmp( pc_src - ui_LEN_TKN_ORG, pc_TKN_ORG, 
												ui_LEN_TKN_ORG) == ei_SAME)
				f_orgLocated = TRUE;
		}else if (!f_CountryReached && memcmp( pc_src - ui_LEN_TKN_COUNTRY, 
											pc_TKN_COUNTRY, 
											ui_LEN_TKN_COUNTRY) == ei_SAME)
			f_CountryReached = TRUE;
		else
			return NULL;

		pc_src += ui_LEN_TKN_CONTENT;
		memcpy( pc_abbrev + ui_offset, pc_src, ui = ++pc - pc_src);
		ui_offset += ui;
	} while (*((pc_src = pc) - 1));

	if (!f_orgLocated)
		return NULL;

	return pc_abbrev;
} //epc_NotesNmAbbreviate(


/** eus_ResolveMailAddresses( ***
Resolves a list of mail addressees according to the current configuration of 
the Domino Directory (Notes Name & Address Book). Groups, even nested groups, 
are resolved to member individuals.

--- parameters & return ----
pt_unrslvd: Input & Output. Address of the list of addresses to be resolved via 
	the Domino Directory. Procedure will set to TRUE the f_found member of the 
	NameFoundInfo structure contained by each node if the address was located 
	during the call.  If members of any _groups_ among the addresses looked up 
	by this procedure cannot be resolved, the procedure will add the unresolved 
	names to the list with their f_found members set to FALSE.
f_LKP_EXHST: Flag telling whether the directories should be searched 
	exhaustively for each name so as to be sure all matches are accounted for. 
	If FALSE, the first match found will be used.
pc_SVRNM: address of the name of the server holding the Domino Directory to be 
	used in the resolution process
ppt_encntrd: Optional Input & Output. Pointer to a pointer variable that refers 
	to a list of group names to be ignored by this procedure, typically in 
	order to guard against infinite recursion via nested groups. If ppt_encntrd 
	pointer is null, the procedure will carry out the guard against infinite 
	recursion itself, but if sequential calls are being made to this procedure 
	with the same list of resolved names, it is more efficient for the caller 
	to control the list and allow it to grow. If the caller wishes to control 
	the list, the caller's _pointer variable_ (*ppt_encntrd) must be NULL at 
	the first call so the procedure will know to start the list on the behalf 
	of the caller. If in control of the list, CALLER IS RESPONSIBLE for freeing 
	resources allocated to the list, typically via e_FreeList(). Pointer 
	variable is guaranteed to be null if list wasn't already started upon 
	procedure entry and procedure fails or resolves no entries.
ppt_rslvd: Input & Output. Address of the pointer variable that refers to the 
	current list of fully resolved addesses generated by this procedure. If the 
	pointer variable is NULL, this procedure will start the list on behalf of 
	the caller. CALLER IS RESPONSIBLE for freeing resources allocated to the 
	list, typically via e_FreeList(). Pointer variable is guaranteed to be null 
	if list wasn't already started upon procedure entry and procedure fails or 
	resolves no entries. Procedure will only add entries unique to the list. 
	Ambiguous matches (e.g. five entries matching the input name "john") are 
	all treated as resolutions and added to the list.
pui_rslvd: Optional Output. Address of the variable in which to store the 
	number of items returned in the list of resolved addresses. If returned 
	negative, ambiguous matches were encountered. Providing a null address as 
	this parameter causes this functionality to be ignored.
pf_ambg: Optional Output. Address of the flag variable to set according to 
	whether ambiguous resolutions were encountered. If omitted, functionality 
	is ignored.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- suggested enhancement ----
8/21/03 PR: for SMTP addresses, would be better for procedture to output phrase 
	(display name) portion if present and let caller whittle it down if it wants

--- revision history ---------
8/21/03 PR: documentation improvement, listing format adjustment

3/14/03 PR
+ support of change to ef_AddStringNodeFifo() function, renamed to 
  ef_AddStringNode()
+ listing format adjustment, token name adjustment, minor documentation 
  adjustment

9/12/99 PR: created			*/
STATUS eus_ResolveMailAddresses( NameFoundNode *const  pt_unrslvd, 
									const BOOL  f_LKP_EXHST, 
									char  pc_SVRNM[], 
									StringNode * *const  ppt_encntrd, 
									StringNode * *const  ppt_rslvd, 
									UINT *const  pui_rslvd, 
									BOOL *const  pf_ambg)	{
	static const WORD  us_LKP_VIEWS = 1, us_LKP_ITMS = 4, 
						us_MAILADDR = 0, us_FULLNM = 1, us_GRP_TYP = 2, 
						us_MBRS = 3;
	static char  pc_VWNM_USERS[] = "$Users", 
					pc_ITMNM_MAILADDR[] = "MailAddress", 
					pc_ITMNM_FULLNM[] = "FullName", 
					pc_ITMNM_GRP_TYP[] = "GroupType", 
					pc_ITMNM_MBRS[] = "Members";

	static BYTE  puc_ItmNms[ sizeof( pc_ITMNM_MAILADDR) + 
										sizeof( pc_ITMNM_FULLNM) + 
										sizeof( pc_ITMNM_GRP_TYP) + 
										sizeof( pc_ITMNM_MBRS)] = {NULL};

	NameFoundNode * pt, * pt_mbrs = NULL, * pt_nde;
	BYTE * puc;
	DWORD  ul = 0, ul_offst = 0;
	HANDLE  h = NULL;
	void * pv_rslt, * pv, * pv_entry = NULL;
	char * pc;
	BOOL  f_NullAtStart_RslvdList, f_NullAtStart_EncntrdList, 
			f_ambg = FALSE, f_fail = FALSE;
	WORD  us = 0;
	UINT  ui_rslvd = 0;
	StringNode * pt_rslvd, * pt_encntrd = NULL, 
				* pt_lastEncntrd = NULL, * pt_nd;
	STATUS  us_err;

	if (!( pt_unrslvd && ppt_rslvd))
		return !eus_SUCCESS;

	if (pui_rslvd)
		*pui_rslvd = NULL;
	if (pf_ambg)
		*pf_ambg = FALSE;

	f_NullAtStart_RslvdList = !(pt_rslvd = *ppt_rslvd);
	f_NullAtStart_EncntrdList = !(pt_encntrd = pt_lastEncntrd = (ppt_encntrd ? 
														*ppt_encntrd : NULL));

	//if not done already, put together the item input needed to do a lookup 
	//	against the Domino Directory
	if (!*puc_ItmNms)	{
		strcpy( puc = puc_ItmNms, pc_ITMNM_MAILADDR);
		strcpy( puc += sizeof( pc_ITMNM_MAILADDR), pc_ITMNM_FULLNM);
		strcpy( puc += sizeof( pc_ITMNM_FULLNM), pc_ITMNM_GRP_TYP);
		strcpy( puc + sizeof( pc_ITMNM_GRP_TYP), pc_ITMNM_MBRS);
	} //if (!*puc_ItmNms)

	//as a start to performing an aggregate lookup on the Notes directory, 
	//	allocate a buffer and then copy in the names as specified by 
	//	NAMELookup()
	pt = pt_unrslvd;
	do	{
		ul += strlen( pt->pt_name->pc_nm) + 1;
		us++;
	} while (pt = pt->pt_next);
	if (!( puc = malloc( ul)))
		return !eus_SUCCESS;
	pt = pt_unrslvd;
	do	{
		ul = strlen( pt->pt_name->pc_nm) + 1;
		memcpy( puc + ul_offst, pt->pt_name->pc_nm, ul);
		ul_offst += ul;
	} while (pt = pt->pt_next);

	//perform the aggregate lookup
	us_err = NAMELookup( pc_SVRNM, (WORD) (f_LKP_EXHST ? NULL : 
											NAME_LOOKUP_NOSEARCHING), 
											us_LKP_VIEWS, pc_VWNM_USERS, us, 
											puc, us_LKP_ITMS, puc_ItmNms, &h);
	free( puc);
	if (us_err)
		goto errJump;

	//for each record in the returned lookup buffer...
	pv_rslt = OSLockObject( h);
	pt = pt_unrslvd;
	while (pv_entry = NAMELocateNextName( pv_rslt, pv_entry, &us))	{
		//if a match was found, indicate this to caller, else iterate to the 
		//	next not-yet-resolved entry
		if (!us)	{
			pt = pt->pt_next;
			continue;
		}else if (us > 1)
			f_ambg = TRUE;

		pv = NULL;
		for (ul = 0; ul < us; ul++)	{
			//if the entry contains the MailAddress item, get a copy of the 
			//	item contents
			if (us_err = eus_CreateNameMatchTextItemCopy( us_MAILADDR, pv = 
												NAMELocateNextMatch( pv_rslt, 
												pv_entry, pv), &pc))
				goto errJump;

			//if we found a specific mail address...
			if (pc)	{
				//if the base e-mail address is enclosed in corner brackets, 
				//	pare the string down to just the address
				const char * pc_end, * pc_strt = strchr( pc, '<');
				if (pc_strt && (pc_end = strchr( pc, '>')) > ++pc_strt)	{
					memmove( pc, pc_strt, pc_end - pc_strt);
					pc[ pc_end - pc_strt] = NULL;
				}

				//If unique, transfer ownership of the string to a new entry in 
				//	the resolved list. If not unique, free the resources 
				//	allocated to the string. In any case, also mark the name as 
				//	having been found.
				pt_nd = pt_rslvd;
				if (f_fail = !ef_AddStringNodeFifoUnique( pc, FALSE, FALSE, 
																		&pt_nd))
					goto errJump;
				if (pt_nd)	{
					ui_rslvd++;
					if (!pt_rslvd)
						pt_rslvd = pt_nd;
				}else
					free( pc);
				pt->pt_name->f_found = TRUE;
			//else let's see whether we've landed on a person document by 
			//	checking whether the FullName item is present
			}else	{
				//get a copy of the first name in the item
				if (us_err = eus_CreateNameMatchTextItemCopy( us_FULLNM, pv, 
																		&pc))
					goto errJump;

				//if we found a full name...
				if (pc)	{
					//If unique, transfer ownership of the string to a new 
					//	entry in the resolved list. If not unique, free the 
					//	resources allocated to the string. In any case, also 
					//	mark the name as having been found.
					pt_nd = pt_rslvd;
					if (f_fail = !ef_AddStringNodeFifoUnique( pc, FALSE, FALSE, 
																		&pt_nd))
						goto errJump;
					if (pt_nd)	{
						ui_rslvd++;
						if (!pt_rslvd)
							pt_rslvd = pt_nd;
					}else
						free( pc);
					pt->pt_name->f_found = TRUE;
				//else assume we've hit on a group and add it to the groups 
				//	list for later lookup & resolution
				}else	{
					BOOL  f_okType;

					if (us_err = eus_CreateNameMatchTextItemCopy( us_GRP_TYP, 
																	pv, &pc))
						goto errJump;

					if (pc)	{
						//if this group is for use with e-mail...
						f_okType = *pc == i_MULTI_PURPOSE || *pc == i_MAIL_ONLY;
						free( pc);
						if (f_okType)	{
							WORD  us_type, us_len, us_entries = 0, us;
							NameFoundNode * pt_node;

							//Get the number of members in the group. If there 
							//	aren't any, short-circuit with success.
							if (!( puc = NAMELocateItem( pv, us_MBRS, &us_type, 
																	&us_len)))
								goto errJump;
							switch (us_type)	{
								case TYPE_TEXT_LIST:
									us_entries = ListGetNumEntries( puc, TRUE);
									break;
								case TYPE_TEXT:
									puc += sizeof( WORD);
									if (us_len -= sizeof( WORD))
										us_entries = 1;
									break;
							} //switch (us_type)

							//for each member of the group...
							for (us = 0; us < us_entries; us++)	{
								//if the member is null or already named in any 
								//	of the lists we've got going (resolved, 
								//	unresolved, groups-encountered), iterate to 
								//	the next member
								if (us_type == TYPE_TEXT_LIST)	{
									if (us_err = eus_CreateTextListEntryCopy( 
															us, puc, TRUE, &pc))
										goto errJump;
									if (!pc)
										continue;
								}else	{
									if (f_fail = !( pc = malloc( us_len + 1)))
										goto errJump;
									memcpy( pc, puc, us_len);
									pc[ us_len] = NULL;
								}
								if (ef_ListContainsName(  pc, pt_unrslvd) || 
													ef_ListContainsString( 
													pc, pt_rslvd, FALSE) || 
													ef_ListContainsString( 
													pc, pt_encntrd, FALSE))	{
									free( pc);
									pc = NULL;
									continue;
								} //if (ef_ListContainsName(

								//add the name to the members list for the 
								//	lookup to follow
								pt_node = pt_mbrs;
								if (f_fail = !ef_AddNameFoundNodeFifo( pc, 
														FALSE, TRUE, &pt_node))
									goto errJump;
								if (pt_node && !pt_mbrs)
									pt_mbrs = pt_node;
							} //for (us = 0; us < us_entries; us++)

							//if there's anyone in the group...
							if (pt_mbrs)	{
								UINT  ui_added;

								//add the group name to the groups-encountered 
								//	list so we can guard against infinite 
								//	recursion
								if (f_fail = !ef_AddStringNode( 
													pt->pt_name->pc_nm, TRUE, 
													TRUE, &pt_lastEncntrd))
									goto errJump;
								if (!pt_encntrd)
									pt_encntrd = pt_lastEncntrd;

								//run the member list through the resolution 
								//	process
								if (us_err = eus_ResolveMailAddresses( pt_mbrs, f_LKP_EXHST, 
													pc_SVRNM, &pt_encntrd, 
													&pt_rslvd, &ui_added, 
													f_ambg ? NULL : &f_ambg))
									goto errJump;
								ui_rslvd += ui_added;

								//add any member names not resolved to the 
								//	unresolved list
								pt_node = pt_mbrs;
								do	{
									if (pt_node->pt_name->f_found)
										continue;

									pt_nde = pt_unrslvd;
									if (f_fail = !ef_AddNameFoundNodeFifo( 
													pt_node->pt_name->pc_nm, 
													FALSE, TRUE, &pt_nde))
										goto errJump;
									if (pt_nde)
										pt_node->pt_name->pc_nm = NULL;
								} while (pt_node = pt_node->pt_next);

								e_FreeNameFoundList( &pt_mbrs);
							} //if (pt_mbrs)

							//mark the name as having been found
							pt->pt_name->f_found = TRUE;
						} //if (f_okType)
					} //if (pc)
				} //if (pc)
			} //if (pc)
		} //for (ul = 0; ul < us

		pt = pt->pt_next;
	} //while (pv_entry = NAMELocateNextName(

	*ppt_rslvd = pt_rslvd;
	if (pui_rslvd)
		*pui_rslvd = ui_rslvd;
	if (pf_ambg)
		*pf_ambg = f_ambg;

errJump:
	//free the buffer returned by NAMELookup(), if any
	if (h)	{
		OSUnlockObject( h);
		OSMemFree( h);
	}

	//if anything went wrong...
	if (us_err || f_fail)	{
		//free all resources allocated for a group-members list or, if begun by 
		//	this procedure, the resolved or groups-completed lists
		if (pt_mbrs)
			e_FreeList( &pt_mbrs, TRUE);
		if (f_NullAtStart_RslvdList && pt_rslvd)
			e_FreeList( &pt_rslvd, TRUE);
		else if (f_NullAtStart_EncntrdList && pt_encntrd)
			e_FreeList( &pt_encntrd, TRUE);
	//else free the groups-completed list if the caller doesn't want it and if 
	//	necessary
	}else if (!ppt_encntrd && pt_encntrd)
		e_FreeList( &pt_encntrd, TRUE);

	return us_err + f_fail;
} //eus_ResolveMailAddresses(


/** e_FreeNameFoundList( ***
Purpose is to free all memory resources allocated to the specified linked 
list of name-information nodes.

--- parameter -----------
ppt_head: pointer to the name list whose memory resources should be freed

--- revision history ----
8/21/03 PR: safety enhancement: free name string conditionally
9/12/99 PR: created			*/
void e_FreeNameFoundList( NameFoundNode * *const  ppt_head)	{
	NameFoundNode * pt;

	_ASSERTE( ppt_head && *ppt_head);

	//as needed, free the "name" component of the name-information structure
	pt = *ppt_head;
	do
		if (pt->pt_name->pc_nm)
			free( pt->pt_name->pc_nm);
	while (pt = pt->pt_next);

	//free the list nodes
	e_FreeList( ppt_head, TRUE);
} //e_FreeNameFoundList(


/** ef_AddNameFoundNodeFifo( ***
Purpose is to append a given name-found node to the end of a given list, if 
necessary.

--- parameters & return -----
pc: pointer to the name to append to the list if it's not already there
f_COPY: flag telling whether a copy of the name (pc) should be stored, instead 
	of the name string itself
f_UNIQUE: flag telling whether the given name should be unique (TRUE) to the 
	list. Comparisons are done on a case-insensitive basis
ppt: Input & Output. Pointer to a node pointer in the list to be appended to. 
	If the name the name to be added must be unique (see f_UNIQUE), the 
	pointer must refer to the list's _head_ node. If a node is added, the 
	node pointer returned will point to the added node, else the node pointer 
	will be null to indicate that the name was /not unique to the list.
RETURN: TRUE if no error occurred; FALSE if memory is insufficient to add the 
	string node

--- revision history -------
3/14/03 PR
+ support of change to ef_AddListNodeFifo() function, renamed to 
  ef_AddListNode()
+ listing format adjustment

9/12/99 PR: created			*/
BOOL ef_AddNameFoundNodeFifo( char *const  pc, 
								const BOOL  f_COPY, 
								const BOOL  f_UNIQUE, 
								NameFoundNode * *const  ppt_nd)	{
	NameFoundNode * pt_node;
	NameFoundInfo * pt;

	if (!( pc && ppt_nd))
		return FALSE;

	pt_node = *ppt_nd;
	if (f_UNIQUE)	{
		*ppt_nd = NULL;
		if (ef_ListContainsName( pc, pt_node))
			return TRUE;
	}else
		pt_node = *ppt_nd;

	if (!( pt = calloc( 1, sizeof( NameFoundInfo))))
		return FALSE;
	if (f_COPY)	{
		if (!( pt->pc_nm = malloc( strlen( pc) + 1)))
			goto errJump;
		strcpy( pt->pc_nm, pc);
	}else
		pt->pc_nm = pc;

	//according to the caller's request, either transfer control of the string 
	//	to the just-created node or allocate new space for the string and copy 
	//	it in
	if (!ef_AddListNode( pt, TRUE, &pt_node))
		goto errJump;
	if (f_COPY)
		strcpy( pt->pc_nm, pc);

	*ppt_nd = pt_node;

	return TRUE;

errJump:
	if (f_COPY)
		free( pt->pc_nm);
	free( pt);

	return FALSE;
} //ef_AddNameFoundNodeFifo(


/** ef_ListContainsName( ***
Returns whether a name-found list includes a given name. Names are always 
treated case-insensitively.

--- parameters & return ----
PC: pointer to the name to be sought for within the list
pt: pointer to the head node of the list to be searched
RETURN: TRUE if list contains name; FALSE otherwise

--- revision history -------
9/12/99 PR: created			*/
BOOL ef_ListContainsName( const char  PC[], 
							const NameFoundNode * pt)	{
	char * pc;

	if (!( PC && pt))
		return FALSE;

	do
		if (pc = pt->pt_name->pc_nm)
			if ( stricmp( PC, pc) == ei_SAME)
				return TRUE;
	while (pt = pt->pt_next);

	return FALSE;	
} //ef_ListContainsName(


