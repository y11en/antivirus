#include "prop_in.h"



// ---
DATA_PROC AVP_dword DATA_PARAM PROP_Arr_Set_Items( HPROP prop, AVP_dword pos, void* from, AVP_dword count ) {
	AVP_dword size;
	AVP_dword c_count;

	if ( !prop || !((AVP_Property*)prop)->array || ((AVP_Property*)prop)->type == avpt_nothing ) {
		_RPT0( _CRT_ASSERT, "Property is not an array" );
		return 0;
	}

	if ( !from || pos > ((AVP_Arr_Property*)prop)->count ) {
		_RPT0( _CRT_ASSERT, "Bad parameters" );
		return 0;
	}

	switch( ((AVP_Property*)prop)->type ) {
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
			count /= ((AVP_Arr_Property*)prop)->isize;
			if ( count == 0 ) {
				_RPT0( _CRT_ASSERT, "Bad parameters" );
				return 0;
			}
			size = 0;
			if ( pos + count > ((AVP_Arr_Property*)prop)->count ) {
				c_count = ((AVP_Arr_Property*)prop)->count - pos;
				count -= c_count;
			}
			else {
				c_count = count;
				count = 0;
			}
			if ( c_count ) {
				size += c_count * ((AVP_Arr_Property*)prop)->isize;
				memcpy( ((AVP_byte*)((AVP_Arr_Property*)prop)->val)+pos*((AVP_Arr_Property*)prop)->isize, from, size );
			}
			if ( count ) 
				size += PROP_Arr_Insert( prop, PROP_ARR_LAST, ((AVP_byte*)from)+c_count*((AVP_Arr_Property*)prop)->isize, count*((AVP_Arr_Property*)prop)->isize );
			break;
		case avpt_str   : 
			if ( pos <= ((AVP_Arr_Property*)prop)->count ) {
				AVP_str* dst;
				if ( !count )
					count = (AVP_dword)strlen( from ) + sizeof(AVP_char);
				dst = &((AVP_str*)((AVP_Arr_Property*)prop)->val)[ pos ];
				if ( *dst ) {
					size = (AVP_dword)strlen( *dst ) + sizeof(AVP_char);
					if ( size != count ) {
						liberator( *dst );
						*dst = allocator( count );
						_ASSERT( *dst );
					}
				}
				else 
					*dst = allocator( count );
				memcpy( *dst, from, count-sizeof(AVP_char) );
				(*dst)[count-1] = 0;
				size = count;
			}
			else 
				size = PROP_Arr_Insert( prop, PROP_ARR_LAST, from, count );
			break;
		case avpt_wstr  : 
			if ( pos <= ((AVP_Arr_Property*)prop)->count ) {
				AVP_wstr* dst;
				if ( !count )
					count = (AVP_dword)( wcslen( from ) + 1 ) * sizeof(AVP_wchar);
				dst = &((AVP_wstr*)((AVP_Arr_Property*)prop)->val)[ pos ];
				if ( *dst ) {
					size = (AVP_dword)( wcslen( (wchar_t *)*dst ) + 1 ) * sizeof(AVP_wchar);
					if ( size != count ) {
						liberator( *dst );
						*dst = allocator( count );
						_ASSERT( *dst );
					}
				}
				else 
					*dst = allocator( count );
				memcpy( *dst, from, count-sizeof(AVP_wchar) );
				(*dst)[count/sizeof(AVP_wchar)-1] = 0;
				size = count;
			}
			else 
				size = PROP_Arr_Insert( prop, PROP_ARR_LAST, from, count );
			break;
		case avpt_bin   : 
			if ( pos <= ((AVP_Arr_Property*)prop)->count ) {
				AVP_Bin_Item* dst;
				if ( !count ) {
					_RPT0( _CRT_ASSERT, "Bad parameters" );
					return 0;
				}
				dst = ((AVP_Bin_Item*)((AVP_Arr_Property*)prop)->val) + pos;
				if ( dst ) {
					if ( dst->size != count ) {
						dst->size = count;
						liberator( dst->data );
						dst->data = allocator( count );
						_ASSERT( dst->data );
					}
				}
				else {
					dst->size = count;
					dst->data = allocator( count );
				}
				memcpy( dst->data, from, count );
				size = count;
			}
			else 
				size = PROP_Arr_Insert( prop, PROP_ARR_LAST, from, count );
			break;
		default : 
			_RPT0( _CRT_ASSERT, "Bad property array type" );
			return 0;
	}
	return size;
}




