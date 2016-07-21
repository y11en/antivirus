#include "prop_in.h"


static AVP_bool  DATA_PARAM In_Process( AVP_dword id, AVP_dword* include_list, AVP_dword* exclude_list );
static void      DATA_PARAM init_put_bytes( Serialize* sz, void* buffer, AVP_dword size, Buffer_Proc fproc, void* user_data );
static AVP_dword DATA_PARAM flush_put_bytes( Serialize* sz );
static AVP_dword DATA_PARAM put_bytes( Serialize* sz, void* val, AVP_dword size );
static AVP_dword DATA_PARAM DATA_Put( Serialize* sz, AVP_Data* data, AVP_bool next, AVP_dword* include_list, AVP_dword* exclude_list );
static AVP_dword DATA_PARAM PROP_Put( Serialize* sz, AVP_Property* prop );
static AVP_dword DATA_PARAM PROP_Arr_Put( Serialize* sz, AVP_Arr_Property* prop );


void * DATA_PARAM DATA_Get_Property_Item_Addr (AVP_Property* prop);

#ifdef _MSC_VER
#define inline
#endif



static inline AVP_dword DATA_PARAM put_byte( Serialize* sz, AVP_byte* val ) {
	return put_bytes( sz, val, sizeof(AVP_byte) );
}

static inline AVP_dword DATA_PARAM put_word( Serialize* sz, AVP_word* val ) {
	AVP_word converted = ReadWordPtr( val );
	return put_bytes( sz, &converted, sizeof(converted) );
}

static inline AVP_dword DATA_PARAM put_dword( Serialize* sz, AVP_dword* val ) {
	AVP_dword converted = ReadDWordPtr( val );
	return put_bytes( sz, &converted, sizeof(converted) );
}

static inline AVP_dword DATA_PARAM put_qword( Serialize* sz, AVP_qword* val ) {
	AVP_qword converted = ReadQWordPtr( val );
	return put_bytes( sz, &converted, sizeof(converted) );
}

static inline AVP_dword DATA_PARAM put_size_t( Serialize* sz, AVP_size_t* val ) {
	AVP_size_t converted = ReadSize_tPtr( val );
	return put_bytes( sz, &converted, sizeof(converted) );
}

static inline AVP_dword DATA_PARAM put_date( Serialize* sz, AVP_date* val ) {
	AVP_struct_date * orig = (AVP_struct_date *)val;
	AVP_struct_date	converted;

	converted.wYear       = ReadWordPtr( &orig->wYear );
	converted.wMonth      = ReadWordPtr( &orig->wMonth );
	converted.wDayOfWeek  = ReadWordPtr( &orig->wDayOfWeek );
	converted.wDay        = ReadWordPtr( &orig->wDay );
	return put_bytes( sz, &converted, sizeof(converted) );
}

static inline AVP_dword DATA_PARAM put_time( Serialize* sz, AVP_time* val ) {
	AVP_struct_time * orig = (AVP_struct_time *) val;
	AVP_struct_time converted;

	converted.wHour			= ReadWordPtr (&orig->wHour);
	converted.wMinute		= ReadWordPtr (&orig->wMinute);
	converted.wSecond		= ReadWordPtr (&orig->wSecond);
	converted.wMilliseconds = ReadWordPtr (&orig->wMilliseconds);

	return put_bytes (sz, &converted, sizeof(converted));
}

static inline AVP_dword DATA_PARAM put_datetime( Serialize* sz, AVP_datetime* val ) {
	AVP_struct_datetime * orig = (AVP_struct_datetime *) val;
	AVP_struct_datetime converted;

	converted.wYear 		= ReadWordPtr (&orig->wYear);
	converted.wMonth		= ReadWordPtr (&orig->wMonth);
	converted.wDayOfWeek	= ReadWordPtr (&orig->wDayOfWeek);
	converted.wDay			= ReadWordPtr (&orig->wDay);
	converted.wHour			= ReadWordPtr (&orig->wHour);
	converted.wMinute		= ReadWordPtr (&orig->wMinute);
	converted.wSecond		= ReadWordPtr (&orig->wSecond);
	converted.wMilliseconds = ReadWordPtr (&orig->wMilliseconds);

	return put_bytes (sz, &converted, sizeof(converted));
}

