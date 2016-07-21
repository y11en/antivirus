#include "kernel.h"
#include <m_utils.h>
#include <Prague/iface/i_string.h>

#define IMPEX_IMPORT_LIB
#include <Prague/plugin/p_string.h>

#define CP_CLUSTER                 16
#define CP_LAST_CLUSTER_COUNT_MASK 0x0f
#define CP_CLUSTER_SHIFT           4
#define CP_NON_MOVED_SIZE          3


#define PRID(p) ( (p) & (~(pCUSTOM_HERITABLE|pCALLBACK_TMP_STOP)) )

#define DataPtr( s ) ((tCustData*)(s->m_data))
#define DataSPtr( s ) ((tCustStrData*)(s->m_data))


// ---
tPTR pr_call _CustomPropertyAddr( tCustProp* cp ) { 
	
	if ( _CustomPropertyTypeInPlace(cp->prid) ) 
		return &cp->m_data;

	switch ( (cp->prid & pTYPE_MASK) ) {
		case pTYPE_STRING:
		case pTYPE_WSTRING:
			return &((tCustStrData*)cp->m_data)->data;
		default:
			return &DataPtr(cp)->data;
	}
}



// ---
tCustProp* pr_call _CustomPropertyAdd( tHANDLE* handle, tPROPID prid ) {
	tCustProp* cprop       = handle->custom_prop;
  tUINT      cprop_count = handle->custom_prop_count;
  
	if ( cprop_count & CP_LAST_CLUSTER_COUNT_MASK )
		cprop += cprop_count;

	else if ( cprop_count ) {
    tUINT clusters = ((cprop_count >> CP_CLUSTER_SHIFT) + 1) * CP_CLUSTER * sizeof(tCustProp);
		if ( PR_FAIL(PrRealloc((tPTR*)&handle->custom_prop,handle->custom_prop,clusters)) )
			return 0;
    cprop = handle->custom_prop + cprop_count;
	}

	else if ( PR_FAIL(PrAlloc((tPTR*)&handle->custom_prop,CP_CLUSTER*sizeof(tCustProp))) )
		return 0;

	else
		cprop = handle->custom_prop;

  ++handle->custom_prop_count;
	cprop->prid = prid;
	if ( _CustomPropertyTypeInPlace(prid) ) {
		cprop->size = _TypeSize(prid);
		cprop->m_data = 0;
	}
	else {
		cprop->size = 0;
		cprop->m_data = 0;
	}
	return cprop;
}




// ---
tERROR pr_call _CustomPropertyDelete( tPO* po, tHANDLE* handle, tPROPID prop ) {
	tCustProp* cprop = handle->custom_prop;
	
	if ( cprop ) {
		tUINT              i, c;
		tERROR             e = errPROPERTY_NOT_FOUND;
		tNotificationSink* sink;
		tBOOL	             callback;
		
		if( prop & pCALLBACK_TMP_STOP )
			callback = cFALSE;
		else
			callback = cTRUE;
		
		prop = PRID( prop );
		for( i=0,c=handle->custom_prop_count; i<c; ++i,++cprop ) {
			if ( PRID(cprop->prid) == prop ) {
				e = errOK;
				wlock(po);
				if ( !_CustomPropertyTypeInPlace(prop) )
					PrFree( (tPTR)cprop->m_data );
				if ( c > (++i) )
					mmove( cprop, cprop+1, sizeof(tCustProp) * (c-i) );
				--handle->custom_prop_count;
				wunlock(po);
				if ( callback && (handle->flags & hf_CALLBACK_SENDER) && PR_SUCC(_ns_get_arr(cSNS_CUSTOM_PROP_DELETE,&sink,&c)) && sink ) {
					_PR_ENTER_PROTECTED_0P(po,handle) {
						for( i=0; i<c; i++,sink++ )
							sink->func( MakeObject(handle), prop, sink->ctx );
					} _PR_LEAVE_PROTECTED_0P( po, e );
				}
				break;
			}
		}
		return e;
	}
	
	return errPROPERTY_NOT_FOUND;
}





