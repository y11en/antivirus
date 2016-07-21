/*-----------------17.03.00 09:31-------------------
 * Project Prague                                 *
 * Subproject Kernel interfaces                   *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague System API                      *
 *                                                *
--------------------------------------------------*/


#include "kernel.h"
#include <m_utils.h>

#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/plugin/p_string.h>

// ---
#define PROP_ID_SET(a,value)      ((a[0]) = (tDATA)(value))
#define PROP_BUFFER_SET(a,value)  ((a[1]) = (tDATA)(value))
#define PROP_OFFSET_SET(a,value)  ((a[1]) = (tDATA)(value))
#define PROP_SIZE_SET(a,value)    ((a[2]) = (tDATA)(value))
#define PROP_MODE_SET(a,value)    ((a[3]) = (tDATA)(value))
#define PROP_GETFN_SET(a,value)   ((a[4]) = (tDATA)(value))
#define PROP_SETFN_SET(a,value)   ((a[5]) = (tDATA)(value))

#define CHECK_TYPE(a)       case pTYPE_##a : pcount = sizeof(t##a); break;


// ---
const tDWORD _type_size[] = {
	/* --  hole --   */ 0,
	/*VOID        0x01l*/ 0,
	/*BYTE        0x02l*/ sizeof(tBYTE),
	/*WORD        0x03l*/ sizeof(tWORD),
	/*DWORD       0x04l*/ sizeof(tDWORD),
	/*QWORD       0x05l*/ sizeof(tQWORD),
	/*BOOL        0x06l*/ sizeof(tBOOL),
	/*CHAR        0x07l*/ sizeof(tCHAR),
	/*WCHAR       0x08l*/ sizeof(tWCHAR),
	/*STRING      0x09l*/ -1,
	/*WSTRING     0x0al*/ -1,
	/*ERROR       0x0bl*/ sizeof(tERROR),
	/*PTR         0x0cl*/ sizeof(tPTR),
	/*INT         0x0dl*/ sizeof(tINT),
	/*UINT        0x0el*/ sizeof(tUINT),
	/*SBYTE       0x0fl*/ sizeof(tSBYTE),
	/* --  hole --   */ 0,
	/*SHORT       0x11l*/ sizeof(tSHORT),
	/*LONG        0x12l*/ sizeof(tLONG),
	/*LONGLONG    0x13l*/ sizeof(tLONGLONG),
	/*IID         0x14l*/ sizeof(tIID),
	/*PID         0x15l*/ sizeof(tPID),
	/*ORIG_ID     0x16l*/ sizeof(tORIG_ID),
	/*OS_ID       0x17l*/ sizeof(tOS_ID),
	/*VID         0x18l*/ sizeof(tVID),
	/*PROPID      0x19l*/ sizeof(tPROPID),
	/*VERSION     0x1al*/ sizeof(tVERSION),
	/*CODEPAGE    0x1bl*/ sizeof(tCODEPAGE),
	/*LCID        0x1cl*/ sizeof(tLCID),
	/*DATA        0x1dl*/ sizeof(tDATA),
	/*DATETIME    0x1el*/ sizeof(tDATETIME),
	/*FUNC_PTR    0x1fl*/ sizeof(tFUNC_PTR),
	/*SBYTE       0x0fl*/ sizeof(tSBYTE),
	/* --  hole --   */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	/*BINARY      0x30l*/ -1, 
	/*IFACEPTR    0x31l*/ sizeof(tPTR),
	/*OBJECT      0x32l*/ sizeof(hOBJECT),
	/*EXPORT      0x33l*/ sizeof(tEXPORT),
	/*IMPORT      0x34l*/ sizeof(tIMPORT),
	/*TRACE_LEVEL 0x35l*/ sizeof(tTRACE_LEVEL),
	/*TYPE_ID     0x36l*/ sizeof(tTYPE_ID),
};




//tDWORD  _slen( tDATA* table, tDWORD type );
static tDWORD  _llen( tVOID* buffer, tDATA* table, tDWORD type );


// ---
tERROR pr_call System_PropertyIsPresent( hOBJECT object, tPROPID prop_id ) {
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A1( object, "Enter System::PropertyIsPresent(%tpid)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( pRANGE_CUSTOM == PROP_RANGE(prop_id) ) {
			if ( !_CustomPropertyFind(handle,prop_id) )
				error = errPROPERTY_NOT_FOUND;
		}
		else if ( !handle->iface )
			error = errOBJECT_BAD_INTERNAL_STATE;
		else if ( !_PropTableSearch(handle->iface,prop_id,0) )
			error = errPROPERTY_NOT_FOUND;
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertyIsPresent ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_PropertyInitialized( hOBJECT object, tPROPID prop_id, tBOOL val ) {
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A1( object, "Enter System::PropertyInitialized(%tpid)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( pRANGE_CUSTOM == PROP_RANGE(prop_id) )
		error = errPROPERTY_INVALID;
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		tINTERFACE* iface;
		if ( 0 == (iface = handle->iface) )
			error = errOBJECT_BAD_INTERNAL_STATE;
		else {
			tDWORD init_prop_pos;
			const tDATA* table = _PropTableSearch( iface, prop_id, &init_prop_pos );
			if ( table && (init_prop_pos < 32) ) {
				tDWORD bit = 1 << init_prop_pos;
				wlock(po);
				if ( val )
					handle->init_props &= ~bit;
				else
					handle->init_props |= bit;
				error = errOK;
				wunlock(po);
			}
			else
				error = errPROPERTY_NOT_FOUND;
		}
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertyInitialized ret %terr", error );
	return error;
}



#if defined(_PRAGUE_TRACE_) && (defined(_DEBUG) || defined(USE_DBG_INFO))
	tCHAR src[260];
	#define OUTPUT_CALLER_INFO(obj) \
		if ( PrDbgGetCallerInfo && PR_SUCC(PrDbgGetCallerInfo(SKIP_THIS_MODULE,0,src,countof(src))) ) \
			PR_TRACE(( obj, prtDANGER, src ))
#else
	#define OUTPUT_CALLER_INFO(obj)
#endif



// ---
tERROR pr_call System_PropertyGet( hOBJECT object, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size ) {
	
	tERROR   error;
	tHANDLE* handle;
	tDATA    rets = 0;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A3( object, "Enter System::PropertyGet(%tpid,buff=%p,c=%u)", prop_id, buffer, size );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		if ( out_size )
			*out_size = 0;
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		tBOOL custom;
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			custom = cTRUE;
		else
			custom = cFALSE;
		switch( prop_id & pTYPE_MASK ) {
			case pTYPE_STRING :
				PR_TRACE(( object, prtDANGER, "krn\tPossible UNICODE unsafe (%tiid): string property must be get by PropertyGetStr method only", GET_IID_C(handle) ));
				OUTPUT_CALLER_INFO( object );
				if ( custom )
					error = _CustomPropertyGetStr( po, handle, &rets, prop_id, buffer, size, cCP_ANSI );
				else {
					//error = errPROPERTY_STR_CODEPAGE_UNKNOWN;
					error = _PropertyGetStr( po, handle, &rets, prop_id, buffer, size, cCP_ANSI );
				}
				break;
			case pTYPE_WSTRING :
				PR_TRACE(( object, prtDANGER, "krn\tPossible UNICODE unsafe (%tiid): string property must be get by \"PropertyGetStr\" method only", GET_IID_C(handle) ));
				OUTPUT_CALLER_INFO( object );
				if ( custom )
					error = _CustomPropertyGetStr( po, handle, &rets, prop_id, buffer, size, cCP_UNICODE );
				else {
					//error = errPROPERTY_STR_CODEPAGE_UNKNOWN;
					error = _PropertyGetStr( po, handle, &rets, prop_id, buffer, size, cCP_UNICODE );
				}
				break;
			default :
				if ( custom )
					error = _CustomPropertyGet( po, handle, &rets, prop_id, buffer, size );
				else {
					error = _PropertyGet( po, handle, &rets, prop_id, buffer, size );
					if ( out_size )
						*out_size = (tDWORD)rets;
				}
				break;
		}
		runlockc(po,lock);
	}
	
	leave_lf();
	if ( out_size )
		*out_size = (tDWORD)rets;
	PR_TRACE_A2( object, "Leave System::PropertyGet ret tDWORD(size) = %u, error = %terr", (tDWORD)rets, error );
	return error;
}




// ---
tERROR pr_call System_PropertySet( hOBJECT object, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size ) {
	
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	tDATA    result = 0;
	
	PR_TRACE_A3( object, "Enter System::PropertySet(%tpid,buff=%p,c=%u)", prop_id, buffer, size );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		if ( out_size )
			*out_size = 0;
		error = errPARAMETER_INVALID;
	}
	else if ( !buffer != !size ) {
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtERROR, "krn\tSystem::PropertySet:parametr \"buffer\"(%p) is not matched to \"size\"(%u)", buffer, size ));
	}
	
	else if ( !buffer && !out_size ) {
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtERROR, "krn\tSystem::PropertySet: parametrs \"buffer\" and \"out_size\" both are NULL" ));
	}
	
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM ) {
			switch ( prop_id & pTYPE_MASK ) {
				case pTYPE_STRING :
					error = _CustomPropertySetStr( po, handle, &result, prop_id, buffer, size, cCP_ANSI, cTRUE );
					break;
				case pTYPE_WSTRING :
					error = _CustomPropertySetStr( po, handle, &result, prop_id, buffer, size, cCP_UNICODE, cTRUE );
					break;
				default:
					error = _CustomPropertySet( po, handle, &result, prop_id, buffer, size, cTRUE );
					break;
			}
		}
		else  {
			switch( prop_id & pTYPE_MASK ) {
				case pTYPE_STRING :
					PR_TRACE(( object, prtDANGER, "krn\tPossible UNICODE unsafe (%tiid): string property must be set by \"PropertySetStr\" method only", GET_IID_C(handle) ));
					OUTPUT_CALLER_INFO( object );
					error = _PropertySetStr( po, handle, &result, prop_id, buffer, size, cCP_ANSI );
					//error = errPROPERTY_STR_CODEPAGE_UNKNOWN;
					break;
				case pTYPE_WSTRING :
					PR_TRACE(( object, prtDANGER, "krn\tPossible UNICODE unsafe (%iid): string property must be set by \"PropertySetStr\" method only", GET_IID_C(handle) ));
					OUTPUT_CALLER_INFO( object );
					error = _PropertySetStr( po, handle, &result, prop_id, buffer, size, cCP_UNICODE );
					//error = errPROPERTY_STR_CODEPAGE_UNKNOWN;
					break;
				default :
					error = _PropertySet( po, handle, &result, prop_id, buffer, size );
			}
		}
		runlockc(po,lock);
	}
	
	leave_lf();
	if ( out_size )
		*out_size = (tDWORD)result;
	PR_TRACE_A2( object, "Leave System::PropertySet ret tDWORD(size) = %u, error = %terr", (tDWORD)rets, error );
	return error;
}



