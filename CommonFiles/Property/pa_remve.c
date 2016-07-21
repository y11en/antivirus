#include "prop_in.h"


// ---
AVP_dword DATA_PARAM PROP_Arr_Remove( HPROP prop, AVP_dword pos, AVP_dword count ) {

	AVP_int   tail;
	AVP_dword i;
	AVP_Arr_Property* arr;

	_ASSERTE( allocator && liberator );

	if ( !prop || !((AVP_Property*)prop)->array || ((AVP_Property*)prop)->type == avpt_nothing ) {
		_RPT0( _CRT_ASSERT, "Property is not an array" );
		return 0;
	}

	if ( !count ) {
		_RPT0( _CRT_ASSERT, "Bad parameters" );
		return 0;
	}

	if ( pos >= ((AVP_Arr_Property*)prop)->count ) {
		_RPT0( _CRT_ASSERT, "Bad parameters" );
		return 0;
	}

	arr = (AVP_Arr_Property*)prop;
	tail = arr->count - pos - count;
	switch( arr->prop.type ) {
		case avpt_char     :
		case avpt_wchar    :
		case avpt_short    :
		case avpt_long     :
		case avpt_byte     :
		case avpt_group    :
		case avpt_word     :
		case avpt_dword    :
		case avpt_qword    :
		case avpt_longlong :
		case avpt_size_t   :
		case avpt_int      :
		case avpt_uint     :
		case avpt_bool     :
		case avpt_date     :
		case avpt_time     :
		case avpt_datetime :
			break;
		case avpt_str      :
		case avpt_wstr     :
			for( i=pos; i<pos+count && i<arr->count; i++ ) 
				liberator( ((void**)arr->val)[i] );
			break; 
		case avpt_bin   :
			for( i=pos; i<pos+count && i<arr->count; i++ ) 
				liberator( ((AVP_Bin_Item*)arr->val)[i].data );
			break;
		default : 
			_RPT0( _CRT_ASSERT, "Bad property array type" );
			return 0;
	}

	if ( tail > 0 ) 
	{
		void * dst = ((AVP_byte*)arr->val) + pos * arr->isize;
		void * src = ((AVP_byte*)dst) + ( count * arr->isize );
		memmove( dst, src, tail * arr->isize );
		arr->count -= count;
	}
	else {
		count = arr->count - pos;
		arr->count = pos;
	}
	return count;
}



