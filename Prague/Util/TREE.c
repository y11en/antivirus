// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  07 March 2002,  12:03 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- _this.c

/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	26.02.2003 22:06:39
  Comments:	
	Fixed:		В релизной версии была возможна ситуация, когда ф-ии Tree_Next, Tree_Prev, 
				Tree_AtLeast, Tree_AtMost возвращали некорректный указатель на tNODE.

 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	11.12.2002 18:40:57
  Comments:	
	Fixed:		Возврат статуса errEND_OF_LIST в ф-ях Tree_Prev, Tree_Next, Tree_Last,
				Tree_First, Tree_PrevDup, Tree_NextDup.

				Возврат статуса errNOT_FOUND в ф-ях Tree_Search, Tree_AtMost, Tree_AtLeast, 
				Tree_Prev, Tree_Next.

 ************************************/





#define Tree_PRIVATE_DEFINITION

#include <Prague/prague.h>
#include <Prague/iface/i_compar.h>
#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_root.h>

#include <memory.h>

#include "TREE.h"
#include "ltree.h"
#include "list.h"
#include "stack.h"
#include "lru.h"

#define AVL_DEPTH 32
#define AVL_LEFT     -1
#define AVL_BALANCED  0
#define AVL_RIGHT     1

#define AVL_NEXT 0
#define AVL_PREV -1





// AVP Prague stamp begin( Tree, Interface comment )
// --- 63f46c61_4d12_11d4_b94e_008048ec2fc7 ---
// Tree interface implementation
// Short comments -- _this sorting and searching interface
// Extended comment
//
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Tree_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "TREE.h"
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Tree interface implementation
// Short comments -- _this sorting and searching interface
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Tree. Inner data structure
  // PLEASE REGENERATE HEADER FILE FOR THIS IMPLEMENTATION
//!typedef struct tag_TreeData{
//!	tUINT          granularity;         // --
//!	tBOOL          dup_allowed;         // --
//!	tBOOL          remove_allowed;      // --
//!	hCOMPARE       compare_object;      // --
//!	tUINT          node_size;           // --
//!	struct sNODE * root;                // --
//!	hHEAP          heap;                // --
//!	hCOMPARE       compare_object_real; // --
//!} TreeData;
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
//!typedef struct tag_hi_Tree{
  // PLEASE REGENERATE HEADER FILE FOR THIS IMPLEMENTATION
//!	const iTreeVtbl*   vtbl; // pointer to the "Tree" virtual table
//!	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
//!	TreeData*         data;   // pointer to the "Tree" data structure
//!} *hi_Tree;
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpTree_Add)         ( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size ); // -- Add new element;
typedef   tERROR (pr_call *fnpTree_Remove)      ( hi_Tree _this, tTREENODE node );        // -- Remove node from _this;
typedef   tERROR (pr_call *fnpTree_Search)      ( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size ); // -- Search node equil to element;
typedef   tERROR (pr_call *fnpTree_AtLeast)     ( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size ); // -- Search node equil or next greater to given element;
typedef   tERROR (pr_call *fnpTree_AtMost)      ( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size ); // -- Search node equil or less to element;
typedef   tERROR (pr_call *fnpTree_First)       ( hi_Tree _this, tTREENODE* result );                        // -- find first node in _this;
typedef   tERROR (pr_call *fnpTree_Last)        ( hi_Tree _this, tTREENODE* result );                        // -- find last node in _this;
typedef   tERROR (pr_call *fnpTree_Next)        ( hi_Tree _this, tTREENODE* result, tTREENODE node );        // -- return next node in _this exluding dups;
typedef   tERROR (pr_call *fnpTree_Prev)        ( hi_Tree _this, tTREENODE* result, tTREENODE node );        // -- return previous node in _this excluding dups;
typedef   tERROR (pr_call *fnpTree_NextDup)     ( hi_Tree _this, tTREENODE* result, tTREENODE node );        // -- return next node equil to given one in _this;
typedef   tERROR (pr_call *fnpTree_PrevDup)     ( hi_Tree _this, tTREENODE* result, tTREENODE node );        // -- return prev node equil to given one in _this;
typedef   tERROR (pr_call *fnpTree_NodeDataGet) ( hi_Tree _this, tDWORD* result, tTREENODE node, tPTR buffer, tDWORD size ); // -- Retrieve node data ( previously stored element);
typedef   tERROR (pr_call *fnpTree_NodeDataSet) ( hi_Tree _this, tTREENODE* result, tTREENODE node, tPTR buffer, tDWORD size ); // -- Set node data;
typedef   tERROR (pr_call *fnpTree_Clear)       ( hi_Tree _this );                        // -- ;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Tree_ObjectInitDone( hi_Tree _this );
// Interface "Tree". Internal method table. Forward declarations
//!static tERROR Tree_ObjectInit( hi_Tree _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Tree_vtbl =
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       NULL,
	(tIntFnObjectInitDone)   Tree_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// Interface "Tree". Internal method table.
