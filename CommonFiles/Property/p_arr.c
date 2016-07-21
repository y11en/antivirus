#include "prop_in.h"

// ---
AVP_dword DATA_PARAM PROP_Arr_Count( HPROP prop ) {
  if ( prop && ((AVP_Property*)prop)->array && ((AVP_Property*)prop)->type != avpt_nothing )
    return ((AVP_Arr_Property*)prop)->count;
  else {
    _RPT0( _CRT_ASSERT, "Property is not an array" );
    return 0;
  }
}


// ---
DATA_PROC AVP_word DATA_PARAM PROP_Arr_Delta( HPROP prop, AVP_word new_delta ) {
  AVP_word old_delta;
  if ( !prop || !((AVP_Property*)prop)->array || ((AVP_Property*)prop)->type == avpt_nothing ) {
    _RPT0( _CRT_ASSERT, "Property is not an array" );
    return 0;
  }
  old_delta = ((AVP_Arr_Property*)prop)->delta;
  if ( new_delta != PROP_DELTA_DONT_CHANGE )
    ((AVP_Arr_Property*)prop)->delta = new_delta;
  return old_delta;
}




