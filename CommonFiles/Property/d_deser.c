#include "prop_in.h"
#include "../AVPPort.h"
#include <byteorder.h>


static void       DATA_PARAM init_get_bytes( Serialize* sz, void* buffer, AVP_dword size, Buffer_Proc fproc, void* user_data );
static AVP_dword  DATA_PARAM flush_get_bytes( Serialize* sz );
static AVP_dword  DATA_PARAM get_bytes( Serialize* sz, void* val, AVP_dword size );
static AVP_byte   DATA_PARAM next_byte( Serialize* sz );

static AVP_dword DATA_PARAM DATA_Get( Serialize* sz, AVP_Data* dad, AVP_Data** out );
static AVP_dword DATA_PARAM PROP_Get( Serialize* sz, AVP_Property* prop );
static AVP_dword DATA_PARAM PROP_Arr_Get( Serialize* sz, AVP_Arr_Property* prop );

#ifdef _MSC_VER
#define inline
#endif


static inline AVP_dword DATA_PARAM get_byte( Serialize* sz, AVP_byte* val ) {
	return get_bytes( sz, val, sizeof(AVP_byte));
}

static inline AVP_dword DATA_PARAM get_word( Serialize* sz, AVP_word* val ) {
	if ( get_bytes(sz,val,sizeof(AVP_word)) != sizeof(AVP_word) )
		return 0;

	WriteWordPtr (val, *val);
	return sizeof(AVP_word);
}

static inline AVP_dword DATA_PARAM get_dword( Serialize* sz, AVP_dword* val ) {
	if ( get_bytes(sz,val,sizeof(AVP_dword)) != sizeof(AVP_dword) )
		return 0;

	WriteDWordPtr (val, *val);
	return sizeof(AVP_dword);
}

static inline AVP_dword DATA_PARAM get_qword ( Serialize* sz, AVP_qword* val ) {
	if ( get_bytes(sz,val,sizeof(AVP_qword)) != sizeof(AVP_qword) )
		return 0;

	WriteQWordPtr( val, *val );
	return sizeof(AVP_qword);
}

static inline AVP_dword DATA_PARAM get_size_t( Serialize* sz, AVP_size_t* val ) {
	if ( get_bytes(sz,val,sizeof(AVP_size_t)) != sizeof(AVP_size_t) )
		return 0;

	WriteSize_tPtr( val, *val );
	return sizeof(AVP_size_t);
}

static inline AVP_dword DATA_PARAM get_date( Serialize* sz, AVP_date* val ) {
	if ( get_bytes(sz,val,sizeof(AVP_date)) == sizeof(AVP_date) ) {
		AVP_struct_date * orig = (AVP_struct_date *) val;

		orig->wYear 		= ReadWordPtr (&orig->wYear);
		orig->wMonth		= ReadWordPtr (&orig->wMonth);
		orig->wDayOfWeek	= ReadWordPtr (&orig->wDayOfWeek);
		orig->wDay			= ReadWordPtr (&orig->wDay);

		return sizeof(AVP_date);
	}

	return 0;
}

static inline AVP_dword DATA_PARAM get_time( Serialize* sz, AVP_time* val ) {
	if ( get_bytes(sz,val,sizeof(AVP_time)) == sizeof(AVP_time) ) {
		AVP_struct_time * orig = (AVP_struct_time *) val;

		orig->wHour			= ReadWordPtr (&orig->wHour);
		orig->wMinute		= ReadWordPtr (&orig->wMinute);
		orig->wSecond		= ReadWordPtr (&orig->wSecond);
		orig->wMilliseconds = ReadWordPtr (&orig->wMilliseconds);

		return sizeof(AVP_time);
	}

	return 0;
}

static inline AVP_dword DATA_PARAM get_datetime( Serialize* sz, AVP_datetime* val ) {
	if ( get_bytes(sz,val,sizeof(AVP_datetime)) == sizeof(AVP_datetime) ) {
		AVP_struct_datetime * orig = (AVP_struct_datetime *) val;

		orig->wYear 		= ReadWordPtr (&orig->wYear);
		orig->wMonth		= ReadWordPtr (&orig->wMonth);
		orig->wDayOfWeek	= ReadWordPtr (&orig->wDayOfWeek);
		orig->wDay			= ReadWordPtr (&orig->wDay);
		orig->wHour			= ReadWordPtr (&orig->wHour);
		orig->wMinute		= ReadWordPtr (&orig->wMinute);
		orig->wSecond		= ReadWordPtr (&orig->wSecond);
		orig->wMilliseconds = ReadWordPtr (&orig->wMilliseconds);

		return sizeof(AVP_time);
	}

	return 0;
}

