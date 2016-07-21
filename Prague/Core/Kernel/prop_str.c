#include "kernel.h"
#include <m_utils.h>
#include <Prague/iface/i_string.h>
#define IMPEX_IMPORT_LIB
#include <Prague/plugin/p_string.h>



// ---
//tERROR pr_call call_func( tPO* po, tFunc func, tUINT pnum, ... );

#if defined( _DEBUG ) && !defined(_WIN64)

	typedef tERROR (pr_call* tFunc)();
	static tERROR _call_func_int( tPO* po, tFunc func, tUINT pnum, ... );

	#define call_func4( err, po, func, p1, p2, p3, p4 )                 err=_call_func_int( po, func, 4, p1, p2, p3, p4 )
	#define call_func5( err, po, func, p1, p2, p3, p4, p5 )             err=_call_func_int( po, func, 5, p1, p2, p3, p4, p5 )
	#define call_func6( err, po, func, p1, p2, p3, p4, p5, p6 )         err=_call_func_int( po, func, 6, p1, p2, p3, p4, p5, p6 )
	#define call_func8( err, po, func, p1, p2, p3, p4, p5, p6, p7, p8 ) err=_call_func_int( po, func, 8, p1, p2, p3, p4, p5, p6, p7, p8 )

#else

  #define call_func4( err, po, func, p1, p2, p3, p4 ) {                 \
		_PR_ENTER_PROTECTED_0(po) {                                         \
			err = (func)( p1, p2, p3, p4 );                                   \
		} _PR_LEAVE_PROTECTED_0( po, err ); }

	#define call_func5( err, po, func, p1, p2, p3, p4, p5 ) {             \
		_PR_ENTER_PROTECTED_0(po) {                                         \
			err = (func)( p1, p2, p3, p4, p5 );                               \
		} _PR_LEAVE_PROTECTED_0( po, err ); }

	#define call_func6( err, po, func, p1, p2, p3, p4, p5, p6 ) {         \
		_PR_ENTER_PROTECTED_0(po) {                                         \
			err = (func)( p1, p2, p3, p4, p5, p6 );                           \
		} _PR_LEAVE_PROTECTED_0( po, err ); }

	#define call_func8( err, po, func, p1, p2, p3, p4, p5, p6, p7, p8 ) { \
		_PR_ENTER_PROTECTED_0(po) {                                         \
			err = (func)( p1, p2, p3, p4, p5, p6, p7, p8 );                   \
		} _PR_LEAVE_PROTECTED_0( po, err ); }

#endif

#define DPO  (PSYNC_ON(handle) ? po : 0)