// ---
tERROR pr_call _CustomPropertySet( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size, tBOOL use_sink ) {

	tUINT      i, c;
	tERROR     error = errOK;
	tCustProp* cprop = 0;
	tCustProp* begin;
	tBOOL	     callback;
	tPROPID    propid;
	
	if( prop & pCALLBACK_TMP_STOP )	{
		callback = cFALSE;
		prop &= ~pCALLBACK_TMP_STOP;
	}
	else
		callback = cTRUE;
	
	propid = PRID( prop );
	for( i=0,c=handle->custom_prop_count,begin=cprop=handle->custom_prop; (i<c) && (PRID(cprop->prid)!=propid); ++i,++cprop )
		;
	if ( i == c )
		cprop = 0;
	else if( !use_sink ) // do not set marshalled properties
		return errOK;
	
	wlock(po);
	if ( !cprop && (0 == (cprop=_CustomPropertyAdd(handle,prop))) )
		error = errNOT_ENOUGH_MEMORY;
	
	else { 
		tCustData* cdata;
		
		if ( (i<c) && (i > CP_NON_MOVED_SIZE) ) {
			tCustProp local = *cprop;
			mmove( begin+1, begin, i*sizeof(tCustProp) );
			*begin = local;
			cprop = begin;
		}
		
		if ( _CustomPropertyTypeInPlace(prop) ) {
			if ( !buffer )
				size = cprop->size;
			else if ( size == cprop->size )
				mcpy( &cprop->m_data, buffer, size );
			else if ( size < cprop->size )
				mcpy( &cprop->m_data, buffer, size=cprop->size );
			else {
				size = 0;
				error = errBUFFER_TOO_SMALL;
			}
		}
		
		else if ( !cprop->size ) {
			if ( !buffer )
				size = (tDWORD)-1;
			else if ( PR_SUCC(error=PrAlloc((tPTR*)&cprop->m_data,size+sizeof(tCustData))) ) {
				cdata = DataPtr(cprop);
				cprop->size = cdata->allocated = size;
				mcpy( &cdata->data, buffer, size );
			}
			else
				size = 0;
		}
		else if ( !buffer )
			size = (tDWORD)-1;
		
		else if ( (cdata=DataPtr(cprop))->allocated >= size )
			mcpy( &cdata->data, buffer, cprop->size=size );
		
		else if ( PR_SUCC(error=PrRealloc((tPTR*)&cprop->m_data,(tPTR)cprop->m_data,size+sizeof(tCustData))) ) {
			cdata = DataPtr(cprop);
			cprop->size = cdata->allocated = size;
			mcpy( &cdata->data, buffer, size );
		}
		
		else
			size = 0;
	}
	wunlock(po);
	
	if ( use_sink && buffer && PR_SUCC(error) ) {
		tNotificationSink* sink;
		if ( callback && (handle->flags & hf_CALLBACK_SENDER) && PR_SUCC(_ns_get_arr(cSNS_CUSTOM_PROP_SET,&sink,&c)) && sink ) {
			_PR_ENTER_PROTECTED_0P(po,handle) {
				for( i=0; i<c; ++i,++sink )
					sink->func( MakeObject(handle), prop, buffer, size, cCP_NULL, sink->ctx );
			} _PR_LEAVE_PROTECTED_0P( po, error );
		}
	}
	
	if ( out_size )
		*out_size = size;
	
	return error;
}




