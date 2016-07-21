// -------- Monday,  18 September 2000,  15:27 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- list.c
// -------------------------------------------

/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	18.12.2002 14:49:44
  Comments:	
	Fixed:	Возврат статуса errPARAMETER_INVALID в ф-ии List_Add
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	11.12.2002 17:16:03
  Comments:	
	Fixed:	Возврат статуса errEND_OF_LIST при выходе за пределы списка 
			в ф-ях List_First, List_Last, List_Next и List_Prev. 
			Ранее возвращалось errOK.
 ************************************/





#define List_PRIVATE_DEFINITION
#include <Prague/prague.h>

#include <memory.h>
#include "list.h"






// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Interface comment )
// --------------------------------------------
// --- 9e55aca2_6dda_11d4_b94e_008048ec2fc7 ---
// --------------------------------------------
// List interface implementation
// Extended comment
//
// AVP Prague stamp end( List, Interface comment )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Data structure )
// Interface List. Inner data structure
typedef struct tag_ListData
{
	tUINT granularity; // --
	tUINT node_size; // --
    hHEAP heap;
    struct sLIST *head;
    struct sLIST *tail;
} ListData;
// AVP Prague stamp end( List, Data structure )
// --------------------------------------------------------------------------------


typedef struct sLIST
{
    struct sLIST *next;
    struct sLIST *prev;
    tUINT size;
} thLIST, *tLIST   ;



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, object declaration )
typedef struct tag_hi_List
{
  const iListVtbl*   vtbl; // pointer to the "List" virtual table
  const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
  ListData*         data;   // pointer to the "List" data structure
} *hi_List;
// AVP Prague stamp end( List, object declaration )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Internal method table prototypes )
// Interface "List". Internal method table. Forward declarations
tERROR pr_call List_ObjectInit( hi_List _this );
// AVP Prague stamp end( List, Internal method table prototypes )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Internal method table )
// Interface "List". Internal method table.
static iINTERNAL i_List_vtbl =
{
  (tIntFnRecognize)        NULL,
  (tIntFnObjectNew)        NULL,
  (tIntFnObjectInit)       List_ObjectInit,
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
// AVP Prague stamp end( List, Internal method table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, interface function forward declarations )
typedef   tERROR (pr_call *fnpList_Add)     ( hi_List _this, tLISTNODE* result, tPTR data, tUINT count, tLISTNODE where, tUINT mode ); // -- ;
typedef   tERROR (pr_call *fnpList_Remove)  ( hi_List _this, tLISTNODE node );                      // -- ;
typedef   tERROR (pr_call *fnpList_Move)    ( hi_List _this, tLISTNODE node, tLISTNODE where, tUINT mode ); // -- ;
typedef   tERROR (pr_call *fnpList_First)   ( hi_List _this, tLISTNODE* result );                                      // -- ;
typedef   tERROR (pr_call *fnpList_Last)    ( hi_List _this, tLISTNODE* result );                                      // -- ;
typedef   tERROR (pr_call *fnpList_Next)    ( hi_List _this, tLISTNODE* result, tLISTNODE node );                      // -- ;
typedef   tERROR (pr_call *fnpList_Prev)    ( hi_List _this, tLISTNODE* result, tLISTNODE node );                      // -- ;
typedef   tERROR (pr_call *fnpList_DataGet) ( hi_List _this, tUINT* result, tLISTNODE node, tPTR buffer, tUINT size ); // -- ;
typedef   tERROR (pr_call *fnpList_DataSet) ( hi_List _this, tLISTNODE* result, tLISTNODE node, tPTR buffer, tUINT size ); // -- ;
typedef   tERROR (pr_call *fnpList_Clear)   ( hi_List _this );                                      // -- ;
// AVP Prague stamp end( List, interface function forward declarations )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, interface declaration )
struct iListVtbl
{
  fnpList_Add      Add;
  fnpList_Remove   Remove;
  fnpList_Move     Move;
  fnpList_First    First;
  fnpList_Last     Last;
  fnpList_Next     Next;
  fnpList_Prev     Prev;
  fnpList_DataGet  DataGet;
  fnpList_DataSet  DataSet;
  fnpList_Clear    Clear;
}; // List
// AVP Prague stamp end( List, interface declaration )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External method table prototypes )
// Interface "List". External method table. Forward declarations
tERROR pr_call List_Add( hi_List _this, tLISTNODE* result, tPTR data, tUINT count, tLISTNODE where, tUINT mode );
tERROR pr_call List_Remove( hi_List _this, tLISTNODE node );
tERROR pr_call List_Move( hi_List _this, tLISTNODE node, tLISTNODE where, tUINT mode );
tERROR pr_call List_First( hi_List _this, tLISTNODE* result );
tERROR pr_call List_Last( hi_List _this, tLISTNODE* result );
tERROR pr_call List_Next( hi_List _this, tLISTNODE* result, tLISTNODE node );
tERROR pr_call List_Prev( hi_List _this, tLISTNODE* result, tLISTNODE node );
tERROR pr_call List_DataGet( hi_List _this, tUINT* result, tLISTNODE node, tPTR buffer, tUINT size );
tERROR pr_call List_DataSet( hi_List _this, tLISTNODE* result, tLISTNODE node, tPTR buffer, tUINT size );
tERROR pr_call List_Clear( hi_List _this );
// AVP Prague stamp end( List, External method table prototypes )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External method table )
// Interface "List". External method table.
static iListVtbl e_List_vtbl =
{
  List_Add,
  List_Remove,
  List_Move,
  List_First,
  List_Last,
  List_Next,
  List_Prev,
  List_DataGet,
  List_DataSet,
  List_Clear
};
// AVP Prague stamp end( List, External method table )
// --------------------------------------------------------------------------------







// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Forwarded property methods declarations )
// Interface "List". Get/Set property methods
tERROR pr_call List_PROP_GranularitySet( hi_List _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( List, Forwarded property methods declarations )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Global property variable declaration )
// Interface "List". Global(shared) property table variables
// AVP Prague stamp end( List, Global property variable declaration )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Property table )
// Interface "List". Property table
mPROPERTY_TABLE(List_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, 1 )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "List handling interface", 24 )
  mLOCAL_PROPERTY( pgLIST_GRANULARITY, ListData, granularity, cPROP_BUFFER_READ_WRITE, NULL, List_PROP_GranularitySet )
  mLOCAL_PROPERTY_BUF( pgLIST_NODE_SIZE, ListData, node_size, cPROP_BUFFER_READ_WRITE )