static inline AVP_dword DATA_PARAM get_avp_property( Serialize* sz, AVP_Property* val ) {
	if ( get_bytes(sz,val,sizeof(AVP_Property)) == sizeof(AVP_Property) ) {
		val->id = ReadWordPtr (&val->id);
		return sizeof(AVP_Property);
	}

	return 0;
}

static inline void DATA_PARAM convert_wchar_string( wchar_t* string ) {
	if ( string ) {
		while ( *string ){
			WriteWordPtr (string, *string);
			string++;
		}
	}
}


// ---
DATA_PROC HDATA DATA_PARAM DATA_Deserialize( AVP_Serialize_Data* opt, void* buffer, AVP_dword size_in, AVP_dword* size_out ) {

	AVP_word            magic;
	AVP_Data*           data = 0;
	AVP_Serialize_Data  sd;
	AVP_Serialize_Data* psd;
	Serialize           sz;

	if ( !buffer || !size_in ) {
		_RPT0( _CRT_ASSERT, "Bad parameters" );
		return 0;
	}

	if ( opt )
		psd = opt;
	else {
		psd = &sd;
		memset( psd, 0, sizeof(AVP_Serialize_Data) );
	}

	init_get_bytes( &sz, buffer, size_in, psd->proc, psd->user_data );

	if ( get_word (&sz, &magic) != sizeof(magic) || magic != 0xadad ) {
		if ( size_out )
			*size_out = sz.tsize;
		return 0;
	}

	if ( get_dword (&sz, &psd->version) != sizeof(psd->version) || psd->version != 1 ) {
		if ( size_out )
			*size_out = sz.tsize;
		return 0;
	}

	if ( get_dword (&sz, &sz.crc_cluster) != sizeof(sz.crc_cluster) ) {
		if ( size_out )
			*size_out = sz.tsize;
		return 0;
	}

	sz.crc = -1;

	if ( !DATA_Get(&sz,0,&data) || !data || !flush_get_bytes(&sz) ) {
		if ( data ) {
			DATA_Remove( (HDATA)data, 0 );
			data = 0;
		}
		else
			flush_get_bytes( &sz );
	}
	if ( size_out )
		*size_out = sz.tsize;

	return DATA_HANDLE(data);
}

// ---
static AVP_dword DATA_PARAM DATA_Get( Serialize* sz, AVP_Data* dad, AVP_Data** out ) {
	AVP_byte      key_byte;
	AVP_Property  prop;
	AVP_Data*     add;
	AVP_dword     total = sz->tsize;

	if ( get_byte( sz, &key_byte) != sizeof(key_byte) ) 
		return 0;

	if ( key_byte & ~(PUT_VAL_PROP|PUT_CHILD_DATA|PUT_NEXT_DATA) ) {
		_RPT0( _CRT_ASSERT, "Bad stream" );
		return 0;
	}

	if ( get_avp_property( sz, &prop) != sizeof(prop) ) 
		return 0;

	*out = (AVP_Data*)DATA_Add( DATA_HANDLE(dad), 0, PROP_ID(&prop), 0, 0 );
	if ( *out ) {
		AVP_Property pr;
		AVP_dword    id;

		if ( (key_byte&PUT_VAL_PROP) && !PROP_Get(sz,&(*out)->value.data) )
			return 0;

		if ( get_avp_property( sz, &pr) != sizeof(pr) )
			return 0;

		id = PROP_ID(&pr);

		while( id != AVP_PID_END ) {
			AVP_Property* ptr = (AVP_Property*)DATA_Add_Prop( DATA_HANDLE(*out), 0, id, 0, 0 );

			if ( !ptr )
				return 0;

			if ( !PROP_Get(sz,ptr) )
				return 0;

			if ( get_avp_property( sz, &pr) != sizeof(pr) )
				return 0;

			id = PROP_ID(&pr);
		}
	}
	if (  (key_byte & PUT_CHILD_DATA)  &&  (!DATA_Get(sz,*out,&add))  ) 
		return 0;
	if ( key_byte & PUT_NEXT_DATA ) {
		AVP_byte end;
		while( next_byte(sz) != AVP_NEXT_END ) {
			if ( !DATA_Get(sz,dad,&add) )
				return 0;
		}
		get_byte( sz, &end );
	}

	return sz->tsize - total;
}