// ---
tERROR pr_call _CustomPropertySetStr( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size, tCODEPAGE cp, tBOOL use_sink ) {
	
	tUINT      i, c;
	tERROR     error = errOK;
	tCustProp* cprop = 0;
	tCustProp* begin;
	tUINT      term = 0;
	tBOOL	     callback = cFALSE;
	tPROPID    propid;
	
	if( prop & pCALLBACK_TMP_STOP ) {
		callback = cFALSE;
		prop &= ~pCALLBACK_TMP_STOP;
	}
	else
		callback = cTRUE;
	
	propid = PRID( prop );
	for( i=0,c=handle->custom_prop_count,begin=cprop=handle->custom_prop; (i<c) && (PRID(cprop->prid)!=propid); ++i,++cprop )
		;
	if ( i == c )
		cprop = 0;
	else if( !use_sink ) // do not set marshalled properties
		return errOK;
	
	if ( size  ) {
		if ( cp == cCP_UNICODE ) {
			if ( 0 != *((tWCHAR*)(((tCHAR*)buffer) + size - sizeof(tWCHAR))) )
				term = sizeof(tWCHAR);
		}
		else if ( 0 != *(((tCHAR*)buffer) + size - sizeof(tCHAR)) )
			term = sizeof(tCHAR);
	}
	else if ( CalcExportLen )
		error = CalcExportLen( buffer, 0, cp, cp, cSTRING_Z, &size );
	else
		error = errSTRING_LEN_NOT_SPECIFIED;
	
	wlock(po);
	if ( PR_FAIL(error) )
		;
	else if ( !cprop && (0 == (cprop=_CustomPropertyAdd(handle,prop))) )
		error = errNOT_ENOUGH_MEMORY;
	
	else {
		tCustStrData* cdata;
		
		if ( (i<c) && (i > CP_NON_MOVED_SIZE) ) {
			tCustProp local = *cprop;
			mmove( begin+1, begin, i*sizeof(tCustProp) );
			*begin = local;
			cprop = begin;
		}
		
		if ( !cprop->size ) {
			if ( PR_SUCC(error=PrAlloc((tPTR*)&cprop->m_data,size+term+sizeof(tCustStrData))) ) {
				cdata = DataSPtr(cprop);
				cdata->cp = cp;
				cprop->size = cdata->allocated = size + term;
				mcpy( &cdata->data, buffer, size );
				size += term;
			}
			else
				size = 0;
		}
		
		else if ( (cdata=DataSPtr(cprop))->allocated >= size ) {
			cdata->cp = cp;
			cprop->size = size;
			mcpy( &cdata->data, buffer, size );
		}
		
		else if ( PR_SUCC(error=PrRealloc((tPTR*)&cprop->m_data,(tPTR)cprop->m_data,size+term+sizeof(tCustStrData))) ) {
			cdata = DataSPtr(cprop);
			cdata->cp = cp;
			cprop->size = cdata->allocated = size + term;
			mcpy( &cdata->data, buffer, size );
			size += term;
		}
		else
			size = 0;
	}
	wunlock(po);
	
	if ( use_sink && buffer && PR_SUCC(error) ) {
		tNotificationSink* sink;
		if ( callback && (handle->flags & hf_CALLBACK_SENDER) && PR_SUCC(_ns_get_arr(cSNS_CUSTOM_PROP_SET,&sink,&c)) && sink ) {
			_PR_ENTER_PROTECTED_0P(po,handle) {
				for( i=0; i<c; i++,sink++ )
					sink->func( MakeObject(handle), prop, buffer, size, cp, sink->ctx );
			} _PR_LEAVE_PROTECTED_0P( po, error );
		}
	}
	
	if ( out_size )
		*out_size = size;
	
	return error;
}




// ---
tDWORD pr_call _CustomPropertySize( tHANDLE* handle, tPROPID prop ) {
	tUINT      i, c;
	tCustProp* cprop;
	
	prop = PRID( prop );
	for( i=0,c=handle->custom_prop_count,cprop=handle->custom_prop; i<c; ++i,++cprop ) {
		if ( PRID(cprop->prid) == prop )
			return cprop->size;
	}
	return 0;
}




