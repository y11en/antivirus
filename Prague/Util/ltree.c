// -------- Monday,  18 September 2000,  15:27 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- ltree.c
// -------------------------------------------

/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	26.02.2003 22:06:39
  Comments:	
	Fixed:		В релизной версии была возможна ситуация, когда ф-ии LTree_TreeNext, LTree_TreePrev, LTree_AtLeast, LTree_AtMost
				возвращали некорректный указатель на tNODE.

 ************************************/

/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	11.12.2002 18:15:42
  Comments:	
	Fixed:		Возврат статуса errEND_OF_LIST в ф-ях LTree_ListPrev, LTree_ListNext, 
				LTree_ListLast, LTree_ListFirst, LTree_TreePrev, LTree_TreeNext, LTree_TreeLast, 
				LTree_TreeFirst.

				Возврат статуса errNOT_FOUND в ф-ях LTree_Search, LTree_AtMost, LTree_AtLeast, 
				LTree_TreePrev, LTree_TreeNext.
			
 ************************************/



#define LTree_PRIVATE_DEFINITION

#include <Prague/prague.h>
#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_compar.h>

#include <memory.h>

#include "ltree.h"


#define AVL_DEPTH 32
#define AVL_LEFT     -1
#define AVL_BALANCED  0
#define AVL_RIGHT     1

