#include "prop_in.h"



// ---
DATA_PROC AVP_dword DATA_PARAM DATA_Set_Val( HDATA data, AVP_dword* addr, AVP_dword prop_id, AVP_size_t val, AVP_dword size ) {
  return PROP_Set_Val( DATA_Find_Prop(data,addr,prop_id), val, size );
}



// ---
AVP_dword DATA_PARAM PROP_Set_Val( HPROP prop, AVP_size_t val, AVP_dword size ) {
  if ( !prop ) {
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
    return 0;
  }

  if ( ((AVP_Property*)prop)->array ) {
    if ( val == 0 || size == 0 ) 
      return PROP_Arr_Free( (AVP_Arr_Property*)prop );
    else
      return PROP_Arr_Insert( prop, PROP_ARR_LAST, (void*)val, size );
  }
  else {
    switch( ((AVP_Property*)prop)->type ) {
      case avpt_nothing : 
        //_RPT0( _CRT_ASSERT, "Cannot set value for empty property" );
        return 0;
      case avpt_char : 
        if ( size == 0 || size == sizeof(AVP_char) ) {
          size = sizeof(AVP_char);
          ((AVP_CHAR_Property*)prop)->val = (AVP_char)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_wchar : 
        if ( size == 0 || size == sizeof(AVP_wchar) ) {
          size = sizeof(AVP_wchar);
          ((AVP_WCHAR_Property*)prop)->val = (AVP_wchar)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_short : 
        if ( size == 0 || size == sizeof(AVP_short) ) {
          size = sizeof(AVP_short);
          ((AVP_SHORT_Property*)prop)->val = (AVP_short)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_long : 
        if ( size == 0 || size == sizeof(AVP_long) ) {
          size = sizeof(AVP_long);
          ((AVP_LONG_Property*)prop)->val = (AVP_long)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_byte : 
        if ( size == 0 || size == sizeof(AVP_byte) ) {
          size = sizeof(AVP_byte);
          ((AVP_BYTE_Property*)prop)->val = (AVP_byte)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_group : 
        if ( size == 0 || size == sizeof(AVP_group) ) {
          size = sizeof(AVP_group);
          ((AVP_GROUP_Property*)prop)->val = (AVP_group)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_word : 
        if ( size == 0 || size == sizeof(AVP_word) ) {
          size = sizeof(AVP_word);
          ((AVP_WORD_Property*)prop)->val = (AVP_word)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_dword : 
        if ( size == 0 || size == sizeof(AVP_dword) ) {
          size = sizeof(AVP_dword);
          ((AVP_DWORD_Property*)prop)->val = (AVP_dword)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_qword : 
      case avpt_longlong : 
        if ( val ) {
          if ( size == 0 || size == sizeof(AVP_qword) ) {
            size = sizeof(AVP_qword);
            memcpy( &((AVP_QWORD_Property*)prop)->val, (void*)val, sizeof(AVP_qword) ); 
            break;
          }
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        else {
          memset( &((AVP_QWORD_Property*)prop)->val, 0, sizeof(AVP_qword) ); 
          size = sizeof(AVP_qword);
          break;
        }
      case avpt_size_t : 
        if ( size == 0 || size == sizeof(AVP_size_t) ) {
          size = sizeof(AVP_size_t);
          ((AVP_SIZE_T_Property*)prop)->val = (AVP_size_t)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_int : 
        if ( size == 0 || size == sizeof(AVP_int) ) {
          size = sizeof(AVP_int);
          ((AVP_INT_Property*)prop)->val = (AVP_int)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_uint : 
        if ( size == 0 || size == sizeof(AVP_uint) ) {
          size = sizeof(AVP_uint);
          ((AVP_UINT_Property*)prop)->val = (AVP_uint)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_bool :
        if ( size == 0 || size == sizeof(AVP_dword) ) {
          size = sizeof(AVP_bool);
          ((AVP_BOOL_Property*)prop)->val = (AVP_bool)val;
          break;
        }
        else {
          _RPT0( _CRT_ASSERT, "Bad data size" );
          return 0;
        }
      case avpt_date : 
        if ( val ) {
          if ( size == 0 || size == sizeof(AVP_date) ) {
            size = sizeof(AVP_date);
            memcpy( ((AVP_DATE_Property*)prop)->val, (void*)val, sizeof(AVP_date) ); 
            break;
          }
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        else {
          memset( ((AVP_DATE_Property*)prop)->val, 0, sizeof(AVP_date) ); 
          size = sizeof(AVP_date);
          break;
        }

      case avpt_time : 
        if ( val ) {
          if ( size == 0 || size == sizeof(AVP_time) ) {
            size = sizeof(AVP_time);
            memcpy( ((AVP_TIME_Property*)prop)->val, (void*)val, sizeof(AVP_time) ); 
            break;
          }
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        else {
          memset( ((AVP_TIME_Property*)prop)->val, 0, sizeof(AVP_time) ); 
          size = sizeof(AVP_time);
          break;
        }

      case avpt_datetime : 
        if ( val ) {
          if ( size == 0 || size == sizeof(AVP_datetime) ) {
            size = sizeof(AVP_datetime);
            memcpy( ((AVP_DATETIME_Property*)prop)->val, (void*)val, sizeof(AVP_datetime) ); 
            break;
          }
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        else {
          memset( ((AVP_DATETIME_Property*)prop)->val, 0, sizeof(AVP_datetime) ); 
          size = sizeof(AVP_datetime);
          break;
        }

      case avpt_str : 
      
        _ASSERTE( allocator && liberator );

        if ( val ) {
          if ( size == 0 )
            size = (AVP_dword)strlen( (AVP_str)val ) + sizeof(AVP_char);

          if ( ((AVP_STR_Property*)prop)->val == 0 ) 
            ((AVP_STR_Property*)prop)->val = allocator( size );
          else {
            AVP_dword now = (AVP_dword)strlen( ((AVP_STR_Property*)prop)->val ) + sizeof(AVP_char);
            if ( now != size ) {
              liberator( ((AVP_STR_Property*)prop)->val );
              ((AVP_STR_Property*)prop)->val = (AVP_str)allocator( size );
            }
          }

          _ASSERT( ((AVP_STR_Property*)prop)->val );
          memcpy( ((AVP_STR_Property*)prop)->val, (AVP_str)val, size-sizeof(AVP_char) );
          ((AVP_STR_Property*)prop)->val[size-1] = 0;
        }
        else {
          size = 0;
          if ( ((AVP_STR_Property*)prop)->val != 0 ) {
            liberator( ((AVP_STR_Property*)prop)->val );
            ((AVP_STR_Property*)prop)->val = 0;
          }
        }

        break;

      case avpt_wstr : 

        _ASSERTE( allocator && liberator );
        if ( val ) {
          if ( size == 0 )
            size = (AVP_dword)wcslen( (wchar_t*)(AVP_wstr)val ) + 1;

          if ( ((AVP_WSTR_Property*)prop)->val == 0 ) 
            ((AVP_WSTR_Property*)prop)->val = allocator( sizeof(AVP_wchar) * size );
          else {
            AVP_dword now = (AVP_dword)wcslen( (wchar_t*)((AVP_WSTR_Property*)prop)->val ) + 1;
            if ( now != size ) {
              liberator( ((AVP_WSTR_Property*)prop)->val );
              ((AVP_WSTR_Property*)prop)->val = (AVP_wstr)allocator( sizeof(AVP_wchar) * size );
            }
          }

          _ASSERT( ((AVP_WSTR_Property*)prop)->val );
          memcpy( ((AVP_WSTR_Property*)prop)->val, (AVP_wstr)val, (size-1)*sizeof(AVP_wchar) );
          ((AVP_WSTR_Property*)prop)->val[size-1] = 0;
        }
        else {
          size = 0;
          if ( ((AVP_WSTR_Property*)prop)->val != 0 ) {
            liberator( ((AVP_WSTR_Property*)prop)->val );
            ((AVP_WSTR_Property*)prop)->val = 0;
          }
        }

        break;

      case avpt_bin : 
        _ASSERTE( allocator && liberator );

        if ( val && size ) {
          if ( ((AVP_BIN_Property*)prop)->val.size != size ) {
            if ( ((AVP_BIN_Property*)prop)->val.data )
              liberator( ((AVP_BIN_Property*)prop)->val.data );
            ((AVP_BIN_Property*)prop)->val.data = (AVP_byte*)allocator( size );
            _ASSERT( ((AVP_BIN_Property*)prop)->val.data );
            ((AVP_BIN_Property*)prop)->val.size = size;
          }
          memcpy( ((AVP_BIN_Property*)prop)->val.data, (void*)val, size );        
        }
        else {
          if ( ((AVP_BIN_Property*)prop)->val.data )
            liberator( ((AVP_BIN_Property*)prop)->val.data );
          ((AVP_BIN_Property*)prop)->val.data = 0;
          ((AVP_BIN_Property*)prop)->val.size = 0;
        }

        break;

      default : 
        _RPT0( _CRT_ASSERT, "Bad property type" );
        return 0;
    };
    return size;
  }
}



