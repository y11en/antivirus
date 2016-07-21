/*----------------- 04.11.00 16:00 ----------------
 * Project Prague                                 *
 * Subproject Kernel                              *
 * Author Petrovitch                              *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague System API                      *
 *                                                *
--------------------------------------------------*/


#include "kernel.h"

#if defined (__unix__) || defined (__MWERKS__) || defined(_Wp64) || defined(_WIN64)
	tERROR pr_call _WrapperAdd( tPO* po, tHANDLE* handle, OneFuncPtr addr_func, tPTR param ) {
		return errNOT_IMPLEMENTED;
	}


	tERROR pr_call _WrapperWithdraw( tPO* po, tHANDLE* handle, OneFuncPtr addr_func, tPTR param ){
		return errNOT_IMPLEMENTED;
	}

	tPTR pr_call _WrapperNext( tHANDLE* handle ) {
		return 0;
	}
	tPTR pr_call _WrapperData( tHANDLE* handle ) {
		return 0;
	}
	tERROR pr_call _WrapperCallNext( hOBJECT* obj, ... ) {
		return errNOT_IMPLEMENTED;
	}
#else

#include <m_utils.h>

// code template
static const tBYTE g_template[] = {
  0x8b, 0x44, 0x24, 0x04,                   // mov eax,dword ptr [esp+4]  mov eax, object(_this)          8B 44 24 04
  0x8b, 0x40, 0xfc,                         // mov eax,dword ptr [eax-4]  mov eax, handle(_this-1)        8B 40 FC
  0x8b, 0x40, 0x1c,                         // mov eax,dword ptr [eax+1c] mov eax, handle->ext            8B 40 1C
  0xc7, 0x40, 0x14, 0x00, 0x00, 0x00, 0x00, // mov dword ptr[eax+14],0    ((tHANDLEX*)eax).wrap_data = 0  C7 40 c0 00 00 00 00
  0xc7, 0x40, 0x18, 0x00, 0x00, 0x00, 0x00, // mov dword ptr[eax+18],0    ((tHANDLEX*)eax).wrap_data2 = 0 C7 40 c4 00 00 00 00
  0xe9, 0x00, 0x00, 0x00, 0x00              // jmp 0
};


// ---
typedef tBYTE tMETHOD [sizeof(g_template)];


// Wrapper structure
struct tag_WRAPPER {
  struct tag_WRAPPER* prev;
  struct tag_WRAPPER* next;
  OneFuncPtr          m_func;
	tVOID*              m_params;
  tDWORD              m_num;
  tMETHOD*            m_methods;
};


// ---
typedef struct tag_VTableCache {
	tUINT       m_ref;
	OneFuncPtr  m_func;
	tUINT       m_num;
	tMETHOD*    m_methods;
} tVTableCache;



#define       VTABLE_CACHE_CLUSTER (0x40)
tVTableCache  g_vtable_static[VTABLE_CACHE_CLUSTER];
tUINT         g_vtable_count = 0;
tVTableCache* g_vtable_ptr = g_vtable_static;



// ---
tBOOL FindHandleByStaticHandle( tHANDLE* h, tPTR p );
tBOOL FindHandleByIfacePtr( tHANDLE* h, tPTR p );


#define ADDR_1( c )  ( ((tDWORD*)&((*(tMETHOD*)c)[13])) )
#define ADDR_2( c )  ( ((tDWORD*)&((*(tMETHOD*)c)[20])) )
#define ADDR_3( c )  ( ((tDWORD*)&((*(tMETHOD*)c)[25])) )

#define ABS_ADDR( a )     ((tPTR)(((int)( ((tDWORD)a) + ((tDWORD)&a) )) + 4))
#define REL_ADDR( v, a )  (v=((tPTR)(tDWORD)( ((int)( ((tDWORD)a) - ((tDWORD)&v) )) - 4)))

#ifdef _DEBUG
  tPTR pr_call METHOD_SET( tMETHOD* m, tUINT num, tUINT ind, tDWORD data, tPTR addr ) {
    mcpy( m, g_template, sizeof(g_template) );
    *ADDR_1(m) = (num<<16) + (ind&0xffff);
    *ADDR_2(m) = data;
    REL_ADDR( *(tPTR*)ADDR_3(m), addr );
    return m;
  }
#else 
  #define METHOD_SET( m, n, i, d, a )     \
     (mcpy(m,g_template,sizeof(g_template)),        \
      *ADDR_1(m)=(((n)<<16)|(i&0xffff)),  \
      *ADDR_2(m)=(d),                     \
      REL_ADDR( *(tPTR*)ADDR_3(m), a ), (m))
