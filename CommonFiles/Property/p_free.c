#include "prop_in.h"

// ---
void DATA_PARAM PROP_Free( AVP_Property* prop ) {
  _ASSERTE( allocator && liberator );
  if ( prop ) {
    if ( prop->array ) {
      PROP_Arr_Free( (AVP_Arr_Property*)prop );
      liberator( ((AVP_Arr_Property*)prop)->val );
      ((AVP_Arr_Property*)prop)->val = 0;
      ((AVP_Arr_Property*)prop)->upper = 0;
      ((AVP_Arr_Property*)prop)->count = 0;
    }
    else if ( prop->type == avpt_str ) {
      liberator( ((AVP_STR_Property*)prop)->val );
      ((AVP_STR_Property*)prop)->val = 0;
    }
    else if ( prop->type == avpt_wstr ) {
      liberator( ((AVP_WSTR_Property*)prop)->val );
      ((AVP_WSTR_Property*)prop)->val = 0;
    }
    else if ( prop->type == avpt_bin ) {
      liberator( ((AVP_BIN_Property*)prop)->val.data );
      ((AVP_BIN_Property*)prop)->val.size = 0;
      ((AVP_BIN_Property*)prop)->val.data = 0;
    }
  }
  else 
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
}


// ---
AVP_bool DATA_PARAM PROP_Arr_Free( AVP_Arr_Property* prop ) {
  
  AVP_dword i;
  AVP_Bin_Item* items;

  _ASSERTE( allocator && liberator );

  if ( !prop || !prop->prop.array || prop->prop.type == avpt_nothing ) {
    _RPT0( _CRT_ASSERT, "Property is not an array" );
    return 0;
  }

  switch ( prop->prop.type ) {
    case avpt_nothing : break;
    case avpt_char    :
    case avpt_wchar   :
    case avpt_short   :
    case avpt_long    :
    case avpt_byte    :
    case avpt_group   :
    case avpt_word    :
    case avpt_dword   :
    case avpt_qword   :
    case avpt_longlong:
		case avpt_size_t  :
    case avpt_int     :
    case avpt_uint    :
    case avpt_bool    :
    case avpt_date    :
    case avpt_time    :
    case avpt_datetime:
      //liberator( ((AVP_Arr_Property*)prop)->val );
      break;
    case avpt_str     : 
    case avpt_wstr    :
      for( i=0; i<prop->count; i++ ) {
        liberator( ((void**)prop->val)[i] );
        ((void**)prop->val)[i] = 0;
      }
      break;
    case avpt_bin     : 
      items = (AVP_Bin_Item*)prop->val;
      for( i=0; i<prop->count; i++,items++ ) {
        liberator( items->data );
        items->size = 0;
        items->data = 0;
      }
      break;
    default           : 
      _RPT0( _CRT_ASSERT, "Bad property array type" );
      break;
  }
  prop->count = 0;
  /*
  ((AVP_Arr_Property*)prop)->upper = 0;
  ((AVP_Arr_Property*)prop)->val   = 0;
  */
  return 1;
}



