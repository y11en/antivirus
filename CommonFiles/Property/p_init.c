#include "prop_in.h"


// ---
AVP_dword DATA_PARAM PROP_Init( AVP_Property* prop, AVP_dword pid, AVP_size_t val, AVP_dword size ) {
  if ( !prop ) {
    _RPT0( _CRT_ASSERT, "Bad parameters" );
    return 0;
  }

  *(AVP_dword*)prop = PROP_ID(&pid);

  if ( prop->array ) {
    if ( PROP_Arr_Init((AVP_Arr_Property*)prop) ) {
      if ( val && size )
        return PROP_Arr_Insert( PROP_HANDLE(prop), PROP_ARR_LAST, (void*)val, size );
      else
        return 1;
    }
    else
      return 0;
  }

  else if ( prop->type >= avpt_str ) {
    switch( prop->type ) {
      case avpt_str :
        ((AVP_STR_Property*)prop)->val = 0;
        break;
      case avpt_wstr :
        ((AVP_WSTR_Property*)prop)->val = 0;
        break;
      case avpt_bin :
        ((AVP_BIN_Property*)prop)->val.size = 0;
        ((AVP_BIN_Property*)prop)->val.data = 0;
        break;
      default :
        _RPT0( _CRT_ASSERT, "Bad property type" );
        return 0;
    }
  }

  return PROP_Set_Val( PROP_HANDLE(prop), val, size );
}


// ---
AVP_bool DATA_PARAM PROP_Arr_Init( AVP_Arr_Property* prop ) {
  if ( !prop ) {
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
    return 0;
  }
  prop->count = 0;
  prop->upper = 0;
  prop->delta = 1;
  prop->val   = 0;
  switch ( ((AVP_Property*)prop)->type ) {
    case avpt_char    : prop->isize = sizeof(AVP_char);     break;
    case avpt_wchar   : prop->isize = sizeof(AVP_wchar);    break;
    case avpt_short   : prop->isize = sizeof(AVP_short);    break;
    case avpt_long    : prop->isize = sizeof(AVP_long);     break;
    case avpt_byte    : prop->isize = sizeof(AVP_byte);     break;
    case avpt_group   : prop->isize = sizeof(AVP_group);    break;
    case avpt_word    : prop->isize = sizeof(AVP_word);     break;
    case avpt_dword   : prop->isize = sizeof(AVP_dword);    break;
    case avpt_qword   : prop->isize = sizeof(AVP_qword);    break;
    case avpt_longlong: prop->isize = sizeof(AVP_longlong); break;
    case avpt_size_t  : prop->isize = sizeof(AVP_size_t);   break;
    case avpt_int     : prop->isize = sizeof(AVP_int);      break;
    case avpt_uint    : prop->isize = sizeof(AVP_uint);     break;
    case avpt_bool    : prop->isize = sizeof(AVP_bool);     break;
    case avpt_date    : prop->isize = sizeof(AVP_date);     break;
    case avpt_time    : prop->isize = sizeof(AVP_time);     break;
    case avpt_datetime: prop->isize = sizeof(AVP_datetime); break;
    case avpt_str     : prop->isize = sizeof(AVP_str);      break;
    case avpt_wstr    : prop->isize = sizeof(AVP_wstr);     break;
    case avpt_bin     : prop->isize = sizeof(AVP_Bin_Item); break;
    case avpt_nothing : 
    default           : 
      prop->isize = 1;   
      _RPT0( _CRT_ASSERT, "Bad property array type" );
      return 0;
  }
  return 1;
}



