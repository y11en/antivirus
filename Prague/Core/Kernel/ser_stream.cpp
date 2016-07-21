#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_stream.h>
#include <Prague/iface/i_buffer.h>
#include <Prague/iface/i_io.h>

#include "ser.h"



// ---
tERROR push_string( cString* str, cOutStream& out, const cSerDescriptorField* field ) {

	if ( !str )
		return out.push( tCODEPAGE(0) );

	tDWORD size;
	cERROR err = str->Length( &size );
	if ( PR_FAIL(err) )
		return err;

	tCODEPAGE cp = cCP_UNICODE;
	if ( PR_FAIL(err=str->GetCP(&cp)) )
		return err;

	cStrBuff buff( str, cp );
	err = out.push( cp );
	if ( PR_SUCC(err) ) {
		if ( cp == cCP_UNICODE )
			err = out.push_str( (tWCHAR*)buff, buff.used() );
		else
			err = out.push_str( (tCHAR*)buff, buff.used() );
	}
	return err;
}




// ---
tERROR push_string( const cStringObj* str, cOutStream& out, const cSerDescriptorField* field ) {
	if ( !str )
		return out.push( tCODEPAGE(0) );
	tCODEPAGE cp = str->CP();
	cStrBuff buff( str, cp );
	cERROR err = out.push( cp );
	if ( PR_SUCC(err) ) {
		if ( cp == cCP_UNICODE )
			err = out.push_str( (tWCHAR*)buff, buff.used() );
		else
			err = out.push_str( (tCHAR*)buff, buff.used() );
	}
	return err;
}




// ---
tERROR push_io( cIO* io, cOutStream& out, const cSerDescriptorField* field ) {
	tQWORD size;
	cERROR err = io->propGet( 0, pgOBJECT_SIZE_Q, &size, sizeof(size) );
	if ( PR_FAIL(err) || PR_FAIL(err=(out<<size)) )
		return err;

	tChunckBuff buff;
	tQWORD      offs = 0;
	while ( size ) {
		tDWORD read = size > buff.size() ? buff.size() : (tDWORD)size;
		if ( PR_FAIL(err=io->SeekRead(&read,offs,buff,read)) || !read )
			break;
		if ( PR_FAIL(err=out.push_bytes(buff,read)) )
			break;
		size -= read;
		offs += read;
	}
	return err;
}




// ---
tERROR push_object( cObject* obj, cOutStream& out, const cSerDescriptorField* field ) {
	if ( !obj )
		return out << tDWORD(0);

	cERROR err = out << obj->propGetDWord( pgINTERFACE_ID );
	if ( PR_FAIL(err) )
		return err;

	err = ::g_root->sysCheckObject( obj, IID_BUFFER );
	if ( PR_FAIL(err) && (err != errINTERFACE_INCOMPATIBLE) )
		return err;
	
	if ( PR_SUCC(err) ) {
		tPTR     ptr;
		tDWORD   size;
		cBuffer* buff = (cBuffer*)obj;

		err = buff->Lock( &ptr );
		if ( PR_SUCC(err) )
			err = buff->propGet( 0, pgOBJECT_SIZE, &size, sizeof(size) );
		if ( PR_SUCC(err) )
			err = out << size;
		if ( PR_SUCC(err) )
			err = out.push_bytes( ptr, size );
		buff->Unlock();
		return err;
	}

	if ( PR_SUCC(::g_root->sysCheckObject(obj,IID_STRING)) )
		return push_string ((cString*)obj, out, field );

	if ( PR_SUCC(::g_root->sysCheckObject(obj,IID_IO)) )
		return push_io( (cIO*)obj, out, field );

	return errOBJECT_INCOMPATIBLE;
}