void * DATA_PARAM DATA_Get_Property_Item_Addr( AVP_Property* prop ) {
	switch( prop->type ) {
		case avpt_nothing : return (void *) 1; // this addr won't be used later

		// Byte-sized types
		case avpt_char    : return &((AVP_CHAR_Property*)prop)->val;
		case avpt_byte    : return &((AVP_BYTE_Property*)prop)->val;
		case avpt_group   : return &((AVP_GROUP_Property*)prop)->val;
		case avpt_bool    : return &((AVP_BOOL_Property*)prop)->val;

		// Word-sized types            
		case avpt_wchar   : return &((AVP_WCHAR_Property*)prop)->val;
		case avpt_short   : return &((AVP_SHORT_Property*)prop)->val;
		case avpt_word    : return &((AVP_WORD_Property*)prop)->val;

		// Dword-sized types      
		case avpt_long    : return &((AVP_LONG_Property*)prop)->val;
		case avpt_dword   : return &((AVP_DWORD_Property*)prop)->val;
		case avpt_int     : return &((AVP_INT_Property*)prop)->val;
		case avpt_uint    : return &((AVP_UINT_Property*)prop)->val;

		// QWord-sized types
		case avpt_qword   : return &((AVP_QWORD_Property*)prop)->val;
		case avpt_longlong: return &((AVP_LONGLONG_Property*)prop)->val;

		// size_t-sized types
		case avpt_size_t  : return &((AVP_SIZE_T_Property*)prop)->val;

		// Custom structures
		case avpt_date    : return &((AVP_DATE_Property*)prop)->val;
		case avpt_time    : return &((AVP_TIME_Property*)prop)->val;
		case avpt_datetime: return &((AVP_DATETIME_Property*)prop)->val;

		// String
		case avpt_str		: return &((AVP_STR_Property*)prop)->val;

		// Windows unicode string
		case avpt_wstr	: return &((AVP_WSTR_Property*)prop)->val;

		case avpt_bin		: return &((AVP_BIN_Property*)prop)->val;

		default :
			_RPT0( _CRT_ASSERT, "Bad property type" );
			return 0;
	}
}


static AVP_dword DATA_PARAM ReadPropertyItem( Serialize* sz, AVP_byte type, void* val ) {
	AVP_word size;
	AVP_byte tmp;
	AVP_Bin_Item* v;

	switch( type ) {
		case avpt_nothing : return 1;

		// Byte-sized types
		case avpt_char    :
		case avpt_byte    :
		case avpt_group   : return get_byte(sz, (AVP_byte *) val);

		case avpt_bool    : 
			size = (AVP_word)get_byte( sz, &tmp);
			*(AVP_bool *)val = tmp;
			return size;

		// Word-sized types            
		case avpt_wchar   :
		case avpt_short   :
		case avpt_word    : return get_word(sz, (AVP_word *) val);

		// Dword-sized types      
		case avpt_long    :
		case avpt_dword   :
		case avpt_int     :
		case avpt_uint    : return get_dword( sz, (AVP_dword*)val );

		// QWord-sized types
		case avpt_qword   :
		case avpt_longlong: return get_qword( sz, (AVP_qword*)val );

		// size_t-sized types
		case avpt_size_t  : return get_size_t( sz, (AVP_size_t*)val );

		// Custom structures
		case avpt_date    : return get_date(sz, (AVP_date *)val);
		case avpt_time    : return get_time(sz, (AVP_time *)val);
		case avpt_datetime: return get_datetime(sz, (AVP_datetime *) val);

		// String
		case avpt_str :                           
			if ( get_word( sz, &size) == sizeof(size) ) {
				if ( size != USHRT_MAX ) {
					AVP_char * ptr = allocator( size + sizeof(AVP_char) );
					_ASSERT( ptr );
					*(AVP_char **)val = ptr;

					if ( get_bytes(sz,ptr,size) == size ) {
						ptr[size] = 0;
						return sizeof(size) + size;
					}
					else {
						ptr[0] = 0;
						return 0;
					}
				}
				else
					return sizeof(size);
			}
			else
				return 0;

		// Windows unicode string
		case avpt_wstr :
			if ( get_word( sz, &size) == sizeof(size) ) {
				if ( size != USHRT_MAX ) {
					AVP_wchar * ptr = allocator( size + sizeof(AVP_wchar) );
					_ASSERT( ptr );
					*(AVP_wchar **)val = ptr;

					if ( get_bytes(sz,ptr,size) == size ) {
						ptr[size/sizeof(AVP_wchar)] = 0;
						convert_wchar_string ((wchar_t*) ptr);
						return sizeof(size) + size;
					}
					else {
						ptr[0] = 0;
						return 0;
					}
				}
				else
					return sizeof(size);
			}
			else
				return 0;

		case avpt_bin :
			v = val;
			v->size = 0;
			if ( get_word( sz, &size) == sizeof(size) )
			{
				v->size = size;
				if ( v->size ) {
					v->data = allocator( v->size );
					_ASSERT( v->data );
				}
				else
					v->data = 0;
				if ( !v->size || (get_bytes(sz,v->data,v->size) == v->size) ) 
					return sizeof(v->size) + v->size;
				else 
					return 0;
			}
			else
				return 0;

		default :
			_RPT0( _CRT_ASSERT, "Bad property type" );
			return 0;
	}

	return size ? size + sizeof(AVP_Property) : 0;
}




