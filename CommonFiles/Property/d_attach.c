#include "prop_in.h"


// ---
DATA_PROC HDATA DATA_PARAM DATA_Attach( HDATA dad, AVP_dword* addr, HDATA node, AVP_dword flags ) {
  AVP_Data* data;

  if ( !node ) {
    _RPT0( _CRT_ASSERT, "Bad parameters" );
    return 0;
  }

  if ( addr )
    dad = DATA_Find( dad, addr );
  if ( flags & DATA_IF_ROOT_SIBLINGS ) {
    if ( dad ) {
      AVP_dword id = PROP_ID( &((AVP_Data*)node)->value.data );

      data = ((AVP_Data*)dad)->dad ? ((AVP_Data*)dad)->dad->child : (AVP_Data*)dad;
      while( data && PROP_ID(&data->value.data) != id ) 
        data = data->next;
    }
    else
      data = 0;
  }
  else if ( dad ) {
    MAKE_ADDR1( addr_d, 0 );
    addr_d[0] = PROP_ID( &((AVP_Data*)node)->value.data );
    data = (AVP_Data*)DATA_Find( dad, addr_d );
  }
  else
    data = 0;

  if ( data ) {
    _RPT0( _CRT_ASSERT, "Use merge function to combain data" );
    return 0;
  }
  else {
    DATA_Detach( node, 0 );

    if ( flags & DATA_IF_ROOT_SIBLINGS ) {
      if ( dad ) {
        data = (AVP_Data*)dad;
        while( data->next ) 
          data = data->next;
        data->next = (AVP_Data*)node;
        ((AVP_Data*)node)->dad = ((AVP_Data*)dad)->dad;
      }
    }
    else {
      ((AVP_Data*)node)->dad = (AVP_Data*)dad;
      if ( dad ) {
        if ( ((AVP_Data*)dad)->child ) {
          data = ((AVP_Data*)dad)->child;
          while( data->next ) 
            data = data->next;
          data->next = (AVP_Data*)node;
        }
        else 
          ((AVP_Data*)dad)->child = (AVP_Data*)node;
      }
    }
  }
//  return DATA_HANDLE( data );
  return node;
}