// ---
tERROR pr_call _streamSerializeIntegrated( const tVOID* obj, cOutStream& out, const cSerDescriptorField* field, tSerializeCallback func, tPTR params ) {
	tERROR err;
	tUINT  len;
	switch( field->m_id ) {
		case tid_STRING     : return out.push_str( (const tCHAR*)obj );
		case tid_WSTRING    : return out.push_str( (const tWCHAR*)obj );
		case tid_OBJECT     : return push_object( *(cObject**)obj, out, field );
		case tid_STRING_OBJ : return push_string( (const cStringObj*)obj, out, field );
		case tid_BINARY     : return out.push_bytes( obj, field->m_size );
		case tid_BUFFER     :
			len = ((cMemChunk*)obj)->used();
			err = out << len;
			if ( PR_SUCC(err) )
				err = out.push_bytes( ((cMemChunk*)obj)->ptr(), len );
			return err;

		case tid_UINT :
			return out.push( *(tUINT*)obj );

		case tid_DWORD :
			return out.push( *(tDWORD*)obj );

		case tid_QWORD :
			return out.push( *(tQWORD*)obj );

		default :
			if ( -1 == (len=pr_sizeometer(field->m_id,0)) )
				return errBAD_SIGNATURE;
			if ( len == sizeof(tDWORD) )
				return out.push( *(tDWORD*)obj );
			if ( len == sizeof(tQWORD) )
				return out.push( *(tQWORD*)obj );
			return out.push_bytes( obj, len );
	}
}



// ---
tERROR pr_call _streamSerializeStruct( const cSerDescriptor* dsc, const tVOID* obj, cOutStream& out, tSerializeCallback func, tPTR params ) {
	if ( !dsc )
		return errNOT_FOUND;
	tDWORD end_point = 0;
	tDWORD begin_point = out.offs();
	cERROR err = out.push_bytes( &end_point, sizeof(end_point) );
	for( const cSerDescriptorField* field = dsc->m_fields; PR_SUCC(err); ++field ) {
		if ( field->m_flags == SDT_FLAG_ENDPOINT )
			break;
		err = _streamSerializeOneField( field, QSADDR(obj,field), out, 0, func, params );
		if ( PR_FAIL(err) ) {
			PR_TRACE(( ::g_root, prtERROR, "krn\tStream serialize(%terr): \"%s::%s\" (Str::field)", (tERROR)err, dsc->m_name, field->m_name ));
			PR_RPT(( "krn\tStream serialize(%terr): \"%s::%s\" (Str::field)", (tERROR)err, dsc->m_name, field->m_name ));
		}
	}
	if ( PR_SUCC(err) ) {
		end_point = out.offs();
		err = out.overlap( begin_point, &end_point, sizeof(end_point) );
	}
	return err;
}