static inline AVP_dword DATA_PARAM put_avp_property( Serialize* sz, AVP_Property* val ) {
	AVP_Property converted;
	
	memcpy (&converted, val, sizeof(AVP_Property));
	converted.id = ReadWordPtr (&converted.id);
	
	return put_bytes (sz, &converted, sizeof(converted));
}

static inline AVP_dword DATA_PARAM put_wchar_string( Serialize* sz, wchar_t * string ) {
	AVP_dword total_size;

	if ( string ) {
		AVP_dword psize = (AVP_dword)wcslen (string) * sizeof(AVP_wchar);
		AVP_word wsize = (AVP_word)psize;
		
		if ( psize >= USHRT_MAX ) {
			_RPT0( _CRT_ASSERT, "String is too long" );
			return 0;
		}
		
		if ( sizeof(AVP_word) != put_word (sz, &wsize) )
			return 0;
	
		for ( ; *string; string++ ) {
			if ( put_word(sz, string) != sizeof(AVP_word) )
				return 0;
		}

		total_size = sizeof(AVP_word) + psize;
	}
	else {
		AVP_word dummy = USHRT_MAX;
		if ( sizeof(AVP_word) == put_word(sz,&dummy) )
			total_size = sizeof(AVP_word);
		else
			total_size = 0;
	}

	return total_size;
}



// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Serialize( HDATA data, AVP_dword* addr, AVP_Serialize_Data* opt, void* buffer, AVP_dword buffer_size, AVP_dword* size_out ) {

  AVP_word    magic;
  AVP_dword   version;
  AVP_dword   max_size;
  AVP_dword*  include;
  AVP_dword*  exclude;
  AVP_bool    root_siblings;
  Serialize   sz;
  Buffer_Proc flush;

  FIND_DATA( data, addr );

  /*
  size_to_save = 
    sizeof( ((AVP_Serialize_Data*)0)->magic )           + // magic number
    sizeof( ((AVP_Serialize_Data*)0)->version )         + // version
    sizeof( ((AVP_Serialize_Data*)0)->translate_method ) + // translate_method
    sizeof( ((AVP_Serialize_Data*)0)->header_size );      // header size
  */

  if ( opt ) {
    include = opt->props_include;
    exclude = opt->props_exclude;
    opt->version = 1;
  }
  else {
    include = 0;
    exclude = 0;
  }

  if ( buffer ) {
    if ( buffer_size ) {
      max_size = buffer_size;
      flush = opt ? opt->proc : 0; 
    }
    else {
      _RPT0( _CRT_ASSERT, "Bad parameters" );
      if ( size_out )
        *size_out = 0;
      return 0;
    }
  }
  else {
    flush = 0; 
    max_size = (AVP_dword)-1;
  }

  init_put_bytes( &sz, buffer, max_size, flush, opt ? opt->user_data : 0 );

  magic = 0xadad;
  if ( put_word(&sz, &magic) != sizeof(magic) ) {
    if ( size_out )
      *size_out = sz.tsize;
    return 0;
  }

  version = 1;
  if ( opt ) {
    root_siblings = opt->root_siblings;
    opt->version = 1;
  }
  else
    root_siblings = 1;


  if ( put_dword (&sz, &version) != sizeof(version) ) {
    if ( size_out )
      *size_out = sz.tsize;
    return 0;
  }

  if ( put_dword (&sz, &sz.crc_cluster) != sizeof(sz.crc_cluster) ) {
    if ( size_out )
      *size_out = sz.tsize;
    return 0;
  }

  sz.crc = -1;

  if ( 0 == DATA_Put(&sz,(AVP_Data*)data,root_siblings,include,exclude) ) {
    if ( size_out )
      *size_out = sz.tsize;
    return 0;
  }
  else {
    AVP_dword res = flush_put_bytes( &sz );
    if ( size_out )
      *size_out = sz.tsize;
    return res != 0;
  }
}