// ---
tERROR pr_call _CustomPropertyGet( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size ) {
	tUINT      i, c;
	tCustProp* cprop;
	tCustProp* begin;
	
	prop = PRID( prop );
	for( i=0,c=handle->custom_prop_count,begin=cprop=handle->custom_prop; (i<c) && (PRID(cprop->prid)!=prop); ++i,++cprop )
		;
	if ( i == c ) {
		if ( out_size )
			*out_size = 0;
		return errPROPERTY_NOT_FOUND;
	}
	else if ( i > CP_NON_MOVED_SIZE ) {
		tCustProp local = *cprop;
		wlock(po);
		mmove( begin+1, begin, i*sizeof(tCustProp) );
		*begin = local;
		wunlock(po);
		cprop = begin;
	}
	
	if ( !buffer ) {
		if ( out_size )
			*out_size = cprop->size;
		return errOK;
	}
	
	else if ( size < cprop->size ) {
		if ( out_size )
			*out_size = cprop->size;
		return errBUFFER_TOO_SMALL;
	}
	
	else {
		if ( out_size )
			*out_size = cprop->size;
		mcpy( buffer, _CustomPropertyAddr(cprop), cprop->size );
		return errOK;
	}
}




// ---
tERROR pr_call _CustomPropertyGetStr( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size, tCODEPAGE cp ) {
	tUINT         i, c;
	tERROR        error;
	tCustProp*    cprop;
	tCustProp*    begin;
	tCustStrData* cdata;
	
	prop = PRID( prop );	
	for( i=0,c=handle->custom_prop_count,begin=cprop=handle->custom_prop; (i<c) && (PRID(cprop->prid)!=prop); ++i,++cprop )
		;
	if ( i == c ) {
		if ( out_size )
			*out_size = 0;
		return errPROPERTY_NOT_FOUND;
	}
	else if ( i > CP_NON_MOVED_SIZE ) {
		tCustProp local = *cprop;
		wlock(po);
		mmove( begin+1, begin, i*sizeof(tCustProp) );
		*begin = local;
		wunlock(po);
		cprop = begin;
	}
	
	if ( (cdata=DataSPtr(cprop)), !buffer ) {
		if ( cdata->cp == cp ) {
			error = errOK;
			size = cprop->size;
		}
		else if ( !ConvertLen ) {
			error = errCODEPAGE_CONVERSION_UNAVAILABLE;
			size = 0;
		}
		else
			error = ConvertLen( cprop->size, &size, cdata->cp, cp );
	}
	
	else if ( cdata->cp == cp ) {
		if ( size < cprop->size ) {
			error = errBUFFER_TOO_SMALL;
			size = cprop->size;
		}
		else {
			error = errOK;
			mcpy( buffer, &cdata->data, size=cprop->size );
		}
	}
	
	else if ( !CopyTo )
		error = errCODEPAGE_CONVERSION_UNAVAILABLE;
	
	else
		error = CopyTo( buffer, size, cp, &cdata->data, cprop->size, cdata->cp, cSTRING_Z, &size );
	
	if ( out_size )
		*out_size = size;
	
	return error;
}




// ---
tERROR pr_call _CustomPropertyDeleteList( tPO* po, tHANDLE* handle ) {
	tUINT      i, c;
	tCustProp* cprop;
	
	if ( handle->custom_prop ) {
		wlock(po);
		for( i=0,c=handle->custom_prop_count,cprop=handle->custom_prop; i<c; ++i,++cprop ) {
			if ( !_CustomPropertyTypeInPlace(cprop->prid) )
				PrFree( (tPTR*)cprop->m_data );
		}
		PrFree( handle->custom_prop );
		handle->custom_prop = 0;
		handle->custom_prop_count = 0;
		wunlock(po);
	}
	return errOK;
}




// ---
tCustProp* pr_call _CustomPropertyFind( tHANDLE* handle, tPROPID propid ) {
	tUINT      i, c;
	tCustProp* cprop;
	
	propid = PRID( propid );
	for( i=0,c=handle->custom_prop_count,cprop=handle->custom_prop; i<c; ++i,++cprop ) {
		if ( PRID(cprop->prid) == propid )
			return cprop;
	}
	return 0;
}