// ---
tERROR pr_call _streamSerializeOneField( const cSerDescriptorField* field, const tVOID* obj, cOutStream& out, tDWORD flags, tSerializeCallback func, tPTR params ) {

	cERROR err;
	tDWORD ser_id;
	tUINT  offs;
	tUINT  count;
	const cSerDescriptor* dsc;

	//obj = SADDR( obj, field );

	if ( !(flags & CALLER_IS_COOL_SERIALIZER) && field->m_serializer ) {
		err = field->m_serializer->StreamSerialize( obj, field, &out );
		if ( err != errNOT_IMPLEMENTED )
			return err;
		err = errOK;
	}

	if ( field->m_flags & SDT_FLAG_VECTOR ) { // it's a vector!!!
		tMemChunk& v = *(tMemChunk*)obj;

		if ( field->m_id < tid_LAST_TYPE ) { // integrated
			tUINT el_size;
			if ( field->m_flags & SDT_FLAG_POINTER )
				el_size = sizeof(tPTR);
			else if ( -1 == (el_size=pr_sizeometer(field->m_id,0)) )
				return errBAD_SIGNATURE;
			
			count = v.m_used / el_size;
			if ( PR_FAIL(err=(out<<count)) )
				return err;
			
			for( offs=0; PR_SUCC(err) && (offs<v.m_used) && (0<count); offs+=el_size, --count )
				err = _streamSerializeIntegrated( VPTR(v)+offs, out, field, func, params );
			return err;
		}
		
		// not integrated
		if ( field->m_flags & SDT_FLAG_POINTER ) {
			count = v.m_used / sizeof(tPTR);
			if ( PR_FAIL(err=(out<<count)) )
				return err;
			
			for( offs=0; PR_SUCC(err) && (offs<v.m_used) && (0<count); offs+=sizeof(tPTR), --count ) {
				tPTR addr = *(tVOID**)(VPTR(v)+offs);
				bool not_zero = !!addr;
				if ( PR_FAIL(err=(out<<not_zero)) )
					return err;
				if ( !not_zero )
					continue;
				tDWORD ser_id = ((cSerializable*)(addr))->getIID();
				if ( 0 == (dsc=cRegistrar::find(ser_id)) )
					return errBAD_SIGNATURE;
				if ( PR_FAIL(err=(out<<ser_id)) )
					return err;
				err = _streamSerializeStruct( dsc, addr, out, func, params );
			}
			return err;
		}
		
		if ( 0 == (dsc=cRegistrar::find(field->m_id)) )
			return errBAD_SIGNATURE;
		
		count = v.m_used / dsc->m_size;
		if ( PR_FAIL(err=(out<<count)) )
			return err;
		
		for( offs=0; PR_SUCC(err) && (offs<v.m_used) && (0<count); offs+=dsc->m_size, --count )
			err = _streamSerializeStruct( dsc, VPTR(v) + offs, out, func, params );
		return err;
	}

	if ( field->m_flags & SDT_FLAG_POINTER ) {
		obj = *(const tVOID**)obj;
		if ( PR_FAIL(err=(out<<((bool)!!obj))) )
			return err;
		if ( !obj )
			return errOK;
	}
	
	// it is not a vector
	if ( field->m_id < tid_LAST_TYPE ) // integrated
		return _streamSerializeIntegrated( obj, out, field, func, params );

	if ( field->m_flags & SDT_FLAG_POINTER ) {
		ser_id = ((cSerializable*)obj)->getIID();
		if ( PR_FAIL(err=(out<<ser_id)) )
			return err;
	}
	else
		ser_id = field->m_id;
	return _streamSerializeStruct( cRegistrar::find(ser_id), obj, out, func, params );
} // _streamSerializeOneField



