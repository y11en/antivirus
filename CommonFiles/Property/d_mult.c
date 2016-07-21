#include "prop_in.h"

// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Multiplication( HDATA data, AVP_dword* addr, HDATA data2, AVP_dword* addr2, HDATA* result, AVP_dword flags ) {
  AVP_Data* f1;
  AVP_Data* f2;
  MAKE_ADDR1( a, 0 );

  if ( !data || !data2 || (result && *result == data2) ) {
    _RPT0( _CRT_ASSERT, "Bad parameters" );
    return 0;
  }

  FIND_DATA( data,  addr  );
  FIND_DATA( data2, addr2 );

  if ( !result )
    result = &data;

  if ( flags & DATA_IF_ROOT_EXCLUDE )
    f1 = ((AVP_Data*)data)->child;
  else
    f1 = (AVP_Data*)data;

  while( f1 ) {
    AVP_Data* nd;
    AVP_Data* next = f1->next;

    a[0] = PROP_ID( &f1->value.data );

    if ( flags & DATA_IF_ROOT_EXCLUDE ) 
      f2 = (AVP_Data*)DATA_Find( data2, a );
    else 
      f2 = PROP_ID( &((AVP_Data*)data2)->value.data ) == a[0] ? (AVP_Data*)data2 : 0;

    nd = 0;
    if ( f2 ) {
      if ( *result == data ) 
        nd = f1;
      else {
        nd = (AVP_Data*)DATA_Copy( *result, 0, DATA_HANDLE(f1), (flags|DATA_IF_DOESNOT_GO_DOWN) & ~(DATA_IF_ROOT_EXCLUDE|DATA_IF_ROOT_SIBLINGS) );
        if ( !*result )
          *result = (HDATA)nd;
      }
      PROP_Multiplication( nd, f2 );
      if ( !(flags & DATA_IF_DOESNOT_GO_DOWN) ) 
        DATA_Multiplication( DATA_HANDLE(f1), 0, DATA_HANDLE(f2), 0, (HDATA*)&nd, flags|DATA_IF_ROOT_EXCLUDE );
    }
    else if ( *result == data ) 
      DATA_Remove( DATA_HANDLE(f1), 0 );

    if ( (flags & DATA_IF_ROOT_EXCLUDE) || (flags & DATA_IF_ROOT_SIBLINGS) )
      f1 = next;
    else
      f1 = 0;
  }

  return 1;
}




