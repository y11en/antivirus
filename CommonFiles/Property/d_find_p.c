#include "prop_in.h"

// ---
HPROP DATA_PARAM DATA_Find_Prop( HDATA data, AVP_dword* addr, AVP_dword prop_id ) {
  AVP_Linked_Property* prop;

  if ( !data ) {
    _RPT0( _CRT_ASSERT, "Data handle is zero" );
    return 0;
  }

  FIND_DATA( data, addr );

  prop_id &= PROP_ID_MASK;
  if ( prop_id == 0 || prop_id == AVP_PID_VALUE 
		/*Будем искать значение узла данных только с prop_id == AVP_PID_VALUE
		|| prop_id == PROP_ID(&((AVP_Data*)data)->value)*/ )
    return PROP_HANDLE( &((AVP_Data*)data)->value.data );

  prop = ((AVP_Data*)data)->value.next;
  while( prop && PROP_ID(&prop->data) != prop_id ) 
    prop = prop->next;

  return prop ? PROP_HANDLE( &prop->data ) : 0;
}




