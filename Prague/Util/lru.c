// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Monday,  19 May 2003,  17:07 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- lru.c
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define LRU_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include <Prague/iface/i_lrustg.h>

#include "lru.h"
// AVP Prague stamp end



#define AVL_DEPTH 32
#define AVL_LEFT     -1
#define AVL_BALANCED  0
#define AVL_RIGHT     1

#define AVL_NEXT 0
#define AVL_PREV -1

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// LRU interface implementation
// Short comments --
// AVP Prague stamp end




typedef struct sNODE* tNODE;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface LRU. Inner data structure
typedef struct tag_LruData{
    tUINT         size;         // --
    tLRU_MODE     mode;         // --
    tERROR        write_error;  // --
    tUINT         hits;         // --
    tUINT         misses;       // --
    struct sNODE* root;         // --
    struct sNODE* head;         // --
    struct sNODE* tail;         // --
    struct sNODE* begin;        // --
    hLRUSTG       storage;      // --
    tNODE*        nodes;        // --
} LruData;

// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_LRU {
    const iLRUVtbl*    vtbl; // pointer to the "LRU" virtual table
    const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
    LruData*           data; // pointer to the "LRU" data structure
} *hi_LRU;

// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call LRU_ObjectClose( hi_LRU _this );
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_LRU_vtbl = {
    (tIntFnRecognize)        NULL,
    (tIntFnObjectNew)        NULL,
    (tIntFnObjectInit)       NULL,
    (tIntFnObjectInitDone)   NULL,
    (tIntFnObjectCheck)      NULL,
    (tIntFnObjectPreClose)   NULL,
    (tIntFnObjectClose)      LRU_ObjectClose,
    (tIntFnChildNew)         NULL,
    (tIntFnChildInitDone)    NULL,
    (tIntFnChildClose)       NULL,
    (tIntFnMsgReceive)       NULL,
    (tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpLRU_Lock)   ( hi_LRU _this, tDWORD* result, tDWORD ext, tBOOL write );     // -- ;
typedef   tERROR (pr_call *fnpLRU_Unlock) ( hi_LRU _this, tDWORD ext );                  // -- ;
typedef   tERROR (pr_call *fnpLRU_Flush)  ( hi_LRU _this );                              // -- ;
typedef   tERROR (pr_call *fnpLRU_Clear)  ( hi_LRU _this );                              // -- ;
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iLRUVtbl {
    fnpLRU_Lock    Lock;
    fnpLRU_Unlock  Unlock;
    fnpLRU_Flush   Flush;
    fnpLRU_Clear   Clear;
}; // "LRU" external virtual table prototype
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call LRU_Lock( hi_LRU _this, tDWORD* result, tDWORD ext, tBOOL write );
tERROR pr_call LRU_Unlock( hi_LRU _this, tDWORD ext );
tERROR pr_call LRU_Flush( hi_LRU _this );
tERROR pr_call LRU_Clear( hi_LRU _this );
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iLRUVtbl e_LRU_vtbl = {
    LRU_Lock,
    LRU_Unlock,
    LRU_Flush,
    LRU_Clear
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call LRU_PROP_SetSize( hi_LRU _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call LRU_PROP_SetMode( hi_LRU _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "LRU". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define LRU_VERSION ((tVERSION)1)
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(LRU_PropTable)
    mSHARED_PROPERTY( pgINTERFACE_VERSION, LRU_VERSION )
    mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Last Recently Used interface", 29 )
    mLOCAL_PROPERTY( pgLRU_SIZE, LruData, size, cPROP_BUFFER_READ_WRITE | cPROP_REQUIRED, NULL, LRU_PROP_SetSize )
    mLOCAL_PROPERTY( pgLRU_MODE, LruData, mode, cPROP_BUFFER_READ, NULL, LRU_PROP_SetMode )
    mLOCAL_PROPERTY_BUF( pgLRU_WRITE_ERROR, LruData, write_error, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgLRU_HITS, LruData, hits, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgLRU_MISSES, LruData, misses, cPROP_BUFFER_READ )
mPROPERTY_TABLE_END(LRU_PropTable)
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "LRU". Register function
tERROR pr_call LRU_Register( hROOT root ){
  tERROR error;

  PR_TRACE_A0( ((hOBJECT)root), "Enter \"LRU::Register\" method" );

    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_LRU,                                // interface identifier
        PID_UTIL,                               // plugin identifier
        0x00000000,                             // subtype identifier
        LRU_VERSION,                            // interface version
        VID_ANDREW,                             // interface developer
        &i_LRU_vtbl,                            // internal(kernel called) function table
        (sizeof(i_LRU_vtbl)/sizeof(tPTR)),      // internal function table size
        &e_LRU_vtbl,                            // external function table
        (sizeof(e_LRU_vtbl)/sizeof(tPTR)),      // external function table size
        LRU_PropTable,                          // property table
        mPROPERTY_TABLE_SIZE(LRU_PropTable),    // property table size
        sizeof(LruData),                        // memory size
        IFACE_SYSTEM                            // interface flags
    );

  #ifdef _PRAGUE_TRACE_
    if ( error != errOK )
      PR_TRACE( (root,prtDANGER,"LRU(IID_LRU) registered [error = 0x%08X]",error) );
  #endif

  PR_TRACE_A1( root, "Leave \"LRU::Register\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end








#define cNODE_DIRTY      1
#define cNODE_USED       2
#define cNODE_READ_LOCK  4
#define cNODE_WRITE_LOCK 8
#define cNODE_LOCK (cNODE_READ_LOCK | cNODE_WRITE_LOCK)


typedef struct
{
    hi_LRU tree;                      /* tree root */
    struct sNODE **nodeptr;             /* pointer to current node */
    signed char  *pathptr;
    struct sNODE *node[AVL_DEPTH];      /* node path */
    signed char  path[AVL_DEPTH];       /* balance path */
    int  level;                         /* current level */
    tDWORD ext_block;                   /* block to find */
} tTREEDATA;


typedef struct sNODE
{
    struct  sNODE *left;
    struct  sNODE *child[2];
    struct  sNODE *next;
    struct  sNODE *prev;
    tDWORD  ext_block;
    tSBYTE  balance;
    tSBYTE  mode;
} thNODE;


static int __fastcall compare(tDWORD value1, tDWORD value2)
{
    int rcode;

    if ( value1 == value2 )
    {
        rcode = 0;
    }
    else if ( value1 < value2 )
    {
        rcode = -1;
    }
    else
    {
        rcode = 1;
    }
    return rcode;
}


static tNODE __fastcall __TreeSearch(tNODE node,tTREEDATA *data)
{
    int res;

    while ( node != NULL )
    {

        if ( (res = compare(node->ext_block,data->ext_block)) == 0 )
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

static tNODE __fastcall _TreeSearch(hi_LRU tree, tTREEDATA *tdata, tDWORD value)
{
    tdata->ext_block = value;
    tdata->level = 0;
    tdata->tree = tree;
    tdata->nodeptr = tdata->node;
    tdata->pathptr = tdata->path;

    return __TreeSearch(tree->data->root,tdata);
}

static void __fastcall _List_Remove(hi_LRU list, tNODE node)
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

static void __fastcall _Tree_Remove(hi_LRU tree, tNODE node)
{
    tTREEDATA tdata;

    tNODE *tnode;
    tNODE delnode;
    tSBYTE *tpath;
    tNODE left, right;
    tSBYTE res;
    int taller;

    _TreeSearch(tree,&tdata,node->ext_block);

    if ( node->child[AVL_LEFT] != NULL && node->child[AVL_RIGHT] != NULL )
    {
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
        /* node   - leftmost node in right tree */
        /* *tnode - parent of node for delete */

        (*tnode)->child[*tpath] = node;

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

    /* balance up tree */
    tdata.level--;
    tdata.nodeptr--;
    tdata.pathptr--;
    taller = 1;

    while ( tdata.level-- >= 0 )
    {
        /* balance tree from this node */
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
    tree->data->root = node;
}

static void _Tree_Add(hi_LRU tree, tNODE node)
{
    int taller;
    tTREEDATA tdata;
    tNODE *tnode;
    tNODE left, right;
    signed char *tpath, res;

    /* node not found */
    _TreeSearch(tree,&tdata,node->ext_block);


    /* add new node in level position */


    /* balance up tree */

    tdata.level--;
    tnode = tdata.node+tdata.level;
    tpath = tdata.path+tdata.level;
    taller = 1;

    while ( tdata.level >= 0 )
    {
        /* balance tree from this node */
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

                    /* different sides, rebalance tree */

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
    tree->data->root = node;

}

tERROR pr_call LRU_ObjectInit( hi_LRU tree)
{
    hLRUSTG storage;
    tERROR error;

    storage = (hLRUSTG)tree->sys->ParentGet((hOBJECT)(tree),IID_ANY);
    if ( storage->sys->PropertyGetDWord((hOBJECT)(storage),pgINTERFACE_ID) == IID_LRUSTG )
    {
        tree->data->storage = storage;
        error = errOK;
    }
    else
    {
        error = errINTERFACE_INCOMPATIBLE;
    }
    return error;
}

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetSize )
// Interface "LRU". Method "Set" for property(s):
//  -- LRU_SIZE
tERROR pr_call LRU_PROP_SetSize( hi_LRU tree, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size )
{
    tUINT rcode;
    tERROR error = errOK;
    PR_TRACE_A0( ((hOBJECT)tree), "Enter *SET* method \"LRU_PROP_SetSize\" for property \"pgLRU_SIZE\"" );

    (void)prop;

    rcode = 0;

    if ( size == sizeof(tree->data->size) )
    {
        if ( *((tUINT*)(buffer)) > 3 )
        {

            LRU_Flush(tree);

            rcode = sizeof(tree->data->size);

            if ( *((tUINT*)(buffer)) != tree->data->size )
            {
                // petrovitch 16.05.03 (because MemoryRealloc became obsolete)
                //if ( errOK == tree->sys->MemoryRealloc((hOBJECT)(tree),sizeof(LruData)+*((tUINT*)(buffer))*sizeof(thNODE)) )
                if ( PR_SUCC(error=CALL_SYS_ObjHeapRealloc(tree,(tPTR*)&tree->data->nodes,tree->data->nodes,*((tUINT*)(buffer))*sizeof(thNODE))) )
                {
                    tree->data->size = *((tUINT*)(buffer));
                }
                else
                {
                    rcode = 0;
                    //error = errNOT_ENOUGH_MEMORY;
                }
            }
            LRU_Clear(tree);
        }
    }

    *out_size = rcode;

  PR_TRACE_A2( ((hOBJECT)tree), "Leave *SET* method \"LRU_PROP_SetSize\" for property \"pgLRU_SIZE\", ret tUINT = %u(size), error = 0x%08X", *out_size, error );
  return error;
}
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetMode )
// Interface "LRU". Method "Set" for property(s):
//  -- LRU_MODE
tERROR pr_call LRU_PROP_SetMode( hi_LRU tree, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size )
{
    tERROR error = errOK;
    tUINT rcode;
    PR_TRACE_A0( ((hOBJECT)tree), "Enter *SET* method \"LRU_PROP_SetMode\" for property \"pgLRU_MODE\"" );


    (void)prop;

    rcode = 0;
    if ( size == sizeof(tree->data->mode) )
    {
        if ( *((tLRU_MODE*)(buffer)) <= 3 )
        {
            tree->data->mode = *((tLRU_MODE*)(buffer));
            rcode = sizeof(tree->data->mode);
        }
    }
    *out_size = rcode;

  PR_TRACE_A2( ((hOBJECT)tree), "Leave *SET* method \"LRU_PROP_SetMode\" for property \"pgLRU_MODE\", ret tUINT = %u(size), error = 0x%08X", *out_size, error );
  return error;
}
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// --- Interface "LRU". Method "ObjectClose"
// Extended method comment
//   Kernel warns object it must be closed
// Behaviour comment
//   Object takes all necessary "before closing" actions
// Result comment
//
tERROR pr_call LRU_ObjectClose( hi_LRU tree )
{

  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)tree), "Enter \"LRU::ObjectClose\" method" );

  // Place your code here

  error = LRU_Flush(tree);

  PR_TRACE_A1( ((hOBJECT)tree), "Leave \"LRU::ObjectClose\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Lock )
// --- Interface "LRU". Method "Get"
tERROR pr_call LRU_Lock( hi_LRU tree, tDWORD* result, tDWORD ext, tBOOL write )
{
    tNODE node;
    tTREEDATA data;
    tDWORD int_block;
    tERROR error = errOK;
    PR_TRACE_A0( ((hOBJECT)tree), "Enter \"LRU::Get\" method" );

    #ifdef _PRAGUE_TRACE_
      int_block = cLRU_ERROR;
    #endif
  // Place your code here

    /* try to find in first block */
    if ( ext == tree->data->head->ext_block && (tree->data->head->mode & cNODE_USED))
    {
        int_block = _toui32(tree->data->head - tree->data->begin);
        tree->data->hits++;
    }
    else
    {
        // try to find node in cache
        node = _TreeSearch(tree,&data,ext);

        if ( node == NULL )
        {
            tree->data->misses++;
            // get node from the end of list
            node = tree->data->tail;

            if ( node == NULL )
            {
                /* no more nodes in free list */
                error = errBUFFER_TOO_SMALL;
                goto abort;
            }


            _List_Remove(tree,node);

            int_block = _toui32(node-tree->data->begin);

            if ( int_block >= tree->data->size )
            {
                /* unknown node found */
                error = errUNEXPECTED;
                goto abort;
            }

            if ( node->mode & cNODE_USED )
            {
                if ( node->mode & cNODE_DIRTY )
                {
                    error = tree->data->storage->vtbl->Write(tree->data->storage,int_block,node->ext_block);
                    if ( error != errOK )
                    {
                        tree->data->write_error = error;
                    }
                }

                /* remove node from tree */
                _Tree_Remove(tree,node);
            }

            node->ext_block = ext;
            error = tree->data->storage->vtbl->Read(tree->data->storage,int_block,node->ext_block);
            if ( error != errOK )
            {
                /* this node become free, add to the end */
                node->mode = 0;
                node->prev = tree->data->tail;
                tree->data->tail->next = node;
                tree->data->tail = node;
                goto abort;
            }
            else
            {
                _Tree_Add(tree,node);
                node->mode = cNODE_USED;
                if ( write )
                {
                    node->mode |= cNODE_WRITE_LOCK | cNODE_DIRTY;
                }
                else
                {
                    node->mode |= cNODE_READ_LOCK;
                }
            }
        }
        else
        {
            if ( node->mode & cNODE_LOCK )
            {
                error = errACCESS_DENIED;
                goto abort;
            }
            // remove node from its position (touch)
            tree->data->hits++;
            _List_Remove(tree,node);
            int_block = _toui32(node-tree->data->begin);
        }

    }

  if ( result )
    *result = int_block;

    abort:
  if ( error != errOK )
  {
     int_block = cLRU_ERROR;
//     CALL_SYS_ErrorCodeSet( tree, error );
     if ( result )
     {
        *result = int_block;
     }
  }

  PR_TRACE_A2( ((hOBJECT)tree), "Leave \"LRU::Get\" method, ret tDWORD = %u, error = 0x%08X", int_block, error );
  return error;
}
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// --- Interface "LRU". Method "Flush"
tERROR pr_call LRU_Flush( hi_LRU tree )
{
    tNODE node;
    tERROR error = errOK;
    PR_TRACE_A0( ((hOBJECT)tree), "Enter \"LRU::Flush\" method" );

  // Place your code here

    node = tree->data->head;

    while ( node != NULL )
    {
        if ( node->mode & cNODE_DIRTY )
        {
            if ( node->mode & cNODE_DIRTY )
            {
                error = tree->data->storage->vtbl->Write(tree->data->storage,_toui32(node-tree->data->begin),node->ext_block);
                if ( error != errOK )
                {
                    tree->data->write_error = error;
                }
            }
            node->mode &= ~cNODE_DIRTY;
        }
        node = node->next;
    }

    error = tree->data->write_error;
    tree->data->write_error = errOK;

  PR_TRACE_A1( ((hOBJECT)tree), "Leave \"LRU::Flush\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Clear )
// --- Interface "LRU". Method "Clear"
tERROR pr_call LRU_Clear( hi_LRU tree )
{
    tNODE node;
    tUINT count;
  tERROR error = errOK;
  PR_TRACE_A0( ((hOBJECT)tree), "Enter \"LRU::Clear\" method" );

  // Place your code here

    count = tree->data->size-1;
    // petrovitch 16.05.03 (because MemoryRealloc became obsolete)
    //node = (tNODE)(tree->data+1);
    node = (tNODE)(tree->data+1);
    tree->data->begin = node;
    tree->data->head = node;

    while ( count-- )
    {
        node->mode = 0;
        node->next = node+1;
        node->prev = node-1;
        node++;
    }

    node->mode = 0;
    node->prev = node-1;
    node->next = NULL;
    tree->data->tail = node;

    tree->data->head->prev = NULL;
    tree->data->write_error = errOK;
    tree->data->hits = 0;
    tree->data->misses = 0;

  PR_TRACE_A1( ((hOBJECT)tree), "Leave \"LRU::Clear\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Unlock )
// --- Interface "LRU". Method "Unlock"
tERROR pr_call LRU_Unlock( hi_LRU tree, tDWORD ext )
{
    tNODE node;
    tTREEDATA data;
    tERROR error = errOK;
	PR_TRACE_A0( tree, "Enter \"LRU::Unlock\" method" );

	// Place your code here
    // try to find node in cache
    node = _TreeSearch(tree,&data,ext);

    if ( node == NULL )
    {
        error = errUNEXPECTED;
    }
    else
    {
        if ( node->mode & cNODE_LOCK )
        {
            node->mode &= ~(cNODE_LOCK);
            // add node to head ( will move to Unlock )
            tree->data->head->prev = node;
            node->next = tree->data->head;
            node->prev = NULL;
        }
        else
        {
            error = errOBJECT_ALREADY_FREED;
        }
    }

	PR_TRACE_A1( tree, "Leave \"LRU::Unlock\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end