// ---
tERROR pr_call System_PropertyGetStr( hOBJECT object, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size, tCODEPAGE receiver_cp ) {
	
	tERROR   error;
	tHANDLE* handle;
	tDATA    result = 0;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A4( object, "Enter \"System::PropertyGetStr\"(%tpid,buff=%p,c=%u,cp=%x)", prop_id, buffer, size, receiver_cp );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( ((prop_id & pTYPE_MASK) != pTYPE_STRING) && ((prop_id & pTYPE_MASK) != pTYPE_WSTRING) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify string type", prop_id ));
		error = errPARAMETER_INVALID;
	}
	else if ( !buffer != !size ) {
		PR_TRACE(( object, prtERROR, "krn\tparameters buffer(%p) and size(%u) are not coherent ((buffer==0) != (count==0))", buffer, size ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGetStr( po, handle, &result, prop_id, buffer, size, receiver_cp );
		else
			error = _PropertyGetStr( po, handle, &result, prop_id, buffer, size, receiver_cp );
		runlockc(po,lock);
	}
	
	leave_lf();
	if ( out_size )
		*out_size = (tDWORD)result;
	PR_TRACE_A2( object, "Leave \"System::PropertyGetStr\" ret tDWORD(size) = %u, error = %terr", (tDWORD)result, error );
	return error;
}




// ---
tERROR pr_call System_PropertySetStr( hOBJECT object, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size, tCODEPAGE src_cp ) {

	tERROR   error;
	tHANDLE* handle;
	tDATA    result = 0;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A4( object, "Enter \"System::PropertySetStr\"(%tpid,buff=%p,c=%u,cp=%x)", prop_id, buffer, size, src_cp );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( ((prop_id & pTYPE_MASK) != pTYPE_STRING) && ((prop_id & pTYPE_MASK) != pTYPE_WSTRING) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify string type", prop_id ));
		error = errPARAMETER_INVALID;
	}
	/*
	else if ( !buffer != !size ) {
		PR_TRACE(( object, prtERROR, "krn\tparameters buffer(%p) and size(%u) are not coherent ((buffer==0) != (count==0))", buffer, size ));
		error = errPARAMETER_INVALID;
	}
	*/
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySetStr( po, handle, &result, prop_id, buffer, size, src_cp, cTRUE );
		else
			error = _PropertySetStr( po, handle, &result, prop_id, buffer, size, src_cp );
		runlockc(po,lock);
	}
	
	leave_lf();
	if ( out_size )
		*out_size = (tDWORD)result;
	PR_TRACE_A2( object, "Leave \"System::PropertySetStr\" ret tDWORD(size) = %u, error = %terr", (tDWORD)result, error );
	return error;
}





// ---
tERROR pr_call System_PropertyGetStrCP( hOBJECT object, tCODEPAGE* cp, tPROPID propid ) {
	
  tERROR   error;
  tHANDLE* handle;
  tPO*     po;
	tINT     lock = 1;
  
  PR_TRACE_A1( object, "Enter \"System::PropertyGetStrCP\"(%tpid)", propid );
  enter_lf();
	
	if ( 0 == propid ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
  else if ( (pTYPE_STRING != PR_PROP_TYPE(propid)) && (pTYPE_WSTRING != PR_PROP_TYPE(propid)) )
		error = errPROPERTY_INVALID;
	
  else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
    error = _PropertyGetStrCP( po, handle, cp, propid );
    runlockc(po,lock);
  }
  
  leave_lf();
  #ifdef _PRAGUE_TRACE_
		if ( cp )
      PR_TRACE_A2( object, "Leave \"System::PropertyGetStrCP\" ret tCODEPAGE = 0x%x, error = %terr", *cp, error );
    else
      PR_TRACE_A1( object, "Leave \"System::PropertyGetStrCP\" ret tCODEPAGE = ???, error = %terr", error );
  #endif
  return error;
}





// ---
tERROR pr_call _PropertyGetStrCP( tPO* po, tHANDLE* handle, tCODEPAGE* cp, tPROPID propid ) {
	tERROR error;
	tINTERFACE* iface;

  if ( (PR_PROP_TYPE(propid) != pTYPE_STRING) && (PR_PROP_TYPE(propid) != pTYPE_WSTRING) )
    return errPROPERTY_INVALID;

	else if ( PROP_RANGE(propid) == pRANGE_CUSTOM ) {
		tCustProp* cprop = _CustomPropertyFind( handle, propid );
		if ( cprop ) {
			if ( cp )
				*cp = ((tCustStrData*)cprop->m_data)->cp;
			return errOK;
		}
		else
			return errPROPERTY_NOT_FOUND;
	}

	else if ( 0 == (iface=handle->iface) ) 
		return  errINTERFACE_NOT_ASSIGNED_YET;

  /* let _PropertyGet routine to check it and (LAST_CALL may be?)
	else if ( 0 == _PropTableSearch(iface,propid,0) )
		return errPROPERTY_NOT_FOUND;
  */

	else if ( PR_SUCC(error=_PropertyGet( po, handle,0,((propid&~pTYPE_MASK)|pTYPE_CODEPAGE),cp,sizeof(tCODEPAGE))) ) 
		return error;

	else if ( error == errPROPERTY_NOT_FOUND )
		return _PropertyGet( po, handle, 0, pgOBJECT_CODEPAGE, cp, sizeof(tCODEPAGE) );

	else
		return error;
}




// ---
tERROR pr_call _PropertyGetStr( tPO* po, tHANDLE* handle, tDATA* result, tPROPID propid, tSTRING buffer, tDWORD size, tCODEPAGE receive_cp ) {
	tERROR       error = errUNEXPECTED;
  tDATA        len = 0;
  const tDATA* table;
	tINTERFACE*  iface;
	tCODEPAGE    src_cp;
	
	PR_TRACE_A0( MakeObject(handle), "Enter _PropertyGetStr method" );
	
	/*
	if ( !KNOWN_CP(receive_cp) )
		error = errCODEPAGE_NOT_SUPPORTED;
	*/

	if ( 0 == (iface=handle->iface) )
		error = errINTERFACE_NOT_ASSIGNED_YET;

  else if ( propid == pgPLUGIN_NAME ) {
    if ( iface->plugin ) {
			tHANDLE* plugin;
			tINT     lock = 1;
			if ( _HCP(plugin,po,lock,iface->plugin,&error) ) {
				error = _PropertyGetStr( po, plugin, &len, pgMODULE_NAME, buffer, size, receive_cp );
				runlockc(po,lock);
			}
		}
    else
      error = errMODULE_NOT_FOUND;
  }

  else if ( (0 == (table=_PropTableSearch(iface,propid,0))) && (0==(table=_PropTableSearch(iface,pgPROP_LAST_CALL,0))) )
		error = errPROPERTY_NOT_FOUND;

	else if ( (cPROP_BUFFER_READ|cPROP_BUFFER_HSTRING) == (PROP_MODE(table) & (cPROP_BUFFER_READ|cPROP_BUFFER_HSTRING)) ) {
		hSTRING hstr = *(hSTRING*)(((tBYTE*)*hdata(handle))+PROP_OFFSET(table));
		if ( hstr ) {
			tDWORD l;
			error = CALL_String_ExportToBuff( hstr, &l, cSTRING_WHOLE, buffer, size, receive_cp, cSTRING_Z );
			len = l;
		}
		else
			error = errOBJECT_NOT_FOUND;
	}

  else {
		tIntFnPropIO func;
		tPTR         src = 0;
		tDATA        src_len = 0;

		if ( PR_FAIL(error=_PropertyGetStrCP(po,handle,&src_cp,propid)) )
			src_cp = receive_cp; // cannot get source CP, just get string witout conversion !!! 
		
		if ( PROP_MODE(table) & cPROP_BUFFER_SHARED ) {
			func = 0;
      if ( PROP_MODE(table) & (cPROP_BUFFER_SHARED_PTR & ~cPROP_BUFFER_SHARED) )
        src = PROP_BUFFER(table);
      else if ( PROP_MODE(table) & (cPROP_BUFFER_SHARED_VAR & ~cPROP_BUFFER_SHARED) )
        src = (tSTRING)(*(tPTR*)PROP_BUFFER(table));
      else
        src = &((tDATA*)table)[1]; // &PROP_BUFFER(table) !!! Palm OS doesn't accept it
		}
		else if ( 0 != (func=PROP_GETFN(table)) )
			src = 0;
		else if ( (PROP_MODE(table) & cPROP_BUFFER_READ) ) {
			func = 0;
			src = ((tBYTE*)*hdata(handle))+PROP_OFFSET(table);
		}
		if ( src )
			src_len = PROP_SIZE(table);
		
		if ( !func && !src )
			error = errPROPERTY_NOT_READABLE;

		else if ( buffer ) {
      len = src_len;
			if ( src_cp != receive_cp ) {
				if ( func ) {
					if ( !CopyTo || !CalcExportLen )
						error = errCODEPAGE_CONVERSION_UNAVAILABLE;
					else {
						tPTR buff = 0;
						tDWORD dwLen = 0;
						call_func5( error, DPO, func, MakeObject(handle), &dwLen, propid, 0, 0 );
						if ( PR_FAIL(error) )
							;
						else if ( PR_FAIL(error=PrAlloc(&buff,dwLen)) )
							;
						else {
							call_func5( error, DPO, func, MakeObject(handle), &dwLen, propid, buff, dwLen );
							if ( PR_SUCC(error) ) {
								//tDWORD ex_len = 0;
								//call_func6( error, 0/*DPO*/, CalcExportLen, buff, dwLen, src_cp, receive_cp, cSTRING_Z, &ex_len );
								//if ( PR_FAIL(error) )
								//	;
								//else if ( size < ex_len )
								//	dwLen = 0, error = errBUFFER_TOO_SMALL;
								//else
									call_func8( error, 0/*DPO*/, CopyTo, buffer, size, receive_cp, buff, dwLen, src_cp, cSTRING_Z, &dwLen );
							}
						}
						if ( buff )
							PrFree( buff );
						len = dwLen;
					}
				}
				else if ( !CalcExportLen || !CalcImportLen )
					len = 0, error = errCODEPAGE_CONVERSION_UNAVAILABLE;
				else {
					tDWORD dwLen = 0;
					tDWORD dwSrcLen = (tDWORD)src_len;
					call_func5( error, 0/*DPO*/, CalcImportLen, src, src_cp, cSTRING_Z, dwSrcLen, &dwSrcLen );
					if ( PR_FAIL(error) )
						dwLen = 0;
					else {
						call_func6( error, 0/*DPO*/, CalcExportLen, src, dwSrcLen, src_cp, receive_cp, cSTRING_Z, &dwLen );
						if ( PR_FAIL(error) )
							dwLen = 0;
						else if ( size < dwLen )
							dwLen = 0, error = errBUFFER_TOO_SMALL;
						else 
							call_func8( error, 0/*DPO*/, CopyTo, buffer, size, receive_cp, src, dwSrcLen, src_cp, cSTRING_Z, &dwLen );
					}
					len = dwLen;
				}
			}
			else if ( func ) {
				tDWORD dwLen = 0;
				call_func5( error, DPO, func, MakeObject(handle), &dwLen, propid, buffer, size );
				len = dwLen;
			}
			else if ( !CalcExportLen )
				len = 0, error = errCODEPAGE_CONVERSION_UNAVAILABLE;
			else {
				tDWORD dwLen = 0;
				call_func6( error, 0/*DPO*/, CalcExportLen, src, (tDWORD)src_len, src_cp, receive_cp, cSTRING_Z, &dwLen );
				if ( PR_FAIL(error) )
					dwLen = 0;
				else if ( size < dwLen )
					dwLen = 0, error = errBUFFER_TOO_SMALL;
				else if ( CopyTo ) {
					call_func8( error, 0/*DPO*/, CopyTo, buffer, size, receive_cp, src, dwLen, src_cp, cSTRING_Z, &dwLen );
				}
				else {
					dwLen = (tDWORD)src_len;
					memcpy( buffer, src, src_len );
				}
				len = dwLen;
			}
		}
		else if ( func ) {
			tDWORD dwLen = 0;
			call_func5( error, DPO, func, MakeObject(handle), &dwLen, propid, 0, 0 );
			if ( PR_SUCC(error) && (src_cp != receive_cp) && dwLen && ConvertLen )
				call_func4( error, 0/*DPO*/, ConvertLen, dwLen, &dwLen, src_cp, receive_cp );
			len = dwLen;
		}
		else if ( !CalcExportLen )
			len = src_len;
		else {
			tDWORD dwLen = 0;
			call_func6( error, 0/*DPO*/, CalcExportLen, src, (tDWORD)src_len, src_cp, receive_cp, cSTRING_Z, &dwLen );
			if ( PR_SUCC(error) )
				len = dwLen;
			else
				len = src_len;
		}
	}
	
	if ( result )
		*result = len;
  PR_TRACE_A2( MakeObject(handle), "Leave _PropertyGetStr method, ret tDWORD = %u, error = %terr", len, error );
	return error;
}





// ---
tERROR pr_call _PropertySetStr( tPO* po, tHANDLE* handle, tDATA* result, tPROPID propid, tSTRING buffer, tDWORD size, tCODEPAGE src_cp ) {
	tERROR       error = errOK;
	tDATA        len = 0;
	const tDATA* table;
	tINTERFACE*  iface;
	tDWORD       init_prop_pos = 0xffffffff;
	
	PR_TRACE_A0( MakeObject(handle), "Enter _PropertySetStr method" );
	
	/*
	if ( !KNOWN_CP(receive_cp) )
		error = errCODEPAGE_NOT_SUPPORTED;
	*/
	
	if ( 0 == (iface=handle->iface) )
		error = errINTERFACE_NOT_ASSIGNED_YET;
	
	else if ( (0 == (table = _PropTableSearch(iface,propid,&init_prop_pos))) && (0==(table=_PropTableSearch(iface,pgPROP_LAST_CALL,&init_prop_pos))) )
		error = errPROPERTY_NOT_FOUND;
	
	/*
	else if ( !KNOWN_CP(src_cp) )
		error = errCODEPAGE_NOT_SUPPORTED;
	*/
	
	else if ( PROP_MODE(table) & cPROP_BUFFER_SHARED ) // shared buffer is RO !!!
		error = errPROPERTY_NOT_WRITABLE;
	
	else if ( ((PROP_MODE(table) & cPROP_WRITABLE_ON_INIT) != 0) && ((handle->flags & hf_OPERATIONAL_MODE) != 0) ) // property cannot be set in this operational mode
		error = errPROPERTY_NOT_WRITABLE;
	
	else if ( (cPROP_BUFFER_WRITE|cPROP_BUFFER_HSTRING) == (PROP_MODE(table) & (cPROP_BUFFER_WRITE|cPROP_BUFFER_HSTRING)) ) {
		hSTRING* pstr = (hSTRING*)(((tBYTE*)*hdata(handle))+PROP_OFFSET(table));
		if ( !*pstr ) {
			tHANDLE* hstr;
			rlock(po);
			if ( PR_SUCC(error=_ObjectIIDCreate(po,g_hRoot,&hstr,IID_STRING,PID_ANY,SUBTYPE_ANY)) ) {
				if ( PR_SUCC(error=_ObjectCreateDone(po,hstr)) )
					*pstr = (hSTRING)MakeObject( hstr );
				else
					_ObjectClose( po, hstr, 0 );
			}
			runlock(po);
		}
		if ( PR_SUCC(error) ) {
			tDWORD dwLen = 0;
			error = CALL_String_ImportFromBuff( *pstr, &dwLen, buffer, size, src_cp, cSTRING_Z );
			len = dwLen;
		}
		if ( PR_SUCC(error) && buffer && (init_prop_pos < 32) )
			handle->init_props &= ~( 1 << init_prop_pos );
	}
	
	else {
		tIntFnPropIO func;
		tPTR         dst;
		tDATA        dst_len = 0;
		tCODEPAGE    dst_cp;
		
		if ( PR_FAIL(error=_PropertyGetStrCP(po,handle,&dst_cp,propid)) )
			dst_cp = src_cp; // cannot get destination CP, just put string witout conversion !!! 
		
		if ( 0 != (func=PROP_SETFN(table)) )
			dst = 0;
		else if ( PROP_MODE(table) & cPROP_BUFFER_WRITE ) {
			dst = ((tBYTE*)*hdata(handle))+PROP_OFFSET(table);
			dst_len = PROP_SIZE(table);
		}
		else
			dst = 0;
		
		if ( !func && !dst )
			error = errPROPERTY_NOT_WRITABLE;
		else if ( buffer ) {
			if ( src_cp != dst_cp ) {
				if ( func ) {
					if ( !CalcExportLen || !CopyTo )
						error = errCODEPAGE_CONVERSION_UNAVAILABLE;
					else {
						tPTR tmp = 0;
						tDWORD tmp_len = size;
						call_func6( error, 0/*DPO*/, CalcExportLen, buffer, size, src_cp, dst_cp, cSTRING_Z, &tmp_len );
						if ( PR_FAIL(error) )
							;
						else if ( PR_FAIL(error=PrAlloc(&tmp,tmp_len)) )
							;
						else {
							call_func8( error, 0/*DPO*/, CopyTo, tmp, tmp_len, dst_cp, buffer, size, src_cp, cSTRING_Z, &tmp_len );
							if ( PR_SUCC(error) ) {
								tDWORD dwLen = 0;
								call_func5( error, DPO, func, MakeObject(handle), &dwLen, propid, tmp, tmp_len );
								len = dwLen;
							}
						}
						if ( tmp )
							PrFree( tmp );
					}
				}
				else if ( CopyTo ) {
					tDWORD dwLen = 0; 
					call_func8( error, 0/*DPO*/, CopyTo, dst, (tDWORD)dst_len, dst_cp, buffer, size, src_cp, cSTRING_Z, &dwLen );
					len = dwLen;
				}
				else
					error = errCODEPAGE_CONVERSION_UNAVAILABLE;
			}
			else if ( !size && ((error=errSTRING_LEN_NOT_SPECIFIED,!CalcExportLen) || PR_FAIL(error=CalcExportLen(buffer,0,src_cp,src_cp,cSTRING_Z,&size))) )
				;
			else if ( func ) {
				tDWORD dwLen = 0;
				call_func5( error, DPO, func, MakeObject(handle), &dwLen, propid, buffer, size );
				len = dwLen;
			}
			else if ( dst_len < size ) {
				len = 0;
				error = errBUFFER_TOO_SMALL;
			}
			else {
				memcpy( dst, buffer, (tDWORD)(len=size) );
				error = errOK;
			}
			if ( PR_SUCC(error) && (init_prop_pos < 32) )
				handle->init_props &= ~( 1 << init_prop_pos );
		}
		else if ( func ) {
			tDWORD dwLen = (tDWORD)len;
			call_func5( error, DPO, func, MakeObject(handle), &dwLen, propid, 0, 0 );
			if ( PR_SUCC(error) && (src_cp != dst_cp) && dwLen ) {
				if ( ConvertLen ) {
					call_func4( error, 0/*DPO*/, ConvertLen, dwLen, &dwLen, dst_cp, src_cp );
				}
				else
					error = errCODEPAGE_CONVERSION_UNAVAILABLE;
			}
			len = dwLen;
		}
		else if ( src_cp == dst_cp )
			len = dst_len;
		else if ( ConvertLen ) {
			tDWORD dwLen = 0;
			call_func4( error, 0/*DPO*/, ConvertLen, (tDWORD)dst_len, &dwLen, dst_cp, src_cp );
			len = dwLen;
		}
		else
			error = errCODEPAGE_CONVERSION_UNAVAILABLE;
	}
	
	if ( result )
		*result = len;
	PR_TRACE_A2( MakeObject(handle), "Leave _PropertySetStr method, ret tDWORD = %u, error = %terr", (tDWORD)len, error );
	return error;
}


#if defined( _DEBUG ) && !defined(_WIN64)
	typedef tERROR  (pr_call *tFunc0) ( );
	typedef tERROR  (pr_call *tFunc1) ( tUINT p1 );
	typedef tERROR  (pr_call *tFunc2) ( tUINT p1, tUINT p2 );
	typedef tERROR  (pr_call *tFunc3) ( tUINT p1, tUINT p2, tUINT p3 );
	typedef tERROR  (pr_call *tFunc4) ( tUINT p1, tUINT p2, tUINT p3, tUINT p4 );
	typedef tERROR  (pr_call *tFunc5) ( tUINT p1, tUINT p2, tUINT p3, tUINT p4, tUINT p5 );
	typedef tERROR  (pr_call *tFunc6) ( tUINT p1, tUINT p2, tUINT p3, tUINT p4, tUINT p5, tUINT p6 );
	typedef tERROR  (pr_call *tFunc7) ( tUINT p1, tUINT p2, tUINT p3, tUINT p4, tUINT p5, tUINT p6, tUINT p7 );
	typedef tERROR  (pr_call *tFunc8) ( tUINT p1, tUINT p2, tUINT p3, tUINT p4, tUINT p5, tUINT p6, tUINT p7, tUINT p8 );

	#define NP va_arg(args,tUINT)

	// ---
	static tERROR _call_func_int( tPO* po, tFunc func, tUINT pnum, ... ) {
	  
		tERROR  err;
		tUINT   p[8];
	  
		va_list args;
		va_start( args, pnum );

		_PR_ENTER_PROTECTED_0(po) {
			switch( pnum ) {
				case 0: 
					err = ((tFunc0)(func))(  ); break;
				case 1: 
					err = ((tFunc1)(func))( NP ); break;
				case 2: 
					p[0] = NP; p[1] = NP;
					err = ((tFunc2)(func))( p[0], p[1] ); break;
				case 3: 
					p[0] = NP; p[1] = NP; p[2] = NP;
					err = ((tFunc3)(func))( p[0], p[1], p[2] ); break;
				case 4: 
					p[0] = NP; p[1] = NP; p[2] = NP; p[3] = NP;
					err = ((tFunc4)(func))( p[0], p[1], p[2], p[3] ); break;
				case 5: 
					p[0] = NP; p[1] = NP; p[2] = NP; p[3] = NP; p[4] = NP;
					err = ((tFunc5)(func))( p[0], p[1], p[2], p[3], p[4] ); break;
				case 6: 
					p[0] = NP; p[1] = NP; p[2] = NP; p[3] = NP; p[4] = NP; p[5] = NP;
					err = ((tFunc6)(func))( p[0], p[1], p[2], p[3], p[4], p[5] ); break;
				case 7: 
					p[0] = NP; p[1] = NP; p[2] = NP; p[3] = NP; p[4] = NP; p[5] = NP; p[6] = NP;
					err = ((tFunc7)(func))( p[0], p[1], p[2], p[3], p[4], p[5], p[6] ); break;
				case 8: 
					p[0] = NP; p[1] = NP; p[2] = NP; p[3] = NP; p[4] = NP; p[5] = NP; p[6] = NP; p[7] = NP;
					err = ((tFunc8)(func))( p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7] ); break;
				default: err = errUNEXPECTED; break;
			}
		} _PR_LEAVE_PROTECTED_0( po, err );
		return err;
	}
#endif
