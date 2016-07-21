#include "prop_in.h"

// ---
DATA_PROC HPROP DATA_PARAM DATA_Replace_Prop( HDATA data, AVP_dword* addr, AVP_dword what, AVP_dword to, AVP_size_t val, AVP_dword size ) {

  _ASSERTE( allocator && liberator );

  what &= PROP_ID_MASK;
  to &= PROP_ID_MASK;

  if ( what == 0 || what == AVP_PID_VALUE ) {
    _RPT0( _CRT_ASSERT, "AVP_PID_VALUE property cannot be replaced" );
    return 0;
  }

  FIND_DATA( data, addr );

  if ( what == to ) {
    HPROP from_prop = DATA_Find_Prop( data, 0, what );
    if ( from_prop ) {
      PROP_Set_Val( from_prop, val, size );
      return from_prop;
    }
    else {
      _RPT0( _CRT_ASSERT, "Cannot find property handle" );
      return 0;
    }
  }
  else {
    AVP_Linked_Property* lprop = &((AVP_Data*)data)->value;
    AVP_Linked_Property* prev;
    do {
      prev = lprop;
      lprop = lprop->next;
    } while( lprop && PROP_ID(&lprop->data) != what );

    if ( lprop ) {
      AVP_dword new_size = sizeof(AVP_Linked_Property) - sizeof(AVP_Property) + PROP_Predict_Size( to );
      AVP_Linked_Property* new_prop = (AVP_Linked_Property*)allocator( new_size );
      _ASSERT( new_prop );
      memset( new_prop, 0, new_size );
      prev->next = new_prop;
      new_prop->next = lprop->next;
      PROP_Free( &lprop->data );
      liberator( lprop );
      PROP_Init( &new_prop->data, to, val, size );
      return PROP_HANDLE(&new_prop->data);
    }
    else {
      _RPT0( _CRT_ASSERT, "Cannot find property handle" );
      return 0;
    }
  }
}



