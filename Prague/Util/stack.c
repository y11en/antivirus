// -------- Monday,  18 September 2000,  15:27 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- _this.c
// -------------------------------------------




#define Stack_PRIVATE_DEFINITION

#include <memory.h>

#include <Prague/prague.h>
#include <Prague/iface/i_heap.h>

#include "stack.h"






// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Interface comment )
// --------------------------------------------
// --- 9e55aca3_6dda_11d4_b94e_008048ec2fc7 ---
// --------------------------------------------
// Stack interface implementation
// Extended comment
//
// AVP Prague stamp end( Stack, Interface comment )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Data structure )
// Interface Stack. Inner data structure
typedef struct tag_StackData
{
	tUINT granularity; // --
	tUINT node_size; // --
	tUINT limit; // --
    hHEAP heap;
    struct sSTACK *top;
    tUINT size;
} StackData;
// AVP Prague stamp end( Stack, Data structure )
// --------------------------------------------------------------------------------

typedef struct sSTACK
{
    struct sSTACK *prev;
    tUINT size;
} thSTACK, *tSTACK   ;


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, object declaration )
typedef struct tag_hi_Stack
{
  const iStackVtbl*  vtbl; // pointer to the "Stack" virtual table
  const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
  StackData*        data;   // pointer to the "Stack" data structure
} *hi_Stack;
// AVP Prague stamp end( Stack, object declaration )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Internal method table prototypes )
// Interface "Stack". Internal method table. Forward declarations
tERROR pr_call Stack_ObjectInit( hi_Stack _this );
// AVP Prague stamp end( Stack, Internal method table prototypes )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Internal method table )
// Interface "Stack". Internal method table.
static iINTERNAL i_Stack_vtbl =
{
  (tIntFnRecognize)        NULL,
  (tIntFnObjectNew)        NULL,
  (tIntFnObjectInit)       Stack_ObjectInit,
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
// AVP Prague stamp end( Stack, Internal method table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, interface function forward declarations )
typedef   tERROR (pr_call *fnpStack_Push)  ( hi_Stack _this, tPTR data, tUINT size );                // -- ;
typedef   tERROR (pr_call *fnpStack_Pop)   ( hi_Stack _this, tUINT* result, tPTR buffer, tUINT size );              // -- ;
typedef   tERROR (pr_call *fnpStack_Get)   ( hi_Stack _this, tUINT* result, tUINT index, tPTR buffer, tUINT size ); // -- ;
typedef   tERROR (pr_call *fnpStack_Set)   ( hi_Stack _this, tUINT index, tPTR buffer, tUINT size ); // -- ;
typedef   tERROR (pr_call *fnpStack_Size)  ( hi_Stack _this, tUINT* result );                                       // -- ;
typedef   tERROR (pr_call *fnpStack_Clear) ( hi_Stack _this, tUINT index );                          // -- ;
// AVP Prague stamp end( Stack, interface function forward declarations )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, interface declaration )
struct iStackVtbl
{
  fnpStack_Push   Push;
  fnpStack_Pop    Pop;
  fnpStack_Get    Get;
  fnpStack_Set    Set;
  fnpStack_Size   Size;
  fnpStack_Clear  Clear;
}; // Stack
// AVP Prague stamp end( Stack, interface declaration )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, External method table prototypes )
// Interface "Stack". External method table. Forward declarations
tERROR pr_call Stack_Push( hi_Stack _this, tPTR data, tUINT size );
tERROR pr_call Stack_Pop( hi_Stack _this, tUINT* result, tPTR buffer, tUINT size );
tERROR pr_call Stack_Get( hi_Stack _this, tUINT* result, tUINT index, tPTR buffer, tUINT size );
tERROR pr_call Stack_Set( hi_Stack _this, tUINT index, tPTR buffer, tUINT size );
tERROR pr_call Stack_Size( hi_Stack _this, tUINT* result );
tERROR pr_call Stack_Clear( hi_Stack _this, tUINT index );
// AVP Prague stamp end( Stack, External method table prototypes )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, External method table )
// Interface "Stack". External method table.
static iStackVtbl e_Stack_vtbl =
{
  Stack_Push,
  Stack_Pop,
  Stack_Get,
  Stack_Set,
  Stack_Size,
  Stack_Clear
};
// AVP Prague stamp end( Stack, External method table )
// --------------------------------------------------------------------------------







// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Forwarded property methods declarations )
// Interface "Stack". Get/Set property methods
tERROR pr_call Stack_PROP_GranularitySet( hi_Stack _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( Stack, Forwarded property methods declarations )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Global property variable declaration )
// Interface "Stack". Global(shared) property table variables
// AVP Prague stamp end( Stack, Global property variable declaration )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Property table )
// Interface "Stack". Property table
mPROPERTY_TABLE(Stack_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, 1 )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Stack handling interface", 25 )
  mLOCAL_PROPERTY( pgSTACK_GRANULARITY, StackData, granularity, cPROP_BUFFER_READ_WRITE, NULL, Stack_PROP_GranularitySet )
  mLOCAL_PROPERTY_BUF( pgSTACK_NODE_SIZE, StackData, node_size, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF( pgSTACK_LIMIT, StackData, limit, cPROP_BUFFER_READ_WRITE )
mPROPERTY_TABLE_END(Stack_PropTable)
// AVP Prague stamp end( Stack, Property table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Registration call )
// Interface "Stack". Register function
tERROR pr_call Stack_Register( hROOT root )
{
  tERROR error;

  PR_TRACE_A0( ((hOBJECT)root), "Enter \"Stack::Register\" method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_STACK,                              // interface identifier
    PID_UTIL,                               // plugin identifier
    0x00000000,                             // subtype identifier
    0x00000001,                             // interface version
    VID_ANDREW,                             // interface developer
    &i_Stack_vtbl,                          // internal(kernel called) function table
    (sizeof(i_Stack_vtbl)/sizeof(tPTR)),    // internal function table size
    &e_Stack_vtbl,                          // external function table
    (sizeof(e_Stack_vtbl)/sizeof(tPTR)),    // external function table size
    Stack_PropTable,                        // property table
    mPROPERTY_TABLE_SIZE(Stack_PropTable),  // property table size
    sizeof(StackData),                      // memory size
    IFACE_SYSTEM                            // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( error != errOK )
      PR_TRACE( (root,prtDANGER,"Stack(IID_STACK) registered [error = 0x%08X]",error) );
  #endif

  PR_TRACE_A1( root, "Leave \"Stack::Register\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( Stack, Registration call )
// --------------------------------------------------------------------------------








// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Property method implementation )
// Interface "Stack". Method "Set" for property(s):
//  -- STACK_GRANULARITY
tERROR pr_call Stack_PROP_GranularitySet( hi_Stack _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size )
{
  tERROR error = errOK;

  PR_TRACE_A0( ((hOBJECT)_this), "Enter *SET* method \"Stack_PROP_GranularitySet\" for property \"pgSTACK_GRANULARITY\"" );

    (void)prop;
    error = _this->data->heap->sys->PropertySet((hOBJECT)(_this->data->heap),(tDWORD*)out_size,pgHEAP_GRANULARITY,buffer,size);

  PR_TRACE_A2( ((hOBJECT)_this), "Leave *SET* method \"Stack_PROP_GranularitySet\" for property \"pgSTACK_GRANULARITY\", ret tUINT = %u(size), error = 0x%08X", *out_size, error );
  return error;
}
// AVP Prague stamp end( Stack, Property method implementation )
// --------------------------------------------------------------------------------



static tSTACK StackAlloc(hi_Stack _this, tPTR data, tUINT count)
{
    tSTACK node;


    if ( errOK == _this->data->heap->vtbl->Alloc(_this->data->heap,(tVOID*) &node,count+sizeof(thSTACK)))
    {
        node->prev = NULL;
        memcpy(((tBYTE*)(node))+sizeof(thSTACK),data,count);
    }

    return node;
}




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, Internal (kernel called) interface method implementation )
// --- Interface "Stack". Method "ObjectInit"
// Extended method comment
//   Kernel notifies an object about successful creating of object
// Behaviour comment
//   Initializes internal object data
// Result comment
//   Returns value differ from errOK if object cannot be initialized
tERROR pr_call Stack_ObjectInit( hi_Stack _this )
{
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"Stack::ObjectInit\" method" );

  // Place your code here
  error=CALL_SYS_ObjectCreateQuick(_this,&_this->data->heap,IID_HEAP,PID_ANY,SUBTYPE_ANY);

  PR_TRACE_A1( ((hOBJECT)_this), "Leave \"Stack::ObjectInit\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( Stack, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, External (user called) interface method implementation )
// --- Interface "Stack". Method "Push"
tERROR pr_call Stack_Push( hi_Stack _this, tPTR data, tUINT size )
{
    tSTACK node;
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"Stack::Push\" method" );

  // Place your code here

    if ( _this->data->limit != 0 )
    {
        if ( _this->data->size >= _this->data->limit )
        {
            error = errSTACK_OVERFLOW;
            goto abort;
        }
    }

    node = StackAlloc(_this,data,size);
    if ( node != NULL )
    {
        node->prev = _this->data->top;
        _this->data->top = node;
        _this->data->size++;
    }
    else
    {
        error = errNOT_ENOUGH_MEMORY;
    }
    abort:

  PR_TRACE_A1( ((hOBJECT)_this), "Leave \"Stack::Push\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( Stack, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, External (user called) interface method implementation )
// --- Interface "Stack". Method "Pop"
tERROR pr_call Stack_Pop( hi_Stack _this, tUINT* result, tPTR buffer, tUINT size )
{
    tSTACK top;
    tUINT rcode;
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"Stack::Pop\" method" );

  // Place your code here

    rcode = 0;

    if ( _this->data->size == 0 )
    {
        error = errSTACK_UNDERFLOW;
    }
    else
    {
        top = _this->data->top;

        if ( buffer == NULL )
        {
            /* size check */
            rcode = top->size;
        }
        else
        {
            if ( size < top->size )
            {
                error = errBUFFER_TOO_SMALL;
                rcode = 0;
            }
            else
            {
                memcpy(buffer,(tBYTE*)(top+1),rcode = _this->data->top->size);
                _this->data->size--;
                _this->data->top = top->prev;
                _this->data->heap->vtbl->Free(_this->data->heap,top);
            }
        }
    }

  if ( result )
    *result = rcode;
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"Stack::Pop\" method, ret tUINT = %u, error = 0x%08X", rcode, error );
  return error;
}
// AVP Prague stamp end( Stack, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, External (user called) interface method implementation )
// --- Interface "Stack". Method "Get"
tERROR pr_call Stack_Get( hi_Stack _this, tUINT* result, tUINT index, tPTR buffer, tUINT size )
{
    tSTACK top;
    tUINT rcode;
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"Stack::Get\" method" );

  // Place your code here

    rcode = 0;

    if ( index >= _this->data->size )
    {
        error = errSTACK_UNDERFLOW;
    }
    else
    {
        top = _this->data->top;

        while ( index-- )
        {
            top = top->prev;
        }

        if ( buffer == NULL )
        {
            /* size check */
            rcode = top->size;
        }
        else
        {
            if ( size < top->size )
            {
                error = errBUFFER_TOO_SMALL;
                rcode = 0;
            }
            else
            {
                memcpy(buffer,(tBYTE*)(top+1),rcode = _this->data->top->size);
            }
        }
    }

  if ( result )
    *result = rcode;
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"Stack::Get\" method, ret tUINT = %u, error = 0x%08X", rcode, error );
  return error;
}
// AVP Prague stamp end( Stack, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, External (user called) interface method implementation )
// --- Interface "Stack". Method "Set"
tERROR pr_call Stack_Set( hi_Stack _this, tUINT index, tPTR buffer, tUINT size )
{
    tSTACK top, prev, temp;
    tERROR error = errOK;
    PR_TRACE_A0( ((hOBJECT)_this), "Enter \"Stack::Set\" method" );

  // Place your code here

    if ( index >= _this->data->size )
    {
        error = errSTACK_UNDERFLOW;
    }
    else
    {
        top = _this->data->top;
        prev = NULL;

        while ( index-- )
        {
            prev = top;
            top = top->prev;
        }

        if ( size <= top->size )
        {
            memcpy((tBYTE*)(top+1),buffer,size);
            top->size = size;
        }
        else
        {
            temp = StackAlloc(_this,buffer,size);
            temp->prev = top->prev;

            if ( prev != NULL )
            {
                prev->prev = temp;
            }
            else
            {
                _this->data->top = temp;
            }
            _this->data->heap->vtbl->Free(_this->data->heap,top);
        }
    }

  PR_TRACE_A1( ((hOBJECT)_this), "Leave \"Stack::Set\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( Stack, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, External (user called) interface method implementation )
// --- Interface "Stack". Method "Size"
tERROR pr_call Stack_Size( hi_Stack _this, tUINT* result )
{
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"Stack::Size\" method" );

  // Place your code here

  if ( result )
    *result = _this->data->size;
  PR_TRACE_A2( ((hOBJECT)_this), "Leave \"Stack::Size\" method, ret tUINT = %u, error = 0x%08X", _this->data->size, error );
  return error;
}
// AVP Prague stamp end( Stack, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Stack, External (user called) interface method implementation )
// --- Interface "Stack". Method "Clear"
tERROR pr_call Stack_Clear( hi_Stack _this, tUINT index )
{
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)_this), "Enter \"Stack::Clear\" method" );

  // Place your code here

    if ( index == 0 )
    {
        _this->data->size = 0;
        _this->data->top = NULL;
        _this->data->heap->vtbl->Clear(_this->data->heap);

    }
    else
    {
        if ( index >= _this->data->size )
        {
            error = errSTACK_UNDERFLOW;
        }
        else
        {
            tSTACK top;

            while ( index-- )
            {
                top = _this->data->top;
                _this->data->size--;
                _this->data->top = top->prev;
                _this->data->heap->vtbl->Free(_this->data->heap,top);
            }
        }
    }


  PR_TRACE_A1( ((hOBJECT)_this), "Leave \"Stack::Clear\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end( Stack, External (user called) interface method implementation )
// --------------------------------------------------------------------------------
