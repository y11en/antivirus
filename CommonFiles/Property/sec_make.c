//#include <stdarg.h>
#include "prop_in.h"


// ---
DATA_PROC AVP_dword* DATA_PARAM DATA_Make_Sequence( HDATA node, AVP_dword* addr, HDATA stop_data, AVP_dword** seq ) {
  
  AVP_dword   count;
  AVP_Data*   data;
  AVP_Data*   data2;
  AVP_dword*  cur;
	AVP_bool    stop_found;

  if( !seq ) {
    _RPT0( _CRT_ASSERT, "Bad parameters" );
    return 0;
  }

  count = 0;
  if ( addr )
    data = (AVP_Data*)DATA_Find( node, addr );
  else
    data = (AVP_Data*)node;

  if ( !data ) {
    if ( *seq )
      **seq = 0;
    return 0;
  }
  data2 = data;

	stop_found = 0;
  while( data2 ) {
    count++;
    data2 = data2->dad;
		if ( data2 && ((stop_data && data2 == (AVP_Data*)stop_data) ||
									 (!stop_data && !data2->dad)) ) {
			stop_found = 1;
			break;
		}
  }

  if ( stop_found && count ) {
    AVP_dword i;
    if ( !*seq ) {
      _ASSERTE( allocator && liberator );
      *seq = allocator( (count + 1)*sizeof(AVP_dword) );
    }
    cur = *seq + count;
    *cur = 0;
		cur--;
    for( i=0; i<count; i++,cur-- ) {
      *cur = PROP_ID(&data->value.data);
      data = data->dad;
    }
    return *seq;
  }
  else {
    if ( *seq )
      **seq = 0;
    return 0;
  }
}



