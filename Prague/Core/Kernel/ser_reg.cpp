#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_stream.h>
#include "ser.h"

#define IMPEX_IMPORT_LIB
#include <Prague/plugin/p_string.h>


// ---
#define VCONTENT_NAME       "vcontent"
#define VCONTENT_NAME_SIZE  (sizeof(VCONTENT_NAME)-sizeof(char))

#if !defined(VCONTENT_MAX_SIZE) 
	#define VCONTENT_MAX_SIZE   16300
#endif

typedef cBuff<tCHAR,100> cStrLocalBuff;


// ---
inline const cAutoRegKey& _check_sub_key( cERROR& err, const tCHAR* name, cRegistry* reg, const cAutoRegKey& base, cAutoRegKey& reserv ) {
	if ( !name || !*name )
		return base;
	err = reserv.open( reg, base, name, cTRUE );
	return reserv;
}



// ---
tERROR pr_call _regSerializeIntegrated( tPTR obj, cRegistry* reg, const cAutoRegKey& key, const tCHAR* val_name, bool& key_name_used, const cSerDescriptorField* field, tSerializeCallback func, tPTR params ) {
	tBOOL  bool_val;
	tPTR   val;
	tDWORD val_size;

	if ( NULL == val_name ) // ignore unnamed fields
		return errOK;
	
	switch( field->m_id ) {
		case tid_STRING  :
		case tid_WSTRING :
			return reg->SetValue( key, val_name, (tTYPE_ID)field->m_id, *(tPTR*)obj, 0, cTRUE );

		case tid_STRING_OBJ :
			if ( !key_name_used && (field->m_flags & SDT_FLAG_KEY_NAME) ) {// already set
				key_name_used = true;
				return errOK;
			}
			return ((cStringObj*)obj)->copy( reg, key, val_name, cTRUE );

		case tid_BUFFER :
			return reg->SetValue( key, val_name, tid_BINARY, (void*)((cMemChunk*)obj)->ptr(), ((cMemChunk*)obj)->used(), cTRUE );

		case tid_BINARY :
			return reg->SetValue( key, val_name, tid_BINARY, obj, field->m_size, cTRUE );

		default:
			val_size = pr_sizeometer( field->m_id, 0 );
			if ( (field->m_id == tid_BOOL) && (val_size == sizeof(bool)) ) {
				bool_val = *(bool*)obj;
				val = &bool_val;
				val_size = sizeof(bool_val);
			}
			else if ( -1 == val_size )
				return errBAD_SIZE;
			else
				val = obj;
			return reg->SetValue( key, val_name, (tTYPE_ID)field->m_id, val, val_size, cTRUE );
	}
}


// ---
static tERROR _make_vcontent_name( const cSerDescriptorField* field, cRegistry* reg, cStrLocalBuff& name, tUINT ind ) {
	tCHAR* ptr;
	if (field->m_name && (*field->m_name != 0) ) {
		tUINT len = (tUINT)strlen( field->m_name );
		if ( name.size() >= (len + VCONTENT_NAME_SIZE + (ind ? 9 : 0) + 1) )
			ptr = name;
		else {
			ptr = name.get( len + VCONTENT_NAME_SIZE + (ind ? 9 : 0) + 1, false );
			if ( !ptr )
				return errNOT_ENOUGH_MEMORY;
		}
		memcpy( ptr, field->m_name, len );
		*(ptr+len) = '_';
		ptr += len + 1;
	}
	else
		ptr = name;
	memcpy( ptr, VCONTENT_NAME, VCONTENT_NAME_SIZE );
	if ( ind > 0 )
		pr_sprintf( ptr+VCONTENT_NAME_SIZE, 9, "_%04x", ind );
	return errOK;
}



