#include "prop_in.h"



// ---
AVP_dword DATA_PARAM PROP_Arr_Get_Items( HPROP prop, AVP_dword pos, void* to, AVP_dword count ) {
	void*     src;
	AVP_dword size;

	if ( !prop || !((AVP_Property*)prop)->array || ((AVP_Property*)prop)->type == avpt_nothing ) {
		_RPT0( _CRT_ASSERT, "Property is not an array" );
		return 0;
	}

	if ( pos >= ((AVP_Arr_Property*)prop)->count ) {
		_RPT0( _CRT_ASSERT, "Array index is out of range" );
		return 0;
	}

	switch( ((AVP_Property*)prop)->type ) {
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
			count /= ((AVP_Arr_Property*)prop)->isize;
			if ( to ) {
				if ( count == 0 )
					return 0;
				if ( pos + count <= ((AVP_Arr_Property*)prop)->count )
					size = count * ((AVP_Arr_Property*)prop)->isize;
				else
					size = ( ((AVP_Arr_Property*)prop)->count - pos ) * ((AVP_Arr_Property*)prop)->isize;
				if ( size )
					memcpy( to, ((AVP_byte*)((AVP_Arr_Property*)prop)->val)+pos*((AVP_Arr_Property*)prop)->isize, size );
			}
			else {
				if ( pos <= ((AVP_Arr_Property*)prop)->count )
					size = ( ((AVP_Arr_Property*)prop)->count - pos ) * ((AVP_Arr_Property*)prop)->isize;
				else
					size = 0;
			}
			break;
		case avpt_str   : 
			src = ((AVP_str*)((AVP_Arr_Property*)prop)->val)[ pos ];
			size = (AVP_dword)strlen( (AVP_str)src ) + sizeof(AVP_char);
			if ( to ) {
				if ( count == 0 )
					return 0;
				if ( count < size )
					size = count;
				memcpy( to, src, size-sizeof(AVP_char) );
				((AVP_str)to)[size-1] = 0;
			}
			break;
		case avpt_wstr  : 
			src = (void*)((AVP_wstr*)((AVP_Arr_Property*)prop)->val)[ pos ];
			size = (AVP_dword)(sizeof(AVP_wchar) * ( wcslen((wchar_t *)(AVP_wstr)src) + 1 ));
			if ( to ) {
				if ( count < sizeof(AVP_wchar) )
					return 0;
				if ( count < size ) 
					size = count / sizeof(AVP_wchar) * sizeof(AVP_wchar);
				memcpy( to, src, size-sizeof(AVP_wchar) );
				//((AVP_wstr)to)[size-1] = 0;
				//VIK 21.02.02 18:20
				*((AVP_wstr)(((AVP_str)to) + (size-sizeof(AVP_wchar)))) = 0;
			}
			break;
		case avpt_bin   : 
			src = ((AVP_Bin_Item*)((AVP_Arr_Property*)prop)->val) + pos;
			if ( to ) {
				if ( count == 0 )
					return 0;
				if ( ((AVP_Bin_Item*)src)->size < count )
					size = ((AVP_Bin_Item*)src)->size;
				else
					size = count;
				memcpy( to, ((AVP_Bin_Item*)src)->data, size );
			}
			else 
				size = ((AVP_Bin_Item*)src)->size;
			break;
		default : 
			_RPT0( _CRT_ASSERT, "Bad property array type" );
			return 0;
	}
	return size;
}



