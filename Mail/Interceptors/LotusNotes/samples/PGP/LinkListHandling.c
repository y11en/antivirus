/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: LinkListHandling.c 15584 2003-07-15 23:46:16Z pryan $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Generic linked-list handling.

--- revision history --------
6/26/03 Version 1.2.7 Paul Ryan
+ added ef_AddListNodeUnique(), f_contentInList(), ef_listToArray(), 
  ul_ctntElements(), ef_listToArray()

3/14/03 Version 1.2.5 Paul Ryan
+ created ef_nodeInList()
+ overhauled ef_AddListNodeFifo() to add and manage FIFO/LIFO input; procedure 
  name changed o ef_AddListNode()

+ minor documentation adjustment, listing format adjustment

8/26/99: Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

//system includes
#include <crtdbg.h>		//for _ASSERTE(), etc.
#include <stdlib.h>		//for malloc(), free(), etc.
#include <memory.h>		//for memcmp(), memcpy()

//local includes
#include "PstGlobals.h"	//for ei_SAME

typedef struct _Node	{
	void * pv;
	struct _Node * pt_next;
}  Node;

typedef enum	{
	FALSE, 
	TRUE
}  BOOL;
typedef unsigned long  ULONG;
typedef unsigned char  BYTE;

static BOOL f_contentInList( void *const, const ULONG, Node * *const);
static __inline ULONG ul_ctntElements( const Node *);


/** ef_AddListNode( ***
Allocate and add a new content node onto a given list, starting that list if 
necessary and in a LIFO or FIFO manner as requested by caller.

--- parameters & return ----
pv: Address of content to associate with the new node to be formed (thus 
	transferring ownership to that node). This content should normally have 
	been formed on the heap.
f_FIFO: whether the node should be appended to the end of the list (FIFO, TRUE) 
	or inserted at the beginning of the list (LIFO, FALSE)
ppt_nd: Input & Output. Address of pointer to list's head-node instance and 
	thus potentially to a list of such instances. Procedure allocates and 
	adds a new node onto the list. If pointer is null, the new node formed by 
	procedure will become the list's head node and the pointer reset to its 
	address in memory. On output, pointer is set to the address of the newly 
	created node.
RETURN: TRUE if no error occurred; FALSE if any input is obviously invalid or 
	if memory could not be allocated on the heap.

--- revision history -------
6/26/03 PR: minor documentation adjustment

3/14/03 PR
+ overhauled to add and manage FIFO/LIFO input; procedure name changed from 
  ef_AddListNodeFifo() to ef_AddListNode()
+ completed standard documentation

8/26/99 PR: created			*/
BOOL ef_AddListNode( void *const  pv, 
						const BOOL  f_FIFO, 
						Node * *const  ppt_nd)	{
	Node * pt_nd;

	if (!( pv && ppt_nd))
		return FALSE;

	if (!( pt_nd = calloc( 1, sizeof( Node))))
		return FALSE;
	if (*ppt_nd)
		if (f_FIFO)	{
			//move to the last node in the list
			Node * pt_nd_ = *ppt_nd;
			while( pt_nd_->pt_next)
				pt_nd_ = pt_nd_->pt_next;
			pt_nd_->pt_next = pt_nd;
		}else
			pt_nd->pt_next = *ppt_nd;
	pt_nd->pv = pv;
	*ppt_nd = pt_nd;

	return TRUE;
} //ef_AddListNode(


/** ef_AddListNodeUnique( ***
If node's content is already in the list, allocate and add a new content node 
onto a given list, starting that list if necessary and in a LIFO or FIFO manner 
as requested by caller.

--- parameters & return ----
pv: Address of content to associate with the new node to be formed (thus 
	transferring ownership to that node). This content should normally have 
	been formed on the heap.
f_FIFO: whether the node should be appended to the end of the list (FIFO, TRUE) 
	or inserted at the beginning of the list (LIFO, FALSE)
UL: length of the content to compare for uniqueness against the other nodes in 
	the list
pf: Optional Output. Address of flag variable to receive whether the content 
	was added to the list (because it was unique) or was not (because it was 
	not unique). If null, output is suppressed.
ppt_nd: Input & Output. Address of pointer to list's head-node instance and 
	thus potentially to a list of such instances. Procedure allocates and 
	adds a new node onto the list. If pointer is null, the new node formed by 
	procedure will become the list's head node and the pointer reset to its 
	address in memory. If pointer is not null and an exact copy of the content 
	already appears in the list, pointer is reset to the address of the copy in 
	the list. If pointer is not null and no copy is present, pointer is reset 
	to address the new node created to add the content to the list.
RETURN: TRUE if no error occurred; FALSE if any input is obviously invalid or 
	if memory could not be allocated on the heap.

--- revision history -------
6/26/03 PR: created			*/
BOOL ef_AddListNodeUnique( void *const  pv, 
							const BOOL  f_FIFO, 
							const ULONG  UL, 
							BOOL *const  pf, 
							Node * *const  ppt_nd)	{
	Node * pt_nd, * pt_nd_;

	if (!( pv && ppt_nd && UL))
		return FALSE;

	if (pf)
		*pf = FALSE;

	//if the content is already in the list, short-circuit
	if (pt_nd_ = *ppt_nd)
		if (f_contentInList( pv, UL, &pt_nd_))
			return TRUE;

	//if the list has been started...
	if (!( pt_nd = calloc( 1, sizeof( Node))))
		return FALSE;
	if (*ppt_nd)
		//if the content should be at the end of the list...
		if (f_FIFO)
			//position the new node there
			pt_nd_->pt_next = pt_nd;
		//else position it at the beginning of the list
		else
			pt_nd->pt_next = *ppt_nd;

	//set the node's content and point caller to the new node
	pt_nd->pv = pv;
	*ppt_nd = pt_nd;
	if (pf)
		*pf = TRUE;

	return TRUE;
} //ef_AddListNodeUnique(