// ---
tBOOL pr_call System_PropertyGetBool( hOBJECT object, tPROPID prop_id ) {
	tBOOL    ret=0;
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A1( object, "Enter System::PropertyGetBool(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(ret) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify type with size equial to sizeof(tBOOL)", prop_id ));
		error = errPARAMETER_INVALID;
		ret = cFALSE;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		else
			error = _PropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		runlockc(po,lock);
		if ( PR_FAIL(error) )
			ret = cFALSE;
	}
	else
		ret = cFALSE;
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyGetBool ret tBOOL = %u, error = %terr", ret, error );
	return ret;
}




// ---
tBYTE pr_call System_PropertyGetByte( hOBJECT object, tPROPID prop_id ) {
	tBYTE    ret=0;
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A1( object, "Enter System::PropertyGetByte(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(ret) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify type with size equial to sizeof(tBYTE)", prop_id ));
		error = errPARAMETER_INVALID;
		ret = cFALSE;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		else
			error = _PropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		runlockc(po,lock);
		if ( PR_FAIL(error) )
			ret = cFALSE;
	}
	else
		ret = cFALSE;
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyGetByte ret tBYTE = %u, error = %terr", ret, error );
	return ret;
}




// ---
tWORD pr_call System_PropertyGetWord( hOBJECT object, tPROPID prop_id ) {
	tWORD    ret=0;
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A1( object, "Enter System::PropertyGetWord(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(ret) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify type with size equial to sizeof(tWORD)", prop_id ));
		error = errPARAMETER_INVALID;
		ret = 0;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		else
			error = _PropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		runlockc(po,lock);
		if ( PR_FAIL(error) )
			ret = 0;
	}
	else
		ret = 0;
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyGetWord ret tWORD = %u, error = %terr", ret, error );
	return ret;
}



// ---
tUINT pr_call System_PropertyGetUInt( hOBJECT object, tPROPID prop_id ) {
	tUINT    ret=0;
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A1( object, "Enter System::PropertyGetUInt(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(ret) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify type with size equial to sizeof(tUINT)", prop_id ));
		error = errPARAMETER_INVALID;
		ret = 0;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		else
			error = _PropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		runlockc(po,lock);
		if ( PR_FAIL(error) )
			ret = 0;
	}
	else
		ret = 0;
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyGetUInt ret tUINT = %u, error = %terr", ret, error );
	return ret;
}



// ---
tDWORD pr_call System_PropertyGetDWord( hOBJECT object, tPROPID prop_id ) {
	tDWORD   ret=0;
	tHANDLE* handle;
	tERROR   error;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A1( object, "Enter System::PropertyGetDWord(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(ret) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify type with size equial to sizeof(tDWORD)", prop_id ));
		error = errPARAMETER_INVALID;
		ret = 0;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		else
			error = _PropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		runlockc(po,lock);
		if ( PR_FAIL(error) )
			ret = 0;
	}
	else
		ret = 0;
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyGetDWord ret tDWORD = %u, error = %terr", ret, error );
	return ret;
}




// ---
tQWORD pr_call System_PropertyGetQWord( hOBJECT object, tPROPID prop_id ) {
	tQWORD ret=0;
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A1( object, "Enter System::PropertyGetQWord(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(ret) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify type with size equial to sizeof(tQWORD)", prop_id ));
		error = errPARAMETER_INVALID;
		ret = 0;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		else
			error = _PropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		runlockc(po,lock);
		if ( PR_FAIL(error) )
			ret = 0;
	}
	else
		ret = 0;
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyGetQWord ret tQWORD = %I64u, error = %terr", ret, error );
	return ret;
}



// ---
tDATA pr_call System_PropertyGetData( hOBJECT object, tPROPID prop_id ) {
	tDATA    ret = 0;
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A1( object, "Enter System::PropertyGetData(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(ret) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify type with size equial to sizeof(tQWORD)", prop_id ));
		error = errPARAMETER_INVALID;
		ret = 0;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		else
			error = _PropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		runlockc(po,lock);
		if ( PR_FAIL(error) )
			ret = 0;
	}
	else
		ret = 0;
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyGetData ret tDATA = " PRINTF_DATA ", error = %terr", ret, error );
	return ret;
}




// ---
tPTR pr_call System_PropertyGetPtr( hOBJECT object, tPROPID prop_id ) {
	tPTR     ret=0;
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A1( object, "Enter System::PropertyGetPtr(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(ret) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify type with size equial to sizeof(tPTR)", prop_id ));
		error = errPARAMETER_INVALID;
		ret = 0;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		else
			error = _PropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		runlockc(po,lock);
		if ( PR_FAIL(error) )
			ret = 0;
	}
	else
		ret = 0;
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyGetPtr ret tPTR = %p, error = %terr", ret, error );
	return ret;
}




