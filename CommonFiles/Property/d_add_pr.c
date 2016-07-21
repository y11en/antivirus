#include "prop_in.h"

// ---
DATA_PROC HPROP DATA_PARAM DATA_Add_Prop( HDATA data, AVP_dword* addr, AVP_dword prop_id, AVP_size_t val, AVP_dword data_size ) {
  HPROP prop;

  _ASSERTE( allocator && liberator );

  FIND_DATA( data, addr );

  prop = DATA_Find_Prop( data, 0, prop_id );
  if ( prop ) {
    _RPT4( 
      _CRT_ASSERT, 
      "Doubled property (%d-%d-%d-%d)", 
      ((AVP_Property*)&prop_id)->id, 
      ((AVP_Property*)&prop_id)->app_id, 
      ((AVP_Property*)&prop_id)->type, 
      ((AVP_Property*)&prop_id)->array
    );
    return 0;
  }
  else {
    AVP_Linked_Property* lprop;
    AVP_dword size = sizeof(AVP_Linked_Property) - sizeof(AVP_Property) + PROP_Predict_Size( prop_id );
    lprop = (AVP_Linked_Property*)allocator( size );
    _ASSERT( lprop );
    memset( lprop, 0, size );
    if ( ((AVP_Data*)data)->value.next ) {
      AVP_Linked_Property* prev = &((AVP_Data*)data)->value;
      AVP_Linked_Property* next = prev->next;
      while( next->next ) {
        prev = next;
        next = next->next;
      }
      lprop->next = next;
      prev->next = lprop;
    }
    else 
      ((AVP_Data*)data)->value.next = lprop;

    PROP_Init( &lprop->data, prop_id, val, data_size );
    return PROP_HANDLE( &lprop->data );
  }
}