#define AVL_NEXT 0
#define AVL_PREV -1





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Interface comment )
// --------------------------------------------
// --- 9e55aca0_6dda_11d4_b94e_008048ec2fc7 ---
// --------------------------------------------
// LTree interface implementation
// Extended comment
//
// AVP Prague stamp end( LTree, Interface comment )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Data structure )
// Interface LTree. Inner data structure
typedef struct tag_LTreeData
{
    tUINT granularity;          // --
    tBOOL dup_allowed;            // --
    tBOOL remove_allowed;         // --
    hCOMPARE compare_object;      // --
    tUINT node_size;            // --
    struct sNODE *root;           // --
    struct  sNODE *head;
    struct  sNODE *tail;
    hHEAP heap;                   // --
    hCOMPARE compare_object_real; // --
} LTreeData;
// AVP Prague stamp end( LTree, Data structure )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, object declaration )
typedef struct tag_hi_LTree
{
  const iLTreeVtbl*  vtbl; // pointer to the "LTree" virtual table
  const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
  LTreeData*        data;   // pointer to the "LTree" data structure
} *hi_LTree;
// AVP Prague stamp end( LTree, object declaration )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Internal method table prototypes )
// Interface "LTree". Internal method table. Forward declarations
tERROR pr_call LTree_ObjectInit( hi_LTree _this );
// AVP Prague stamp end( LTree, Internal method table prototypes )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Internal method table )
// Interface "LTree". Internal method table.
static iINTERNAL i_LTree_vtbl =
{
  (tIntFnRecognize)        NULL,
  (tIntFnObjectNew)        NULL,
  (tIntFnObjectInit)       LTree_ObjectInit,
  (tIntFnObjectInitDone)   NULL,
  (tIntFnObjectCheck)      NULL,
  (tIntFnObjectPreClose)   NULL,
  (tIntFnObjectClose)      NULL,
  (tIntFnChildNew)         NULL,
  (tIntFnChildInitDone)    NULL,
  (tIntFnChildClose)       NULL,
  (tIntFnMsgReceive)       NULL,
  (tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end( LTree, Internal method table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, interface function forward declarations )
typedef   tERROR (pr_call *fnpLTree_Add)       ( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size, tLTREENODE where, tUINT mode ); // -- ;
typedef   tERROR (pr_call *fnpLTree_Remove)    ( hi_LTree _this, tLTREENODE node );                  // -- ;
typedef   tERROR (pr_call *fnpLTree_Search)    ( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size );            // -- ;
typedef   tERROR (pr_call *fnpLTree_AtLeast)   ( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size );            // -- ;
typedef   tERROR (pr_call *fnpLTree_AtMost)    ( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size );            // -- ;
typedef   tERROR (pr_call *fnpLTree_TreeFirst) ( hi_LTree _this, tLTREENODE* result );                                   // -- ;
typedef   tERROR (pr_call *fnpLTree_TreeLast)  ( hi_LTree _this, tLTREENODE* result );                                   // -- ;
typedef   tERROR (pr_call *fnpLTree_TreeNext)  ( hi_LTree _this, tLTREENODE* result, tLTREENODE node );                  // -- ;
typedef   tERROR (pr_call *fnpLTree_TreePrev)  ( hi_LTree _this, tLTREENODE* result, tLTREENODE node );                  // -- ;
typedef   tERROR (pr_call *fnpLTree_ListFirst) ( hi_LTree _this, tLTREENODE* result );                                   // -- ;
typedef   tERROR (pr_call *fnpLTree_ListLast)  ( hi_LTree _this, tLTREENODE* result );                                   // -- ;
typedef   tERROR (pr_call *fnpLTree_ListNext)  ( hi_LTree _this, tLTREENODE* result, tLTREENODE node );                  // -- ;
typedef   tERROR (pr_call *fnpLTree_ListPrev)  ( hi_LTree _this, tLTREENODE* result, tLTREENODE node );                  // -- ;
typedef   tERROR (pr_call *fnpLTree_ListMove)  ( hi_LTree _this, tLTREENODE node, tLTREENODE where, tUINT mode ); // -- ;
typedef   tERROR (pr_call *fnpLTree_DataGet)   ( hi_LTree _this, tUINT* result, tLTREENODE node, tPTR buffer, tUINT size ); // -- ;
typedef   tERROR (pr_call *fnpLTree_DataSet)   ( hi_LTree _this, tLTREENODE* result, tLTREENODE node, tPTR buffer, tUINT size ); // -- ;
typedef   tERROR (pr_call *fnpLTree_Clear)     ( hi_LTree _this );                                   // -- ;
// AVP Prague stamp end( LTree, interface function forward declarations )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, interface declaration )
struct iLTreeVtbl
{
  fnpLTree_Add        Add;
  fnpLTree_Remove     Remove;
  fnpLTree_Search     Search;
  fnpLTree_AtLeast    AtLeast;
  fnpLTree_AtMost     AtMost;
  fnpLTree_TreeFirst  TreeFirst;
  fnpLTree_TreeLast   TreeLast;
  fnpLTree_TreeNext   TreeNext;
  fnpLTree_TreePrev   TreePrev;
  fnpLTree_ListFirst  ListFirst;
  fnpLTree_ListLast   ListLast;
  fnpLTree_ListNext   ListNext;
  fnpLTree_ListPrev   ListPrev;
  fnpLTree_ListMove   ListMove;
  fnpLTree_DataGet    DataGet;
  fnpLTree_DataSet    DataSet;
  fnpLTree_Clear      Clear;
}; // LTree
// AVP Prague stamp end( LTree, interface declaration )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External method table prototypes )
// Interface "LTree". External method table. Forward declarations
tERROR pr_call LTree_Add( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size, tLTREENODE where, tUINT mode );
tERROR pr_call LTree_Remove( hi_LTree _this, tLTREENODE node );
tERROR pr_call LTree_Search( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size );
tERROR pr_call LTree_AtLeast( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size );
tERROR pr_call LTree_AtMost( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size );
tERROR pr_call LTree_TreeFirst( hi_LTree _this, tLTREENODE* result );
tERROR pr_call LTree_TreeLast( hi_LTree _this, tLTREENODE* result );
tERROR pr_call LTree_TreeNext( hi_LTree _this, tLTREENODE* result, tLTREENODE node );
tERROR pr_call LTree_TreePrev( hi_LTree _this, tLTREENODE* result, tLTREENODE node );
tERROR pr_call LTree_ListFirst( hi_LTree _this, tLTREENODE* result );
tERROR pr_call LTree_ListLast( hi_LTree _this, tLTREENODE* result );
tERROR pr_call LTree_ListNext( hi_LTree _this, tLTREENODE* result, tLTREENODE node );
tERROR pr_call LTree_ListPrev( hi_LTree _this, tLTREENODE* result, tLTREENODE node );
tERROR pr_call LTree_ListMove( hi_LTree _this, tLTREENODE node, tLTREENODE where, tUINT mode );
tERROR pr_call LTree_DataGet( hi_LTree _this, tUINT* result, tLTREENODE node, tPTR buffer, tUINT size );
tERROR pr_call LTree_DataSet( hi_LTree _this, tLTREENODE* result, tLTREENODE node, tPTR buffer, tUINT size );
tERROR pr_call LTree_Clear( hi_LTree _this );
// AVP Prague stamp end( LTree, External method table prototypes )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External method table )
// Interface "LTree". External method table.
static iLTreeVtbl e_LTree_vtbl =
{
  LTree_Add,
  LTree_Remove,
  LTree_Search,
  LTree_AtLeast,
  LTree_AtMost,
  LTree_TreeFirst,
  LTree_TreeLast,
  LTree_TreeNext,
  LTree_TreePrev,
  LTree_ListFirst,
  LTree_ListLast,
  LTree_ListNext,
  LTree_ListPrev,
  LTree_ListMove,
  LTree_DataGet,
  LTree_DataSet,
  LTree_Clear
};
// AVP Prague stamp end( LTree, External method table )
// --------------------------------------------------------------------------------







// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Forwarded property methods declarations )
// Interface "LTree". Get/Set property methods
tERROR pr_call LTree_PROP_GranularitySet( hi_LTree _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
tERROR pr_call LTree_PROP_CompareSet( hi_LTree _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( LTree, Forwarded property methods declarations )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Global property variable declaration )
// Interface "LTree". Global(shared) property table variables
// AVP Prague stamp end( LTree, Global property variable declaration )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Property table )
// Interface "LTree". Property table
mPROPERTY_TABLE(LTree_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, 1 )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Tree and List", 14 )
  mLOCAL_PROPERTY_BUF( pgLTREE_DUP_ALLOWED, LTreeData, dup_allowed, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY( pgLTREE_GRANULARITY, LTreeData, granularity, cPROP_BUFFER_READ_WRITE, NULL, LTree_PROP_GranularitySet )
  mLOCAL_PROPERTY_BUF( pgLTREE_NODE_SIZE, LTreeData, node_size, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY( pgLTREE_COMPARE_OBJECT, LTreeData, compare_object, cPROP_BUFFER_READ_WRITE, NULL, LTree_PROP_CompareSet )
mPROPERTY_TABLE_END(LTree_PropTable)
// AVP Prague stamp end( LTree, Property table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Registration call )
// Interface "LTree". Register function
tERROR pr_call LTree_Register( hROOT root )
{
  tERROR error;

  PR_TRACE_A0( ((hOBJECT)root), "Enter \"LTree::Register\" method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_LTREE,                              // interface identifier
    PID_UTIL,                               // plugin identifier
    0x00000000,                             // subtype identifier
    0x00000001,                             // interface version
    VID_ANDREW,                             // interface developer
    &i_LTree_vtbl,                          // internal(kernel called) function table
    (sizeof(i_LTree_vtbl)/sizeof(tPTR)),    // internal function table size
    &e_LTree_vtbl,                          // external function table
    (sizeof(e_LTree_vtbl)/sizeof(tPTR)),    // external function table size
    LTree_PropTable,                        // property table
    mPROPERTY_TABLE_SIZE(LTree_PropTable),  // property table size
    sizeof(LTreeData),                      // memory size
    IFACE_SYSTEM                            // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( error != errOK )
      PR_TRACE( (root,prtDANGER,"LTree(IID_LTREE) registered [error = 0x%08X]",error) );
  #endif

  PR_TRACE_A1( root, "Leave \"LTree::Register\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( LTree, Registration call )
// --------------------------------------------------------------------------------






typedef struct
{
    tPTR data;                          /* data to add/remove */
    tUINT count;                      /* size of data */
    //int taller;
    //int  error;
    hi_LTree _this;                      /* _this root */
    struct sNODE **nodeptr;             /* pointer to current node */
    signed char  *pathptr;
    struct sNODE *node[AVL_DEPTH];      /* node path */
    signed char  path[AVL_DEPTH];       /* balance path */
    tERROR (*compare_fun)(hCOMPARE,tINT*, void *, tUINT, void *, tUINT);
    hCOMPARE compare_obj;
    int  level;                         /* current level */
} tTREEDATA;



typedef struct sNODE
{
    struct  sNODE *left;
    struct  sNODE *child[2];
    struct  sNODE *next;
    struct  sNODE *prev;
    tUINT count;
    tSBYTE  balance;
    tSBYTE  mode;
} thNODE, *tNODE   ;



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Property method implementation )
// Interface "LTree". Method "Set" for property(s):
//  -- LTREE_GRANULARITY
tERROR pr_call LTree_PROP_GranularitySet( hi_LTree _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size )
{
    tERROR error;

    PR_TRACE_A0( ((hOBJECT)_this), "Enter *SET* method \"LTree_PROP_GranularitySet\" for property \"pgLTREE_GRANULARITY\"" );

    (void)prop;

    error = _this->data->heap->sys->PropertySet((hOBJECT)(_this->data->heap),(tDWORD*)out_size,pgHEAP_GRANULARITY,buffer,size);

    PR_TRACE_A2( ((hOBJECT)_this), "Leave *SET* method \"LTree_PROP_GranularitySet\" for property \"pgLTREE_GRANULARITY\", ret tUINT = %u(size), error = 0x%08X", *out_size, error );
    return error;
}
// AVP Prague stamp end( LTree, Property method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Property method implementation )
// Interface "LTree". Method "Set" for property(s):
//  -- LTREE_COMPARE_OBJECT
tERROR pr_call LTree_PROP_CompareSet( hi_LTree _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size )
{
    tUINT rcode;
    tERROR error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter *SET* method \"LTree_PROP_CompareSet\" for property \"pgLTREE_COMPARE_OBJECT\"" );

    *out_size = 0;

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

    PR_TRACE_A2( ((hOBJECT)_this), "Leave *SET* method \"LTree_PROP_CompareSet\" for property \"pgLTREE_COMPARE_OBJECT\", ret tUINT = %u(size), error = 0x%08X", *out_size, error );
    return error;
}
// AVP Prague stamp end( LTree, Property method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, Internal (kernel called) interface method implementation )
// --- Interface "LTree". Method "ObjectInit"
// Extended method comment
//   Kernel notifies an object about successful creating of object
// Behaviour comment
//   Initializes internal object data
// Result comment
//   Returns value differ from errOK if object cannot be initialized
tERROR pr_call LTree_ObjectInit( hi_LTree _this )
{
  tERROR error = errOBJECT_NOT_FOUND;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::ObjectInit\" method" );

  // Place your code here
  error=CALL_SYS_ObjectCreateQuick(_this,&_this->data->heap,IID_HEAP,PID_ANY,SUBTYPE_ANY);

  PR_TRACE_A1( ((hOBJECT)_this), "Leave \"LTree::ObjectInit\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( LTree, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------

static tNODE NodeValid(hi_LTree _this, tLTREENODE node)
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

static tNODE Alloc(hi_LTree _this, tPTR data, tUINT count)
{
    tNODE node;

    if ( _this->data->heap->vtbl->Alloc(_this->data->heap,(tVOID*) &node,count+sizeof(thNODE)) == errOK)
    {
        node->prev = NULL;
        node->next = NULL;
        node->child[AVL_LEFT] = NULL;
        node->child[AVL_BALANCED] = NULL;
        node->child[AVL_RIGHT] = NULL;
        node->balance = AVL_BALANCED;
        node->count = count;
        memcpy(((tBYTE*)(node))+sizeof(thNODE),data,count);
    }

    return node;
}

static tNODE _TreeSearch(hi_LTree _this, tTREEDATA *tdata, tPTR data, tUINT count)
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

static tNODE _List_Add(hi_LTree list, tNODE node, tLTREENODE _where, tUINT mode)
{
    tNODE where;

    switch ( mode )
    {
        case cLTREE_AFTER:
            if ( (where = NodeValid(list,_where)) != NULL )
            {
                node->next = where->next;
                where->next = node;
                node->prev = where;
                if ( node->next != NULL )
                {
                    node->next->prev = node;
                }
                else
                {
                    list->data->tail = node;
                }
            }
            else goto error;
            break;

        case cLTREE_BEFORE:
            if ( (where = NodeValid(list,_where)) != NULL )
            {
                node->prev = where->prev;
                where->prev = node;
                node->next = where;
                if ( node->prev != NULL )
                {
                    node->prev->next = node;
                }
                else
                {
                    list->data->head = node;
                }
            }
            else goto error;
            break;

        case cLTREE_FIRST:
            if ( list->data->head != NULL )
            {
                node->next = list->data->head;
            }
            list->data->head = node;
            break;

        case cLTREE_LAST:
            if ( list->data->tail != NULL )
            {
                node->prev = list->data->tail;
            }
            list->data->tail = node;
            break;

        default:
            goto error;
    }
    error:
        list->data->heap->vtbl->Free(list->data->heap,node);
//        list->sys->ErrorCodeSet((hOBJECT)list,errPARAMETER_INVALID);
    return cLTREE_NULL;
}

static void _List_Remove(hi_LTree list, tNODE node)
{
    if ( node->prev == NULL )
    {
        list->data->head = node->next;
    }
    else
    {
        node->prev->next = node->next;
    }

    if ( node->next == NULL )
    {
        list->data->tail = node->prev;
    }
    else
    {
        node->next->prev = node->prev;
    }
}






// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "Add"
tERROR pr_call LTree_Add( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT count, tLTREENODE _where, tUINT mode )
{
    tNODE node;
    tNODE *tnode;
    tNODE left, right;
    signed char *tpath, res;

    tTREEDATA tdata;
    tERROR     error = errOBJECT_NOT_FOUND;

    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::Add\" method" );

    // Place your code here

    node = NULL;


    if ( (mode != cLTREE_BEFORE && mode != cLTREE_AFTER) || NodeValid(_this,_where) != NULL )
    {

        node = _TreeSearch(_this,&tdata,data,count);

        if ( node == NULL )
        {
            int taller;
            /* node not found */

            /* add new node in level position */


            if ( (node = Alloc(_this,data,count)) == NULL )
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
                tNODE new_node;

                if ( (new_node = Alloc(_this,data,count)) == NULL )
                {
                    node = NULL;
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
                node = NULL;
            }
        }
    }
    else
    {
        error = errPARAMETER_INVALID;
    }

    if ( node != NULL )
    {
        _List_Add(_this,node,_where,mode);
    }

    abort:

  if ( result )
    *result = (tLTREENODE)(node);
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::Add\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "Remove"
tERROR pr_call LTree_Remove( hi_LTree _this, tLTREENODE enode )
{
    tNODE node;
    tNODE next;
    tERROR error;
    error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::Remove\" method" );

  // Place your code here

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
            tTREEDATA tdata;

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

    PR_TRACE_A1( ((hOBJECT)_this), "Leave \"LTree::Remove\" method, ret tERROR = 0x%08X", error );
    return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "Search"
tERROR pr_call LTree_Search( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size )
{
    tTREEDATA tdata;
    tERROR     error = errOK;
	tLTREENODE node;
    PR_TRACE_A0( _this, "Enter \"LTree::Search\" method" );

  // Place your code here
	node = (tLTREENODE)_TreeSearch(_this,&tdata,data,size);

	if (!node)
		error = errNOT_FOUND;

	if ( result )
		*result = node;
	PR_TRACE_A2( _this, "Leave \"LTree::Search\" method, ret tLTREENODE = %p, error = 0x%08X", (result ? *result : 0), error );
	return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "AtLeast"
tERROR pr_call LTree_AtLeast( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size )
{
    tTREEDATA tdata;
    tNODE node;
    tERROR     error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::AtLeast\" method" );

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
		*result = (tLTREENODE)(node);
	PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::AtLeast\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "AtMost"
tERROR pr_call LTree_AtMost( hi_LTree _this, tLTREENODE* result, tPTR data, tUINT size )
{
    tTREEDATA tdata;
    tNODE node;
    tERROR     error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::AtMost\" method" );

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
    *result = (tLTREENODE)(node);
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::AtMost\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "TreeFirst"
tERROR pr_call LTree_TreeFirst( hi_LTree _this, tLTREENODE* result )
{
    tNODE node;
    tERROR     error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::TreeFirst\" method" );

	// Place your code here

    node = _this->data->root;

    while ( node != NULL && node->child[AVL_LEFT] != NULL)
    {
        node = node->child[AVL_LEFT];
    }

	if (!node)
		error = errEND_OF_THE_LIST;

	if ( result )
		*result = (tLTREENODE)(node);
	
	PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::TreeFirst\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "TreeLast"
tERROR pr_call LTree_TreeLast( hi_LTree _this, tLTREENODE* result )
{
    tNODE node;
    tERROR     error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::TreeLast\" method" );

  // Place your code here

    node = _this->data->root;

    while ( node != NULL && node->child[AVL_RIGHT] != NULL)
    {
        node = node->child[AVL_RIGHT];
    }

	if (!node)
		error = errEND_OF_THE_LIST;

    if ( result )
        *result = (tLTREENODE)(node);
    PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::TreeLast\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "TreeNext"
tERROR pr_call LTree_TreeNext( hi_LTree _this, tLTREENODE* result, tLTREENODE enode )
{
    tNODE node;
  tERROR     error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::TreeNext\" method" );

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
    *result = (tLTREENODE)(node);
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::TreeNext\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "TreePrev"
tERROR pr_call LTree_TreePrev( hi_LTree _this, tLTREENODE* result, tLTREENODE enode )
{
    tNODE node;
  tERROR     error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::TreePrev\" method" );

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
    *result = (tLTREENODE)(node);
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::TreePrev\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "ListFirst"
tERROR pr_call LTree_ListFirst( hi_LTree _this, tLTREENODE* result )
{
  tERROR     error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::ListFirst\" method" );

  // Place your code here

	if (!_this->data->head)
		error = errEND_OF_THE_LIST;

  if ( result )
    *result = (tLTREENODE)_this->data->head;
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::ListFirst\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)_this->data->head, error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "ListLast"
tERROR pr_call LTree_ListLast( hi_LTree _this, tLTREENODE* result )
{
  tERROR     error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::ListLast\" method" );

  // Place your code here
	if (!_this->data->tail)
		error = errEND_OF_THE_LIST;

  if ( result )
    *result = (tLTREENODE)_this->data->tail;
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::ListLast\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)_this->data->tail, error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "ListNext"
tERROR pr_call LTree_ListNext( hi_LTree _this, tLTREENODE* result, tLTREENODE _node )
{
    tNODE node;
  tERROR     error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::ListNext\" method" );

  // Place your code here

    if ( (node = NodeValid(_this,_node)) != NULL )
    {
        node = node->next;
    }

	if (!node)
		error = errEND_OF_THE_LIST;

	if ( result )
		*result = (tLTREENODE)(node);
	PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::ListNext\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "ListPrev"
tERROR pr_call LTree_ListPrev( hi_LTree _this, tLTREENODE* result, tLTREENODE _node )
{
    tNODE node;
  tERROR     error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::ListPrev\" method" );

  // Place your code here

    if ( (node = NodeValid(_this,_node)) != NULL )
    {
        node = node->prev;
    }

	if (!node)
		error = errEND_OF_THE_LIST;

	if ( result )
		*result = (tLTREENODE)(node);
	PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::ListPrev\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
	return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "ListMove"
tERROR pr_call LTree_ListMove( hi_LTree _this, tLTREENODE _node, tLTREENODE _where, tUINT mode )
{
    tNODE node;
    tERROR error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::ListMove\" method" );

  // Place your code here

    if ( (node = NodeValid(_this,_node)) != NULL )
    {
        if ( (mode != cLTREE_BEFORE && mode != cLTREE_AFTER) || NodeValid(_this,_where) != NULL )
        {
            _List_Remove(_this,node);
            _List_Add(_this,node,_where,mode);
        }
    }
    else
    {
        error = errPARAMETER_INVALID;
    }

  PR_TRACE_A1( ((hOBJECT)_this), "Leave \"LTree::ListMove\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "DataGet"
tERROR pr_call LTree_DataGet( hi_LTree _this, tUINT* result, tLTREENODE node, tPTR buffer, tUINT count)
{
    tNODE inode;
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::DataGet\" method" );

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

  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::DataGet\" method, ret tUINT = %u, error = 0x%08X", count, error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "DataSet"
tERROR pr_call LTree_DataSet( hi_LTree _this, tLTREENODE* result, tLTREENODE node, tPTR buffer, tUINT count)
{
    tNODE inode;
    int res;
    tERROR     error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::DataSet\" method" );

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
                    node = cLTREE_NULL;
                    goto abort;
                }
                CompareDefault(NULL,&res,(tPTR)(inode+1),inode->count,buffer,count);
            }
            else
            {
                _this->data->compare_object_real->vtbl->Compare(_this->data->compare_object_real,&res,(tPTR)(inode+1),inode->count,buffer,count);
            }
            if ( res == 0 && inode->count <= count )
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
                tNODE tmp;

                tmp = inode->prev;
                LTree_Remove(_this,node);

                if ( tmp == NULL )
                {
                    LTree_Add(_this,&node,buffer,count,cLTREE_NULL,cLTREE_FIRST);
                }
                else
                {
                    LTree_Add(_this,&node,buffer,count,(tLTREENODE)(tmp),cLTREE_AFTER);
                }
            }
        }
    }
    else
    {
        node = cLTREE_NULL;
    }

    abort:
  if ( result )
    *result = (tLTREENODE)(node);
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"LTree::DataSet\" method, ret tLTREENODE = %p, error = 0x%08X", (tLTREENODE)(node), error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( LTree, External (user called) interface method implementation )
// --- Interface "LTree". Method "Clear"
tERROR pr_call LTree_Clear( hi_LTree _this )
{
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"LTree::Clear\" method" );

  // Place your code here
    _this->data->head = NULL;
    _this->data->tail = NULL;
    _this->data->root = NULL;
    _this->data->heap->vtbl->Clear(_this->data->heap);

  PR_TRACE_A1( ((hOBJECT)_this), "Leave \"LTree::Clear\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( LTree, External (user called) interface method implementation )
// --------------------------------------------------------------------------------
