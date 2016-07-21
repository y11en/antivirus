#include "prop_in.h"


// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Remove( HDATA node, AVP_dword* addr ) {

  AVP_Data* data;

  _ASSERTE( allocator && liberator );

  FIND_DATA_EX( data, node, addr );

  while( data->child ) 
    DATA_Remove( DATA_HANDLE(data->child), 0 );
  
  DATA_Detach( DATA_HANDLE(data), 0 );
  DATA_Free( data );
  
  liberator( data );
  return 1;
}