mPROPERTY_TABLE_END(List_PropTable)
// AVP Prague stamp end( List, Property table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Registration call )
// Interface "List". Register function
tERROR pr_call List_Register( hROOT root )
{
  tERROR error;

  PR_TRACE_A0( ((hOBJECT)root), "Enter \"List::Register\" method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_LIST,                               // interface identifier
    PID_UTIL,                               // plugin identifier
    0x00000000,                             // subtype identifier
    0x00000001,                             // interface version
    VID_ANDREW,                             // interface developer
    &i_List_vtbl,                           // internal(kernel called) function table
    (sizeof(i_List_vtbl)/sizeof(tPTR)),     // internal function table size
    &e_List_vtbl,                           // external function table
    (sizeof(e_List_vtbl)/sizeof(tPTR)),     // external function table size
    List_PropTable,                         // property table
    mPROPERTY_TABLE_SIZE(List_PropTable),   // property table size
    sizeof(ListData),                       // memory size
    IFACE_SYSTEM                            // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( error != errOK )
      PR_TRACE( (root,prtDANGER,"List(IID_LIST) registered [error = 0x%08X]",error) );
  #endif

  PR_TRACE_A1( root, "Leave \"List::Register\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( List, Registration call )
// --------------------------------------------------------------------------------








// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Property method implementation )
// Interface "List". Method "Set" for property(s):
//  -- LIST_GRANULARITY
tERROR pr_call List_PROP_GranularitySet( hi_List _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size )
{
    tERROR error = errOK;

    (void)prop;

    PR_TRACE_A0( ((hOBJECT)_this), "Enter *SET* method \"List_PROP_GranularitySet\" for property \"pgLIST_GRANULARITY\"" );


    error = _this->data->heap->sys->PropertySet((hOBJECT)(_this->data->heap),(tDWORD*)out_size,pgHEAP_GRANULARITY,buffer,size);

    PR_TRACE_A2( ((hOBJECT)_this), "Leave *SET* method \"List_PROP_GranularitySet\" for property \"pgLIST_GRANULARITY\", ret tUINT = %u(size), error = 0x%08X", *out_size, error );
    return error;
}
// AVP Prague stamp end( List, Property method implementation )
// --------------------------------------------------------------------------------




static tLIST ListAlloc(hi_List _this, tPTR data, tUINT count)
{
    tLIST node;

    node = NULL;

    if ( errOK == _this->data->heap->vtbl->Alloc(_this->data->heap,(tVOID*)&node,count+sizeof(thLIST)))
    {
        node->prev = NULL;
        node->next = NULL;
        node->size = count;
        memcpy(((tBYTE*)(node))+sizeof(thLIST),data,count);
    }

    return node;
}

static tLIST ListValid(hi_List _this, tLISTNODE node)
{
    (void)_this;

    return (tLIST)(node);
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, Internal (kernel called) interface method implementation )
// --- Interface "List". Method "ObjectInit"
// Extended method comment
//   Kernel notifies an object about successful creating of object
// Behaviour comment
//   Initializes internal object data
// Result comment
//   Returns value differ from errOK if object cannot be initialized
tERROR pr_call List_ObjectInit( hi_List _this )
{
    tERROR error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::ObjectInit\" method" );

    // Place your code here
    error=CALL_SYS_ObjectCreateQuick(_this,&_this->data->heap,IID_HEAP,PID_ANY,SUBTYPE_ANY);

    PR_TRACE_A1( ((hOBJECT)_this), "Leave \"List::ObjectInit\" method, ret tERROR = 0x%08X", error );
    return error;
}
// AVP Prague stamp end( List, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------




static tLIST _List_Add(hi_List _this, tLIST node, tLISTNODE _where, tUINT mode)
{
    tLIST where;

    switch ( mode )
    {
        case cLIST_AFTER:
            if ( (where = ListValid(_this,_where)) != NULL )
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
                    _this->data->tail = node;
                }
            }
            else goto error;
            break;

        case cLIST_BEFORE:
            if ( (where = ListValid(_this,_where)) != NULL )
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
                    _this->data->head = node;
                }
            }
            else goto error;
            break;

        case cLIST_FIRST:
            if ( _this->data->head != NULL )
            {
				_this->data->head->prev = node;
                node->next = _this->data->head;
            }
			else
			{
				_this->data->tail = node;
			}
            _this->data->head = node;
            break;

        case cLIST_LAST:
            if ( _this->data->tail != NULL )
            {
				_this->data->tail->next = node;
                node->prev = _this->data->tail;
            }
			else
			{
				_this->data->head = node;
			}
            _this->data->tail = node;
            break;

        default:
            goto error;
    }
    return node;