#endif



// ---
tERROR _get_vtable( tHANDLE* handle, tWRAPPER* novel, const tWRAPPER* old ) {
	tUINT         i;
	tERROR        err;
	tINTERFACE*   iface = handle->iface;
	tVTableCache* pvt = g_vtable_ptr;
	
	for( i=0; i<g_vtable_count; ++i,++pvt ) {
		if ( (pvt->m_func == novel->m_func) && (pvt->m_num == novel->m_num) ) {
			++pvt->m_ref;
			novel->m_methods = pvt->m_methods;
			return errOK;
		}
	}

	if ( g_vtable_count && (0 == (g_vtable_count%VTABLE_CACHE_CLUSTER)) ) {
		if ( g_vtable_count == VTABLE_CACHE_CLUSTER ) {
			err = PrAlloc( (tPTR*)&pvt, 2*VTABLE_CACHE_CLUSTER );
			if ( PR_SUCC(err) )
				mcpy( pvt, g_vtable_static, sizeof(g_vtable_static) );
		}
		else
			err = PrRealloc( (tPTR*)&pvt, g_vtable_ptr, g_vtable_count+VTABLE_CACHE_CLUSTER );
		if ( PR_FAIL(err) )
			return err;
		g_vtable_ptr = pvt;
	}
	pvt = g_vtable_ptr + g_vtable_count++;
	++pvt->m_ref;

	if ( PR_FAIL(err=PrAlloc((tPTR*)&novel->m_methods,0x40*sizeof(tMETHOD))) )
		return err;

	for( i=0; i<0x40; i++ ) {
		tMETHOD *m;
		tDWORD v = 0;
		tPTR   a = novel->m_func( handle, novel->m_params, cTRUE, i, &v );
		if ( a )
			;
		else if ( old ) {
			m = old->m_methods + i;
			a = ABS_ADDR( *ADDR_3(m) );
			v = *ADDR_2(m);
		}
		else
			a = ((tPTR*)iface->external)[i];
		m = novel->m_methods + i;
		METHOD_SET( m, novel->m_num, i, v, a );
	}
	
	pvt->m_func    = novel->m_func;
	pvt->m_num     = novel->m_num;
	pvt->m_methods = novel->m_methods;
	return errOK;
}




// ---
const tPTR NotInitializedMethod( tHANDLE* handle, tPTR param, tBOOL ex_in, tDWORD num, tDWORD* data );



// ---
tERROR pr_call _WrapperAdd( tPO* po, tHANDLE* handle, OneFuncPtr addr_func, tPTR params ) {

  tUINT     i, c;
  tUINT     num;
  tWRAPPER* add;
  tWRAPPER* now;
  tPTR*     vtbl;
	tERROR    err;
	tHANDLEX* ext;

	if ( g_unsave_vtbl )
		return errNOT_SUPPORTED;

  if ( !handle->iface || !handle->obj )
    return errINTERFACE_NOT_ASSIGNED_YET;

	if ( _is_vtbl(handle,handle->iface->external) && (NotInitializedMethod == addr_func) ) {
		_set_vtbl( handle, _ObjNotInitializedFunctions );
		return errOK;
	}

	if ( PR_FAIL(_chk_ext(po,handle,ext,err)) )
		return err;

  now = ext->wrappers;
  while( now ) {
    if ( now->m_func == addr_func ) 
      return errOK;
    now = now->next;
  }

  if ( PR_FAIL(err=PrAlloc((tPTR*)&add,sizeof(tWRAPPER))) )
    return err;

	c = handle->iface->ex_count;
  now = ext->wrappers;
  if ( now ) {
    num = now->m_num + 1;
    now->prev = add;
    vtbl = (tPTR*)_get_vtbl(handle);
  }
	else if ( PR_FAIL(err=PrAlloc((tPTR*)&vtbl,c*sizeof(tPTR))) ) {
		PrFree( add );
		return err;
	}
  else
    num = 0;

  add->m_num    = num;
  add->m_func   = addr_func;
	add->m_params = params;
  add->prev     = 0;
  add->next     = now;
	err = _get_vtable( handle, add, now );
	if ( PR_FAIL(err) ) {
		PrFree( add );
		if ( !_is_vtbl(handle,vtbl) )
			PrFree( vtbl );
		return err;
	}

  ext->wrappers = add;
	for( i=0; i<c; ++i )
		vtbl[i] = add->m_methods[i];
  if ( !num )
    _set_vtbl( handle, vtbl );
  
  return errOK;
}




