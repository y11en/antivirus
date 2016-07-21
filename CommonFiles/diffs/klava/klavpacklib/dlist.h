#ifndef _KLAVPACK_DYN_LIST_H_
#define _KLAVPACK_DYN_LIST_H_  "Simple dynamic list storage"

#include <stdlib.h>
#include <memory.h>
#include "types.h"

/* ------------------------------------------------------------------------- */

/* list nodes data free function prototype */
typedef tVOID (*fDLIST_FREE)(tVOID* Data);

/* ------------------------------------------------------------------------- */

class CDynList
{
public:
  CDynList(fDLIST_FREE FreeProc = NULL);
 ~CDynList() { Clear(); }

  tBOOL   AddHead(tVOID* DPtr);
  tBOOL   AddTail(tVOID* DPtr);
  tBOOL   Remove(tVOID*  DPtr);
  tBOOL   Swap(tVOID* DPtr1, tVOID* DPtr2);
  tVOID** Array();
  tUINT   Count() { return(Cnt); }
  tVOID   Clear();

  tVOID*  operator[] (tUINT I);  /* get ptr to element */
  tBOOL   operator+= (tVOID* DPtr) { return AddTail(DPtr); }  /* add to tail */
  
private:
  tVOID*  Root;
  tVOID*  Tail;
  tVOID** Arr;
  tUINT   Cnt;
  tBOOL   Chg;

  fDLIST_FREE  CurFreeProc;
  static tVOID DefFreeProc(tVOID* DPtr);
};

/* ------------------------------------------------------------------------- */

#endif /* _KLAVPACK_DYN_LIST_H_ */