error:
    _this->data->heap->vtbl->Free(_this->data->heap,node);
//    _this->sys->ErrorCodeSet((hOBJECT)_this,errPARAMETER_INVALID);
    return cLIST_NULL;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "Add"
tERROR pr_call List_Add( hi_List _this, tLISTNODE* result, tPTR data, tUINT count, tLISTNODE _where, tUINT mode )
{
    tLIST node;
    tERROR    error = errOK;

    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::Add\" method" );

  // Place your code here

    node = ListAlloc(_this,data,count);
    if ( node != NULL )
    {
        node = _List_Add(_this,node,_where,mode);
		if (node == NULL)
			error = errPARAMETER_INVALID;
    }
    else
    {
        error = errNOT_ENOUGH_MEMORY;
    }

  if ( result )
    *result = (tLISTNODE)(node);

  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"List::Add\" method, ret tLISTNODE = %p, error = 0x%08X", (tLISTNODE)(node), error );
  return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------

void _List_Remove(hi_List _this, tLIST node)
{
    if ( node->prev == NULL )
    {
        _this->data->head = node->next;
    }
    else
    {
        node->prev->next = node->next;
    }

    if ( node->next == NULL )
    {
        _this->data->tail = node->prev;
    }
    else
    {
        node->next->prev = node->prev;
    }
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "Remove"
tERROR pr_call List_Remove( hi_List _this, tLISTNODE _node )
{
    tLIST node;
    tERROR error;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::Remove\" method" );

  // Place your code here

    if ( (node = ListValid(_this,_node)) != NULL )
    {
        _List_Remove(_this,node);
        _this->data->heap->vtbl->Free(_this->data->heap,node);
        error = errOK;
    }
    else
    {
        error = errPARAMETER_INVALID;
    }

    PR_TRACE_A1( ((hOBJECT)_this), "Leave \"List::Remove\" method, ret tERROR = 0x%08X", error );
    return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "Move"
tERROR pr_call List_Move( hi_List _this, tLISTNODE _node, tLISTNODE _where, tUINT mode )
{
    tLIST node;
    tERROR error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::Move\" method" );

    // Place your code here

    if ( (node = ListValid(_this,_node)) != NULL )
    {
        if ( (mode != cLIST_BEFORE && mode != cLIST_AFTER) || ListValid(_this,_where) != NULL )
        {
            _List_Remove(_this,node);
            _List_Add(_this,node,_where,mode);
        }
        else
        {
            error = errPARAMETER_INVALID;
        }
    }
    else
    {
    }

  PR_TRACE_A1( ((hOBJECT)_this), "Leave \"List::Move\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "First"
tERROR pr_call List_First( hi_List _this, tLISTNODE* result )
{
  tERROR    error = errOK;
  PR_TRACE_A0( _this, "Enter \"List::First\" method" );

  if ( result )
    *result = (tLISTNODE)(_this->data->head);

  if (!_this->data->head)
	  error = errEND_OF_THE_LIST;

  PR_TRACE_A2( _this, "Leave \"List::First\" method, ret tLISTNODE = %p, error = 0x%08X", (tLISTNODE)(_this->data->head), error );
  return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "Last"
tERROR pr_call List_Last( hi_List _this, tLISTNODE* result )
{
  tERROR    error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::Last\" method" );

  // Place your code here

  if (!_this->data->tail)
	  error = errEND_OF_THE_LIST;

  if ( result )
    *result = (tLISTNODE)(_this->data->tail);
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"List::Last\" method, ret tLISTNODE = %p, error = 0x%08X", (tLISTNODE)(_this->data->tail), error );
  return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "Next"
tERROR pr_call List_Next( hi_List _this, tLISTNODE* result, tLISTNODE _node )
{
    tERROR    error = errPARAMETER_INVALID;
    tLIST node;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::Next\" method" );

  // Place your code here

    if ( (node = ListValid(_this,_node)) != NULL )
    {
        node = node->next;
        error = errOK;
    }

  if (!node)
	  error = errEND_OF_THE_LIST;

  if ( result )
    *result = (tLISTNODE)(node);
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"List::Next\" method, ret tLISTNODE = %p, error = 0x%08X", (tLISTNODE)(node), error );
  return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "Prev"
tERROR pr_call List_Prev( hi_List _this, tLISTNODE* result, tLISTNODE _node )
{
    tLIST node;
    tERROR    error = errPARAMETER_INVALID;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::Prev\" method" );

    // Place your code here
    if ( (node = ListValid(_this,_node)) != NULL )
    {
        node = node->prev;
        error = errOK;
    }

  if (!node)
	  error = errEND_OF_THE_LIST;

  if ( result )
    *result = (tLISTNODE)(node);
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"List::Prev\" method, ret tLISTNODE = %p, error = 0x%08X", (tLISTNODE)(node), error );
  return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "DataGet"