// ---
tERROR _CustomPropInherit( tPO* po, tHANDLE* handle ) { // try to inherit heritable propeties
	tUINT      i, c;
	tCustProp* cprop;
	tHANDLE* parent = handle->parent;
	if ( !parent || !parent->custom_prop )
		return errOK;
	for( i=0,c=parent->custom_prop_count,cprop=parent->custom_prop; i<c; ++i,++cprop ) {
		if ( cprop->prid & pCUSTOM_HERITABLE ) {
			tDWORD type = PR_PROP_TYPE(cprop->prid);
			if ( (type == pTYPE_STRING) || (type == pTYPE_WSTRING) ) {
				tCustStrData* str_data = DataSPtr(cprop);
				_CustomPropertySetStr( po, handle, 0, cprop->prid, str_data->data, cprop->size, str_data->cp, cTRUE );
			}
			else
				_CustomPropertySet( po, handle, 0, cprop->prid, _CustomPropertyAddr(cprop), cprop->size, cTRUE );
		}
	}
	return errOK;
}



// ---
tERROR pr_call _CustomPropertyGetAll( tPO* po, tHANDLE* handle, tUINT obj_size, tDWORD* out_size, tBYTE** buffer, tDWORD* buff_size, tObjSerializeFunc func, tObjCheckCustomPropFunc prop_check, tPTR ctx ) {
	tUINT      i, c;
	tERROR     err   = errOK;
	tCustProp* cprop = handle->custom_prop;
	tBYTE*     buff  = *buffer;
	tUINT      total = *buff_size;
	tUINT      size  = CPLIST_HEAD_SIZE;
	tUINT      count = 0;
	
	if ( !buff != !total )
		err = errPARAMETER_INVALID;
	
	else if ( !cprop ) {
		if ( buff && (total < size) )
			err = errBUFFER_TOO_SMALL;
	}
	
	else if ( !buff ) {
		for( i=0,c=handle->custom_prop_count,cprop=handle->custom_prop; i<c; ++i,++cprop )
			size += CPSIZE(cprop,obj_size);
	}
	
	else {
		buff += CPLIST_HEAD_SIZE;
		for( i=0,c=handle->custom_prop_count,cprop=handle->custom_prop; i<c; ++i,++cprop ) {
			tUINT cps;
			
			if ( prop_check ) {
				tERROR l_err;
				_PR_ENTER_PROTECTED_0P(po,handle) {
					l_err = prop_check( MakeObject(handle), cprop->prid, ctx );
				} _PR_LEAVE_PROTECTED_0P( po, l_err );
				if ( PR_FAIL(l_err) )
					continue;
			}
			
			cps = CPSIZE(cprop,obj_size);
			if ( total < (size+cps) ) {
				err = errBUFFER_TOO_SMALL;
				break;
			}
			
			TYPE_ASSIGN( tPROPID, buff, cprop->prid );
			TYPE_ASSIGN( tDWORD,  buff, cprop->size );
			
			if ( IS_PROP_TYPE(cprop->prid,pTYPE_STRING) || IS_PROP_TYPE(cprop->prid,pTYPE_WSTRING) )
				TYPE_ASSIGN( tDWORD,  buff, DataSPtr(cprop)->cp );
			
			if ( func && IS_PROP_TYPE(cprop->prid,pTYPE_OBJECT) ) {
				_PR_ENTER_PROTECTED_0P(po,handle) {
					err = func( MakeObject(handle), (hOBJECT)cprop->m_data, buff, ctx );
				} _PR_LEAVE_PROTECTED_0P( po, err );
				if ( PR_FAIL(err) )
					break;
				buff += obj_size;
			}
			else {
				mcpy( buff, _CustomPropertyAddr(cprop), cprop->size );
				buff += cprop->size;
			}
			
			size += cps;
			++count;
		}
	}
	
	if ( buff && (total >= CPLIST_HEAD_SIZE) ) {
		buff = *buffer;
		TYPE_ASSIGN( tDWORD, buff, CPLIST_SIGNATURE );
		TYPE_ASSIGN( tDWORD, buff, CPLIST_VERSION   );
		TYPE_ASSIGN( tDWORD, buff, count );
		TYPE_ASSIGN( tDWORD, buff, size-CPLIST_HEAD_SIZE );
	}  
	
	if ( out_size )
		*out_size = size;
	
	if ( buff ) 
		*buffer += size;
	*buff_size -= size;
	
	return err;
}




