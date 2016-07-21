#include <stdlib.h>
#include <memory.h>
#include "dlist.h"
#include "types.h"

/* ------------------------------------------------------------------------- */

// one list entry node
typedef struct sDL_NODE {
  struct sDL_NODE* Next;
  tVOID* Data;
} tDL_NODE, *pDL_NODE;

/* ------------------------------------------------------------------------- */

CDynList::CDynList(fDLIST_FREE FreeProc)
{
  Cnt = 0;             // elements count 
  Arr = NULL;          // data pointers array
  Chg = cTRUE;         // is changed flag
  Root = Tail = NULL;  // first and last node
  CurFreeProc = (NULL != FreeProc) ? (FreeProc) : (DefFreeProc);
}

/* ------------------------------------------------------------------------- */

/* default list-data free function */
tVOID CDynList::DefFreeProc(tVOID* DPtr)
{
  if ( NULL != DPtr )
    free(DPtr);
}

/* ------------------------------------------------------------------------- */

// make data pointers array
tVOID** CDynList::Array()
{
  tDL_NODE* Node;
  tVOID**   APtr;

  if ( cFALSE == Chg )
    return(Arr);  // ready array
  if ( 0 == Cnt )
    return(NULL); // empty array
  if ( NULL == (Arr = (tVOID**)realloc(Arr, Cnt * sizeof(tVOID*))) )
    return(NULL); // can't alloc

  // build array
  APtr = (tVOID**)(Arr);
  for ( Node = (tDL_NODE*)(Root); NULL != Node; Node = Node->Next )
  {
    *(APtr++) = Node->Data;
  }

  Chg = (cFALSE);
  return(Arr);
}

/* ------------------------------------------------------------------------- */

// free all resources
tVOID CDynList::Clear(tVOID)
{
  fDLIST_FREE FreeProc;
  tDL_NODE* Node;
  tDL_NODE* Next;

  // free array
  if ( NULL != Arr )
  {
    free(Arr);
    Arr = NULL;
  }
  
  // free data and nodes
  FreeProc = CurFreeProc;
  for ( Node = (tDL_NODE*)(Root); NULL != Node; )
  {
    // free data
    FreeProc(Node->Data);
    
    // free node
    Next = Node->Next;
    free(Node);
    Node = Next;
  }

  Cnt = 0;
  Chg = cTRUE;
  Root = Tail = NULL;
}

/* ------------------------------------------------------------------------- */

// add data as first element
tBOOL CDynList::AddHead(tVOID* DPtr)
{
  tDL_NODE* Node;

  // alloc node
  Node = (tDL_NODE*)malloc(sizeof(tDL_NODE));
  if ( NULL == Node )
    return(cFALSE); // can't alloc

  // set node data
  Node->Data = (DPtr);
  Node->Next = (tDL_NODE*)(Root);
  if ( NULL == Tail )
    Tail = Node;
  Root = Node;
  
  Cnt += 1;
  Chg = (cTRUE);
  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

// add data as last element
tBOOL CDynList::AddTail(tVOID* DPtr)
{
  tDL_NODE* Node;

  // alloc node
  Node = (tDL_NODE*)malloc(sizeof(tDL_NODE));
  if ( NULL == Node )
    return(cFALSE); // can't alloc

  // set node data
  Node->Data = (DPtr);
  Node->Next = (NULL);
  if ( NULL != Tail )
    ((tDL_NODE*)(Tail))->Next = Node;
  if ( NULL == Root )
    Root = Node;
  Tail = Node;

  Cnt += 1;
  Chg = (cTRUE);
  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

// delete element from list
tBOOL CDynList::Remove(tVOID* DPtr)
{
  tDL_NODE* Prev;
  tDL_NODE* Node;
  tDL_NODE* ENode;

  // find data node
  Prev = Node = (NULL);
  for ( ENode = (tDL_NODE*)(Root); NULL != ENode; ENode = ENode->Next )
  {
    if ( ENode->Data == DPtr )
    {
      Node = ENode;
      break;
    }
    Prev = (ENode);
  }

  // check for not founded
  if ( NULL == Node )
    return(cFALSE);

  // relink prev node
  if ( NULL != Prev )
    Prev->Next = Node->Next;

  // check for root or tail node
  if ( Root == Node )
    Root = Node->Next;
  if ( Tail == Node )
    Tail = Prev;

  // free data and node
  CurFreeProc(Node->Data);
  free(Node);

  Cnt = (Cnt - 1);
  Chg = (cTRUE);
  return(cTRUE);
}               

/* ------------------------------------------------------------------------- */

// swap two nodes by data pointers
tBOOL CDynList::Swap(tVOID* DPtr1, tVOID* DPtr2)
{
  tDL_NODE* Node1;
  tDL_NODE* Node2;
  tDL_NODE* ENode;
  tVOID*    Data;

  if ( DPtr1 == DPtr2 )
    return(cTRUE); // crosslink

  // find data nodes
  Node1 = Node2 = NULL;
  ENode = (tDL_NODE*)(Root);
  for ( ; NULL != ENode; ENode = ENode->Next )
  {
    if ( NULL == Node1 && ENode->Data == DPtr1 )
    {
      Node1 = ENode;
    }
    else if ( NULL == Node2 && ENode->Data == DPtr2 )
    {
      Node2 = ENode;
    }
    else if ( NULL != Node1 && NULL != Node2 )
      break;
  }

  // check for not founded
  if ( NULL == ENode )
    return(cFALSE);
  
  // swap nodes data
  Data = Node1->Data;
  Node1->Data = Node2->Data;
  Node2->Data = Data;
  
  Chg = (cTRUE);
  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

// overloaded [] operator, read only
tVOID* CDynList::operator[] (tUINT I)
{
  tVOID** Arr ;

  if ( I >= Cnt )
    return(NULL);

  Arr = Array();
  if ( NULL == Arr )
    return(NULL);
  
  return Arr[I];
}

/* ------------------------------------------------------------------------- */