// ---
static AVP_dword DATA_PARAM DATA_Put( Serialize* sz, AVP_Data* data, AVP_bool next, AVP_dword* include_list, AVP_dword* exclude_list ) {
  if ( data ) {
    AVP_dword id;
    AVP_dword saved = sizeof(AVP_BYTE_Property);
    AVP_dword total = 0;
    AVP_bool  start = 1;
    AVP_bool  value = In_Process( AVP_PID_VALUE, include_list, exclude_list );
    AVP_byte  key_byte = ( value ? PUT_VAL_PROP : 0 ) | ( data->child ? PUT_CHILD_DATA : 0 ) | ( (next && data->next) ? PUT_NEXT_DATA : 0 );
    AVP_Linked_Property* prop;

    saved = put_bytes( sz, &key_byte, sizeof(key_byte) );
    if ( saved < sizeof(key_byte) )
      return 0;
    else
      total += saved;

    prop = &data->value;
    while( 1 ) {
      if ( !prop ) {
        _RPT0( _CRT_ASSERT, "Bad property list" );
        return 0;
      }
      id = PROP_ID(&prop->data);
      if ( id == AVP_PID_END ) {
		saved = put_avp_property (sz, (AVP_Property*) &id);
        if ( saved < sizeof(id) )
          return 0;
        else
          total += saved;
        break;
      }
      if ( start ) {
        if ( value ) {
          saved = PROP_Put( sz, &prop->data );
          if ( !saved )
            return 0;
        }
        else {
          saved = put_avp_property (sz, &prop->data);
          if ( saved < sizeof(AVP_Property) )
            return 0;
        }
        total += saved;
        start = 0;
      }
      else if ( In_Process(id,include_list,exclude_list) ) {
        saved = PROP_Put( sz, &prop->data );
        if ( saved )
          total += saved;
        else
          return 0;
      }
      prop = prop->next;
    }
    if ( data->child ) {
      saved = DATA_Put( sz, data->child, 1, include_list, exclude_list );
      if ( saved )
        total += saved;
      else
        return 0;
    }
    if ( next && data->next ) {
      AVP_byte  e = AVP_NEXT_END;
      AVP_Data* n = data->next;
      while( n ) {
        saved = DATA_Put( sz, n, 0, include_list, exclude_list );
        if ( saved )
          total += saved;
        else
          return 0;
        n = n->next;
      }
      saved = put_bytes( sz, &e, sizeof(e) );
      if ( saved == sizeof(AVP_byte) )
        total += saved;
      else
        return 0;
    }

    return total;
  }
  else {
    _RPT0( _CRT_ASSERT, "Data handle is zero" );
    return 0;
  }
}