// ---
tERROR pr_call _streamDeserializeIntegrated( tVOID* obj, cInStream& in, tUINT end_point, const cSerDescriptorField* field, tSerializeCallback func, tPTR params ) {

	cERROR err;
	tDWORD len;
	tCODEPAGE cp;
	tPTR        ptr;
	cStringObj* str;
	cMemChunk* mem;
	tWCHAR buff[0x100];
	
	if ( PR_FAIL(err=_check_integrated_ptr(obj,field,true)) )
		return err;

	switch( field->m_id ) {
		case tid_STRING  : 
		case tid_WSTRING : 
			if ( obj && *(tPTR*)obj ) {
				::g_root->heapFree( *(tPTR*)obj );
				*((tPTR*)obj) = 0;
			}
			if ( field->m_id == tid_STRING )
				err = in.pop_str( (tCHAR*)buff, sizeof(buff), end_point, &len );
			else
				err = in.pop_str( (tWCHAR*)buff, sizeof(buff), end_point, &len );
			if ( err == errBUFFER_TOO_SMALL ) {
				err = ::g_root->heapAlloc( (tPTR*)obj, len );
				if ( PR_SUCC(err) ) {
					if ( field->m_id == tid_STRING )
						err = in.pop_str( (tCHAR*)*(tPTR*)obj, len, end_point, 0 );
					else
						err = in.pop_str( (tWCHAR*)*(tPTR*)obj, len, end_point, 0 );
				}
			}
			else if ( PR_SUCC(err) ) {
				if ( len && PR_SUCC(err=::g_root->heapAlloc((tPTR*)obj,len)) )
					memcpy( *(tPTR*)obj, buff, len );
			}
			return err;
			
		case tid_STRING_OBJ :
			str = ((cStringObj*)obj);
			err = in.pop(cp,end_point);
			if ( PR_FAIL(err) ) {
				if ( str )
					str->erase();
				return err;
			}
			else if ( err == errEOF ) {
				if ( str )
					str->erase();
				return err; //errOUT_OF_SPACE;
			}
			else if ( !cp ) {
				if ( str )
					str->erase();
				return errUNEXPECTED;
			}

			ptr = buff;
			if ( cp == cCP_UNICODE )
				err = in.pop_str( (tWCHAR*)buff, sizeof(buff), end_point, &len );
			else
				err = in.pop_str( (tCHAR*)buff, sizeof(buff), end_point, &len );
			if ( err == errBUFFER_TOO_SMALL ) {
				err = ::g_root->heapAlloc( &ptr, len );
				if ( PR_SUCC(err) ) {
					if ( cp == cCP_UNICODE )
						err = in.pop_str( (tWCHAR*)ptr, len, end_point, 0 );
					else
						err = in.pop_str( (tCHAR*)ptr, len, end_point, 0 );
				}
			}
			if ( str ) {
				//str->setCP( cp );
				if ( PR_SUCC(err) ) {
					cERROR assigned = str->assign( ptr, cp );
					if ( PR_FAIL(assigned) )
						err = assigned;
				}
				else
					str->erase();
			}
			if ( ptr != buff )
				::g_root->heapFree( ptr );
			return err;

		case tid_BUFFER     :
			len = 0;
			ptr = 0;
			mem = ((cMemChunk*)obj);
			err = in.pop( len, end_point );
			//if ( err == errEOF )
			//	err = errOUT_OF_SPACE;
			if ( PR_SUCC(err) && mem && (0 == (ptr=mem->reserve(len,false))) )
				err = errNOT_ENOUGH_MEMORY;
			if ( PR_SUCC(err) )
				err = in.pop_bytes( ptr, len, end_point );
			//if ( err == errEOF )
			//	err = errOUT_OF_SPACE;
			return err;

		case tid_BINARY :
			return in.pop_bytes( obj, field->m_size, end_point );

		case tid_UINT :
			err = in.pop( *(tUINT*)obj, end_point );
			//if ( err == errEOF )
			//	err = errOUT_OF_SPACE;
			return err;

		case tid_DWORD :
			err = in.pop( *(tDWORD*)obj, end_point );
			//if ( err == errEOF )
			//	err = errOUT_OF_SPACE;
			return err;

		case tid_QWORD :
			err = in.pop( *(tQWORD*)obj, end_point );
			//if ( err == errEOF )
			//	err = errOUT_OF_SPACE;
			return err;

		default :
			if ( -1 == (len=pr_sizeometer(field->m_id,0)) )
				return errBAD_SIGNATURE;
			if ( len == sizeof(tDWORD) ) {
				err = in.pop( *(tDWORD*)obj, end_point );
				//if ( err == errEOF )
				//	err = errOUT_OF_SPACE;
				return err;
			}
			if ( len == sizeof(tQWORD) ) {
				err = in.pop( *(tQWORD*)obj, end_point );
				//if ( err == errEOF )
				//	err = errOUT_OF_SPACE;
				return err;
			}
			err = in.pop_bytes( obj, len, end_point );
			//if ( err == errEOF )
			//	err = errOUT_OF_SPACE;
			return err;
	}
}



