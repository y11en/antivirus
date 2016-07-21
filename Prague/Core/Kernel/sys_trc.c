/*----------------- 04.11.00 16:00 ----------------
 * Project Prague                                 *
 * Subproject Kernel interfaces                   *
 * Author Petrovitch                              *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague System API                      *
 *                                                *
--------------------------------------------------*/


#include "kernel.h"

#define FL(m,f)  ( (f) == ((who) & (m)) )
#define FE(f)    FL(f,f)

// ---
tERROR pr_call System_TraceLevelGet( hOBJECT object, tDWORD who, tDWORD* max_level, tDWORD* min_level ) {

  tERROR   error;
  tHANDLE* handle;
  tPO*     po;
	tINT     lock = 1;
  
  PR_TRACE_A0( object, "Enter \"System::TraceLevelGet\"" );
  enter_lf();

  _HCC( handle, po, lock, object, &error );
  if ( handle ) {
		error = errPARAMETER_INVALID;
		if ( who == tlsALL_OBJECTS ) {
			PR_TRACE(( object, prtERROR, "krn\tParameter \"who==tlsALL_OBJECTS\" is invalid for \"TraceLevelGet\"" ));
		}
    if ( FL(tlsID_MASK,tlsTHIS_OBJECT) ) {
      if ( max_level || min_level ) {
        error = errOK;
        if ( max_level ) 
          error = _PropertyGetSync( po, handle, 0, pgTRACE_LEVEL, max_level, sizeof(tDWORD) );
        if ( PR_SUCC(error) && min_level )
          error = _PropertyGetSync( po, handle, 0, pgTRACE_LEVEL_MIN, min_level, sizeof(tDWORD) );
      }
    }
    else if ( FE(tlsTHIS_IFACE) || FE(tlsTHIS_IFACE|tlsTHIS_SUBTYPE) ) {
      if ( min_level || max_level ) {
        error = errOK;
        if ( min_level ) {
          *min_level = handle->iface->trace_level.min;
          error = errOK;
        }
        if ( max_level ) {
          *max_level = handle->iface->trace_level.max;
          error = errOK;
        }
      }
    }
    runlockc(po,lock);
  }

  leave_lf();
  PR_TRACE_A1( object, "Leave \"System::TraceLevelGet\" ret ?, error = %terr", error );
  return error;
}



// ---
tERROR pr_call System_TraceLevelSet( hOBJECT object, tDWORD who, tDWORD max_level, tDWORD min_level ) {
	
  tERROR   error;
  tHANDLE* handle;
  tPO*     po;
	tINT     lock = 1;
  
  //PR_TRACE_A0( object, "Enter \"System::TraceLevelSet\"" );
  enter_lf();
	
  _HCC( handle, po, lock, object, &error );
  if ( handle ) {
    error = _TraceLevelSetHandle( po, handle, who, max_level, min_level );
    runlockc(po,lock);
  }
	
  leave_lf();
  //PR_TRACE_A1( object, "Leave \"System::TraceLevelSet\" ret ?, error = %terr", error );
  return error;
}



// ---
tERROR pr_call System_TracerGet( hOBJECT object, hTRACER* result ) {
  tERROR   error;
  tHANDLE* handle;
	hTRACER  tracer = 0;
  tPO*     po;
	tINT     lock = 1;
  
  PR_TRACE_A0( object, "Enter \"System::TracerGet\"" );
  enter_lf();
	
  _HCC( handle, po, lock, object, &error );
	if ( handle ) {
		tHANDLE* h = handle;
		while( h ) {
			if ( h->tracer ) {
				tracer = h->tracer;
				break;
			}
			h = h->parent;
		}
		runlockc(po,lock);
	}

  if ( result )
    *result = tracer;
  
  if ( !tracer )
    error = errOBJECT_NOT_FOUND;
  
  leave_lf();
  PR_TRACE_A2( object, "Leave \"System::TracerGet\" ret hTRASER = %p, error = %terr", (result ? *result : 0), error );
  return error;
}