// ---
static AVP_dword DATA_PARAM PROP_Get( Serialize* sz, AVP_Property* prop ) {
	if ( !prop ) {
		_RPT0( _CRT_ASSERT, "Property handle is zero" );
		return 0;
	}
	if ( prop->array )
	{
		AVP_dword retval = PROP_Arr_Get( sz, (AVP_Arr_Property*)prop );
		return retval;
	}
	else {
		void * addr = DATA_Get_Property_Item_Addr (prop);
		if ( addr )
			return ReadPropertyItem( sz, prop->type, addr);
	}
	return 0;
}

// ---
static AVP_dword DATA_PARAM PROP_Arr_Get( Serialize* sz, AVP_Arr_Property* prop ) {

	AVP_dword i, c;
	AVP_dword size;
	AVP_dword vsize;
	AVP_word  tmpvsize;
	AVP_byte* s;
	AVP_byte  arr[10];
	AVP_bool  res;

	if ( !prop || !prop->prop.array || !prop->prop.type == avpt_nothing ) {
		_RPT0( _CRT_ASSERT, "Property is not an array" );
		return 0;
	}

	vsize = 0;

	if ( 
		(sizeof( AVP_word ) == get_word( sz, &tmpvsize )) &&
		(sizeof( prop->delta ) == get_word( sz, &prop->delta ))
	) {
		vsize = tmpvsize;
		size = sizeof( prop->count ) + sizeof( prop->delta );
	}
	else
		return 0;

	if ( !vsize )
		return size;

	res = PROP_Arr_Catch_Mem( PROP_HANDLE(prop), vsize );

	_ASSERT( res );

	prop->count = vsize;

	switch( prop->prop.type ) {
		case avpt_bool    : 
			c = (vsize / 8) + ( (vsize % 8) > 0 );
			if ( c > sizeof(arr) )
				s = allocator( c );
			else
				s = arr;
			if ( c == get_bytes(sz,s,c) ) {
				for( i=0; i<vsize; i++ ) {
					AVP_dword byte = i / 8;
					AVP_dword bit = i % 8;
					((AVP_bool*)prop->val)[i] = (s[byte] & (1 << bit)) != 0;
				}
			}
			else
				size = 0;
			if ( c > sizeof(arr) )
				liberator( s );
			return size;

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
		case avpt_date    :
		case avpt_time    :
		case avpt_datetime:
		case avpt_str     :
		case avpt_wstr :
		case avpt_bin :
			break;

		case avpt_nothing : 
		default :
			_RPT0( _CRT_ASSERT, "Bad property type" );
			return 0;
	}

	for ( i = 0; i < prop->count; i++ ) 
	{
		if ( (c = ReadPropertyItem( sz, prop->prop.type, ((char *) prop->val) + i * prop->isize)) == 0 )
		{
			size = 0;
			break;
		}

		size += c;
	}

	return size;
}