// ---
tERROR pr_call _regSerializeOneField( tPTR obj, hREGISTRY reg, const cAutoRegKey& key, const cSerDescriptorField* field, const cSerDescriptorField* key_field, bool& key_name_used, tDWORD flags, tSerializeCallback func, tPTR params ) {
	
	cERROR err;
	tDWORD ser_id;
	cAutoRegKey sub_key;
	const tCHAR* name;

	//obj = SADDR( obj, field );

	if ( func ) {
		err = func( params, sa_serialize_field, obj, field, 0 );
		if ( PR_FAIL(err) )
			return err;
		if ( err == warnFALSE )
			return errOK;
	}

	name = field->m_name;
	if ( !(flags & CALLER_IS_COOL_SERIALIZER) && field->m_serializer ) {
		err = field->m_serializer->RegSerialize( obj, field, reg, name );
		if ( err != errNOT_IMPLEMENTED )
			return err;
		err = errOK;
	}
	
	if ( field->m_flags & SDT_FLAG_VSTREAM ) {
		tDWORD max_value_size;
		cOutStreamAlloc stream( (cObj*)g_root, 0x200 );
		err = _streamSerializeOneField( field, obj, stream, flags, func, params );
		if ( PR_SUCC(err) ) {
			max_value_size = reg->get_pgMAX_VALUE_SIZE();
			if ( !max_value_size || (-1 == max_value_size) )
				max_value_size = VCONTENT_MAX_SIZE;
		}
		if ( PR_SUCC(err) ) {
			cStrLocalBuff vname( reg );
			tBYTE* bptr = (tBYTE*)stream.ptr();
			for( tUINT i=0, wsize=stream.size(); PR_SUCC(err) && (!i || wsize); ++i ) {
				err = _make_vcontent_name( field, reg, vname, i );
				if ( PR_SUCC(err) ) {
					tUINT csize = (wsize > max_value_size) ? max_value_size : wsize;
					err = reg->SetValue( key, vname, tid_BINARY, bptr, csize, cTRUE );
					if ( PR_SUCC(err) ) {
						wsize -= csize;
						bptr += csize;
					}
				}
			}
		}
		return err;
	}

	if ( field->m_flags & SDT_FLAG_VECTOR ) {
		tUINT  vect_el_size;
		tUINT  i, offs;
		tCHAR  tmp_name[20];
		tPTR   ptr;
		const cSerDescriptor* dsc;
		const cSerDescriptorField* old_key_field;
		
		if ( name && *name )
			reg->DeleteKey( key, name );

		tMemChunk& v = *(tMemChunk*)obj;
//		if ( !VPTR(v) || !v.m_used )
//			return errOK;

		const cAutoRegKey& my_key = _check_sub_key( err, name, reg, key, sub_key );
		if ( PR_FAIL(err) )
			return err;
		
		cStrBuff tmp_buff;
		if ( field->m_id < tid_LAST_TYPE ) { // integrated
			if ( field->m_flags & SDT_FLAG_POINTER )
				vect_el_size = sizeof(tPTR);
			else if ( -1 == (vect_el_size=pr_sizeometer(field->m_id,0)) )
				return errBAD_SIGNATURE;
			
			for( i=offs=0;   ;  ++i,offs+=vect_el_size ) {
				pr_sprintf( tmp_name, sizeof(tmp_name), "%04u", i );
				if ( offs < v.m_used ) {
					ptr = VPTR(v) + offs;
					_check_integrated_ptr( ptr, field, false );
					err = _regSerializeIntegrated( ptr, reg, my_key, tmp_name, key_name_used, field, func, params );
					if ( _check_err(err) )
						break;
					continue;
				}
				err = reg->DeleteValue( my_key, tmp_name );
				if ( _check_err(err) )
					break;
			}
		}
		else {
			if ( field->m_flags & SDT_FLAG_POINTER ) {
				dsc = cRegistrar::find( field->m_id );
				vect_el_size = sizeof( tPTR );
			}
			else if ( 0 == (dsc=cRegistrar::find(field->m_id)) )
				return errBAD_SIGNATURE;
			else
				vect_el_size = dsc->m_size;
			
			old_key_field = key_field;
			if ( dsc )
				key_field = _find_key_field( dsc->m_fields );
			else
				key_field = 0;
			for( i=offs=0;   offs < v.m_used;  ++i,offs+=vect_el_size ) {
				const tCHAR* sub_name;
				
				ptr = VPTR(v) + offs;
				
				if ( key_field ) {
					tPTR p = field->m_flags & SDT_FLAG_POINTER ? (*(tPTR*)ptr) : ptr;
					cStrObj& str = *(cStrObj*)SADDR(p,key_field);
					tmp_buff.init( str );
					sub_name = tmp_buff;
					if ( !*sub_name )
						continue;
				}
				else {
					sub_name = tmp_name;
					pr_sprintf( tmp_name, sizeof(tmp_name), "%04u", i );
				}
				
				cAutoRegKey el_key;
				if ( PR_FAIL(err=el_key.open(reg,my_key,sub_name,cTRUE)) )
					break;
				if ( field->m_flags & SDT_FLAG_POINTER ) {
					tTYPE_ID type = tid_DWORD;
					ser_id = (*((cSerializable**)ptr))->getIID();
					if ( ser_id == cSerializable::eIID )
						continue;
					dsc = cRegistrar::find( ser_id );
					if ( !dsc )
						continue;
					reg->SetValue( el_key, "unique", type, &ser_id, sizeof(ser_id), cTRUE );
					*((cSerializable**)&ptr) = *((cSerializable**)ptr);
				}
				
				const cSerDescriptorField* sub_field;
				for( sub_field=dsc->m_fields; PR_SUCC(err) && (sub_field->m_flags != SDT_FLAG_ENDPOINT); ++sub_field )
					err = _regSerializeOneField( SADDR(ptr,sub_field), reg, el_key, sub_field, key_field, key_name_used, 0, func, params );
				el_key.close();

				if ( PR_FAIL(err) ) {
					reg->DeleteKey( my_key, sub_name );
					break;
				}
			}
			key_field = old_key_field;
		}
		return err;
	}
	
	// now we know - it's not a vector!!!
	_check_integrated_ptr( obj, field, false );
	
	if ( field->m_id < tid_LAST_TYPE ) // integated
		return _regSerializeIntegrated( obj, reg, key, name, key_name_used, field, func, params );
			
	bool pointer;
	if ( field->m_flags & SDT_FLAG_POINTER ) {
		if ( !field->m_name || !*field->m_name )
			return errBAD_SIGNATURE;
		if ( !obj )
			return errOK;
		pointer = true;
		ser_id = ((cSerializable*)obj)->getIID();
	}
	else {
		pointer = false;
		ser_id = field->m_id;
	}

	if ( 0 != (field=_go(ser_id)) ) {
		cAutoRegKey str_key;
		const cAutoRegKey& my_key = _check_sub_key( err, name, reg, key, sub_key );
		if ( PR_SUCC(err) ) {
			const cAutoRegKey& work = _check_key_field( err, reg, my_key, field, key_field, obj, str_key );
			if ( pointer )
				reg->SetValue( work, "unique", tid_TYPE_ID, &ser_id, sizeof(ser_id), cTRUE );
			for( ; PR_SUCC(err) && (field->m_flags != SDT_FLAG_ENDPOINT); ++field )
				err = _regSerializeOneField( SADDR(obj,field), reg, work, field, key_field, key_name_used, 0, func, params );
		}
		return err;
	}
	return errBAD_SIGNATURE;
}