//!  (tIntFnObjectInit)       Tree_ObjectInit,
//!  (tIntFnObjectInitDone)   NULL,
//!  (tIntFnIFaceTransfer)    NULL,
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
//!}; // Tree
struct iTreeVtbl
{
	fnpTree_Add          Add;
	fnpTree_Remove       Remove;
	fnpTree_Search       Search;
	fnpTree_AtLeast      AtLeast;
	fnpTree_AtMost       AtMost;
	fnpTree_First        First;
	fnpTree_Last         Last;
	fnpTree_Next         Next;
	fnpTree_Prev         Prev;
	fnpTree_NextDup      NextDup;
	fnpTree_PrevDup      PrevDup;
	fnpTree_NodeDataGet  NodeDataGet;
	fnpTree_NodeDataSet  NodeDataSet;
	fnpTree_Clear        Clear;
//!struct iTreeVtbl{
}; // "Tree" external virtual table prototype
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
// Interface "Tree". External method table. Forward declarations
tERROR pr_call Tree_Add( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size );
tERROR pr_call Tree_Remove( hi_Tree _this, tTREENODE node );
tERROR pr_call Tree_Search( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size );
tERROR pr_call Tree_AtLeast( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size );
tERROR pr_call Tree_AtMost( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size );
tERROR pr_call Tree_First( hi_Tree _this, tTREENODE* result );
tERROR pr_call Tree_Last( hi_Tree _this, tTREENODE* result );
tERROR pr_call Tree_Next( hi_Tree _this, tTREENODE* result, tTREENODE node );
tERROR pr_call Tree_Prev( hi_Tree _this, tTREENODE* result, tTREENODE node );
tERROR pr_call Tree_NextDup( hi_Tree _this, tTREENODE* result, tTREENODE node );
tERROR pr_call Tree_PrevDup( hi_Tree _this, tTREENODE* result, tTREENODE node );
tERROR pr_call Tree_NodeDataGet( hi_Tree _this, tDWORD* result, tTREENODE node, tPTR buffer, tDWORD size );
tERROR pr_call Tree_NodeDataSet( hi_Tree _this, tTREENODE* result, tTREENODE node, tPTR buffer, tDWORD size );
tERROR pr_call Tree_Clear( hi_Tree _this );
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iTreeVtbl e_Tree_vtbl =
{
	Tree_Add,
	Tree_Remove,
	Tree_Search,
	Tree_AtLeast,
	Tree_AtMost,
	Tree_First,
	Tree_Last,
	Tree_Next,
	Tree_Prev,
	Tree_NextDup,
	Tree_PrevDup,
	Tree_NodeDataGet,
	Tree_NodeDataSet,
	Tree_Clear
};
// Interface "Tree". External method table.
// AVP Prague stamp end






// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// Interface "Tree". Get/Set property methods
tERROR pr_call Tree_PROP_GranularitySet( hi_Tree _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Tree_PROP_CompareSet( hi_Tree _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Tree". Global(shared) property table variables
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// Interface "Tree". Property table
mPROPERTY_TABLE(Tree_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ((tVERSION)(1)) )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Tree manipulating interface", 28 )
	mLOCAL_PROPERTY_BUF( pgTREE_DUP_ALLOWED, TreeData, dup_allowed, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY( pgTREE_GRANULARITY, TreeData, granularity, cPROP_BUFFER_READ_WRITE, NULL, Tree_PROP_GranularitySet )
	mLOCAL_PROPERTY_BUF( pgTREE_NODE_SIZE, TreeData, node_size, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY( pgTREE_COMPARE_OBJECT, TreeData, compare_object, cPROP_BUFFER_READ_WRITE, NULL, Tree_PROP_CompareSet )
mPROPERTY_TABLE_END(Tree_PropTable)
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "Tree". Register function
tERROR pr_call Tree_Register( hROOT root )
{
	tERROR error;

	PR_TRACE_A0( root, "Enter \"Tree::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TREE,                               // interface identifier
		PID_UTIL,                               // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000001,                             // interface version
		VID_ANDREW,                             // interface developer
		&i_Tree_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Tree_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Tree_vtbl,                           // external function table
		(sizeof(e_Tree_vtbl)/sizeof(tPTR)),     // external function table size
		Tree_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Tree_PropTable),   // property table size
		sizeof(TreeData),                       // memory size
		IFACE_SYSTEM                            // interface flags
	);

  #ifdef _PRAGUE_TRACE_
    if ( error != errOK )
		if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"Tree(IID_TREE) registered [error = 0x%08X]",error) );
  #endif

  PR_TRACE_A1( root, "Leave \"Tree::Register\" method, ret tERROR = 0x%08X", error );
	return error;
}
// AVP Prague stamp end






typedef struct
{
    tPTR data;                          /* data to add/remove */
    tUINT count;                      /* size of data */
    hi_Tree _this;                       /* _this root */
    struct sNODE **nodeptr;             /* pointer to current node */
    signed char  *pathptr;
    struct sNODE *node[AVL_DEPTH];      /* node path */
    signed char  path[AVL_DEPTH];       /* balance path */
    tERROR (*compare_fun)(hCOMPARE, tINT*, void *, tUINT, void *, tUINT);
    hCOMPARE compare_obj;
    int  level;                         /* current level */
    tDWORD granularity;                 /* saved granularity */
} tTREEDATA;



typedef struct sNODE
{
    struct  sNODE *left;
    struct  sNODE *child[2];
    tUINT count;
    tSBYTE  balance;
    tSBYTE  mode;
} thNODE, *tNODE   ;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GranularitySet )
// Interface "Tree". Method "Set" for property(s):
//  -- TREE_GRANULARITY
tERROR pr_call Tree_PROP_GranularitySet( hi_Tree _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter *SET* method \"Tree_PROP_GranularitySet\" for property \"pgTREE_GRANULARITY\"" );

    (void)prop;

    if ( _this->data->heap != NULL )
    {
        error = _this->data->heap->sys->PropertySet((hOBJECT)(_this->data->heap),(tDWORD*)out_size,pgHEAP_GRANULARITY,buffer,size);
    }
    else
    {
        _this->data->granularity = *((tDWORD*)(buffer));
    }

    if ( out_size )
    {
        *out_size = sizeof(tDWORD);
    }

    PR_TRACE_A2( _this, "Leave *SET* method \"Tree_PROP_GranularitySet\" for property \"pgTREE_GRANULARITY\", ret tUINT = %u(size), error = 0x%08X", *out_size, error );
	return error;
}
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, CompareSet )
// Interface "Tree". Method "Set" for property(s):
//  -- TREE_COMPARE_OBJECT
tERROR pr_call Tree_PROP_CompareSet( hi_Tree _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tUINT rcode;
	tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter *SET* method \"Tree_PROP_CompareSet\" for property \"pgTREE_COMPARE_OBJECT\"" );


    (void)prop;
    (void)buffer;
    (void)size;

    rcode = 0;

    if ( _this->data->root == NULL )
    {
        if ( _this->data->compare_object != NULL )
        {
            if ( _this->sys->ObjectCheck((hOBJECT)_this, (hOBJECT)(_this->data->compare_object),IID_COMPARE,PID_ANY,SUBTYPE_ANY,cTRUE) == errOK )
            {
                _this->data->compare_object_real = _this->data->compare_object;
                rcode = sizeof(hOBJECT);
            }
            else
            {
                _this->data->compare_object_real = NULL;
                error = errINTERFACE_INCOMPATIBLE;
            }
        }
        else
        {
            _this->data->compare_object_real = NULL;
            rcode = sizeof(hOBJECT);
        }
    }
    else
    {
        error = errPROPERTY_INVALID;
    }
    *out_size = rcode;

    PR_TRACE_A2( _this, "Leave *SET* method \"Tree_PROP_CompareSet\" for property \"pgTREE_COMPARE_OBJECT\", ret tUINT = %u(size), error = 0x%08X", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:

tERROR pr_call Tree_ObjectInitDone( hi_Tree _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter \"Tree::ObjectInitDone\" method" );

	// Place your code here
	error = CALL_SYS_PropertyGet(_this, NULL, pgOBJECT_HEAP, &(_this->data->heap), sizeof(_this->data->heap));
    if (PR_FAIL(error) || _this->data->heap == NULL)
	{
		error = CALL_SYS_ObjectCreateQuick((hOBJECT)(_this),&(_this->data->heap),IID_HEAP,PID_ANY,SUBTYPE_ANY);

		if (PR_FAIL(error))
		{
			error = errOBJECT_NOT_FOUND;
		}
		/* Graf - property not writable....
        else
        {
            if ( _this->data->granularity )
            {
                _this->data->heap->sys->PropertySetDWord((hOBJECT)(_this->data->heap),pgHEAP_GRANULARITY,_this->data->granularity);
            }
        }
		*/
	}


	PR_TRACE_A1( _this, "Leave \"Tree::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



static tNODE NodeValid(hi_Tree _this, tTREENODE node)
{
    (void)_this;

    return (tNODE)(node);
}

static tERROR CompareDefault(hCOMPARE cmp,tINT *result,tPTR data1, tUINT size1, tPTR data2, tUINT size2)
{
    tDWORD d1;
    tDWORD d2;

    (void)cmp;
    (void)size1;
    (void)size2;

    d1 = *((tDWORD*)(data1));
    d2 = *((tDWORD*)(data2));

    if ( d1==d2 )
    {
        *result = 0;
    }
    else if ( d1 < d2 )
    {
        *result = -1;
    }
    else
    {
        *result = 1;
    }

    return errOK;
}

/*
 * internal TreeSearch
 *
 * searches for given data in _this
 * return node  ( NULL - node not found)
 *
 * save path in data
 *
 */
static tNODE __fastcall __TreeSearch(tNODE node,tTREEDATA *data)
{
    int res;

    while ( node != NULL )
    {
        data->compare_fun(data->compare_obj,&res,data->data,data->count,(tPTR)(node+1),node->count);
        if ( res == AVL_BALANCED )
        {
            break;
        }
        *data->nodeptr++ = node;   /* from this node */
        *data->pathptr++ = (tSBYTE)res;    /* on this side */
        data->level++;
        node = node->child[res];
    }
    return node;
}

static tNODE Alloc(hi_Tree _this, tPTR data, tUINT count)
{
    tNODE node;



    if ( errOK == _this->data->heap->vtbl->Alloc(_this->data->heap,(tVOID*) &node,count+sizeof(thNODE)))
    {
        node->child[AVL_LEFT] = NULL;
        node->child[AVL_BALANCED] = NULL;
        node->child[AVL_RIGHT] = NULL;
        node->balance = AVL_BALANCED;
        node->count = count;
        node->mode = 0;
        memcpy(((tBYTE*)(node))+sizeof(thNODE),data,count);
    }

    return node;
}

static tNODE _TreeSearch(hi_Tree _this, tTREEDATA *tdata, tPTR data, tUINT count)
{
    tdata->data = data;
    tdata->count = count;
    //tdata->error = 0;
    tdata->level = 0;
    tdata->_this = _this;
    tdata->nodeptr = tdata->node;
    tdata->pathptr = tdata->path;

    if ( _this->data->compare_object_real == NULL )
    {
        tdata->compare_fun = CompareDefault;
        if ( count < sizeof(tDWORD) )
        {
//            _this->sys->ErrorCodeSet((hOBJECT)(_this),errBUFFER_TOO_SMALL);
            return NULL;
        }
    }
    else
    {
        tdata->compare_fun = _this->data->compare_object_real->vtbl->Compare;
        tdata->compare_obj = _this->data->compare_object_real;
    }

    return __TreeSearch(_this->data->root,tdata);
}

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Add )
// --- Interface "Tree". Method "Add"
// Extended method comment
//   Add new element into _this. If equil element alredy present in _this functiuon behaviour depends on TREE_DUP_ALLOWED property state. If property value is cTRUE element will be added iat the end of special list else function fails ( return cNODE_NULL). Property TREE_DUP_ALLOWED can be changed any time. Function behaviour will be chanded accordingly
// Result comment
//   magic number  of added none or cNODE_NULL if failed
//!tERROR pr_call Tree_Add( hi_Tree _this, tTREENODE* result, tPTR data, tUINT count){
tERROR pr_call Tree_Add( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD count )
{
    tNODE node;
    tNODE new_node;
    tNODE *tnode;
    tNODE left, right;
    signed char *tpath, res;
    tTREEDATA tdata;

    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::Add\" method" );


    node = _TreeSearch(_this,&tdata,data,count);

    if ( node == NULL )
    {
        int taller;
        /* node not found */

        /* add new node in level position */


        if ( (node = new_node = Alloc(_this,data,count)) == NULL )
        {
            error = errNOT_ENOUGH_MEMORY;
            goto abort;
        }


        /* balance up _this */

        tdata.level--;
        tnode = tdata.node+tdata.level;
        tpath = tdata.path+tdata.level;
        taller = 1;

        while ( tdata.level >= 0 )
        {
            /* balance _this from this node */
            (*tnode)->child[*tpath] = node;
            res = *tpath;

            node = *tnode;

            if ( taller )
            {
                switch ( node->balance * res )
                {
                    case AVL_LEFT:
                        /* same side */
                        node->balance = AVL_BALANCED;
                        taller = 0;
                        break;

                    case AVL_BALANCED:
                        node->balance = res;
                        break;

                    case AVL_RIGHT:

                        /* different sides, rebalance _this */

                        left = node->child[res];

                        switch ( left->balance *(-res) )
                        {
                            case AVL_LEFT:
                                node->balance = left->balance = AVL_BALANCED;
                                /* Rotate right */
                                node->child[res] = left->child[-res];
                                left->child[-res] = node;
                                node = left;
                                taller = 0;
                                break;

                            case AVL_BALANCED:
                                /* balance error */
                                //printf("BALANCE ERROR\n");
                                //tdata.error = -2;
                                break;

                            case AVL_RIGHT:
                                right = left->child[-res];
                                /* was without (-res) */
                                switch ( right->balance * res )
                                {
                                    case AVL_RIGHT:
                                        node->balance = (tSBYTE)(-res);
                                        left->balance = AVL_BALANCED;
                                        break;

                                    case AVL_BALANCED:
                                        node->balance = left->balance = AVL_BALANCED;
                                        break;

                                    case AVL_LEFT:
                                        node->balance = AVL_BALANCED;
                                        /* was '-res' */
                                        left->balance = res;
                                        break;
                                }
                                right->balance = AVL_BALANCED;
                                left->child[-res] = right->child[res];
                                right->child[res] = left;

                                node->child[res] = right->child[-res];
                                right->child[-res] = node;

                                node = right;
                                taller = 0;
                                break;
                        }
                        break;
                }
            }

            tnode--;
            tpath--;
            tdata.level--;
        }
        _this->data->root = node;
    }
    else
    {
        if ( _this->data->dup_allowed )
        {
            if ( (new_node = Alloc(_this,data,count)) == NULL )
            {
				error = errNOT_ENOUGH_MEMORY;
                goto abort;
            }

            while ( node->child[AVL_NEXT] != NULL )
            {
                node = node->child[AVL_NEXT];
            }

            // add node to the end of list
            node->child[AVL_NEXT] = new_node;
            new_node->child[AVL_PREV] = node;
            new_node->mode = 1;

        }
        else
        {
            new_node = NULL;
			error = errOBJECT_ALREADY_EXISTS;
        }
    }
    abort:

	if ( result )
        *result = (tTREENODE)(new_node);
    PR_TRACE_A2( _this, "Leave \"Tree::Add\" method, ret tTREENODE = %p, error = 0x%08X", (tTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end

//#pragma optimize ("", off)


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Remove )
// --- Interface "Tree". Method "Remove"
// Extended method comment
//   Remove node from _this. Stored datat will lost
// Behaviour comment
//   previously stored node magic numbers (type tTREENODE) may become invalid
// Result comment
//   error if given node not found
//!tERROR pr_call Tree_Remove( hi_Tree _this, tTREENODE enode ){
tERROR pr_call Tree_Remove( hi_Tree _this, tTREENODE enode )
{
	tTREEDATA tdata;
    tNODE node;
    tNODE next;
    tERROR error;
    PR_TRACE_A0( _this, "Enter \"Tree::Remove\" method" );

	// Place your code here
    error = errOK;

    if ( (node = NodeValid(_this,enode)) != NULL )
    {
        if ( node->mode )
        {
            tNODE prev;

            // node in dup list
            prev = node->child[AVL_PREV] ;
            next = node->child[AVL_NEXT] ;

            prev->child[AVL_NEXT] =  next;

            if ( next != NULL )
            {
                next->child[AVL_PREV] =  prev;
            }
        }
        else
        {
            // node in _this itself

            // create path to node

            if ( _TreeSearch(_this,&tdata,(tPTR)(node+1),node->count) != node )
            {
                // _this is destroyed or damaged
                error = errUNEXPECTED;
            }
            else
            {
                if ( (next = node->child[AVL_NEXT]) != NULL )
                {
                    // duplicate list present

                    next->child[AVL_LEFT] = node->child[AVL_LEFT];
                    next->child[AVL_RIGHT] = node->child[AVL_RIGHT];
                    next->mode = 0;
                    next->balance = node->balance;

                    if ( tdata.level == 0 )
                    {
                        _this->data->root = next;
                    }
                    else
                    {
                        tdata.nodeptr--;
                        tdata.pathptr--;
                        (*tdata.nodeptr)->child[*tdata.pathptr] = next;
                    }
                    _this->data->heap->vtbl->Free(_this->data->heap,node);
                }
                else
                {
                    tNODE *tnode;
                    tNODE delnode;
                    tSBYTE *tpath;
                    tNODE left, right;
                    tSBYTE res;
                    int taller;



                    // duplicate list not present just remove node
                    if ( node->child[AVL_LEFT] != NULL && node->child[AVL_RIGHT] != NULL )
                    {
						tDWORD level = tdata.level;
                        /* goes right look for leftmost node */

                        /* save parent of removed node */
                        tnode = tdata.nodeptr-1;
                        tpath = tdata.pathptr-1;

                        /* step right */
                        *tdata.nodeptr++ = node;         /* from this node */
                        *tdata.pathptr++ = AVL_RIGHT;    /* on this side */
                        tdata.level++;
                        node = node->child[AVL_RIGHT];

                        /* goes left */
                        while ( node->child[AVL_LEFT] != NULL )
                        {
                            *tdata.nodeptr++ = node;   /* from this node */
                            *tdata.pathptr++ = AVL_LEFT;    /* on this side */
                            tdata.level++;
                            node = node->child[AVL_LEFT];
                        }

                        /* exchange nodes  node <-> *tnode */
                        /* node   - leftmost node in right _this */
                        /* *tnode - parent of node for delete */

                        if ( level == 0 )
                        {
                            _this->data->root = node;
                        }
                        else
                        {
                            (*tnode)->child[*tpath] = node;
                        }

                        /* get next node in list (node to delete) */
                        tnode++;

                        /* change left child */
                        node->child[AVL_LEFT] = (*tnode)->child[AVL_LEFT];
                        (*tnode)->child[AVL_LEFT] = NULL;

                        /* change right child */
                        delnode = node->child[AVL_RIGHT];
                        node->child[AVL_RIGHT] = (*tnode)->child[AVL_RIGHT];
                        (*tnode)->child[AVL_RIGHT] = delnode;

                        /* change node balance */
                        res = node->balance;
                        node->balance = (*tnode)->balance;
                        (*tnode)->balance = res;

                        /* change node pointer */
                        delnode = node;
                        node = *tnode;
                        *tnode = delnode;

                    }

                    /* check node's children */
                    delnode = node;
                    if ( node->balance == AVL_BALANCED )
                    {
                        /* no children */
                        node = NULL;
                    }
                    else
                    {
                        /* one child */
                        node = node->child[node->balance];
                    }

                    /* delallocate node */
                    _this->data->heap->vtbl->Free(_this->data->heap,delnode);

                    /* balance up _this */
                    tdata.level--;
                    tdata.nodeptr--;
                    tdata.pathptr--;
                    taller = 1;

                    while ( tdata.level-- >= 0 )
                    {
                        /* balance _this from this node */
                        (*tdata.nodeptr)->child[*tdata.pathptr] = node;

                        res  = *tdata.pathptr--;
                        node = *tdata.nodeptr--;

                        if ( taller )
                        {
                            switch ( node->balance * res )
                            {
                                case AVL_LEFT:
                                    /* different sides */
                                    right = node->child[-res];

                                    switch ( right->balance * res )
                                    {
                                        case AVL_LEFT:
                                            /* other child on other side */
                                            node->balance = right->balance = AVL_BALANCED;

                                            /* single rotation */
                                            node->child[-res] = right->child[res];
                                            right->child[res] = node;
                                            node = right;
                                            break;

                                        case AVL_BALANCED:
                                            /* other child is balanced */
                                            node->balance = (tSBYTE)-res;
                                            right->balance = res;
                                            taller = 0;

                                            /* single rotation */
                                            node->child[-res] = right->child[res];
                                            right->child[res] = node;
                                            node = right;
                                            break;

                                        case AVL_RIGHT:
                                            /* other child on same side */
                                            left = right->child[res];
                                            switch ( left->balance * res )
                                            {
                                                case AVL_LEFT:
                                                    node->balance = (tSBYTE)res;    //03.09.00 13:51
                                                    right->balance = AVL_BALANCED;
                                                    break;

                                                case AVL_BALANCED:
                                                    node->balance = right->balance = AVL_BALANCED;
                                                    break;

                                                case AVL_RIGHT:
                                                    node->balance = AVL_BALANCED;
                                                    right->balance = (tSBYTE)-res; //03.09.00 13:51
                                                    break;
                                            }
                                            left->balance = AVL_BALANCED;

                                            /* double rotation */
                                            node->child[-res] = left->child[res];
                                            left->child[res] = node;
                                            right->child[res] = left->child[-res];
                                            left->child[-res] = right;
                                            node = left;
                                    }

                                    break;

                                case AVL_BALANCED:
                                    /* balanced state before */
                                    node->balance = (tSBYTE)(-res);
                                    taller = 0;
                                    break;

                                case AVL_RIGHT:
                                    /* same sides */
                                    node->balance = AVL_BALANCED;
                                    break;
                            }

                        }
                    }
                    _this->data->root = node;
                }
            }
        }
    }
    else
    {
        error = errPARAMETER_INVALID;
    }

    PR_TRACE_A1( ((hOBJECT)_this), "Leave \"Tree::Remove\" method, ret tERROR = 0x%08X", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Search )
// --- Interface "Tree". Method "Search"
// Extended method comment
//   Search element in _this, return node equil to given element
// Result comment
//   magic number  of  node found  or cNODE_NULL if such node not fount in _this
//!tERROR pr_call Tree_Search( hi_Tree _this, tTREENODE* result, tPTR data, tUINT size ){
tERROR pr_call Tree_Search( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size )
{
    tTREEDATA tdata;
	tTREENODE node;
    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::Search\" method" );

	node = (tTREENODE)_TreeSearch(_this,&tdata,data,size);
	if( node == 0)
		error=errNOT_FOUND;

	if ( result )
        *result = node;

	PR_TRACE_A2( _this, "Leave \"Tree::Search\" method, ret tTREENODE = %p, error = 0x%08X", node, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AtLeast )
// --- Interface "Tree". Method "AtLeast"
// Extended method comment
//   Search element in _this, return node equil or next greater to given element
// Result comment
//   magic number  of  node found  or cNODE_NULL if no such node present
//!tERROR pr_call Tree_AtLeast( hi_Tree _this, tTREENODE* result, tPTR data, tUINT size ){
tERROR pr_call Tree_AtLeast( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size )
{
    tTREEDATA tdata;
    tNODE node;
    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::AtLeast\" method" );

	// Place your code here

    node = _TreeSearch(_this,&tdata,data,size);

    /* find nearest left step */
    if ( node == NULL )
    {
        while ( tdata.level > 0 )
        {
            tdata.pathptr--;
            tdata.nodeptr--;

            if ( *tdata.pathptr == AVL_LEFT)
            {
                /* last left found, so it is result */
                node = *tdata.nodeptr;
                break;
            }
            tdata.level--;
        }
		
		if (!node)
			error = errNOT_FOUND;
    }


	if ( result )
        *result = (tTREENODE)(node);
    PR_TRACE_A2( _this, "Leave \"Tree::AtLeast\" method, ret tTREENODE = %p, error = 0x%08X", (tTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AtMost )
// --- Interface "Tree". Method "AtMost"
// Extended method comment
//   Search element in _this, return node equil to given element
// Result comment
//   magic number of  node found  or cNODE_NULL if such node not fount in _this
//!tERROR pr_call Tree_AtMost( hi_Tree _this, tTREENODE* result, tPTR data, tUINT size ){
tERROR pr_call Tree_AtMost( hi_Tree _this, tTREENODE* result, const tPTR data, tDWORD size )
{
    tTREEDATA tdata;
    tNODE node;
    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::AtMost\" method" );

	// Place your code here

    node = _TreeSearch(_this,&tdata,data,size);

    /* find nearest left step */
    if ( node == NULL )
    {
        while ( tdata.level > 0 )
        {
            tdata.pathptr--;
            tdata.nodeptr--;

            if ( *tdata.pathptr == AVL_RIGHT)
            {
                /* last right found, so it is result */
                node = *tdata.nodeptr;
                break;
            }
            tdata.level--;
        }

		if (!node)
			error = errNOT_FOUND;
    }

	if ( result )
        *result = (tTREENODE)(node);
    PR_TRACE_A2( _this, "Leave \"Tree::AtMost\" method, ret tTREENODE = %p, error = 0x%08X", (tLISTNODE)(node), error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, First )
// --- Interface "Tree". Method "First"
// Result comment
//   node magic number or cNODE_NULL if _this is empty
//!tERROR pr_call Tree_First( hi_Tree _this, tTREENODE* result ){
tERROR pr_call Tree_First( hi_Tree _this, tTREENODE* result )
{
    tNODE node;
    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::First\" method" );

	// Place your code here

    node = _this->data->root;

    while ( node != NULL && node->child[AVL_LEFT] != NULL)
    {
        node = node->child[AVL_LEFT];
    }

	if (!node)
		error = errEND_OF_THE_LIST;

	if ( result )
        *result = (tTREENODE)(node);
    PR_TRACE_A2( _this, "Leave \"Tree::First\" method, ret tTREENODE = %p, error = 0x%08X", (tTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Last )
// --- Interface "Tree". Method "Last"
// Result comment
//   node magic number or cNODE_NULL if _this is empty
//!tERROR pr_call Tree_Last( hi_Tree _this, tTREENODE* result ){
tERROR pr_call Tree_Last( hi_Tree _this, tTREENODE* result )
{
    tNODE node;
    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::Last\" method" );

	// Place your code here

    node = _this->data->root;

    while ( node != NULL && node->child[AVL_RIGHT] != NULL)
    {
        node = node->child[AVL_RIGHT];
    }

	if (!node)
		error = errEND_OF_THE_LIST;

	if ( result )
        *result = (tTREENODE)(node);
    PR_TRACE_A2( _this, "Leave \"Tree::Last\" method, ret tTREENODE = %p, error = 0x%08X", (tTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// --- Interface "Tree". Method "Next"
// Result comment
//   node or cNODE_NULL if given node is not valid or given node is last
tERROR pr_call Tree_Next( hi_Tree _this, tTREENODE* result, tTREENODE enode )
{
    tNODE node;
    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::Next\" method" );

	// Place your code here

    if ( (node = NodeValid(_this,enode)) != NULL )
    {
        tTREEDATA tdata;

        node = _TreeSearch(_this,&tdata,(tPTR)(node+1),node->count);

        if ( node != NULL )
        {
            if ( node->child[AVL_RIGHT] != NULL )
            {
                /* step right */
                node = node->child[AVL_RIGHT];

                /* then goes left */
                while ( node->child[AVL_LEFT] != NULL )
                {
                    node = node->child[AVL_LEFT];
                }
            }
            else
            {
                /* goes to previous node with left step */
                node = NULL;
                while ( tdata.level > 0 )
                {
                    tdata.pathptr--;
                    tdata.nodeptr--;

                    if ( *tdata.pathptr == AVL_LEFT )
                    {
                        /* last left found, so it is result */
                        node = *tdata.nodeptr;
                        break;
                    }
                    tdata.level--;
                }
            }
			
			if (!node)
				error = errEND_OF_THE_LIST;
        }
		else
			error = errNOT_FOUND;
    }
    else
    {
        error = errPARAMETER_INVALID;
        node = NULL;
    }


	if ( result )
        *result = (tTREENODE)(node);
    PR_TRACE_A2( _this, "Leave \"Tree::Next\" method, ret tTREENODE = %p, error = 0x%08X", (tTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Prev )
// --- Interface "Tree". Method "Prev"
// Result comment
//   node or cNODE_NULL if given node is not valid or given node is first
tERROR pr_call Tree_Prev( hi_Tree _this, tTREENODE* result, tTREENODE enode )
{
    tNODE node;
    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::Prev\" method" );

	// Place your code here

    if ( (node = NodeValid(_this,enode)) != NULL )
    {
        tTREEDATA tdata;

        node = _TreeSearch(_this,&tdata,(tPTR)(node+1),node->count);

        if ( node != NULL )
        {
            if ( node->child[AVL_LEFT] != NULL )
            {
                /* step LEFT */
                node = node->child[AVL_LEFT];

                /* then goes right */
                while ( node->child[AVL_RIGHT] != NULL )
                {
                    node = node->child[AVL_RIGHT];
                }
            }
            else
            {
                /* goes to previous node with right step */
                node = NULL;
                while ( tdata.level > 0 )
                {
                    tdata.pathptr--;
                    tdata.nodeptr--;

                    if ( *tdata.pathptr == AVL_RIGHT )
                    {
                        /* last left found, so it is result */
                        node = *tdata.nodeptr;
                        break;
                    }
                    tdata.level--;
                }
            }
			if (!node)
				error = errEND_OF_THE_LIST;
        }
		else
			error = errNOT_FOUND;
    }
    else
    {
        error = errPARAMETER_INVALID;
        node = NULL;
    }


	if ( result )
        *result = (tTREENODE)(node);
    PR_TRACE_A2( _this, "Leave \"Tree::Prev\" method, ret tTREENODE = %p, error = 0x%08X", (tTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, NextDup )
// --- Interface "Tree". Method "NextDup"
// Result comment
//   node or cNODE_NULL if given node is not valid or given node is last in duplicate node list
//!tERROR pr_call Tree_NextDup( hi_Tree _this, tTREENODE* result, tTREENODE node ){
tERROR pr_call Tree_NextDup( hi_Tree _this, tTREENODE* result, tTREENODE node )
{
    tNODE inode;
    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::NextDup\" method" );

	// Place your code here

    if ( (inode = NodeValid(_this,node)) != NULL )
    {
        node = (tTREENODE)(inode->child[AVL_NEXT]);
		if (!node)
			error = errEND_OF_THE_LIST;
    }
    else
    {
        error = errPARAMETER_INVALID;
        node = cTREE_NULL;
    }

	if ( result )
        *result = node;
    PR_TRACE_A2( _this, "Leave \"Tree::NextDup\" method, ret tTREENODE = %p, error = 0x%08X", node, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, PrevDup )
// --- Interface "Tree". Method "PrevDup"
// Result comment
//   node or cNODE_NULL if given node is not valid or given node is first in duplicate node list
//!tERROR pr_call Tree_PrevDup( hi_Tree _this, tTREENODE* result, tTREENODE node ){
tERROR pr_call Tree_PrevDup( hi_Tree _this, tTREENODE* result, tTREENODE node )
{
    tNODE inode;
    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::PrevDup\" method" );

    if ( (inode = NodeValid(_this,node)) != NULL )
    {
        if ( inode->mode )
        {
			// warning: possible bug? AVL_PREV? // Mike 11.12.2002
            node = (tTREENODE)(inode->child[AVL_NEXT]);
			if (!node)
				error = errEND_OF_THE_LIST;
        }
        else
        {
            node = cTREE_NULL;
			error = errEND_OF_THE_LIST;
        }
    }
    else
    {
        error = errPARAMETER_INVALID;
        node = cTREE_NULL;
    }

	if ( result )
        *result = node;
    PR_TRACE_A2( _this, "Leave \"Tree::PrevDup\" method, ret tTREENODE = %p, error = 0x%08X", node, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, NodeDataGet )
// --- Interface "Tree". Method "NodeDataGet"
// Extended method comment
//   Retrieve element data from node. If buffer too small to accept whole element only part of element will be stored and error will be set. To determine amount of bytes required for element just pass NULL as buffer. If node not valid 0 will be returned and error will set.
// Behaviour comment
//   if passed buffer value is NULL function return amount of bytes required to store whoile element
// Result comment
//   bytes stored to buffer
//!tERROR pr_call Tree_NodeDataGet( hi_Tree _this, tUINT* result, tTREENODE node, tPTR buffer, tUINT count){
tERROR pr_call Tree_NodeDataGet( hi_Tree _this, tDWORD* result, tTREENODE node, tPTR buffer, tDWORD count )
{
    tNODE inode;
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::NodeDataGet\" method" );

	// Place your code here


    if ( (inode = NodeValid(_this,node)) != NULL )
    {
        if ( buffer != NULL )
        {
            if ( count < inode->count )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                count = inode->count;
            }
            memcpy(buffer,(tBYTE*)(inode+1),count);
        }
        else
        {
            count = inode->count;
        }
    }
    else
    {
        error = errPARAMETER_INVALID;
        count = 0;
    }

	if ( result )
        *result = count;
    PR_TRACE_A2( _this, "Leave \"Tree::NodeDataGet\" method, ret tUINT = %u, error = 0x%08X", count, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, NodeDataSet )
// --- Interface "Tree". Method "NodeDataSet"
// Extended method comment
//   Set node data for existing node. If key part of data is affected node will moved.
// Behaviour comment
//   if passed buffer value is NULL function return amount of bytes required to store whoile element
// Result comment
//   bytes stored to buffer
tERROR pr_call Tree_NodeDataSet( hi_Tree _this, tTREENODE* result, tTREENODE node, tPTR buffer, tDWORD count)
{
    tNODE inode;
    int res;

    tERROR    error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::NodeDataSet\" method" );

	// Place your code here


    if ( (inode = NodeValid(_this,node)) != NULL )
    {
        if ( buffer != NULL )
        {
            if ( _this->data->compare_object_real == NULL )
            {
                if ( count < sizeof(tDWORD) )
                {
                    error = errBUFFER_TOO_SMALL;
                    node = cTREE_NULL;
                    goto abort;
                }
                CompareDefault(NULL,&res,(tPTR)(inode+1),inode->count,buffer,count);
            }
            else
            {
                _this->data->compare_object_real->vtbl->Compare(_this->data->compare_object_real,&res,(tPTR)(inode+1),inode->count,buffer,count);
            }
            if ( res == 0 && inode->count >= count ) // fix: was "<="
            {
                /* just replace node data without rebuilding _this */
                if ( inode->count < count )
                {
                    inode->count = count;
                }
                memcpy((tPTR)(inode+1),buffer,inode->count);
            }
            else
            {
                Tree_Remove(_this,node);
                Tree_Add(_this,&node,buffer,count);
            }
        }
    }
    else
    {
        node = cTREE_NULL;
        error = errPARAMETER_INVALID;
    }

    abort:
	if ( result )
        *result = node;
    PR_TRACE_A2( _this, "Leave \"Tree::NodeDataSet\" method, ret tTREENODE = %p, error = 0x%08X", node, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Clear )
// --- Interface "Tree". Method "Clear"
//!tERROR pr_call Tree_Clear( hi_Tree _this ){
tERROR pr_call Tree_Clear( hi_Tree _this )
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter \"Tree::Clear\" method" );

	// Place your code here
    _this->data->root = NULL;
    _this->data->heap->vtbl->Clear(_this->data->heap);

    PR_TRACE_A1( ((hOBJECT)_this), "Leave \"Tree::Clear\" method, ret tERROR = 0x%08X", error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {

	switch( dwReason ) {
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH : 
		break;
		
	case PRAGUE_PLUGIN_LOAD :
		{
			tERROR error=errOK;
			g_root=(hROOT)hInstance;;
			error = Tree_Register( g_root );
			if ( PR_FAIL(error) )	break;
			error = LTree_Register( g_root );
			if ( PR_FAIL(error) )	break;
			error = LRU_Register( g_root );
			if ( PR_FAIL(error) )	break;
			error = List_Register( g_root );
			if ( PR_FAIL(error) )	break;
			error = Stack_Register( g_root );
			if ( PR_FAIL(error) )	break;
			if(pERROR)*pERROR=error;
			if(PR_FAIL(error)) return cFALSE;
		}
		break;
		
	case PRAGUE_PLUGIN_UNLOAD :
		g_root = NULL;
		break;
	}
	return cTRUE;
}

// AVP Prague stamp end