// ---
tERROR pr_call _TraceLevelSetHandle( tPO* po, tHANDLE* handle, tDWORD who, tDWORD max_level, tDWORD min_level ) {

  tERROR error;

  //PR_TRACE_A0( MakeObject(handle), "Enter \"_TraceLevelSetHandle\"" );

  if ( CHECK_IID_C(handle,IID_NONE) )
    error = errOBJECT_ALREADY_FREED;

  else if ( FL(tlsID_MASK,tlsTHIS_OBJECT) ) 
    error = _TraceLevelPropSet( po, handle, max_level, min_level );

  else if ( handle->iface )
    error = _TraceLevelSetID( po, who, GET_IID(handle), GET_PID(handle), handle->iface->subtype, handle->iface->vid, max_level, min_level );

  else
    error = errINTERFACE_INCOMPATIBLE;

  //PR_TRACE_A1( MakeObject(handle), "Leave \"_TraceLevelSetHandle\" error = %terr", error );
  return error;
}



// ---
typedef struct tag_FHP {
  tIID    i;
  tPID    p;
  tDWORD  s;
  tVID    v;
} FHP;


// ---
static tBOOL FindHandle( tHANDLE* h, tPTR p ) {
  #define FHPT(a) ((FHP*)a)
  return 
		( !CHECK_IID_C(h,IID_TRACER) && CHECK_C(h,FHPT(p)->i,FHPT(p)->p) && \
    ( (FHPT(p)->s == SUBTYPE_ANY) || (h->iface && (FHPT(p)->s == h->iface->subtype)) ) &&
    ( (FHPT(p)->v == VID_ANY)     || (h->iface && (FHPT(p)->s == h->iface->vid)) ) );
  #undef FHPT
}


// ---
tERROR pr_call _TraceLevelSetID( tPO* po, tDWORD who, tIID iid, tPID pid, tDWORD sub, tVID vid, tDWORD max_level, tDWORD min_level ) {

  tDWORD c = 0;
  tERROR e = errPARAMETER_INVALID;

  //PR_TRACE_A0( 0, "Enter \"_TraceLevelSetID\"" );

  if ( FE(tlsTHIS_IFACE) ) {
    if ( (iid == IID_ANY) || !FE(tlsTHIS_SUBTYPE) ) 
      sub = SUBTYPE_ANY;
  }
  else {
    iid = IID_ANY;
    sub = SUBTYPE_ANY;
  }
  if ( !FE(tlsTHIS_PLUGIN) )
    pid = PID_ANY;
  if ( !FE(tlsTHIS_VENDOR) )
    vid = VID_ANY;

  if ( FE(tlsNEW_ONLY) || FE(tlsAND_NEW) ) { // new objects are affected
    tINTERFACE* iface = _InterfaceFind( &c, iid, pid, sub, vid, cTRUE );
    while( iface ) {
      wlock(po);
      iface->trace_level.min = min_level;
      iface->trace_level.max = max_level;
      wunlock(po);
      iface = _InterfaceFind( &c, iid, pid, sub, vid, cTRUE );
    }
    e = errOK;
  }
  if ( !FE(tlsNEW_ONLY) ) { // existing objects are affected
    tHANDLE* h;
    FHP      f;

    c = 0;
    f.i = iid;
    f.p = pid;
    f.s = sub;
    f.v = vid;
    h = _HandleFindStat( po, &c, FindHandle, &f );
    while( h ) {
      _TraceLevelSetHandle( po, h, tlsTHIS_OBJECT, max_level, min_level );
      h = _HandleFindStat( po, &c, FindHandle, &f );
    }
    e = errOK;
  }

  //PR_TRACE_A1( 0, "Leave \"_TraceLevelSetID\" ret ?, error = %terr", e );
  return e;
}