// ---
inline tPTR prepare_val( tUINT& el_size, tPTR& src, tDWORD& spare_val, tDWORD& cur_size ) {
	if ( el_size < sizeof(tDWORD) ) {
		cur_size = sizeof(tDWORD);
		return &spare_val;
	}
	cur_size = el_size;
	return src;
}




// ---
tERROR pr_call _regDeserializeIntegrated( tPTR obj, const cSerDescriptorField* field, const tCHAR* key_name, bool& key_name_used, cRegistry* reg, const cAutoRegKey& key, const tCHAR* val_name, tSerializeCallback func, tPTR params, tBOOL* really_read ) {

	tPTR       val;
	tTYPE_ID   val_type;
	tDWORD     val_size;
	tDWORD     spare_val;
	tCODEPAGE  dst_cp;
	cMemChunk* chunk;
	cERROR     err( _check_integrated_ptr(obj,field,true) );

	
	if ( NULL == val_name ) // ignore unnamed fields
		return errOK;
	
	if ( PR_FAIL(err) )
		return err;

	dst_cp = cCP_UNICODE;
	switch( field->m_id ) {
		case tid_STRING :
			dst_cp = cCP_ANSI;
		case tid_WSTRING :
			if ( *(tPTR*)obj ) {
				::g_root->heapFree( *(tPTR*)obj );
				*((tPTR*)obj) = 0;
			}
			if ( !key_name_used && (field->m_flags & SDT_FLAG_KEY_NAME) ) {
				key_name_used = true;
				if ( !CopyTo ) {
					err = errSTRING_LEN_NOT_SPECIFIED;
					break;
				}
				if ( PR_FAIL(err=CopyTo(0,0,dst_cp,(tPTR)key_name,0,cCP_ANSI,cSTRING_Z,&val_size)) )
					break;
				if ( PR_FAIL(err=::g_root->heapAlloc(&val,val_size)) )
					break;
				if ( PR_FAIL(err=CopyTo(val,val_size,dst_cp,(tPTR)key_name,0,cCP_ANSI,cSTRING_Z,&val_size)) )
					break;
			}
			else {
				val_type = (tTYPE_ID)field->m_id;
				if ( PR_FAIL(err=reg->GetValue(&val_size,key,val_name,&val_type,0,0)) )
					break;
				if ( PR_FAIL(err=::g_root->heapAlloc(&val,val_size)) )
					break;
				val_type = (tTYPE_ID)field->m_id;
				if ( PR_FAIL(err=reg->GetValue(&val_size,key,val_name,&val_type,val,val_size)) ) {
					::g_root->heapFree( val );
					break;
				}
			}
			
			if ( PR_SUCC(err) ) {
				if ( really_read )
					*really_read = cTRUE;
				*(tPTR*)obj = val;
			}
			break;
			
		case tid_STRING_OBJ :
			if ( !key_name_used && (field->m_flags & SDT_FLAG_KEY_NAME) ) {
				key_name_used = true;
				const tCHAR* l_key = key_name;
				while ( l_key && *l_key++ )
					;
				while ( l_key && ( l_key != key_name ) && ( *(l_key-1) != '\\' ) && ( *(l_key-1) != '/' ) ) 
					--l_key;
				err = ((cStrObj*)obj)->assign( l_key, cCP_ANSI );
			}
			else
				err = ((cStringObj*)obj)->assign( reg, key, val_name );
			if ( PR_SUCC(err) && really_read )
				*really_read = cTRUE;
			break;
			
		case tid_BUFFER :
			chunk = (cMemChunk*)obj;
			val_type = tid_BINARY;
			err = reg->GetValue( &val_size, key, val_name, &val_type, (tVOID*)chunk->ptr(), chunk->allocated() );
			if ( PR_FAIL(err) && (err != errBUFFER_TOO_SMALL) )
				return err;
			if ( val_type != tid_BINARY )
				return errBAD_SIGNATURE;
			if ( err == errBUFFER_TOO_SMALL ) {
				if ( !chunk->reserve(val_size,false) )
					return errNOT_ENOUGH_MEMORY;
				val_type = tid_BINARY;
				err = reg->GetValue( &val_size, key, val_name, &val_type, (tVOID*)chunk->ptr(), chunk->allocated() );
			}
			if ( PR_SUCC(err) && really_read )
				*really_read = cTRUE;
			return err;
			
		case tid_BINARY :
			val_type = tid_BINARY;
			err = reg->GetValue( &val_size, key, val_name, &val_type, obj, field->m_size );
			if ( PR_SUCC(err) && really_read )
				*really_read = cTRUE;
			return err;

		default:
			val_type = (tTYPE_ID)field->m_id;
			val_size = pr_sizeometer( field->m_id, 0 );
			if ( -1 == val_size )
				err = errBAD_SIGNATURE;
			if ( PR_SUCC(err) ) {
				tUINT cur_size = sizeof(tDWORD);
				tPTR  cur_ptr  = prepare_val( val_size, obj, spare_val, cur_size );
				err = reg->GetValue( 0, key, val_name, &val_type, cur_ptr, cur_size );
			}
			if ( PR_SUCC(err) && (val_size < sizeof(tDWORD)) ) {
				if ( field->m_id == tid_BOOL )
					*(bool*)obj = !!spare_val;
				else if ( val_size == sizeof(tBYTE) )
					*(tBYTE*)obj = (tBYTE)spare_val;
				else if ( val_size == sizeof(tWORD) )
					*(tWORD*)obj = (tWORD)spare_val;
			}
			if ( PR_SUCC(err) && really_read )
				*really_read = cTRUE;
			break;
	}
	return _check_err_ptr( err, obj, field );
}