// ---
tERROR pr_call _streamDeserializeOneField( bool found_mode, const cSerDescriptor*& dsc_to_find, const cSerDescriptorField* field, tVOID* obj, cInStream& in, tUINT end_point, tDWORD flags, tSerializeCallback func, tPTR params ) {
	
	cERROR err;
	tDWORD ser_id;
	tUINT  field_num;
	cSerializable* ser;

	if ( dsc_to_find && found_mode ) {
		if ( dsc_to_find->m_unique == field->m_id )
			dsc_to_find = 0; // !!! found acceptable
	}

	//if ( !dsc_to_find ) 
	//	obj = SADDR( obj, field ); // jump to object offset

	if ( !(flags & CALLER_IS_COOL_SERIALIZER) && field->m_serializer ) {
		cInStream tmp( in, end_point );
		err = field->m_serializer->StreamDeserialize( dsc_to_find ? 0 : obj, field, &tmp );
		if ( err != errNOT_IMPLEMENTED ) {
			if ( PR_SUCC(err) )
				in.advance( tmp.cp(), end_point );
			if ( !dsc_to_find && !(field->m_flags & SDT_FLAG_VECTOR) && IS_SERIALIZABLE(field->m_id) )
				static_cast<cSerializable*>( obj )->finalizeStreamDeser( err, -1, 0 );
			return err;
		}
		err = errOK;
	}

	if ( field->m_flags & SDT_FLAG_VECTOR )	{
		tUINT count;
		tUINT el_size;
		tPTR  addr;
		const cSerDescriptor* dsc;
		const cSerDescriptorField* sub_field;
		tCleanFunc clean_func;
		tMemChunk& v = *(tMemChunk*)obj;

		if ( !dsc_to_find )
			_clean_vector( v, field );

		// integrated
		if ( field->m_id < tid_LAST_TYPE ) { 
			el_size = pr_sizeometer(field->m_id, &clean_func);
			if ( -1 == el_size )
				return errBAD_SIGNATURE;
			if ( field->m_flags & SDT_FLAG_POINTER )
				el_size = sizeof(tPTR);

			if ( PR_FAIL(err=in.pop(count,end_point)) )
				count = 0;
			
			if ( dsc_to_find || PR_SUCC(err=_alloc_vect_mem(v,count,el_size)) ) {
				for( ; count; --count ) {
					if ( dsc_to_find )
						addr = 0;
					else
						addr = VPTR(v) + v.m_used;
					
					err = _streamDeserializeIntegrated( addr, in, end_point, field, func, params );
					
					if ( addr ) {
						_check_err_ptr( err, addr, field );
						if ( _check_err(err) )
							break;
						v.m_used += el_size;
					}
				}
			}
			_empty_vector( v, 0, 0 );
			return err;
		}
		
		// not integrated
		if( PR_FAIL(err=in.pop(count,end_point)) )
			count = 0;
		
		// deserialize vector of serialisable structures (vector of pointers)
		if ( field->m_flags & SDT_FLAG_POINTER ) {
			if ( dsc_to_find || PR_SUCC(err=_alloc_vect_mem(v,count,sizeof(tPTR))) ) {
				for ( ; count; --count ) {
					if ( dsc_to_find )
						addr = 0;
					else {
						addr = VPTR(v) + v.m_used;
						*(tPTR*)addr = 0;
					}
					
					bool not_zero = false;
					if( PR_FAIL(err=in.pop(not_zero,end_point)) )
						break;
					if( not_zero ) {
						ser_id = cSerializable::eIID;
						if( PR_FAIL(err=in.pop(ser_id,end_point)) )
							break;
						if( (dsc=cRegistrar::find(ser_id)) == 0 ) {
							err = errBAD_SIGNATURE;
							break;
						}
						if ( dsc_to_find || PR_FAIL(err=_alloc_str(&dsc,(cSerializable**)addr,field,func,params)) )
							break;
						
						tPTR el_addr = addr ? *(tPTR*)addr : 0;
						tDWORD local_end_point = 0;
						if ( 1 == in.ver() )
							local_end_point = end_point;
						else
							err = in.pop_bytes( &local_end_point, sizeof(local_end_point), end_point );
						for( field_num=0,sub_field=dsc->m_fields; PR_SUCC(err); ++field_num,++sub_field ) {
							if ( sub_field->m_flags == SDT_FLAG_ENDPOINT )
								break;
							if ( in.cp() >= local_end_point )
								break;
							err = _streamDeserializeOneField( false, dsc_to_find, sub_field, QSADDR(el_addr,sub_field), in, local_end_point, 0, func, params );
							if ( err == warnFALSE ) {
								err = errOK;
								--field_num;
							}
						}

						if ( addr ) {
							_check_err_ptr( err, *(tPTR*)addr, field );
							if( _check_err(err) )
								break;
							ser = static_cast<cSerializable*>( *(tPTR*)addr );
							if ( ser )
								ser->finalizeStreamDeser( err, field_num, dsc );
						}
					}
					if ( addr )
						v.m_used += sizeof(tPTR);
				}
			}
			_empty_vector( v, 0, 0 );
			return err;
		}
		
		dsc = cRegistrar::find( field->m_id );
		if( !dsc )
			return errBAD_SIGNATURE;
		
		// deserialize vector of serialisable structures (vector of objects)
		if ( dsc_to_find || PR_SUCC(err=_alloc_vect_mem(v,count,dsc->m_size)) ) {
			for( ; count; --count ) {
				if ( dsc_to_find )
					addr = 0;
				else {
					addr = VPTR(v) + v.m_used;
					dsc->m_constructor( (cSerializable*)addr, 0 );
				}

				tDWORD local_end_point = 0;
				if ( 1 == in.ver() )
					local_end_point = end_point;
				else
					err = in.pop_bytes( &local_end_point, sizeof(local_end_point), end_point );
				for( field_num=0,sub_field=dsc->m_fields; PR_SUCC(err); ++field_num,++sub_field ) {
					if ( sub_field->m_flags == SDT_FLAG_ENDPOINT )
						break;
					if ( in.cp() >= local_end_point )
						break;
					err = _streamDeserializeOneField( false, dsc_to_find, sub_field, QSADDR(addr,sub_field), in, local_end_point, 0, func, params );
					if ( err == warnFALSE ) {
						err = errOK;
						--field_num;
					}
				}
				
				if ( addr ) {
					_check_err_ptr( err, addr, field );
					if( _check_err(err) ) {
						dsc->m_destructor((cSerializable*)addr);
						break;
					}
					v.m_used += dsc->m_size;
					ser = static_cast<cSerializable*>( addr );
					if ( ser )
						ser->finalizeStreamDeser( err, field_num, dsc );
				}
			}
		}
		_empty_vector( v, 0, 0 );
		return err;
	}

	// here we know - it's not a vector!
	bool p = false;
	tVOID* ptr;
	if ( field->m_flags & SDT_FLAG_POINTER ) {
		if ( dsc_to_find )
			ptr = obj;
		else
			ptr = *((tVOID**)obj);
		if ( (PR_FAIL(err=in.pop(p,end_point)) || !p) ) {
			if ( !dsc_to_find ) {
				_clean_ptr( ptr, field );
				*((tVOID**)obj) = 0;
			}
			return err;
		}
	}
	else
		ptr = obj;
	
	if ( field->m_id < tid_LAST_TYPE ) // integrated
		return _streamDeserializeIntegrated( dsc_to_find ? 0 : ptr, in, end_point, field, func, params );

	// not integrated
	const cSerDescriptor* dsc;
	const cSerDescriptorField* sub_field;

	ser_id = cSerializable::eIID;
	if ( !(field->m_flags & SDT_FLAG_POINTER) )
		ser_id = field->m_id;
	else if ( PR_FAIL(err=in.pop(ser_id,end_point)) )
		return err;

	if ( 0 == (dsc=cRegistrar::find(ser_id)) )
		return errNOT_FOUND;

	ser = static_cast<cSerializable*>( ptr );
	if ( !dsc_to_find && !ser && PR_FAIL(err=_alloc_str(&dsc,&ser,field,func,params)) )
		return err;

	if ( PR_FAIL(err=in.pop_bytes(&end_point,sizeof(tDWORD),end_point)) )
		return err;

	const cSerDescriptor* saved = dsc_to_find;
	for( field_num=0,sub_field=dsc->m_fields; PR_SUCC(err) && (!saved || dsc_to_find); ++field_num,++sub_field ) {
		if ( sub_field->m_flags == SDT_FLAG_ENDPOINT )
			break;
		if ( in.cp() >= end_point )
			break;
		err = _streamDeserializeOneField( true, dsc_to_find, sub_field, QSADDR(ser,sub_field), in, end_point, 0, func, params );
		if ( err == warnFALSE ) {
			err = errOK;
			--field_num;
		}
	}

	if ( !saved && (field->m_flags & SDT_FLAG_POINTER) ) {
		if ( PR_FAIL(err) && ser ) {
			_free_str( dsc, ser );
			ser = 0;
		}
		*((cSerializable**)obj) = ser;
	}
	if ( ser )
		ser->finalizeStreamDeser( err, field_num, dsc );
	return err;
} // _streamDeserializeOneField