// ---
static void DATA_PARAM init_get_bytes( Serialize* sz, void* buffer, AVP_dword size, Buffer_Proc fproc, void* user_data ) {
	sz->buffer					= (AVP_byte*)buffer;
	sz->rsize						= size;
	sz->bsize						= size;
	sz->tsize						= 0;
	sz->csize						= fproc ? size : 0;
	sz->crc							= -1;
	sz->crc_len					= 0;
	sz->crc_cluster			= CRC_CLUSTER;
	sz->crc_not_in_use  = 1;
	sz->buff_loaded			= 0;
	sz->proc						= fproc;
	sz->data						= user_data;
}



// ---
static AVP_dword DATA_PARAM flush_get_bytes( Serialize* sz ) {
	if ( sz->crc_cluster && sz->crc_len != 0 && sz->crc != (AVP_dword)-1 ) {
		AVP_dword crc_saved;
		AVP_dword crc_counted = sz->crc;

		sz->crc_len = 0;
		if ( get_dword(sz, &crc_saved) == sizeof(sz->crc) && crc_saved == crc_counted ) {
			/*
			if ( sz->proc ) {
			if ( sz->proc(sz->buffer,sz->csize,sz->data) && sz->csize == out_size )
			return sz->tsize;
			else
			return 0;
			}
			else
			return sz->tsize;
			*/
			return sz->tsize;
		}
		else
			return 0;
	}
	else
		return sz->tsize;
}                 


// ---
static AVP_dword DATA_PARAM get_bytes( Serialize* sz, void* val, AVP_dword size ) {

	AVP_dword loaded = 0;

	if ( !size )
		return 0;

	if ( sz->crc_cluster && sz->crc_not_in_use && sz->crc_len + size >= sz->crc_cluster ) {
		AVP_dword crc_counted;
		AVP_dword crc_saved;
		AVP_dword r_size = sz->crc_cluster - sz->crc_len;
		AVP_dword s_size;
		sz->crc_not_in_use = 0;
		s_size = r_size ? get_bytes( sz, val, r_size ) : 0;
		sz->crc_not_in_use = 1;
		crc_counted = sz->crc;
		sz->crc_len = 0;

		if ( s_size == r_size && get_dword( sz, &crc_saved) == sizeof(sz->crc) && crc_saved == crc_counted ) {
			sz->crc = -1;
			sz->crc_len = 0;
			r_size = size - r_size;
			loaded = s_size + ( r_size ? get_bytes(sz,((AVP_byte*)val)+s_size,r_size) : 0 );
		}
	}
	else {
		if ( sz->bsize > sz->csize + size ) {
			memcpy( val, sz->buffer+sz->csize, size );
			if ( sz->crc_cluster ) {
				sz->crc = CountCRC32WithCRC( size, (AVP_byte*)val, sz->crc );
				sz->crc_len += size;
			}
			sz->csize += size;
			sz->tsize += size;
			loaded = size;
		}
		else {
			while( size ) {
				AVP_dword rest = sz->bsize - sz->csize;
				if ( rest > size )
					rest = size;
				if ( rest ) {
					memcpy( val, sz->buffer+sz->csize, rest );
					if ( sz->crc_cluster ) {
						sz->crc = CountCRC32WithCRC( rest, (AVP_byte*)val, sz->crc );
						sz->crc_len += rest;
					}
					*((AVP_byte**)&val) += rest;
					size -= rest;
					sz->csize += rest;
					sz->tsize += rest;
					loaded += rest;
				}

				if ( sz->csize == sz->bsize ) {
					if ( sz->buff_loaded && sz->bsize	== 0 )
						return 0;
					sz->buff_loaded = 1;
					sz->bsize = 0;
					sz->csize = 0;
					if ( sz->proc ) {
						if ( !sz->proc(sz->buffer,sz->rsize,&sz->bsize,sz->data) ) 
							return 0;
					}
				}
			}
		}
	}
	return loaded;
}

// ---
static AVP_byte DATA_PARAM next_byte( Serialize* sz ) {
	return *(sz->buffer + sz->csize);
}