static AVP_dword DATA_PARAM WritePropertyItem( Serialize* sz, AVP_byte type, void* val ) {
	AVP_dword size = 0;
	AVP_dword psize;
	AVP_word wsize;    
	AVP_byte tmpbyte;

	switch( type ) {
		case avpt_nothing : return sizeof(AVP_Property);

		// Byte-sized types
		case avpt_char    : 
		case avpt_byte    : 
		case avpt_group   : size = put_byte( sz, (AVP_byte*)val ); break;
		case avpt_bool    : 
			tmpbyte = *((AVP_bool*)val) ? 1 : 0;
			size = put_byte(sz, &tmpbyte);
			break;

		// Word-sized types            
		case avpt_wchar   :
		case avpt_short   :
		case avpt_word    : size = put_word( sz, (AVP_word *)val ); break;

		// Dword-sized types      
		case avpt_long    : 
		case avpt_dword   : 
		case avpt_int     : 
		case avpt_uint    : size = put_dword( sz, (AVP_dword *)val ); break;

		// QWord-sized types
		case avpt_qword   : 
		case avpt_longlong: size = put_qword( sz, (AVP_qword*)val ); break;

		// size_t-sized types
		case avpt_size_t  : size = put_size_t( sz, (AVP_size_t*)val ); break;

		// Custom structures
		case avpt_date    : size = put_date(sz, (AVP_date *)val); break;
		case avpt_time    : size = put_time(sz, (AVP_time *)val); break;
		case avpt_datetime: size = put_datetime(sz, (AVP_datetime *)val); break;

		// String
		case avpt_str     :
			if ( *((AVP_char**)val) ) {
				psize = (AVP_dword)strlen( *((AVP_char**)val) );
				wsize = (AVP_word)psize;
				if ( psize >= USHRT_MAX ) {
					_RPT0( _CRT_ASSERT, "String is too long" );
					return 0;
				}
				if ( sizeof(AVP_word) == put_word (sz, &wsize) && psize == put_bytes(sz,*((AVP_char**)val),psize) )
					size += sizeof(AVP_word) + psize;
				else
					size = 0;
			}
			else {
				AVP_word dummy = USHRT_MAX;
				if ( sizeof(AVP_word) == put_word (sz, &dummy) )
					size += sizeof(AVP_word);
				else
					size = 0;
			}
			break;

			// Windows unicode string
		case avpt_wstr :
			size = put_wchar_string(sz,*((wchar_t**) val));
			break;

		case avpt_bin :
			if ( ((AVP_Bin_Item *)val)->size > USHRT_MAX ) {
				_RPT0( _CRT_ASSERT, "Binary property is too large" );
				return 0;
			}
			wsize = (AVP_word)((AVP_Bin_Item *)val)->size;
			if ( sizeof(AVP_word) == put_word (sz, &wsize ) &&
				((AVP_Bin_Item *)val)->size == put_bytes( sz, ((AVP_Bin_Item *)val)->data, ((AVP_Bin_Item *)val)->size ) 
				) 
				size = sizeof(AVP_word) + ((AVP_Bin_Item *)val)->size;
			else
				size = 0;
			break;

		default :
			_RPT0( _CRT_ASSERT, "Bad property type" );
			return 0;
	}

	return size ? size + sizeof(AVP_Property) : 0;
}


// ---
static AVP_dword DATA_PARAM PROP_Put( Serialize* sz, AVP_Property* prop ) {
  if ( !prop ) {
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
    return 0;
  }
  if ( prop->array ) 
    return PROP_Arr_Put( sz, (AVP_Arr_Property*)prop );
  else {
	void * addr;
    if ( put_avp_property (sz,prop) != sizeof(AVP_Property) )
    	return 0;
    addr = DATA_Get_Property_Item_Addr (prop);
    return WritePropertyItem (sz, prop->type, addr);
  }
}


// ---
static AVP_dword DATA_PARAM PROP_Arr_Put( Serialize* sz, AVP_Arr_Property* prop ) {

  AVP_dword i, c;
  AVP_dword size;
  AVP_word wput;
  AVP_byte* s;
  AVP_byte  arr[10];

  if ( !prop || !prop->prop.array || prop->prop.type == avpt_nothing ) {
    _RPT0( _CRT_ASSERT, "Property is not an array" );
    return 0;
  }

  if ( prop->count > USHRT_MAX ) {
    _RPT0( _CRT_ASSERT, "Property array is too large" );
    return 0;
  }

  wput = (AVP_word)prop->count;
  
  if ( 
    sizeof( AVP_Property ) == put_avp_property (sz, &prop->prop) &&
    sizeof( AVP_word ) == put_word (sz, &wput) &&
    sizeof( prop->delta ) == put_word (sz, &prop->delta)
  ) 
    size = 
      sizeof( AVP_Property ) + 
      sizeof( AVP_word ) + 
      sizeof( prop->delta );
  else
    return 0;

  switch( prop->prop.type ) {
    case avpt_bool    :
      c = (prop->count / 8) + ( (prop->count % 8) > 0 );
      if ( c > sizeof(arr) )
        s = allocator( c );
      else
        s = arr;
      memset( s, 0, c );
      for( i=0; i<prop->count; i++ ) {
        if ( ((AVP_bool*)prop->val)[i] ) {
          AVP_dword byte = i / 8;
          AVP_dword bit = i % 8;
          s[byte] |= 1 << bit;
        }
      }
      if ( c == put_bytes(sz,s,c) )
        size += c;
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
    case avpt_str	  :
    case avpt_wstr	  :
    case avpt_bin	  :
      break;
      
    case avpt_nothing : 
    default :
      _RPT0( _CRT_ASSERT, "Bad array property type" );
      return size;
  }
  
  for ( i = 0; i < prop->count; i++ ) {
   	if ( (c = WritePropertyItem(sz,prop->prop.type,((char*)prop->val)+i*prop->isize)) == 0 ) {
  		size = 0;
  		break;
  	}
  	size += c;
  }
  
  return size;
}


