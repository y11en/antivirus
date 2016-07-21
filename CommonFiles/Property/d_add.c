#include "prop_in.h"

// ---
DATA_PROC HDATA DATA_PARAM DATA_Add( HDATA dad, AVP_dword* addr, AVP_dword pid, AVP_size_t val, AVP_dword data_size ) {
  
  AVP_Data* data;
  AVP_dword size;

  MAKE_ADDR1( addr_d, 0 );

  _ASSERTE( allocator && liberator );

  addr_d[0] = pid;

  if ( addr )
    dad = DATA_Find( dad, addr );

  data = (AVP_Data*)DATA_Find( dad, addr_d );
  if ( data ) {
    _RPT4( _CRT_ASSERT, "Doubled data node (%d-%d-%d-%d)", ((AVP_Property*)&pid)->id, ((AVP_Property*)&pid)->app_id, ((AVP_Property*)&pid)->type, ((AVP_Property*)&pid)->array );
    return 0;
  }

  size = sizeof(AVP_Data) - sizeof(AVP_Property) + PROP_Predict_Size( pid );
  data = (AVP_Data*)allocator( size );
  _ASSERT( data );
  memset( data, 0, size );

  PROP_Init( &data->value.data, pid, val, data_size );
  DATA_Add_Prop( DATA_HANDLE(data), 0, AVP_PID_END, (AVP_size_t)data, 0 );
  DATA_Attach( dad, 0, DATA_HANDLE(data), 0 );

  return DATA_HANDLE(data);
}




