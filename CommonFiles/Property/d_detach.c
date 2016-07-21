#include "prop_in.h"


// ---
DATA_PROC HDATA DATA_PARAM DATA_Detach( HDATA node, AVP_dword* addr ) {
  AVP_Data* data;

  FIND_DATA_EX( data, node, addr );

  if ( data->dad ) {
    AVP_Data* prev = data->dad->child;
    if ( prev == data ) 
      data->dad->child = data->next;
    else {
      while( prev && prev->next != data ) 
        prev = prev->next;
      _ASSERT( prev );
      prev->next = data->next;
    }
    data->dad = 0;
  }
  data->next = 0;
  return DATA_HANDLE(data);
}



