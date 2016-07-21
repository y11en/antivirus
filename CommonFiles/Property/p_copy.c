#include "prop_in.h"

// ---
AVP_Property* DATA_PARAM PROP_Copy( AVP_Property* dst, AVP_Property* src ) {
  
  if ( dst->type != src->type || dst->array != src->array )
    return 0;

  if ( dst->array ) {
    #ifdef _DEBUG
      AVP_Arr_Property* s = (AVP_Arr_Property*)src;
    #else
      #define s ((AVP_Arr_Property*)src)
    #endif

		if ( s->val && s->count ) {
			AVP_dword i;
			if ( s->count > s->delta ) {
				AVP_word del = (AVP_word)(( ( s->count / s->delta ) + ( ( s->count % s->delta ) > 0 ) ) * s->delta);
				PROP_Arr_Delta( PROP_HANDLE(dst), del );
			}
			else
				PROP_Arr_Delta( PROP_HANDLE(dst), s->delta );

			switch( dst->type ) {
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
					PROP_Arr_Insert( PROP_HANDLE(dst), PROP_ARR_LAST, s->val, s->count * s->isize );
					break;
				case avpt_str     : 
					for( i=0; i<s->count; i++ ) 
						PROP_Arr_Insert( PROP_HANDLE(dst), PROP_ARR_LAST, ((AVP_str*)s->val)[i], 0 );
					break;
				case avpt_wstr    : 
					for( i=0; i<s->count; i++ ) 
						PROP_Arr_Insert( PROP_HANDLE(dst), PROP_ARR_LAST, ((AVP_wstr*)s->val)[i], 0 );
					break;
				case avpt_bin     : 
					for( i=0; i<s->count; i++ ) 
						PROP_Arr_Insert( PROP_HANDLE(dst), PROP_ARR_LAST, ((AVP_Bin_Item*)s->val)[i].data, ((AVP_Bin_Item*)s->val)[i].size );
					break;
				case avpt_nothing : 
				default           : 
					_RPT0( _CRT_ASSERT, "Bad property type" );
					break;
			}
		}
    PROP_Arr_Delta( PROP_HANDLE(dst), s->delta );
    #undef s
  }
  else {
    #define EQU( TYPE ) (((AVP_##TYPE##_Property*)dst)->val = ((AVP_##TYPE##_Property*)src)->val)
    switch( dst->type ) {
      case avpt_nothing : break;
      case avpt_char    : EQU( CHAR  );			break;
      case avpt_wchar   : EQU( WCHAR );			break;
      case avpt_short   : EQU( SHORT );			break;
      case avpt_long    : EQU( LONG  );			break;
      case avpt_byte    : EQU( BYTE  );			break;
      case avpt_group   : EQU( GROUP );			break;
      case avpt_word    : EQU( WORD  );			break;
      case avpt_dword   : EQU( DWORD );			break;
      case avpt_qword   : EQU( QWORD );			break;
      case avpt_longlong: EQU( LONGLONG );	break;
			case avpt_size_t  : EQU( SIZE_T );    break;
      case avpt_int     : EQU( INT   );			break;
      case avpt_uint    : EQU( UINT  );			break;
      case avpt_bool    : EQU( BOOL  );			break;
      case avpt_date    : 
        memcpy( ((AVP_DATE_Property*)dst)->val, ((AVP_DATE_Property*)src)->val, sizeof(((AVP_DATE_Property*)dst)->val) );
        break;
      case avpt_time    : 
        memcpy( ((AVP_TIME_Property*)dst)->val, ((AVP_TIME_Property*)src)->val, sizeof(((AVP_TIME_Property*)dst)->val) );
        break;
      case avpt_datetime    : 
        memcpy( ((AVP_DATETIME_Property*)dst)->val, ((AVP_DATETIME_Property*)src)->val, sizeof(((AVP_DATETIME_Property*)dst)->val) );
        break;
      case avpt_str     : 
        PROP_Set_Val( PROP_HANDLE(dst), (AVP_size_t)((AVP_STR_Property*)src)->val, 0 );
        break;
      case avpt_wstr    : 
        PROP_Set_Val( PROP_HANDLE(dst), (AVP_size_t)((AVP_WSTR_Property*)src)->val, 0 );
        break;
      case avpt_bin     : 
        PROP_Set_Val( PROP_HANDLE(dst), (AVP_size_t)((AVP_BIN_Property*)src)->val.data, ((AVP_BIN_Property*)src)->val.size );
        break;
      default           : 
        _RPT0( _CRT_ASSERT, "Bad property type" );
        break;
    }
    #undef EQU
  }
  return dst;
}