// ---
tERROR pr_call _regDeserializeOneField( tPTR obj, hREGISTRY reg, const cAutoRegKey& key, const tCHAR* key_name, bool& key_name_used, const tCHAR* val_name, const cSerDescriptorField* field, tDWORD flags, tSerializeCallback func, tPTR params, tBOOL& really_read ) {
	
	cERROR err;
	const cAutoRegKey* sk;
	const cSerDescriptor* dsc;
	const cSerDescriptorField* sub_field;
	cSerializable* ser;

	if ( func ) {
		err = func( params, sa_deserialize_field, obj, field, 0 );
		if ( PR_FAIL(err) )
			return err;
		if ( err == warnFALSE ) {
			really_read = cTRUE;
			return errOK;
		}
	}
	
	if ( !(flags & CALLER_IS_COOL_SERIALIZER) && field->m_serializer ) {
		err = field->m_serializer->RegDeserialize( obj, field, reg, key_name );
		if ( err != errNOT_IMPLEMENTED ) {
			if ( !(field->m_flags & SDT_FLAG_VECTOR) && IS_SERIALIZABLE(field->m_id) )
				static_cast<cSerializable*>( obj )->finalizeRegDeser( err, -1, 0 );
			if ( PR_SUCC(err) )
				really_read = cTRUE;
			return err;
		}
		err = errOK;
	}

	if ( field->m_flags & SDT_FLAG_VSTREAM ) {
		tDWORD res;
		tBOOL do_not_try_ordinary = cTRUE;
		tTYPE_ID type = tid_BINARY;
		cStrLocalBuff vname( reg );
		cBuff<tBYTE,0x200> buff;

		for( tUINT i=0; PR_SUCC(err); ++i ) {
			err = _make_vcontent_name( field, reg, vname, i );
			if ( PR_FAIL(err) )
				break;
			err = reg->GetValue( &res, key, vname, &type, 0, 0 );
			if ( (err == errKEY_NOT_FOUND) || (err == errNOT_FOUND) || (err == errOBJECT_NOT_FOUND) ) {
				if ( i )
					err = errOK;
				else
					do_not_try_ordinary = cFALSE;
				break;
			}
			else if ( PR_FAIL(err) )
				break;
			tBYTE* ptr = buff.add( res );
			if ( !ptr ) {
				err = errNOT_ENOUGH_MEMORY;
				break;
			}
			type = tid_BINARY;
			err = reg->GetValue( &res, key, vname, &type, ptr, res );
		}
		if ( PR_SUCC(err) ) {
			cInStream stream( buff, buff.used() );
			dsc = 0;
			if ( PR_SUCC(err) ) {
				tUINT end_offset = buff.used();
				err = _streamDeserializeOneField( true, dsc, field, obj, stream, end_offset, flags, func, params );
			}
		}
		if ( PR_SUCC(err) )
			really_read = cTRUE;
		if ( PR_SUCC(err) || do_not_try_ordinary )
			return err;
	}

	if ( field->m_flags & SDT_FLAG_VECTOR ) {
		tPTR   ptr;
		tDWORD size = 0;
		tUINT  count, vect_el_size;
		cAutoRegKey vect_key;
		const cSerDescriptorField* key_field;
		tCleanFunc clean_func;
		tMemChunk& v = *(tMemChunk*)obj;

		err = errOK;
		if ( !field->m_name || !*field->m_name ) 
			sk = &key;
		else {
			err = vect_key.open( reg, key, field->m_name, cFALSE );
			if ( _check_err(err) )
				return err;
			sk = &vect_key;
		}
		really_read = cTRUE;
		_clean_vector( v, field );
		
		tChunckBuff name_buff;

		// vector of integrated
		if ( field->m_id < tid_LAST_TYPE ) {
			vect_el_size = pr_sizeometer( field->m_id, &clean_func );
			if ( -1 == vect_el_size )
				return errBAD_SIGNATURE;
			if ( field->m_flags & SDT_FLAG_POINTER )
				vect_el_size = sizeof(tPTR);

			// this code works, but has an issue - if vector values are mixed
			// with non-vector values under the same registry key, - there's no way to
			// tell vector values from non-vector ones.

			if( PR_FAIL(reg->GetValueCount(&count,*sk)) )
				count = 0;
			
			if( PR_SUCC(err = _alloc_vect_mem(v, count,vect_el_size)) ) {
				for (tUINT idx = 0; idx < count; ++idx ) {
					size = 0;
					if ( field->m_flags & SDT_FLAG_KEY_NAME ) {
						err = reg->GetValueNameByIndex( &size, *sk, idx, name_buff, name_buff.size() );
						if ( PR_SUCC(err) )
							;
						else if ( (err==errKEY_NOT_FOUND) || (err==errKEY_INDEX_OUT_OF_RANGE) ) {
							err = errOK;
							break;
						}
						else if ( err == errBUFFER_TOO_SMALL )
							err = reg->GetValueNameByIndex( &size, *sk, idx, name_buff.get(size,false), size );
						if ( PR_FAIL(err) )
							break;
					}
					else
						pr_sprintf( name_buff, name_buff.size(), "%04u", idx );
					
					ptr = VPTR(v) + v.m_used;
					
					err = _regDeserializeIntegrated( ptr, field, key_name, key_name_used, reg, *sk, name_buff, func, params, 0 );
					
					if( _check_err(err) )
						break;
					
					v.m_used += vect_el_size;
				}
			} // if ( PR_SUCC(err=_alloc_vect_mem(v, count,vect_el_size)) )
			_empty_vector( v, 0, 0 );
			return err;
		} // if ( field->m_id < tid_LAST_TYPE )
		
		// vector of serialisable
		if ( field->m_flags & SDT_FLAG_POINTER )
			dsc = cRegistrar::find( field->m_id );
		else if ( 0 == (dsc=cRegistrar::find(field->m_id)) )
			return errBAD_SIGNATURE;

		if ( dsc )
			key_field = _find_key_field( dsc->m_fields );
		else
			key_field = 0;
		
		vect_el_size = (field->m_flags & SDT_FLAG_POINTER) ? sizeof(tPTR) : dsc->m_size;

		if( PR_FAIL(reg->GetKeyCount(&count,*sk)) )
			count = 0;

		if ( PR_SUCC(err=_alloc_vect_mem(v,count,vect_el_size)) ) {
			for (tUINT idx = 0; idx < count; idx++) {
				if ( key_field ) {
					size = 0;
					err = reg->GetKeyNameByIndex( &size, *sk, idx, name_buff, name_buff.size(), cFALSE );
					if ( PR_SUCC(err) )
						;
					else if ( (err==errKEY_NOT_FOUND) || (err==errKEY_INDEX_OUT_OF_RANGE) ) {
						err = errOK;
						break;
					}
					else if ( err == errBUFFER_TOO_SMALL )
						err = reg->GetKeyNameByIndex( &size, *sk, idx, name_buff.get(size,false), size, cFALSE );
					if ( PR_FAIL(err) )
						break;
				}
				else
					pr_sprintf( name_buff, name_buff.size(), "%04u", idx );
				
				cAutoRegKey sub_key;
				err = sub_key.open( reg, *sk, name_buff/*tmp_name*/, cFALSE );
				if( _check_err(err) )
					break;

				ptr = (cSerializable*)(VPTR(v) + v.m_used);
				tPTR addr = 0;
				if ( field->m_flags & SDT_FLAG_POINTER ) {
					tTYPE_ID type = tid_DWORD;
					tDWORD ser_id = cSerializable::eIID;
					if( PR_FAIL(reg->GetValue(0, sub_key, "unique", &type, &ser_id, sizeof(ser_id))) )
						ser_id = field->m_id;
					if( ser_id == cSerializable::eIID )
						continue;
					if ( (dsc=cRegistrar::find(ser_id)) == 0 ) {
						err = errBAD_SIGNATURE;
						break;
					}
					addr = ptr;
					if( PR_FAIL(err = _alloc_str(&dsc,(cSerializable**)addr,field,func,params)) )
						break;
					ptr = *(tVOID**)addr;
				}
				else
					dsc->m_constructor((cSerializable*)ptr, 0);

				bool l_key_name_used = false;
				tUINT field_num = 0;
				for( const cSerDescriptorField* sub_field = dsc->m_fields; PR_SUCC(err) && (sub_field->m_flags != SDT_FLAG_ENDPOINT); ++sub_field ) {
					tBOOL read = cFALSE;
					err = _regDeserializeOneField( SADDR(ptr,sub_field), reg, sub_key, name_buff, l_key_name_used, sub_field->m_name, sub_field, 0, func, params, read );
					_check_err( err );
					if ( PR_SUCC(err) && read )
						++field_num;
				}

				if ( addr && *(tVOID**)addr ) {
					_check_err_ptr( err, *(tVOID**)addr, field );
					ptr = *(tVOID**)addr;
				}

				ser = static_cast<cSerializable*>( ptr );
				if ( ser )
					ser->finalizeRegDeser( err, field_num, dsc );
				
				if ( _check_err(err) ) {
					if( !(field->m_flags & SDT_FLAG_POINTER) )
						dsc->m_destructor((cSerializable *)ptr);
					break;
				}
				
				v.m_used += vect_el_size;
			} // for (tUINT idx = 0; idx < count; idx++) 
		} // if ( PR_SUCC(err=_alloc_vect_mem(v,count,vect_el_size)) )
		if ( field->m_flags & SDT_FLAG_POINTER )
			_empty_vector( v, 0, sizeof(tPTR) );
		else
			_empty_vector( v, dsc, dsc->m_size );
		return err;
	} // if ( field->m_flags & SDT_FLAG_VECTOR ) 

	// now we know - it's not a vector!!!
	if ( field->m_id < tid_LAST_TYPE ) // integrated
		return _regDeserializeIntegrated( obj, field, key_name, key_name_used, reg, key, val_name, func, params, &really_read );

	cAutoRegKey sub_key;
	if ( !val_name || !*val_name ) {
		if ( field->m_flags & SDT_FLAG_POINTER )
			return errBAD_SIGNATURE;
		sk = &key;
	}
	else if ( PR_FAIL(err=sub_key.open(reg,key,val_name,cFALSE)) )
		return err;
	else
		sk = &sub_key;

	really_read = cTRUE;

	tDWORD ser_id;
	if ( field->m_flags & SDT_FLAG_POINTER ) {
		tTYPE_ID type = tid_DWORD;
		if ( PR_SUCC(reg->GetValue(0,*sk,"unique",&type,&ser_id,sizeof(ser_id))) )
			;
		else if ( cSerializable::eIID != field->m_id )
			ser_id = field->m_id;
		else
			return errOK;
	}
	else
		ser_id = field->m_id;
	
	if ( ser_id == cSerializable::eIID )
		return errOK;

	dsc = cRegistrar::find(ser_id);
	if ( !dsc )
		return errBAD_SIGNATURE;

	ser = 0;
	if ( !(field->m_flags & SDT_FLAG_POINTER) )
		ser = (cSerializable*)obj;
	else {
		ser = *(cSerializable**)obj;
		if( ser && !ser->isBasedOn(ser_id) )
			_free_str( dsc, ser ), ser = NULL;

		if( !ser )
			err=_alloc_str(&dsc,&ser,field,func,params);
	}

	tUINT field_num = 0;
	for( sub_field=dsc->m_fields; PR_SUCC(err) && (sub_field->m_flags != SDT_FLAG_ENDPOINT); ++sub_field ) {
		tBOOL read = cFALSE;
		err = _regDeserializeOneField( SADDR(ser,sub_field), reg, *sk, (val_name && *val_name) ? val_name : key_name, key_name_used, sub_field->m_name, sub_field, 0, func, params, read );
		_check_err( err );
		if ( PR_SUCC(err) && read )
			++field_num;
	}

	if ( field->m_flags & SDT_FLAG_POINTER ) {
		if ( PR_FAIL(err) && ser ) {
			_free_str( dsc, ser );
			ser = 0;
		}

		*(cSerializable**)obj = ser;
	}
	if ( ser )
		ser->finalizeRegDeser( err, field_num, dsc );
	return err;
}



