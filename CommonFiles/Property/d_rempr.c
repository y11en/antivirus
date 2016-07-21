#include "prop_in.h"


// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Remove_Prop_ID( HDATA data, AVP_dword* addr, AVP_dword prop_id ) {
  AVP_Linked_Property* prev;
  AVP_Linked_Property* prop;

  _ASSERTE( allocator && liberator );

  prop_id &= PROP_ID_MASK;

  if ( prop_id == AVP_PID_END ) {
    _RPT0( _CRT_ASSERT, "Cannot remove END property" );
    return 0;
  }

  FIND_DATA( data, addr );

  prev = &((AVP_Data*)data)->value;

  if ( prop_id == AVP_PID_ID || prop_id == 0 || prop_id == PROP_ID(&prev->data) ) {
    _RPT0( _CRT_ASSERT, "Cannot remove VALUE property" );
    return 0;
  }

  prop = prev->next;
  while( prop && PROP_ID(&prop->data) != prop_id ) {
    prev = prop;
    prop = prop->next;
  }
  if ( prop ) {
    prev->next = prop->next;
    PROP_Free( &prop->data );
    liberator( prop );
    return 1;
  }
  else {
    _RPT0( _CRT_ASSERT, "Cannot find property" );
    return 0;
  }
}


// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Remove_Prop_H( HDATA data, AVP_dword* addr, HPROP prop ) {
  if ( prop ) {
    AVP_Linked_Property* lp;
    //AVP_dword id = PROP_Get_Id( prop );
    //return DATA_Remove_Prop_ID( data, addr, id );
    if ( PROP_ID(prop) == AVP_PID_END ) {
      _RPT0( _CRT_ASSERT, "Cannot remove END property" );
      return 0;
    }

    FIND_DATA( data, addr );

    lp = &((AVP_Data*)data)->value;
    if ( &lp->data == (AVP_Property*)prop ) {
      _RPT0( _CRT_ASSERT, "Cannot remove VALUE property" );
      return 0;
    }
    else {
      AVP_Linked_Property* pr;
      do {
        pr = lp;
        lp = lp->next;
      } while ( lp && &lp->data != (AVP_Property*)prop );
      if ( lp ) {
        pr->next = lp->next;
        PROP_Free( &lp->data );
        liberator( lp );
        return 1;
      }
      else {
        _RPT0( _CRT_ASSERT, "Cannot find property" );
        return 0;
      }
    }
  }
  else {
    _RPT0( _CRT_ASSERT, "Cannot find property" );
    return 0;
  }
}



