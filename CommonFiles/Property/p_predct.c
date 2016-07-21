#include "prop_in.h"

// ---
AVP_dword DATA_PARAM PROP_Predict_Size( AVP_dword prop_id ) {

	if ( ((AVP_Property*)&prop_id)->array ) {
		switch( ((AVP_Property*)&prop_id)->type ) {
			case avpt_char      :
			case avpt_wchar     :
			case avpt_short     :
			case avpt_long      :
			case avpt_byte      :
			case avpt_group     :
			case avpt_word      :
			case avpt_dword     :
			case avpt_qword     :
			case avpt_longlong  :
			case avpt_size_t    :
			case avpt_int       :
			case avpt_uint      :
			case avpt_bool      :
			case avpt_date      :
			case avpt_time      :
			case avpt_datetime  :
			case avpt_str       :
			case avpt_wstr      :
			case avpt_bin       :
				break;
			case avpt_nothing :
			default :
				_RPT0( _CRT_ASSERT, "Bad array type" );
				break;
		}
		return sizeof(AVP_Arr_Property);
	}
	else {
		switch( ((AVP_Property*)&prop_id)->type ) {
			case avpt_nothing : return sizeof(AVP_Property);
			case avpt_char    : return sizeof(AVP_CHAR_Property);
			case avpt_wchar   : return sizeof(AVP_WCHAR_Property);
			case avpt_short   : return sizeof(AVP_SHORT_Property);
			case avpt_long    : return sizeof(AVP_LONG_Property);
			case avpt_byte    : return sizeof(AVP_BYTE_Property);
			case avpt_group   : return sizeof(AVP_GROUP_Property);
			case avpt_word    : return sizeof(AVP_WORD_Property);
			case avpt_dword   : return sizeof(AVP_DWORD_Property);
			case avpt_qword   : return sizeof(AVP_QWORD_Property);
			case avpt_longlong: return sizeof(AVP_QWORD_Property);
			case avpt_size_t  : return sizeof(AVP_SIZE_T_Property);
			case avpt_int     : return sizeof(AVP_INT_Property);
			case avpt_uint    : return sizeof(AVP_UINT_Property);
			case avpt_bool    : return sizeof(AVP_BOOL_Property);
			case avpt_date    : return sizeof(AVP_DATE_Property);
			case avpt_time    : return sizeof(AVP_TIME_Property);
			case avpt_datetime: return sizeof(AVP_DATETIME_Property);
			case avpt_str     : return sizeof(AVP_STR_Property);
			case avpt_wstr    : return sizeof(AVP_WSTR_Property);
			case avpt_bin     : return sizeof(AVP_BIN_Property);
			default           :
				_RPT0( _CRT_ASSERT, "Bad property type" );
				return sizeof(AVP_Property);
		}
	}
}



