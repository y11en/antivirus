#include "prop_in.h"

// ---
DATA_PROC AVP_dword DATA_PARAM DATA_Get_Val( HDATA data, AVP_dword* addr, AVP_dword prop_id, void* val, AVP_dword size ) {
  return PROP_Get_Val( DATA_Find_Prop(data,addr,prop_id), val, size );
}



// ---
AVP_dword DATA_PARAM PROP_Get_Val( HPROP prop, void* val, AVP_dword size ) {
  AVP_dword now;
  
  if ( !prop ) {
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
    return 0;
  }

  if ( ((AVP_Property*)prop)->array ) {
    AVP_dword i;
    AVP_Arr_Property* aprop = ((AVP_Arr_Property*)prop);
    switch( aprop->prop.type ) {
      case avpt_nothing : 
        _RPT0( _CRT_ASSERT, "Bad array type" );
        return 0;

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
      case avpt_bool    :
      case avpt_int     : 
      case avpt_uint    : 
      case avpt_date    : 
      case avpt_time    : 
      case avpt_datetime: 
        now = aprop->isize * aprop->count;
        if ( val ) {
          size = size / aprop->isize * aprop->isize;
          if ( now < size )
            now = size;
          if ( size == 0 ) {
            _RPT0( _CRT_ASSERT, "Not enough room for data" );
            return 0;
          }
          else if ( now )
            memcpy( val, aprop->val, now );
        }
        return now;

      case avpt_str   : 
        now = 0;
        if ( val ) {
          for( i=0; i<aprop->count && (now+1 < size); i++ ) {
            AVP_dword len = (AVP_dword)strlen( ((AVP_str*)aprop->val)[i] );
            if ( (now+len+sizeof(AVP_char)) > size ) 
              len = size - now - sizeof(AVP_char);
            if ( len )
              memcpy( (void*)val, ((AVP_str*)aprop->val)[i], len );
            ((AVP_str)val)[len++] = 0;
            *(AVP_str*)val += len;
            now += len;
          }
        }
        else {
          for( i=0; i<aprop->count; i++ ) {
            if ( ((AVP_str*)aprop->val)[i] )
              now += (AVP_dword)strlen( ((AVP_str*)aprop->val)[i] ) + sizeof(AVP_char);
          }
        }
        return now;

      case avpt_wstr  : 
        now = 0;
        if ( val ) {
          for( i=0; (i<aprop->count) && (now+sizeof(AVP_wchar) < size); i++ ) {
            AVP_dword len = (AVP_dword)wcslen( (wchar_t*)((AVP_wstr*)aprop->val)[i] );
            if ( now+(len+2)*sizeof(AVP_wchar) > size ) 
              len = (( size - now ) / sizeof(AVP_wchar)) - 2;
            if ( len )
              memcpy( (void*)val, ((AVP_str*)aprop->val)[i], len * sizeof(AVP_wchar) );
            
						((AVP_wstr)val)[len++] = 0;

            *(AVP_wstr*)val += len;
            now += len * sizeof(AVP_wchar);
          }
        }
        else {
          for( i=0; i<aprop->count; i++ ) {
            if ( ((AVP_wstr*)aprop->val)[i] )
              now += (AVP_dword)( wcslen( (wchar_t*)((AVP_wstr*)aprop->val)[i] ) + 1 ) * sizeof(AVP_char);
          }
        }
        return now;

      case avpt_bin   : 
        now = 0;
        if ( val ) {
          AVP_Bin_Item* items = (AVP_Bin_Item*)aprop->val;
          for( i=0; i<aprop->count; i++,items++ ) {
            AVP_dword s = items->size;
            if ( now + s > size )
              s = size - now;
            if ( s && items->data ) {
              memcpy( (void*)val, items->data, s );
              *(char**)val += s;
            }
            now += s;
          }
        }
        else {
          AVP_Bin_Item* items = (AVP_Bin_Item*)aprop->val;
          for( i=0; i<aprop->count; i++,items++ )
            now += items->size;
        }
        return now;

      default : 
        _RPT0( _CRT_ASSERT, "Bad array type" );
        return 0;
    }
  }
  else {
    switch( ((AVP_Property*)prop)->type ) {
      case avpt_nothing : 
        return 0;

      case avpt_char    : 
        if ( val ) {
          if ( size >= sizeof(AVP_char) )
            *(AVP_char*)val = ((AVP_CHAR_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_char);

      case avpt_wchar    : 
        if ( val ) {
          if ( size >= sizeof(AVP_wchar) )
            *(AVP_wchar*)val = ((AVP_WCHAR_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_wchar);

      case avpt_short   : 
        if ( val ) {
          if ( size >= sizeof(AVP_short) )
            *(AVP_short*)val = ((AVP_SHORT_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_short);

      case avpt_long    : 
        if ( val ) {
          if ( size >= sizeof(AVP_long) )
            *(AVP_long*)val = ((AVP_LONG_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_long);

      case avpt_byte    : 
        if ( val ) {
          if ( size >= sizeof(AVP_byte) )
            *(AVP_byte*)val = ((AVP_BYTE_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_byte);

      case avpt_group   : 
        if ( val ) {
          if ( size >= sizeof(AVP_group) )
            *(AVP_group*)val = ((AVP_GROUP_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_group);

      case avpt_word    : 
        if ( val ) {
          if ( size >= sizeof(AVP_word) )
            *(AVP_word*)val = ((AVP_WORD_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_word);

      case avpt_dword   : 
        if ( val ) {
          if ( size >= sizeof(AVP_dword) )
            *(AVP_dword*)val = ((AVP_DWORD_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_dword);

      case avpt_qword    : 
			case avpt_longlong :
        if ( val ) {
          if ( size >= sizeof(AVP_qword) )
            memcpy( val, &((AVP_QWORD_Property*)prop)->val, sizeof(AVP_qword) ); 
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_qword);

      case avpt_size_t   : 
        if ( val ) {
          if ( size >= sizeof(AVP_size_t) )
            *(AVP_size_t*)val = ((AVP_SIZE_T_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_size_t);

      case avpt_int    : 
        if ( val ) {
          if ( size >= sizeof(AVP_int) )
            *(AVP_int*)val = ((AVP_INT_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_int);

      case avpt_uint    : 
        if ( val ) {
          if ( size >= sizeof(AVP_uint) )
            *(AVP_uint*)val = ((AVP_UINT_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_uint);

      case avpt_bool :
        if ( val ) {
          if ( size >= sizeof(AVP_bool) )
            *(AVP_bool*)val = ((AVP_BOOL_Property*)prop)->val;
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_bool);

      case avpt_date    : 
        if ( val ) {
          if ( size >= sizeof(AVP_date) )
            memcpy( val, ((AVP_DATE_Property*)prop)->val, sizeof(AVP_date) ); 
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_date);

      case avpt_time    : 
        if ( val ) {
          if ( size >= sizeof(AVP_time) )
            memcpy( val, ((AVP_TIME_Property*)prop)->val, sizeof(AVP_time) ); 
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_time);

      case avpt_datetime    : 
        if ( val ) {
          if ( size >= sizeof(AVP_datetime) )
            memcpy( val, ((AVP_DATE_Property*)prop)->val, sizeof(AVP_datetime) ); 
          else {
            _RPT0( _CRT_ASSERT, "Bad data size" );
            return 0;
          }
        }
        return sizeof(AVP_datetime);

      case avpt_str   : 
        if ( ((AVP_STR_Property*)prop)->val ) {
          now = (AVP_dword)strlen( ((AVP_STR_Property*)prop)->val ) + sizeof(AVP_char);
          if ( size < now ) {
            if ( val ) {
              if ( size ) {
                memcpy( (void*)val, ((AVP_STR_Property*)prop)->val, size-sizeof(AVP_char) );
                ((AVP_str)val)[size-1] = 0;
              }
            }
            else 
              size = now;
          }
          else {
            size = now;
            if ( val )
              strcpy( (AVP_str)val, ((AVP_STR_Property*)prop)->val );
          }
        }
        else if ( val && size ) {
          *((AVP_str)val) = 0;
          size = 0;
        }
        else
          size = 0;

        return size;

      case avpt_wstr  : 
        if ( ((AVP_WSTR_Property*)prop)->val ) {
          now = (AVP_dword)(sizeof(AVP_wchar) * (wcslen( (wchar_t*)((AVP_WSTR_Property*)prop)->val ) + 1));
          if ( size < now ) {
            if ( val ) {
              if ( size ) {
                memcpy( (void*)val, ((AVP_WSTR_Property*)prop)->val, sizeof(AVP_wchar)*(size-1) );
                //((AVP_wstr)val)[size-1] = 0;
								//VIK 21.02.02 18:20
								*((AVP_wstr)(((AVP_str)val) + (size-sizeof(AVP_wchar)))) = 0;
              }
            }
            else
              size = now;
          }
          else {
            size = now;
            if ( val )
              wcscpy( (wchar_t*)(AVP_wchar*)val, (wchar_t*)((AVP_WSTR_Property*)prop)->val );
          }
        }
        else if ( val && size ) {
          *((AVP_wstr)val) = 0;
          size = 0;
        }
        else
          size = 0;
        return size;

      case avpt_bin   : 
        if ( ((AVP_BIN_Property*)prop)->val.data ) {
          now = ((AVP_BIN_Property*)prop)->val.size;
          if ( size < now ) {
            if ( val ) {
              if ( size ) 
                memcpy( (void*)val, ((AVP_BIN_Property*)prop)->val.data, size );
            }
            else
              size = now;
          }
          else {
            size = now;
            if ( val )
              memcpy( (void*)val, ((AVP_BIN_Property*)prop)->val.data, size );
          }
        }
        else
          size = 0;

        return size;

      default : 
        _RPT0( _CRT_ASSERT, "Bad property type" );
        return 0;
    }
  }
}



