#include "prop_in.h"


DATA_PROC HDATA DATA_PARAM DATA_Insert( HDATA node, AVP_dword* addr, HDATA insrt ) {

  if ( !node || !insrt ) {
    _RPT0( _CRT_ASSERT, "Bad parameters" );
    return 0;
  }

  if ( addr )
    node = DATA_Find( node, addr );

  if ( node ) {
    if ( ((AVP_Data*)node)->dad ) {
      AVP_Data* data;
      AVP_Data* dad = ((AVP_Data*)node)->dad;
      MAKE_ADDR1( addr_d, 0 );

      addr_d[0] = PROP_ID( &((AVP_Data*)insrt)->value.data );
      if ( DATA_Find(DATA_HANDLE(dad),addr_d) ) {
        _RPT0( _CRT_ASSERT, "Use merge function to combain data" );
        return 0;
      }
      DATA_Detach( insrt, 0 );

      data = ((AVP_Data*)dad)->child;
      if ( data == (AVP_Data*)node ) {
        ((AVP_Data*)insrt)->next = data;
        ((AVP_Data*)dad)->child = (AVP_Data*)insrt;
      }
      else {
        while( data && data->next != (AVP_Data*)node ) 
          data = data->next;

        if ( data ) {
          ((AVP_Data*)insrt)->next = data->next;
          data->next = (AVP_Data*)insrt;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad tree" );
          return 0;
        }
      }
      ((AVP_Data*)insrt)->dad = dad;
    }
    else {
      _RPT0( _CRT_ASSERT, "Parent is not presented" );
      return 0;
    }
  }
  else {
    _RPT0( _CRT_ASSERT, "Node is not found" );
    return 0;
  }

  return insrt;
}