// ---
tERROR pr_call _WrapperWithdraw( tPO* po, tHANDLE* handle, OneFuncPtr addr_func, tPTR param ) {

	tDWORD i, c;
  tWRAPPER* str;
  tWRAPPER* now;
	tHANDLEX* ext;

	if ( g_unsave_vtbl )
		return errNOT_SUPPORTED;

  if ( !handle->iface || !handle->obj )
    return errINTERFACE_NOT_ASSIGNED_YET;

	if ( (addr_func == NotInitializedMethod) && _is_vtbl(handle,_ObjNotInitializedFunctions) ) {
		_set_vtbl( handle, 0 );
		return errOK;
	}
	
	ext = handle->ext;
	if ( !ext )
		return errNOT_FOUND;

  now = str = ext->wrappers;
  while( now ) {
		tVTableCache* pvt;
    tWRAPPER* tmp = now;
    now = now->next;
    if ( !addr_func || (tmp->m_func == addr_func) ) {
      if ( tmp->prev )
        tmp->prev->next = now;
      else
        ext->wrappers = now;
      if ( now )
        now->prev = tmp->prev;

			pvt = g_vtable_ptr;
			for( i=0; i<g_vtable_count; ++i,++pvt ) {
				if ( (pvt->m_func == tmp->m_func) && (pvt->m_num == tmp->m_num) ) {
					--pvt->m_ref;
					break;
				}
			}

      PrFree( tmp );
      if ( addr_func )
        break;
    }
  }

  if ( str != ext->wrappers ) {
    if ( ext->wrappers ) {
			tPTR* vtbl = (tPTR*)_get_vtbl( handle );
      for( i=0, c=handle->iface->ex_count, now=ext->wrappers; i<c; ++i )
        vtbl[i] = now->m_methods[i];
    }
    else if ( !_is_vtbl(handle,handle->iface->external) ) {
			PrFree( (tPTR)_get_vtbl(handle) );
      _set_vtbl( handle, 0 );
    }
  }
	else if ( (addr_func == NotInitializedMethod) && _is_vtbl(handle,_ObjNotInitializedFunctions) )
		_set_vtbl( handle, 0 );

  return errOK;
}




// ---
tPTR pr_call _WrapperNext( tHANDLE* handle ) {

	tHANDLEX* ext;
	tUINT     n;
	tUINT     m;
	tWRAPPER* w;

	if ( g_unsave_vtbl )
		return 0;

	ext = handle->ext;
	if ( !ext )
		return 0;

	n = (ext->wrap_data) >> 16;
	m = ext->wrap_data & 0xffff;
	w = ext->wrappers;

	if ( n == 0xffff )
		return ((tPTR*)handle->iface->external)[m];

	ext->wrap_data = ((n-1) << 16) | m;

	for( ; w; w=w->next ) {
		if ( w->m_num == n ) {
			w = w->next;
			if ( w )
				return ABS_ADDR( *ADDR_3(w->m_methods[m]) );
			return ((tPTR*)handle->iface->external)[m];
		}
	}
	return 0;
}



// ---
tPTR pr_call _WrapperData( tHANDLE* handle ) {
  
  tHANDLEX* ext;
	tUINT     n;
  tWRAPPER* w;

	if ( g_unsave_vtbl )
		return 0;

	ext = handle->ext;
	if ( !ext )
		return 0;

  n = ext->wrap_data >> 16;
  w = ext->wrappers;

  for( ; w; w=w->next ) {
    if ( w->m_num == n )
      return w->m_params;
  }
  return 0;
}



#ifndef _INC_STDARG
  #include <stdarg.h>
#endif

// ---
tERROR pr_call _WrapperCallNext( hOBJECT* obj, ... ) {
	if ( g_unsave_vtbl )
		return errNOT_SUPPORTED;

  if ( obj ) {
    typedef tERROR (pr_call *F)(hOBJECT,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD,tDWORD );
    F f = (F)_WrapperNext(MakeHandle(*obj));
    if ( f ) {
      int i;
      tDWORD p[20];
      va_list va;
      va_start( va, *obj );
      for( i=0; i<(sizeof(p)/sizeof(p[0])); i++ )
        p[i] = va_arg( va, tUINT );
      va_end( va );
      return f( *obj, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15], p[16], p[17], p[18], p[19] );
    }
  }
  return errOK;
}


#endif // __unix__
