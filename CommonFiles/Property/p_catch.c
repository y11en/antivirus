#include "prop_in.h"

// ---
AVP_bool DATA_PARAM PROP_Arr_Catch_Mem( HPROP prop, AVP_dword count ) {
  _ASSERTE( allocator && liberator );

  count += ((AVP_Arr_Property*)prop)->count;

  if ( !prop ) {
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
    return 0;
  }

  if ( ((AVP_Arr_Property*)prop)->upper < count ) {
    void* tmp;
    AVP_dword new_size;
  
    AVP_dword del = ((AVP_Arr_Property*)prop)->delta;
    if ( del == 0 )
      del = 1;
  
    while( ((AVP_Arr_Property*)prop)->upper < count )
      ((AVP_Arr_Property*)prop)->upper += del;

    new_size = (((AVP_Arr_Property*)prop)->upper) * ((AVP_dword)((AVP_Arr_Property*)prop)->isize);
    tmp = allocator( new_size );
    _ASSERT( tmp );
    

    if ( ((AVP_Arr_Property*)prop)->count ) {
      AVP_dword old_size = ((AVP_Arr_Property*)prop)->count * ((AVP_dword)((AVP_Arr_Property*)prop)->isize);
      memcpy( tmp, ((AVP_Arr_Property*)prop)->val, old_size );
      memset( ((AVP_byte*)tmp)+old_size, 0, new_size-old_size );
      liberator( ((AVP_Arr_Property*)prop)->val );
    }
    else
      memset( tmp, 0, new_size );

    ((AVP_Arr_Property*)prop)->val = tmp;
  }
  return 1;
}