// ---
hOBJECT pr_call System_PropertyGetObj( hOBJECT object, tPROPID prop_id ) {
	hOBJECT  ret=0;
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A1( object, "Enter System::PropertyGetObj(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(ret) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop does't identify type with size equial to sizeof(hOBJECT)", prop_id ));
		error = errPARAMETER_INVALID;
		ret = 0;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		else
			error = _PropertyGet( po, handle, 0, prop_id, &ret, sizeof(ret) );
		runlockc(po,lock);
#ifdef ____DEBUG
		if ( ret && PR_SUCC(error) )
			_HandleCheck_real( 0, ret, &error );
#endif
		if ( PR_FAIL(error) )
			ret = 0;
	}
	else
		ret = 0;
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyGetObj ret hOBJECT = %p, error = %terr", ret, error );
	return ret;
}




// ---
tERROR pr_call System_PropertySetBool( hOBJECT object, tPROPID prop_id, tBOOL val ) {
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A2( object, "Enter System::PropertySetBool(%tprop, val=%u)", prop_id, val );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(val) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop doesn't identify type with size equial to sizeof(tBOOL)", prop_id ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySet( po, handle, 0, prop_id, &val, sizeof(val), cTRUE );
		else
			error = _PropertySet( po, handle, 0, prop_id, &val, sizeof(val) );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySetBool ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_PropertySetByte( hOBJECT object, tPROPID prop_id, tBYTE val ) {
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A2( object, "Enter System::PropertySetByte(%tprop, val=%u)", prop_id, val );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(val) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop doesn't identify type with size equial to sizeof(tBYTE)", prop_id ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySet( po, handle, 0, prop_id, &val, sizeof(val), cTRUE );
		else
			error = _PropertySet( po, handle, 0, prop_id, &val, sizeof(val) );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySetByte ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_PropertySetWord( hOBJECT object, tPROPID prop_id, tWORD val ) {
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A2( object, "Enter System::PropertySetWord(%tprop, val=%u)", prop_id, val );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(val) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop doesn't identify type with size equial to sizeof(tWORD)", prop_id ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySet( po, handle, 0, prop_id, &val, sizeof(val), cTRUE );
		else
			error = _PropertySet( po, handle, 0, prop_id, &val, sizeof(val) );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySetWord ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_PropertySetUInt( hOBJECT object, tPROPID prop_id, tUINT val ) {
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A2( object, "Enter System::PropertySetUint(%tprop, val=%u)", prop_id, val );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(val) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop doesn't identify type with size equial to sizeof(tUINT)", prop_id ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySet( po, handle, 0, prop_id, &val, sizeof(val), cTRUE );
		else
			error = _PropertySet( po, handle, 0, prop_id, &val, sizeof(val) );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySetUint ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_PropertySetDWord( hOBJECT object, tPROPID prop_id, tDWORD val ) {
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A2( object, "Enter System::PropertySetDWord(%tprop, val=%u)", prop_id, val );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(val) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop doesn't identify type with size equial to sizeof(tDWORD)", prop_id ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySet( po, handle, 0, prop_id, &val, sizeof(val), cTRUE );
		else
			error = _PropertySet( po, handle, 0, prop_id, &val, sizeof(val) );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySetDWord ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_PropertySetQWord( hOBJECT object, tPROPID prop_id, tQWORD val ) {
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A2( object, "Enter System::PropertySetQWord(%tprop, val=%u)", prop_id, val );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(val) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop doesn't identify type with size equial to sizeof(tQWORD)", prop_id ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySet( po, handle, 0, prop_id, &val, sizeof(val), cTRUE );
		else
			error = _PropertySet( po, handle, 0, prop_id, &val, sizeof(val) );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySetQWord ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_PropertySetData( hOBJECT object, tPROPID prop_id, tDATA val ) {
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A2( object, "Enter System::PropertySetData(%tprop, val=%u)", prop_id, val );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(val) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop doesn't identify type with size equial to sizeof(tQWORD)", prop_id ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySet( po, handle, 0, prop_id, &val, sizeof(val), cTRUE );
		else
			error = _PropertySet( po, handle, 0, prop_id, &val, sizeof(val) );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySetData ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_PropertySetPtr( hOBJECT object, tPROPID prop_id, tPTR val ) {
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A2( object, "Enter System::PropertySetPtr(%tprop, val=%p)", prop_id, val );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(val) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop doesn't identify type with size equial to sizeof(tPTR)", prop_id ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySet( po, handle, 0, prop_id, &val, sizeof(val), cTRUE );
		else
			error = _PropertySet( po, handle, 0, prop_id, &val, sizeof(val) );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySetPtr ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_PropertySetObj( hOBJECT object, tPROPID prop_id, hOBJECT val ) {
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A2( object, "Enter System::PropertySetObj(%tprop, val=%p)", prop_id, val );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( sizeof(val) != _TypeSize(prop_id) ) {
		PR_TRACE(( object, prtERROR, "krn\tparameter %tprop doesn't identify type with size equial to sizeof(hOBJECT)", prop_id ));
		error = errPARAMETER_INVALID;
	}
#ifdef ____DEBUG
	else if ( val && !_HandleCheck_real(0,val,&error) )
		;
#endif
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			error = _CustomPropertySet( po, handle, 0, prop_id, &val, sizeof(val), cTRUE );
		else
			error = _PropertySet( po, handle, 0, prop_id, &val, sizeof(val) );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySetObj ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_PropertyDelete( hOBJECT object, tPROPID prop_id ) {
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A1( object, "Enter System::PropertyDelete(%tprop)", prop_id );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( !(prop_id & pRANGE_CUSTOM) ) {
		PR_TRACE(( object, prtERROR, "krn\tTrying to delete non custom property (%tprop)", prop_id ));
		error = errPROPERTY_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		error = _CustomPropertyDelete( po, handle, prop_id );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertyDelete ret %terr", error );
	return error;
}



// ---
tDWORD pr_call System_PropertySize( hOBJECT object, tPROPID prop_id ) {

	tDWORD   size;
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;

	PR_TRACE_A0( object, "Enter System::PropertySize" );
	enter_lf();
	
	if ( 0 == prop_id ) {
		PR_TRACE(( object, prtERROR, "krn\tRequest property: property identifier is 0." ));
		error = errPARAMETER_INVALID;
	}
	else if ( 0 != _HCP(handle,po,lock,object,&error) ) {
		if ( (prop_id & pRANGE_MASK) == pRANGE_CUSTOM )
			size = _CustomPropertySize( handle, prop_id );
		else
			size = _PropertySize( po, handle, prop_id );
		runlockc(po,lock);
	}
	else
		size = 0;
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertySize ret tDWORD = %u", size );
	return size;
}



// ---
tERROR pr_call System_PropertyEnumId( hOBJECT object, tPROPID* prop_id_result, tDWORD* cookie ) {
	tERROR   error;
	tHANDLE* handle;
	tDATA*   cprop = 0;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::PropertyEnumId" );
	enter_lf();
	
	_HCP( handle, po, lock, object, &error );
	if ( handle ) {
		tUINT  pcount;
		if ( cookie && (*cookie & 0x80000000l) ) {
			error = errPARAMETER_INVALID;
			PR_TRACE(( object, prtERROR, "krn\tThis is a CUSTOM property cookie passed (0x%x)", *cookie ));
		}
		else if ( 0 == (pcount=handle->iface->propcount) ) {
			error = errEND_OF_THE_LIST;
			PR_TRACE(( object, prtNOTIFY, "krn\tObject's interface has no properties implemented" ));
		}
		else if ( !cookie )
			cprop = handle->iface->proptable;
		else if ( *cookie >= pcount ) {
			error = errEND_OF_THE_LIST;
			PR_TRACE(( object, prtNOTIFY, "krn\tEnd of the property list reached" ));
		}
		else {
			cprop = handle->iface->proptable + (*cookie * PROP_ARRAY_SIZE);
			(*cookie)++;
		}
		if ( !prop_id_result ) 
			;
		else if ( cprop )
			*prop_id_result = PROP_IDENTIFIER(cprop);
		else
			*prop_id_result = 0;
		runlockc(po,lock);
	}
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyEnumId ret tPROPID = %tprop, error = %terr", (cprop ? PROP_ID(cprop) : 0), error );
	return error;
}



// ---
tERROR pr_call System_PropertyEnumCustomId( hOBJECT object, tPROPID* prop_id_result, tDWORD* cookie ) {
	tERROR     error;
	tHANDLE*   handle;
	tCustProp* cprop;
	tPO*       po;
	tUINT      position;
	tINT       lock = 1;
	
	PR_TRACE_A0( object, "Enter System::PropertyEnumCustomId" );
	enter_lf();
	
	_HCP( handle, po, lock, object, &error );
	if ( handle ) {
		if ( cookie && (*cookie) && !(*cookie & 0x80000000l) ) {
			cprop = 0;
			error = errPARAMETER_INVALID;
			PR_TRACE(( object, prtERROR, "krn\tThis is an IMPLEMENTED property cookie passed (0x%x)", *cookie ));
		}
		else if ( 0 == (cprop=handle->custom_prop) ) {
			error = errEND_OF_THE_LIST;
			PR_TRACE(( object, prtNOTIFY, "krn\tObject has no custom property set" ));
		}
		else if ( !cookie )
			;
		else if ( (position = (*cookie & 0x7fffffffl)) >= handle->custom_prop_count ) {
			error = errEND_OF_THE_LIST;
			PR_TRACE(( object, prtNOTIFY, "krn\tEnd of the custom property list reached" ));
		}
		else {
			cprop += position;
			(*cookie) = (position+1) | 0x80000000l;
		}
		if ( !prop_id_result ) 
			;
		else if ( cprop )
			*prop_id_result = cprop->prid;
		else
			*prop_id_result = 0;
		runlockc(po,lock);
	}
	leave_lf();
	PR_TRACE_A2( object, "Leave System::PropertyEnumCustomId ret tPROPID = %tprop, error = %terr", (cprop ? cprop->prid : 0), error );
	return error;
}



// ---
// cBROADCAST_AND             0x00 // result is true if all objects accepted broadcast
// cBROADCAST_OR              0x01 // result is true if at least one object accepted broadcast
// cBROADCAST_COUNT           0x02 // result is count of accepted broadcasts
// cBROADCAST_FIRST_LEVEL     0x10 // depth of broadcasts is one level down
// cBROADCAST_ALL_LEVELS      0x30 // depth of broadcasts is all levels down
// cBROADCAST_OBJECT_ITSELF   0x40 // message is sent to object itself
static tDWORD pr_call _PropBroadcast( tHANDLE* handle, tIID iid, tDWORD type, tPROPID prop_id, tPTR buffer, tDWORD p_count ) {

	tDWORD res;
	
	PR_TRACE_A0( MakeObject(handle), "Enter _PropBroadcast" );
	
	if ( (type & cBROADCAST_OBJECT_ITSELF) && CHECK_IID_C(handle,iid) )
		res = PR_SUCC(System_PropertySet(MakeObject(handle),0,prop_id,buffer,p_count));
	else
		res = 0;
	
	if ( handle->child ) {
		tHANDLE* h;
		for( h=handle->child; h; h=h->next ) {
			tDWORD again;
			if ( (type & cBROADCAST_ALL_LEVELS) == cBROADCAST_ALL_LEVELS )
				again = _PropBroadcast( h, iid, (type|cBROADCAST_OBJECT_ITSELF), prop_id, buffer, p_count );
			else if ( CHECK_IID_C(h,iid) )
				again = (errOK == System_PropertySet(MakeObject(h),0,prop_id,buffer,p_count));
			else
				continue;
			switch( type & (cBROADCAST_AND|cBROADCAST_OR|cBROADCAST_COUNT) ) {
				case cBROADCAST_AND   : res = res && again; break;
				case cBROADCAST_OR    : res = res || again; break;
				case cBROADCAST_COUNT :
				default               : res = res +  again; break;
			}
		}
	}
	
	PR_TRACE_A1( MakeObject(handle), "Leave _PropBroadcast ret tDWORD = %u", res );
	return res;
}




// ---
tDWORD pr_call System_PropertyBroadcast( hOBJECT object, tIID iid, tDWORD type, tPROPID prop_id, tPTR buffer, tDWORD p_count ) {

	tERROR   error;
	tDWORD   count = 0;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::PropertyBroadcast" );
	enter_lf();
	
	_HCP( handle, po, lock, object, &error );
	if ( handle ) {
		count = _PropBroadcast( handle, iid, type, prop_id, buffer, p_count );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::PropertyBroadcast ret tDWORD = %u", count );
	return count;
}




// ---
tPID pr_call _PropertyGetPID( tPO* po, tHANDLE* handle, tDATA* pid_prop_desc ) {
	tPID pid;
	
	if ( !handle || !handle->obj || (handle->flags & hf_HIDDEN) ) {
		PR_TRACE(( g_root, prtDANGER, "krn\tProcessed handle(%p) is broken!!!", handle ));
		return PID_NONE;
	}

	if ( PROP_MODE(pid_prop_desc) & cPROP_BUFFER_SHARED ) {
		if ( handle->iface->plugin && PR_SUCC(CALL_Plugin_GetInterfaceProperty(handle->iface->plugin,0,handle->iface->index,pgPLUGIN_ID,&pid,sizeof(pid))) ) 
			return pid;
		if ( PROP_MODE(pid_prop_desc) & (cPROP_BUFFER_SHARED_PTR & ~cPROP_BUFFER_SHARED) )
			return *(tPID*)PROP_BUFFER(pid_prop_desc);
		if ( PROP_MODE(pid_prop_desc) & (cPROP_BUFFER_SHARED_VAR & ~cPROP_BUFFER_SHARED) )
			return *(tPID*)(*(tDATA*)PROP_BUFFER(pid_prop_desc));
		return (tPID)_toi(PROP_BUFFER(pid_prop_desc));
	}
	
	if ( PROP_GETFN(pid_prop_desc) != NULL ) { // call GetProperty
		tERROR error;
		tDWORD pcount = 0;
		
		_PR_ENTER_PROTECTED_0(po) {
			error = PROP_GETFN(pid_prop_desc)( MakeObject(handle), &pcount, pgPLUGIN_ID, &pid, sizeof(pid) );
		} _PR_LEAVE_PROTECTED_0( po, error );
		return PR_SUCC(error) ? pid : PID_ANY;
	}
	
	if ( (PROP_MODE(pid_prop_desc) & cPROP_BUFFER_READ) )
		return *(tPID*)(((tBYTE*)(handle->obj+1))+PROP_OFFSET(pid_prop_desc));

  return PID_NONE; // ???
}



// ---
hHEAP _ObjHeap( tHANDLE* handle ) {
	while( handle && !handle->mem )
		handle = handle->parent;
	if ( handle && handle->mem )
		return handle->mem;
	return g_heap;
}


// ---
tERROR pr_call _PropertyGetSync( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size ) {
	tERROR err;
	tBOOL  prop_isnt_synchronized = PSYNC_OFF(handle);
	
	if ( prop_isnt_synchronized )
		handle->flags |= hf_PROP_SYNCHRONIZED;

	err = _PropertyGet( po, handle, out_size, prop, buffer, size );

	if ( prop_isnt_synchronized )
		handle->flags &= ~hf_PROP_SYNCHRONIZED;
	return err;
}


// ---
tERROR pr_call _PropertyGet( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop_id, tPTR buffer, tDWORD count ) {
	
	tERROR         error;
	tDWORD         type;
	tINTERFACE*    iface;
	const tDATA*   table;
	tDATA          localarray[PROP_ARRAY_SIZE];
	tQWORD         local_qword;
	tPTR           orig_buffer;
	tDWORD         orig_count;
	tBOOL          transfer;
	tDATA          pcount;
	tExceptionReg  reg;
	tIID           iid;
	
	PR_TRACE_A3( MakeObject(handle), "Enter _PropertyGet(%tprop, buffer=%p, count=%u)", prop_id, buffer, count );
	
	pcount = 0;
	if ( !buffer != !count ) {
		error = errPARAMETER_INVALID;
		goto exit;
	}
	
	type = 0;
	table = 0;
	error = errOK;
	transfer = cFALSE;
	orig_count = 0;
	orig_buffer = NULL;
	
	iface = handle->iface;
	if ( !iface ) {
		PROP_SIZE_SET( localarray, sizeof(tDWORD) );
		PROP_MODE_SET( localarray, cPROP_BUFFER_SHARED );
		table = localarray;
		
		switch( prop_id ) {
			case psOBJECT_OPERATIONAL       : PROP_BUFFER_SET( localarray, ((handle->flags & hf_OPERATIONAL_MODE) ? cTRUE : cFALSE) ); break;
			case psCALLBACK_SENDER          : PROP_BUFFER_SET( localarray, ((handle->flags & hf_CALLBACK_SENDER) ? cTRUE : cFALSE) ); break;
			case pgOBJECT_HEAP              : PROP_BUFFER_SET( localarray, handle->mem );    break;
			case pgTRACE_LEVEL              : PROP_BUFFER_SET( localarray, handle->trace_level.max ); break;
			case pgTRACE_LEVEL_MIN          : PROP_BUFFER_SET( localarray, handle->trace_level.min ); break;
			case pgOBJECT_USAGE_COUNT       : PROP_BUFFER_SET( localarray, /*handle->static_link ? (handle->static_link->ref-1) :*/ handle->ref ); break;
			case pgOBJECT_INHERITABLE_HEAP  : PROP_BUFFER_SET( localarray, _ObjHeap(handle) );    break;
			case pgOBJECT_PROP_SYNCHRONIZED : PROP_BUFFER_SET( localarray, !!PSYNC_ON(handle) ); break;
			case pgOBJECT_MEM_SYNCHRONIZED  : PROP_BUFFER_SET( localarray, !!MSYNC_ON(handle) ); break;
			case pgTRACE_OBJ                : PROP_BUFFER_SET( localarray, _TracerGet(po,handle,&error,0) ); break;
			default: 
				error = errINTERFACE_NOT_ASSIGNED_YET; // errINCOMPATIBLE_OBJECT ???
				goto exit;
		}
	}
	else {
		
		type = prop_id & pTYPE_MASK;
		
		if ( buffer != NULL ) {  // check parameter size
			tDWORD t = type >> pTYPE_SHIFT;
			if ( t > tid_LAST_TYPE ) {
				pcount = 0;
				error = errPROPERTY_INVALID;
				goto exit;
			}
			else if ( ((tDWORD)-1) == (pcount = _type_size[t]) )
				;
			else if ( count < pcount ) {
				pcount = 0;
				error = errBUFFER_TOO_SMALL;
				goto exit;
			}
		}
		
		PROP_SIZE_SET( localarray, sizeof(tDWORD) );
		PROP_MODE_SET( localarray, cPROP_BUFFER_SHARED );
		table = localarray;
		
		switch ( prop_id ) {
			case psOBJECT_OPERATIONAL      : PROP_BUFFER_SET( localarray, ((handle->flags & hf_OPERATIONAL_MODE) ? cTRUE : cFALSE) ); break;
			case psCALLBACK_SENDER         : PROP_BUFFER_SET( localarray, ((handle->flags & hf_CALLBACK_SENDER) ? cTRUE : cFALSE) ); break;
			case pgOBJECT_HEAP             : PROP_BUFFER_SET( localarray, handle->mem );    break;
			case pgINTERFACE_ID            : PROP_BUFFER_SET( localarray, iface->iid );     break;
			case pgINTERFACE_SUBTYPE       : PROP_BUFFER_SET( localarray, iface->subtype ); break;
			case pgINTERFACE_VERSION       : PROP_BUFFER_SET( localarray, iface->version ); break;
			case pgINTERFACE_FLAGS         : PROP_BUFFER_SET( localarray, iface->flags );   break;
			case pgINTERFACE_COMPATIBILITY : PROP_BUFFER_SET( localarray, iface->compat );  break;
			case psINTERFACE_COMPATIBILITY_BASE :
				error = _InterfaceCompatibleTopBase( handle->iface, &iid );
				if ( PR_SUCC(error) )
					PROP_BUFFER_SET( localarray, iface->compat );  
				else
					goto exit;
				break;
			case pgVENDOR_ID               : PROP_BUFFER_SET( localarray, iface->vid );     break;
			case pgPLUGIN_ID               : 
				rlock( po );
				PROP_BUFFER_SET( localarray, GET_PID(handle) );
				runlock( po );
				break;
			case pgTRACE_LEVEL             : PROP_BUFFER_SET( localarray, handle->trace_level.max ); break;
			case pgTRACE_LEVEL_MIN         : PROP_BUFFER_SET( localarray, handle->trace_level.min ); break;
			case pgTRACE_OBJ               : PROP_BUFFER_SET( localarray, _TracerGet(po,handle,&error,0) ); break;
			case pgPLUGIN_HANDLE           : PROP_BUFFER_SET( localarray, iface->plugin );  break;
			case pgOBJECT_USAGE_COUNT      : PROP_BUFFER_SET( localarray, /*handle->static_link ? (handle->static_link->ref-1) :*/ handle->ref ); break;
			case pgOBJECT_INHERITABLE_HEAP : PROP_BUFFER_SET( localarray, _ObjHeap(handle) );    break;
			case pgOBJECT_PROP_SYNCHRONIZED: PROP_BUFFER_SET( localarray, !!PSYNC_ON(handle) ); break;
			case pgOBJECT_MEM_SYNCHRONIZED : PROP_BUFFER_SET( localarray, !!MSYNC_ON(handle) ); break;
			case pgOBJECT_CODEPAGE         : 
				if ( !(iface->flags & IFACE_OWN_CP) ) { 
					PROP_BUFFER_SET( localarray, iface->cp );  
					break; 
				}
				
			default:
				
				table = _PropTableSearch( iface, prop_id, 0 );
				
				if ( !table ) {
					
					// convert from tDWORD to tQWORD and back in case of lack of original prop_id
					if ( type == pTYPE_DWORD ) {
						table = _PropTableSearch( iface, (prop_id&(~pTYPE_MASK))|pTYPE_QWORD, 0 );
						if ( table && buffer ) {
							orig_buffer = buffer;
							orig_count = count;
							buffer = &local_qword;
							count = sizeof( local_qword );
						}
					}
					else if ( type == pTYPE_QWORD ) {
						table = _PropTableSearch( iface, (prop_id&(~pTYPE_MASK))|pTYPE_DWORD, 0 );
						if ( table && buffer )
							mset( buffer, 0, count );
					}
					
					if ( !table ) {
						table = _PropTableSearch( iface, pgPROP_LAST_CALL, 0 );
						transfer = cTRUE;
					}
					
					if ( !table ) {
						if ( iface->flags & IFACE_PROP_TRANSFER ) {
							tINT lock = rlockc( po, PSYNC_ON(handle->parent) );
							error = _PropertyGet( po, handle->parent, &pcount, prop_id, buffer, count );
							runlockc( po, lock );
						}
						else {
							rlock(po);
							error = _InterfaceGetProp( po, iface, &pcount, prop_id, buffer, count );
							runlock(po);
						}
						goto exit;
					}
				}
				break;
		}
	}
	
	pcount = 0;
	
	if ( PROP_MODE(table) & cPROP_BUFFER_SHARED ) {
		tDWORD tmp = 0;
		if ( (prop_id != pgINTERFACE_COMMENT) || (table == localarray) || !handle->iface || !handle->iface->plugin || !PR_SUCC(handle->iface->plugin->vtbl->GetInterfaceProperty(handle->iface->plugin,&tmp,handle->iface->index,prop_id,buffer,count)) ) {
			pcount = PROP_SIZE(table); //_slen( table, type );
			if ( buffer != NULL ) {
				if ( pcount > count )
					pcount = count;
				if ( PROP_MODE(table) & (cPROP_BUFFER_SHARED_PTR & ~cPROP_BUFFER_SHARED) )
					mcpy( buffer, PROP_BUFFER(table), (tDWORD)pcount );
				else if ( PROP_MODE(table) & (cPROP_BUFFER_SHARED_VAR & ~cPROP_BUFFER_SHARED) )
					mcpy( buffer, (tSTRING)(*(tDATA*)PROP_BUFFER(table)), (tDWORD)pcount );
				else
					mcpy( buffer, &table[1], (tDWORD)pcount ); // PROP_BUFFER(table) !!! Palm OS doesn't accept it
			}
		}
		else
			pcount = tmp;
	}
	else {
		tBOOL from_buffer = cTRUE;
		if ( PROP_GETFN(table) != NULL ) { // call GetProperty
			tSHARE_LEVEL  lev;
			tINT   lock = rlunlockc( po, PSYNC_ON(handle), &lev );

			from_buffer = cFALSE;
			if ( errOK == si.EnterProtectedCode(0,&reg) ) {
				tDWORD tmp;
				error = PROP_GETFN(table)( MakeObject(handle), &tmp, prop_id, buffer, count );
				pcount = tmp;
			}
			else
				error = errUNEXPECTED;
			si.LeaveProtectedCode( 0, &reg );
			rllockc( po, lock, lev );
			if ( PR_FAIL(error) ) {
				pcount = 0;
				goto exit;
			}
		}
		else if ( (PROP_MODE(table) & cPROP_BUFFER_READ) )
			pcount = _llen( ((tBYTE*)(handle->obj+1))+PROP_OFFSET(table), (tDATA*)table, type );
		else {
			pcount = 0;
			error = errPROPERTY_NOT_READABLE;
		}
		
		if ( from_buffer && pcount && (PROP_MODE(table) & cPROP_BUFFER_READ) ) {
			if ( pcount > (tDWORD)PROP_SIZE(table) ) // cool exeption
				pcount = PROP_SIZE(table);
			if ( buffer ) {
				if ( pcount > count ) // copy value
					pcount = count;
				mcpy( buffer, ((tBYTE*)(handle->obj+1))+PROP_OFFSET(table), (tDWORD)pcount );
			}
		}
	}
	
	if ( orig_buffer != NULL ) {
		if ( local_qword <= 0xffffffff )
			mcpy( orig_buffer, &local_qword, orig_count );
		else {
			error = errBUFFER_TOO_SMALL;
			goto exit;
		}
	}
	
	if ( !pcount ) {
		if ( transfer && handle->iface && (handle->iface->flags & IFACE_PROP_TRANSFER) ) {
			tINT lock = rlockc( po, PSYNC_ON(handle->parent) );
			error = _PropertyGet( po, handle->parent, &pcount, prop_id, buffer, count );
			runlockc( po, lock );
			goto exit;
		}
		if ( iface->plugin && iface->index ) {
			tDWORD tmp;
			error = CALL_Plugin_GetInterfaceProperty( iface->plugin, &tmp, iface->index, prop_id, buffer, count );
			pcount = tmp;
			goto exit;
		}
	}
	
exit:
	if ( out_size )
		*out_size = pcount;
	
	if ( PR_FAIL(error) ) {
		if (PR_PROP_TYPE(prop_id) != pTYPE_CODEPAGE)
			PR_TRACE(( MakeObject(handle), (error == errPROPERTY_NOT_FOUND ? prtNOTIFY : prtERROR), "krn\t_PropertyGet returns tDWORD = %u, error = %terr", pcount, error ));
	}
	PR_TRACE_A2( MakeObject(handle), "Leave _PropertyGet ret tDWORD = %u, error = %terr", pcount, error );
	return error;
}



// ---
tERROR pr_call _PropertySet( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop_id, tPTR buffer, tDWORD count ) {
	
	tERROR         error;
	tINTERFACE*    iface;
	const tDATA*   table;
	tQWORD         local_qword;
	tDWORD         local_dword;
	tDWORD         init_prop_pos;
	tBOOL          transfer;
	tDATA          pcount;
	tExceptionReg  reg;
	tSHARE_LEVEL   lev;
	
	PR_TRACE_A3( MakeObject(handle), "Enter _PropertySet(%tprop, buffer=%p, count=%u)", prop_id, buffer, count );
	
	error = errOK;
	init_prop_pos = 0xffffffff;
	transfer = cFALSE;
	pcount = 0;
	
	switch( prop_id ) {
		case pgOBJECT_HEAP:
		case pgOBJECT_HEAP | pCALLBACK_TMP_STOP:
			if ( !buffer || (count!=sizeof(hHEAP)) )
				error = errPARAMETER_INVALID;
			
			else if ( handle->mem )
				error = errPROPERTY_NOT_WRITABLE;
			
			else {
				tUINT              i, count;
				tNotificationSink* sink;
				
				if ( PR_SUCC(error) ) {
					
					handle->mem = *(hHEAP*)buffer;
					pcount = sizeof(hHEAP);
					error = errOK;
					
					if ( !(prop_id & pCALLBACK_TMP_STOP) && (handle->flags & hf_CALLBACK_SENDER) && PR_SUCC(_ns_get_arr(cSNS_HEAP_SET,&sink,&count)) && sink ) {
						tSHARE_LEVEL  lev;
						tINT lock = rlunlockc( po, PSYNC_ON(handle), &lev );
						if ( errOK == si.EnterProtectedCode(0,&reg) ) {
							for( i=0; i<count; i++,sink++ )
								sink->func( MakeObject(handle), *(hHEAP*)buffer, sink->ctx );
						}
						si.LeaveProtectedCode( 0, &reg );
						rllockc( po, lock, lev );
					}
				}
			}
			goto exit;
			break;
			
		case psCALLBACK_SENDER :
			if ( !buffer || (count!=sizeof(tBOOL)) )
				error = errPARAMETER_INVALID;
			else if ( (error=errOK), *(tBOOL*)buffer )
				handle->flags |= hf_CALLBACK_SENDER;
			else
				handle->flags &= ~hf_CALLBACK_SENDER;
			goto exit;
			break;
			
		case psOBJECT_OPERATIONAL      :
		case pgOBJECT_USAGE_COUNT      :
		case pgINTERFACE_ID            :
		case pgINTERFACE_SUBTYPE       :
		case pgINTERFACE_VERSION       :
		case pgINTERFACE_FLAGS         :
		case pgINTERFACE_COMPATIBILITY :
		case psINTERFACE_COMPATIBILITY_BASE :
		case pgVENDOR_ID               :
		case pgPLUGIN_ID               :
			if ( handle->iface && handle->iface->pid_user )
				break;
		case pgPLUGIN_HANDLE           :
			error = errPROPERTY_NOT_WRITABLE;
			goto exit;
			break;
			
		case pgTRACE_LEVEL :
			pcount = sizeof(tDWORD);
			if ( buffer ) {
				if ( count < sizeof(tDWORD) )
					error = errBUFFER_TOO_SMALL;
				else 
					_TraceLevelPropSet( po, handle, *(tDWORD*)buffer, prtDO_NOT_CHANGE_TRACE_LEVEL );
			} 
			goto exit;
			break;
			
		case pgTRACE_LEVEL_MIN :
			pcount = sizeof(tDWORD);
			if ( buffer ) {
				if ( count < pcount )
					error = errBUFFER_TOO_SMALL;
				else 
					_TraceLevelPropSet( po, handle, prtDO_NOT_CHANGE_TRACE_LEVEL, *(tDWORD*)buffer );
			} 
			goto exit;
			break;
			
		case pgTRACE_OBJ :
			pcount = sizeof(hTRACER);
			if ( buffer ) {
				if ( count < pcount )
					error = errBUFFER_TOO_SMALL;
				else {
					tHANDLE* tr = MakeHandle( *(hOBJECT*)buffer );
					if ( !tr || !tr->iface )
						error = errOBJECT_INCOMPATIBLE;
					else
						error = _InterfaceCompatible( tr->iface, IID_TRACER );
					if ( PR_SUCC(error) )
						handle->tracer = *(hTRACER*)buffer;
				}
			}
			goto exit;
			break;

		case pgOBJECT_PROP_SYNCHRONIZED :
			pcount = sizeof(tBOOL);
			if ( buffer ) {
				if ( count < pcount )
					error = errBUFFER_TOO_SMALL;
				else if ( *(tBOOL*)buffer )
					handle->flags |= hf_PROP_SYNCHRONIZED;
				else
					handle->flags &= ~hf_PROP_SYNCHRONIZED;
			}
			goto exit;
			break;

		case pgOBJECT_MEM_SYNCHRONIZED :
			pcount = sizeof(tBOOL);
			if ( buffer ) {
				if ( count < pcount )
					error = errBUFFER_TOO_SMALL;
				else if ( *(tBOOL*)buffer )
					handle->flags |= hf_MEM_SYNCHRONIZED;
				else
					handle->flags &= ~hf_MEM_SYNCHRONIZED;
			}
			goto exit;
			break;

		case pgOBJECT_CODEPAGE         :
			if ( !handle->iface || !(handle->iface->flags & IFACE_OWN_CP) ) {
				error = errPROPERTY_NOT_WRITABLE;
				goto exit;
				break;
			} // else: try set own CP
			
		default:
			error = errOK;
			break;
	}
	
	iface = handle->iface;
	if ( iface ) {
		tDWORD type = prop_id & pTYPE_MASK;
		
		if ( buffer ) {  // check parameter size
			tHANDLE* h;
			switch( type ) {
				case pTYPE_OBJECT :
				case pTYPE_IFACEPTR :
					if ( count < sizeof(hOBJECT) ) {
						error = errBUFFER_TOO_SMALL; // errINCOMPATIBLE_OBJECT ???
						goto exit;
					}
					if ( *((hOBJECT*)buffer) && !_HC(h,0,*(hOBJECT*)buffer,&error) ) {
						//error = errPARAMETER_INVALID; // errINCOMPATIBLE_OBJECT ???
						goto exit;
					}
					break;
					CHECK_TYPE(BYTE);       CHECK_TYPE(WORD);     CHECK_TYPE(DWORD); 
					CHECK_TYPE(CODEPAGE);   CHECK_TYPE(DATETIME); CHECK_TYPE(INT);    //CHECK_TYPE(PTR);
					CHECK_TYPE(BOOL);       CHECK_TYPE(CHAR);     CHECK_TYPE(WCHAR);
					CHECK_TYPE(ERROR);      CHECK_TYPE(UINT);     CHECK_TYPE(SBYTE);
					CHECK_TYPE(SHORT);      CHECK_TYPE(LONG);     CHECK_TYPE(LONGLONG);
					CHECK_TYPE(IID);        CHECK_TYPE(PID);
					CHECK_TYPE(ORIG_ID);
					CHECK_TYPE(OS_ID);
					CHECK_TYPE(VID);        CHECK_TYPE(PROPID);   CHECK_TYPE(VERSION);
					CHECK_TYPE(DATA);
				default :
					pcount = 0;
					break;
			}
			
			if ( count < pcount ) {
				error = errBUFFER_TOO_SMALL;
				goto exit;
			}
		}
		
		pcount = 0;
		table = _PropTableSearch( iface, prop_id, &init_prop_pos );
		
		if ( table == NULL ) { // if property not found try anothe ways for this property
			
			// convert from tDWORD to tQWORD and back in case of lack of original prop_id
			if ( type == pTYPE_DWORD ) {
				table = _PropTableSearch( iface, (prop_id&(~pTYPE_MASK))|pTYPE_QWORD, &init_prop_pos );
				if ( table && buffer ) {
					local_qword = *(tDWORD*)buffer;
					buffer = &local_qword;
					count = sizeof( local_qword );
				}
			}
			else if ( type == pTYPE_QWORD ) {
				
				if ( *(tQWORD*)buffer <= 0xffffffff ) {
					table = _PropTableSearch( iface, (prop_id&(~pTYPE_MASK))|pTYPE_DWORD, &init_prop_pos );
					if ( table ) {
						local_dword = (tDWORD)(*(tQWORD*)buffer);
						buffer = &local_dword;
						count = sizeof( local_dword );
					}
				}
				else {
					error = errBUFFER_TOO_SMALL;
					goto exit;
				}
			}
			if ( !table ) {
				table = _PropTableSearch( iface, pgPROP_LAST_CALL, &init_prop_pos );
				transfer = cTRUE;
			}
			
			if ( table == NULL ) {
				if ( iface->flags & IFACE_PROP_TRANSFER )
					error = _PropertySet( po, handle->parent, &pcount, prop_id, buffer, count );
				else
					error = errPROPERTY_NOT_FOUND;
				goto exit;
			}
		}
		
		if ( table ) { // property has been found !
			
			if (
				((PROP_MODE(table) & cPROP_BUFFER_SHARED) != 0) || // shared buffer RO !!!
				(((PROP_MODE(table) & cPROP_WRITABLE_ON_INIT) != 0) && ((handle->flags & hf_OPERATIONAL_MODE) != 0)) // property cannot be set in this operational mode
				) {
				error = errPROPERTY_NOT_WRITABLE;
				goto exit;
			}
			
			if ( buffer ) {
				// get value ftom the buffer if possible
				if ( PROP_MODE(table) & (cPROP_BUFFER_WRITE|cPROP_WRITABLE_ON_INIT) ) {
					if ( count < PROP_SIZE(table) )
						pcount = count;
					else
						pcount = PROP_SIZE(table);
					mcpy( ((tBYTE*)(handle->obj+1)+PROP_OFFSET(table)), buffer, (tDWORD)pcount ); // copy value
				}
				else
					error = errPROPERTY_NOT_WRITABLE;
				
				// then call "SET" function
				if ( PROP_SETFN(table) ) { // call SetProperty
					tINT lock = rlunlockc( po, PSYNC_ON(handle), &lev );
					if ( errOK == si.EnterProtectedCode(0,&reg) ) {
						tDWORD tmp;
						error = PROP_SETFN(table)( MakeObject(handle), &tmp, prop_id, buffer, count );
						pcount = tmp;
					}
					else
						error = errUNEXPECTED;
					si.LeaveProtectedCode( 0, &reg );
					rllockc( po, lock, lev );
				}
			}
			
			else if ( PROP_SETFN(table) ) {
				tINT lock = rlunlockc( po, PSYNC_ON(handle), &lev );
				if ( errOK == si.EnterProtectedCode(0,&reg) ) {
					tDWORD tmp;
					error = PROP_SETFN(table)( MakeObject(handle), &tmp, prop_id, 0, 0 );
					pcount = tmp;
				}
				else
					error = errUNEXPECTED;
				si.LeaveProtectedCode( 0, &reg );
				rllockc( po, lock, lev );
			}
			
			else if ( PROP_MODE(table) & cPROP_BUFFER_WRITE )
				pcount = PROP_SIZE(table);
			
			else
				error = errPROPERTY_NOT_WRITABLE;
		}
	}
	else {
		error = errHANDLE_INVALID; // errINCOMPATIBLE_OBJECT ???
		goto exit;
	}

	if ( pcount ) {
		if ( init_prop_pos < 32 )
			handle->init_props &= ~( 1 << init_prop_pos );
	}
	else if ( transfer && handle->iface && (handle->iface->flags & IFACE_PROP_TRANSFER) )
	error = _PropertySet( po, handle->parent, &pcount, prop_id, buffer, count );

	exit:
	if ( out_size )
	*out_size = pcount;

	if ( PR_FAIL(error) ) {
		//    PR_TRACE(( MakeObject(handle), (error == errPROPERTY_NOT_FOUND ? prtIMPORTANT : prtERROR), "krn\t_PropertySet returns tDWORD = %u, error = %terr", pcount, error ));
		PR_TRACE(( MakeObject(handle), prtERROR, "krn\t_PropertySet returns tDWORD = %u, error = %terr", pcount, error ));
	}

	PR_TRACE_A2( MakeObject(handle), "Leave _PropertySet ret tDWORD = %u, error = %terr", pcount, error );
	return error;
}



// ---
tDWORD pr_call _PropertySize( tPO* po, tHANDLE* handle, tPROPID prop_id ) {
	
	tERROR         error = errOK;
	tDWORD         size=0;
	tINTERFACE*    iface;
	const tDATA*   table;
	tDWORD         type;
	tExceptionReg  reg;
	
	PR_TRACE_A1( MakeObject(handle), "Enter _PropertySize(%tprop)", prop_id );
	
	type = prop_id & pTYPE_MASK;
	
	iface = handle->iface;
	if ( iface ) {
		
		table = _PropTableSearch( iface, prop_id, 0 );
		
		if ( !table ) {
			// convert from tDWORD to tQWORD and back in case of lack of original prop_id
			if ( type == pTYPE_DWORD )
				table = _PropTableSearch( iface, type=(prop_id&(~pTYPE_MASK))|pTYPE_QWORD, 0 );
			else if ( type == pTYPE_QWORD )
				table = _PropTableSearch( iface, type=(prop_id&(~pTYPE_MASK))|pTYPE_DWORD, 0 );
			if ( !table )
				table = _PropTableSearch( iface, pgPROP_LAST_CALL, 0 );
			if ( !table ) {
				if ( iface && (iface->flags & IFACE_PROP_TRANSFER) )
					size = _PropertySize( po, handle->parent, prop_id );
				else if ( iface->plugin && iface->index )
					error = CALL_Plugin_GetInterfaceProperty( iface->plugin, &size, iface->index, prop_id, 0, 0 );
				else
					size = 0;
				goto exit;
			}
		}
	}
	else { // if ( (iface = handle->iface) != 0 )
		error = errHANDLE_INVALID; // errINCOMPATIBLE_OBJECT ???
		goto exit;
	}
	
	if ( PROP_MODE(table) & cPROP_BUFFER_SHARED )
		size = (tDWORD)PROP_SIZE(table);
	
	else {
		
#define RETURN_TYPE_SIZE(a) \
case pTYPE_##a : \
	PR_TRACE_A1( MakeObject(handle), "Leave _PropertySize ret tDWORD = %u, error = errOK", sizeof(t##a) ); \
		return sizeof(t##a);
		
		if ( PROP_GETFN(table) != NULL || (PROP_MODE(table) & cPROP_BUFFER_READ) ) {
			// return immidiately if predefined size
			switch( type ) {
				RETURN_TYPE_SIZE(BYTE);       RETURN_TYPE_SIZE(WORD);
				RETURN_TYPE_SIZE(DWORD);      RETURN_TYPE_SIZE(QWORD);
				RETURN_TYPE_SIZE(CODEPAGE);   RETURN_TYPE_SIZE(DATETIME);
				RETURN_TYPE_SIZE(INT);
				
				case pTYPE_IFACEPTR :
				case pTYPE_OBJECT   :
					PR_TRACE_A1( MakeObject(handle), "Leave _PropertySize ret tDWORD = %u, error = errOK", sizeof(tVOID*) );
					return sizeof(tVOID*);
					//RETURN_TYPE_SIZE(PTR);
				
				RETURN_TYPE_SIZE(BOOL);       RETURN_TYPE_SIZE(CHAR);
				RETURN_TYPE_SIZE(WCHAR);      RETURN_TYPE_SIZE(ERROR);
				RETURN_TYPE_SIZE(UINT);       RETURN_TYPE_SIZE(SBYTE);
				RETURN_TYPE_SIZE(SHORT);      RETURN_TYPE_SIZE(LONG);
				RETURN_TYPE_SIZE(LONGLONG);   RETURN_TYPE_SIZE(IID);
				RETURN_TYPE_SIZE(PID);        RETURN_TYPE_SIZE(ORIG_ID);
				RETURN_TYPE_SIZE(OS_ID);      RETURN_TYPE_SIZE(VID);
				RETURN_TYPE_SIZE(PROPID);     RETURN_TYPE_SIZE(VERSION);
				RETURN_TYPE_SIZE(DATA);
			}
		}
		
#undef RETURN_TYPE_SIZE
		
		// call GetProperty
		if ( PROP_GETFN(table) != NULL ) {
			tSHARE_LEVEL  lev;
			tINT lock = rlunlockc( po, PSYNC_ON(handle), &lev );
			if ( errOK == si.EnterProtectedCode(0,&reg) )
				error = PROP_GETFN(table)( MakeObject(handle), &size, prop_id, 0, 0 );
			else
				error = errUNEXPECTED;
			si.LeaveProtectedCode( 0, &reg );
			rllockc( po, lock, lev );
		}
		else if ( (PROP_MODE(table) & cPROP_BUFFER_READ) )
			size = (tDWORD)PROP_SIZE(table);
		else
			size = 0;
	}
	
exit:
	if ( (errOK == error) && (size == 0) )
		error = errPROPERTY_NOT_FOUND;
	
	PR_TRACE_A2( MakeObject(handle), "Leave _PropertySize ret tDWORD = %u, error = %terr", size, error );
	return size;
}




// ---
/*
tDWORD _slen( tDATA* table, tDWORD type ) {
  tDWORD i, m = PROP_SIZE(table);
  if ( type == pTYPE_STRING ) {
    tSTRING s = (tSTRING)PROP_BUFFER(table);
    for( i=0; s[i] && i<m; i++ );
  }
  else if ( type == pTYPE_WSTRING ) {
    tWSTRING s = (tWSTRING)PROP_BUFFER(table);
    for( i=0; s[i] && i<m; i++ );
  }
  else
    i = m;
  return i;
}
*/



// ---
static tDWORD _llen( tVOID* buffer, tDATA* table, tDWORD type ) {
	tDWORD i, m = (tDWORD)PROP_SIZE(table);
	if ( type == pTYPE_STRING ) {
		tCHAR* pchar = (tCHAR*)buffer;
		for( i=0; *pchar && i<(m-1); i++ )
			;
		i++;
	}
	else if ( type == pTYPE_WSTRING ) {
		tWCHAR* pwchar = (tWCHAR*)buffer;
		m /= sizeof(tWCHAR);
		for( i=0; *pwchar && i<m; i++ )
			;
		i++;
		i *= sizeof(tWCHAR);
	}
	else
		i = m;
	return i;
}