tERROR pr_call List_DataGet( hi_List _this, tUINT* result, tLISTNODE node, tPTR buffer, tUINT count)
{
    tLIST inode;
    tERROR error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::DataGet\" method" );

  // Place your code here


    if ( (inode = ListValid(_this,node)) != NULL )
    {
        if ( buffer != NULL )
        {
            if ( count < inode->size )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                count = inode->size;
            }
            memcpy(buffer,(tBYTE*)(inode+1),count);
        }
        else
        {
            count = inode->size;
        }
    }
    else
    {
        error = errPARAMETER_INVALID;
        count = 0;
    }


  if ( result )
    *result = count;
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"List::DataGet\" method, ret tUINT = %u, error = 0x%08X", count, error );
  return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "DataSet"
tERROR pr_call List_DataSet( hi_List _this, tLISTNODE* result, tLISTNODE _node, tPTR buffer, tUINT size )
{
    tLIST node;
    tLIST newnode;
  tERROR    error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::DataSet\" method" );

  // Place your code here

    if ( (node = ListValid(_this,_node)) != NULL )
    {
        if ( node->size >= size )
        {
            memcpy((tBYTE*)(node+1),buffer,size);
            node->size = size;
        }
        else
        {
            newnode = ListAlloc(_this,buffer,size);
            if ( newnode != NULL )
            {
                newnode->prev = node->prev;
                newnode->next = node->next;

                if ( node->prev == NULL )
                {
                    _this->data->head = newnode;
                }
                else
                {
                    node->prev->next = newnode;
                }

                if ( node->next == NULL )
                {
                    _this->data->tail = newnode;
                }
                else
                {
                    node->next->prev = newnode;
                }
                _this->data->heap->vtbl->Free(_this->data->heap,node);
                node = newnode;
            }
            else
            {
                error = errNOT_ENOUGH_MEMORY;
                node = NULL;
            }
        }
    }
    else
    {
        error = errPARAMETER_INVALID;
    }


  if ( result )
    *result = (tLISTNODE)(node);
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"List::DataSet\" method, ret tLISTNODE = %p, error = 0x%08X", (tLISTNODE)(node), error );
  return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( List, External (user called) interface method implementation )
// --- Interface "List". Method "Clear"
tERROR pr_call List_Clear( hi_List _this )
{
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"List::Clear\" method" );

  // Place your code here
    _this->data->tail = NULL;
    _this->data->head = NULL;
    _this->data->heap->vtbl->Clear(_this->data->heap);

  PR_TRACE_A1( ((hOBJECT)_this), "Leave \"List::Clear\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( List, External (user called) interface method implementation )
// --------------------------------------------------------------------------------