/** f_contentInList( ***
Tell whether an exact copy of the specified content appears in a given list.

--- parameters & return ----
pv: Address of the content to use in checking for an exact copy
UL: length of the content
ppt_nd: Input & Output. Pointer to the address of the head node of the list to 
	check. If an exact copy is found, address is reset to point to the node 
	containing the copy. If no copy is found, address is reset to point the 
	tail node of the list.
RETURN: TRUE if an exact copy appears in the list; FALSE if not

--- revision history -------
6/26/03 PR: created			*/
static BOOL f_contentInList( void *const  pv, 
								const ULONG  UL, 
								Node * *const  ppt_nd)	{
	Node * pt_nd, * pt_nd_;

	_ASSERTE( pv && UL && ppt_nd);

	//if the list is empty, short-circuit that no copy is present
	if (!( pt_nd = *ppt_nd))
		return FALSE;

	//for each node in the list...
	do	{
		//if the content of the node is an exact copy, short-circuit that one 
		//	is present and here's where it is
		if (pt_nd->pv && memcmp( pt_nd->pv, pv, UL) == ei_SAME)	{
			*ppt_nd = pt_nd;
			return TRUE;
		}
		pt_nd_ = pt_nd;
	} while (pt_nd = pt_nd->pt_next);

	//tell caller that no copy is present, and here's where the list ends
	*ppt_nd = pt_nd_;
	return FALSE;
} //f_contentInList(


/** ef_nodeInList( ***
Tells whether a specified node is present in the given list.

--- parameters & return ----
PT: address of node to search for
pt_nd: address of the head node of the list to be searched
RETURN: TRUE if node is found in the list; FALSE if not or if any input is 
	obviously invalid

--- revision history -------
3/14/03 PR: created			*/
BOOL ef_nodeInList( const Node *const  PT, 
					const Node * pt_nd)	{
	if (!( PT && pt_nd))
		return FALSE;

	do
		if (PT == pt_nd)
			return TRUE;
	while (pt_nd = pt_nd->pt_next);

	return FALSE;
} //ef_nodeInList(


/** e_FreeList( ***
Free the resources allocated to the given linked-list.

--- parameters ----------
ppt: Input & Output. Address of the head node of the list to be freed. The head 
	node is set to NULL by this procedure, a safety aid for the caller to help 
	ensure that the list is not reused.
f_CONTENTS_TOO: flag telling whether the contents of each node in the list 
	should be freed as well as the nodes themselves

--- revision history ----
3/14/03 PR: listing format adjustment, minor documentation adjustment
8/26/99 PR: created		*/
void e_FreeList( Node * * ppt, 
					const BOOL  f_CONTENTS_TOO)	{
	Node * pt, * pt_next;

	if (!( ppt && *ppt))
		return;

	pt = *ppt;
	do	{
		//get a handle on the next node before freeing the current node
		pt_next = pt->pt_next;

		if (f_CONTENTS_TOO && pt->pv)
			free( pt->pv);
		free( pt);
	} while (pt = pt_next);

	*ppt = NULL;
} //e_FreeList(


/** ef_listToArray( ***
Creates an array of the contents of the list.

--- parameters & return ----
pt_nd: address of head node of list to convert
UL: length of the contents of each node in the list
ppv: Output. Address of pointer to receive the address of the array created by 
	the procedure. Caller is responsible for ultimately using the pointer to 
	release the memory resource allocated to hold the array.
pul: Output. Address of variable to receive the number of elements in the array.
RETURN: TRUE if the array was successfully created; FALSE if any argument was 
	obviously invalid or if the system seems to have run out of memory

--- revision history -------
6/26/03 PR: created			*/
BOOL ef_listToArray( const Node * pt_nd, 
						const ULONG  UL, 
						void * *const  ppv, 
						ULONG *const  pul)	{
	ULONG  ul_len, ul = 0;
	BYTE * puc;

	if (!( pt_nd && UL && ppv && pul && (ul_len = ul_ctntElements( pt_nd))))
		return FALSE;

	if (!( puc = malloc( ul_len * UL)))
		return FALSE;
	do
		if (pt_nd->pv)
			memcpy( puc + ul++ * UL, pt_nd->pv, UL);
	while (pt_nd = pt_nd->pt_next);

	*ppv = puc;
	*pul = ul_len;

	return TRUE;
} //ef_listToArray(


/** ul_ctntElements( ***
Tells number of list elements that seemingly contain content. "Seemingly" here 
simply means that a node with a null content pointer is deemed not to contain 
content.

--- parameter & return ----
pt_nd: address of head node to list to count
RETURN: the number of nodes that seemingly contain content

--- revision history ------
6/26/03 PR: created			*/
static __inline ULONG ul_ctntElements( const Node * pt_nd)	{
	ULONG  ul = 0;

	while (pt_nd)	{
		if (pt_nd->pv)
			ul++;
		pt_nd = pt_nd->pt_next;
	}

	return ul;
} //ul_ctntElements(


