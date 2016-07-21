#include "prop_in.h"


// ---
AVP_dword DATA_PARAM PROP_Arr_Insert( HPROP prop, AVP_dword pos, void* from, AVP_dword size ) {

	_ASSERTE( allocator && liberator );

	if ( !from ) {
		_RPT0( _CRT_ASSERT, "Bad parameters" );
		return 0;
	}

	if ( !prop || !((AVP_Property*)prop)->array || ((AVP_Property*)prop)->type == avpt_nothing ) {
		_RPT0( _CRT_ASSERT, "Property is not an array" );
		return 0;
	}

	if ( pos > ((AVP_Arr_Property*)prop)->count ) 
		pos = ((AVP_Arr_Property*)prop)->count;

	if ( ((AVP_Property*)prop)->type == avpt_str ) {
		if ( PROP_Arr_Catch_Mem(prop,1) ) {
			AVP_bool add = 0;
			AVP_str* dst = ((AVP_str*)((AVP_Arr_Property*)prop)->val) + pos;
			if ( size ) 
				add = ((AVP_str)from)[size-1] != 0;
			else
				size = (AVP_dword)strlen( (AVP_str)from ) + sizeof(AVP_char);
			if ( pos < ((AVP_Arr_Property*)prop)->count ) 
				memmove( dst+1, dst, (((AVP_Arr_Property*)prop)->count-pos)*sizeof(AVP_str) );
			*dst = allocator( size + add );
			_ASSERT( *dst );
			memcpy( *dst, from, size );
			if ( add ) 
				(*dst)[ size++ ] = 0;
			((AVP_Arr_Property*)prop)->count += sizeof(AVP_char);
			return size;
		}
		else {
			_RPT0( _CRT_ASSERT, "Cannot chatch memory" );
			return 0;
		}
	}
	else if ( ((AVP_Property*)prop)->type == avpt_wstr ) {
		if ( PROP_Arr_Catch_Mem(prop,1) ) {
			AVP_bool  add = 0;
			AVP_wstr* dst = ((AVP_wstr*)((AVP_Arr_Property*)prop)->val) + pos;
			if ( size )
				add = ((AVP_wstr)from)[size-1] != 0;
			else
				size = (AVP_dword)(sizeof(AVP_wchar) * ( wcslen((wchar_t *)(AVP_word*)from) + 1 ));
			if ( pos < ((AVP_Arr_Property*)prop)->count ) 
				memmove( dst+1, dst, (((AVP_Arr_Property*)prop)->count-pos)*sizeof(AVP_wstr) );
			*dst = allocator( size + sizeof(AVP_wchar)*add );
			_ASSERT( *dst );
			memcpy( *dst, from, size );
			if ( add ) {
				//(*dst)[ size ] = 0;
				//VIK 21.02.02 18:30
				*((AVP_wstr)(((AVP_str)(*dst))+size)) = 0;
				size += sizeof(AVP_wchar);
			}
			((AVP_Arr_Property*)prop)->count += 1;
			return size;
		}
		else {
			_RPT0( _CRT_ASSERT, "Cannot chatch memory" );
			return 0;
		}
	}
	else if ( ((AVP_Property*)prop)->type == avpt_bin ) {
		if ( !size ) {
			_RPT0( _CRT_ASSERT, "Bad parameters" );
			return 0;
		}

		if ( PROP_Arr_Catch_Mem(prop,1) ) {
			AVP_Bin_Item* dst = ((AVP_Bin_Item*)((AVP_Arr_Property*)prop)->val) + pos;

			if ( pos < ((AVP_Arr_Property*)prop)->count ) 
				memmove( dst+1, dst, (((AVP_Arr_Property*)prop)->count-pos)*sizeof(AVP_Bin_Item) );

			dst->size = size;
			dst->data = allocator( size );
			_ASSERT( dst->data );
			memcpy( dst->data, from, size );
			((AVP_Arr_Property*)prop)->count += 1;
			return size;
		}
		else {
			_RPT0( _CRT_ASSERT, "Cannot chatch memory" );
			return 0;
		}
	}
	else if ( !size ) {
		_RPT0( _CRT_ASSERT, "Bad parameters" );
		return 0;
	}
	else {
		AVP_dword count = size / ((AVP_Arr_Property*)prop)->isize;
		if ( PROP_Arr_Catch_Mem(prop,count) ) {
			void* dst;
			AVP_dword bytes = count * ((AVP_Arr_Property*)prop)->isize;

			dst = ((AVP_byte*)((AVP_Arr_Property*)prop)->val) + pos * ((AVP_Arr_Property*)prop)->isize;

			if ( pos < ((AVP_Arr_Property*)prop)->count ) {
				//        memmove( ((AVP_byte*)dst)+bytes, dst, bytes );
				// VAM - 31.03.00
				memmove( ((AVP_byte*)dst)+bytes, dst, (((AVP_Arr_Property*)prop)->count-pos)*((AVP_Arr_Property*)prop)->isize );
			}

			switch ( ((AVP_Property*)prop)->type ) {
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
					memcpy( dst, from, bytes );
					break;
				default : 
					_RPT0( _CRT_ASSERT, "Bad property array type" );
					return 0;
			};
			((AVP_Arr_Property*)prop)->count += count;
			return bytes;
		}
		else {
			_RPT0( _CRT_ASSERT, "Cannot chatch memory" );
			return 0;
		}
	}
}




