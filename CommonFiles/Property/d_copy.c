#include "prop_in.h"


// ---
DATA_PROC HDATA DATA_PARAM DATA_Copy( HDATA dad, AVP_dword* addr, HDATA copy, AVP_dword flags ) {
  AVP_Data* nd;

  if ( !copy ) {
    _RPT0( _CRT_ASSERT, "Bad parameters" );
    return 0;
  }

  nd = 0;
  if ( !(flags & DATA_IF_ROOT_EXCLUDE) ) {
    MAKE_ADDR1( a, 0 );
    if ( addr ) {
      dad = DATA_Find( dad, addr );
      addr = 0;
    }
    a[0] = PROP_ID( &((AVP_Data*)copy)->value.data );
    nd = (AVP_Data*)DATA_Find( dad, a );
    if ( nd ) 
      DATA_Free( nd );
    else 
      nd = (AVP_Data*)DATA_Add( dad, 0, a[0], 0, 0 );
    if ( nd ) {
      AVP_Linked_Property* lp = ((AVP_Data*)copy)->value.next;
      PROP_Copy( &nd->value.data, &((AVP_Data*)copy)->value.data );
      while( lp && lp->next ) {
        AVP_Property* prop = (AVP_Property*)DATA_Add_Prop( DATA_HANDLE(nd), 0, PROP_ID(&lp->data), 0, 0 );
        PROP_Copy( prop, &lp->data );
        lp = lp->next;
      }
    }
    if ( flags & DATA_IF_ROOT_SIBLINGS ) {
      AVP_Data* next = ((AVP_Data*)copy)->next;
      while( next && DATA_Copy(dad,0,DATA_HANDLE(next),flags&~(DATA_IF_ROOT_EXCLUDE|DATA_IF_ROOT_SIBLINGS)) )
        next = next->next;
    }
    dad = (HDATA)nd;
  }
  if ( !(flags & DATA_IF_DOESNOT_GO_DOWN) && ((AVP_Data*)copy)->child ) 
    DATA_Copy( dad, addr, (HDATA)((AVP_Data*)copy)->child, (flags & ~DATA_IF_ROOT_EXCLUDE)|DATA_IF_ROOT_SIBLINGS );
  return DATA_HANDLE(nd);
}