//// ---
//tERROR pr_call _CustomPropertySetAll( tHANDLE* handle, tUINT obj_size, tDWORD* out_size, tBYTE** buffer, tDWORD* buff_size, tObjDeserializeFunc func, tPTR ctx ) {
//  tERROR     err   = errOK;
//  tCustProp* cp    = handle->custom_prop;
//  tBYTE*     buff  = *buffer;
//  tUINT      total = 0;
//  
//  if ( !buff )
//    err = errPARAMETER_INVALID;
//  
//  else if ( !func && (obj_size != sizeof(hOBJECT)) )
//    err = errPARAMETER_INVALID;
//  
//  else if ( *buff_size < CPLIST_HEAD_SIZE )
//    err = errBUFFER_TOO_SMALL;
//
//  else if ( CPLIST_SIGNATURE != TYPE_GET(tDWORD,buff) ) {
//    total += sizeof(tDWORD);
//    err = errBAD_SIGNATURE;
//  }
//  
//  else if ( CPLIST_VERSION != TYPE_GET(tDWORD,buff) ) {
//    total += sizeof(tDWORD);
//    err = errBAD_VERSION;
//  }
//  
//  else {
//
//    tUINT i;
//    tUINT count = TYPE_GET( tDWORD, buff );
//    tUINT size  = TYPE_GET( tDWORD, buff );
//
//    total += 4 * sizeof(tDWORD);
//    
//    if ( *buff_size < size )
//      err = errBUFFER_TOO_SMALL;
//    
//    // ??? _CustomPropertyDeleteList( handle );
//    
//    for( i=0; i<count; i++ ) {
//      tDWORD  out;
//      tUINT   get = sizeof(tDWORD) + sizeof(tDWORD);
//      tPROPID pid = TYPE_GET( tPROPID, buff );
//      tDWORD  siz = TYPE_GET( tDWORD,  buff );
//      
//      total += 2 * sizeof(tDWORD);
//      
//      if ( IS_PROP_TYPE(pid,pTYPE_STRING) || IS_PROP_TYPE(pid,pTYPE_WSTRING) ) {
//        tCODEPAGE codepage = TYPE_GET( tCODEPAGE, buff );
//        total += sizeof(tDWORD);
//        err = _CustomPropertySetStr( handle, &out, pid, buff, siz, codepage );
//        buff += siz;
//        total += siz;
//      }
//      else if ( func && IS_PROP_TYPE(pid,pTYPE_OBJECT) ) {
//        hOBJECT obj;
//        _PR_ENTER_PROTECTED_0P(po) {
//          err = func( MakeObject(handle), buff, &obj, ctx );
//        } _PR_LEAVE_PROTECTED_0P( po, err );
//        if ( PR_FAIL(err) )
//          break;
//
//        err = _CustomPropertySet( handle, &out, pid, &obj, siz );
//        buff += obj_size;
//        total += obj_size;
//      }
//      else {
//        err = _CustomPropertySet( handle, &out, pid, buff, siz );
//        buff += siz;
//        total += siz;
//      }
//
//      if ( PR_FAIL(err) )
//        break;
//    }
//  }
//
//  if ( out_size )
//    *out_size = total;
//  *buffer += total;
//
//  return err;
//}
