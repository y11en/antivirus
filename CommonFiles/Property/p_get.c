#include "prop_in.h"

// ---
DATA_PROC AVP_TYPE DATA_PARAM PROP_Get_Type( HPROP prop ) {
  if ( prop )
    return ((AVP_Property*)prop)->type;
  else {
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
    return AVP_PID_NOTHING;
  }
}


// ---
AVP_dword DATA_PARAM PROP_Get_Id( HPROP prop ) {
  if ( prop )
    return PROP_ID(prop);
  else {
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
    return AVP_PID_NOTHING;
  }
}