// ---
hTRACER pr_call _TracerGet( tPO* po, tHANDLE* handle, tERROR* error, tDWORD flags ) {
  tHANDLE* h = handle;
	
	while( h ) {
		if ( h->tracer ) {
			h = MakeHandle( h->tracer );
			break;
		}
		h = h->parent;
	}

	if ( !h )
		h = MakeHandle(g_tracer);

  if ( !h && (flags & TRACER_GET_CREATE_IF_NOT_EXIST) && PR_SUCC(*error=_ObjectIIDCreate(po,handle,&h,IID_TRACER,PID_ANY,0)) && h ) {
		tDWORD max_trace_level = handle->trace_level.max;
		tDWORD min_trace_level = handle->trace_level.min;
		_PropertySet( po, h, 0, pgTRACE_LEVEL,     &max_trace_level, sizeof(max_trace_level) );
		_PropertySet( po, h, 0, pgTRACE_LEVEL_MIN, &min_trace_level, sizeof(min_trace_level) );
		if ( PR_SUCC(*error=_ObjectCreateDone(po,h)) )
			*error = CALL_Tracer_Start( (hTRACER)MakeObject(h), 0 );
		else
			_ObjectClose(po,h,0), h=0;
  }
  return (hTRACER)MakeObject( h );
}



// ---
static tERROR pr_call _TraceWrapper( hOBJECT obj, ... ) {
	tHANDLE* handle = MakeHandle(obj);
	typedef tERROR (pr_call *FuncPtr)(hOBJECT,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD );
	FuncPtr fptr = (FuncPtr)_WrapperNext( handle );
  if ( fptr ) {
		int i;
		tIID  iid = GET_IID_C(handle);
		tPID  pid = GET_PID_C(handle);
		tUINT num = handle->ext ? ((handle->ext->wrap_data) & 0xffff) : 0xffff;
		tERROR error;
		tDWORD p[20];
		va_list va;
		va_start( va, obj );
		for( i=0; i<(sizeof(p)/sizeof(p[0])); i++ )
			p[i] = va_arg( va, tDWORD );
		va_end( va );

		pr_trace( obj, prtFUNCTION_FRAME, "Enter %tiid(%tpid)::method %u", iid, pid, num );
		error = fptr( obj, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15], p[16], p[17], p[18], p[19] );
		pr_trace( obj, prtFUNCTION_FRAME, "Leave %tiid(%tpid)::method %u", iid, pid, num );
		return error;
  }
  return errOK;
}



// ---
static const tPTR OneMethod( tHANDLE* handle, tPTR* param, tBOOL ex_in, tDWORD num, tDWORD* data ) {
  *data = 0;
  return &_TraceWrapper;
}



// ---
tERROR pr_call _TraceLevelPropSet( tPO* po, tHANDLE* handle, tDWORD max_level, tDWORD min_level ) {

  tERROR error = errPARAMETER_INVALID;

	if ( CHECK_IID_C(handle,IID_TRACER) || (handle->parent && CHECK_IID_C(handle->parent,IID_TRACER)) )
		return errOK;

	if ( ((tINT)min_level) > ((tINT)max_level) ) {
		min_level ^= max_level;
		max_level ^= min_level;
		min_level ^= max_level;
	}

	if ( max_level != prtDO_NOT_CHANGE_TRACE_LEVEL ) {
    handle->trace_level.max = max_level;
    error = errOK;
  }

  if ( min_level != prtDO_NOT_CHANGE_TRACE_LEVEL ) {
    handle->trace_level.min = min_level;
    error = errOK;
  }

  if ( errOK == error ) {
		hTRACER tr;
    if ( handle->trace_level.max == prtMIN_TRACE_LEVEL )
      tr = handle->tracer = 0;
    else if ( !handle->tracer ) 
      tr = handle->tracer = _TracerGet( po, handle, &error, TRACER_GET_CREATE_IF_NOT_EXIST );
		else
			tr = handle->tracer;

		if ( tr ) {
			if ( handle == g_hRoot ) {
				tHANDLE* htr = MakeHandle(tr);
				htr->trace_level = handle->trace_level;
			}
			if ( errOK != CALL_Tracer_IsRunning(tr) )
				CALL_Tracer_Start( tr, 0 );
		}

    if ( HDL_TR_CHK(handle,prtFUNCTION_FRAME) ) 
      _WrapperAdd( po, handle, (OneFuncPtr)OneMethod, 0 );
    else if ( PR_FAIL(_WrapperWithdraw(po,handle,(OneFuncPtr)OneMethod,0)) && handle->obj )
      _set_vtbl( handle, 0 );
  }

  return error;
}