// ---
static AVP_bool DATA_PARAM In_Process( AVP_dword id, AVP_dword* include_list, AVP_dword* exclude_list ) {
  AVP_dword* list;
  AVP_bool in_process = 1;
  if ( include_list ) {
    list = include_list;
    while( *list && PROP_ID(list) != id )
      list++;
    if ( !*list )
      in_process = 0;
  }
  if ( in_process && exclude_list ) {
    list = exclude_list;
    while( *list && PROP_ID(list) != id )
      list++;
    if ( *list )
      in_process = 0;
  }
  return in_process;
}



// ---
static void DATA_PARAM init_put_bytes( Serialize* sz, void* buffer, AVP_dword size, Buffer_Proc fproc, void* user_data ) {
  sz->buffer					=	(AVP_byte*)buffer;
  sz->rsize						= size;
  sz->bsize						= size;
  sz->tsize						= 0;
  sz->csize						= 0;
  sz->crc							= -1;
  sz->crc_len					= 0;
  sz->crc_cluster			= CRC_CLUSTER;
  sz->crc_not_in_use  = 1;
	sz->buff_loaded			= 0;
  sz->proc						= fproc;
  sz->data						= user_data;
}


// ---
static AVP_dword DATA_PARAM flush_put_bytes( Serialize* sz ) {
  AVP_dword crc = sz->crc;

  if ( !sz->crc_cluster || put_dword (sz, &crc) == sizeof(sz->crc) ) {
    if ( sz->proc ) {
      AVP_dword out_size = 0;
      if ( sz->proc(sz->buffer,sz->csize,&out_size,sz->data) && sz->csize == out_size )
        return sz->tsize;
      else
        return 0;
    }
    else
      return  sz->tsize;
  }
  else
    return 0;
}                 
                  
                  
// ---
static AVP_dword DATA_PARAM put_bytes( Serialize* sz, void* val, AVP_dword size ) {

  AVP_dword saved = 0;

  if ( !size )
    return 0;

  if ( sz->crc_cluster && sz->crc_len + size > sz->crc_cluster ) {
    AVP_dword crc;
    AVP_dword r_size = sz->crc_cluster - sz->crc_len;
    AVP_dword s_size = r_size ? put_bytes( sz, val, r_size ) : 0;
    if ( s_size == r_size ) {
      crc = sz->crc;
      sz->crc_len = 0;
      if ( put_dword (sz, &crc) == sizeof(sz->crc) ) {
        sz->crc = -1;
        sz->crc_len = 0;
        r_size = size - r_size;
        saved = s_size + ( r_size ? put_bytes(sz,((AVP_byte*)val)+s_size,r_size) : 0 );
      }
    }
  }
  else {
    if ( sz->crc_cluster ) {
      if ( sz->buffer )
        sz->crc = CountCRC32WithCRC( size, (AVP_byte*)val, sz->crc );
      sz->crc_len += size;
    }

    while( sz->bsize < sz->csize + size ) {
      AVP_dword out_size = 0;
      AVP_dword psize = sz->bsize - sz->csize;
      if ( psize && sz->buffer ) 
        memcpy( sz->buffer+sz->csize, val, psize );

      if ( sz->proc && sz->proc(sz->buffer,sz->bsize,&out_size,sz->data) ) {
        if ( sz->bsize > out_size )
          return 0;
        if ( psize ) {
          *((AVP_byte**)&val) += psize;
          size -= psize;
          saved += psize;
          sz->tsize += psize;
        }
      }
      else 
        return 0;

      sz->csize = 0;
    }

    if ( size ) {
      if ( sz->buffer )
        memcpy( sz->buffer+sz->csize, val, size );
      sz->csize += size;
      sz->tsize += size;
      saved += size;
    }

  }
  return saved;
}



