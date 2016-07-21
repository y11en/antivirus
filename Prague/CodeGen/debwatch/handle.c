#include "custview.h"
#include <iface/i_string.h>
#include <iface/i_io.h>



// ---
HRESULT read_debuggee_mem( DEBUGHELPER* pHelper, DWORD addr, DWORD size, tVOID* buff, tSTRING pref, char* out, size_t max );
//HRESULT old_handle( t_old_HANDLE* oh, DWORD dwValue, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved );



// ---
hiOBJECT _get_obj( tHANDLE* handle, DEBUGHELPER* pHelper, char* pOutputStr, size_t dwMaxSize ) {
	tRelObj* robj;
	tDebObj  dobj;
	tPTR addr = handle->obj;
	if ( FAILED(read_debuggee_mem(pHelper,(tDWORD)addr,sizeof(tDebObj),&dobj,"obj value",pOutputStr,dwMaxSize)) )
		return 0;
	robj = (tRelObj*)&dobj;
	if ( OBJD(addr,4) == robj->data )
		return (hiOBJECT)OBJD(addr,4);
	if ( OBJD(addr,6) == dobj.data )
		return (hiOBJECT)OBJD(addr,6);
	return 0;
}




// ---
HRESULT WINAPI phandle( DWORD dwValue, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved ) {
	tHANDLE    handle;
  tINTERFACE iface;
  DWORD      addr;
  tCHAR*     pname;
  tCHAR      iid[MAX_PATH];
  tCHAR      pid[MAX_PATH];
	hiOBJECT   obj;
  //HRESULT    hr;
    
  __try {
	  memset( pOutputStr, 0, dwMaxSize );
  
    if ( ((DWORD)dwValue) == 0 ) {
      lstrcpyn( pOutputStr, "null pointer", dwMaxSize );
      return S_OK;
    }
    if ( ((DWORD)dwValue) == 0xccccccccl ) {
      lstrcpyn( pOutputStr, "not initialized", dwMaxSize );
      return S_OK;
    }
  
    if ( FAILED(read_debuggee_mem(pHelper,dwValue,sizeof(tHANDLE),&handle,"handle ptr",pOutputStr,dwMaxSize)) )
      return S_OK;
  
//    hr = read_debuggee_mem( pHelper, (tDWORD)handle.obj, sizeof(robj), &robj, "object", pOutputStr, dwMaxSize );
//		if ( SUCCEEDED(hr) && (robj.data == (((tDWORD)handle.obj) + 3*sizeof(tPTR))) ) { // its release
//		}
//		else {
//			hr = read_debuggee_mem( pHelper, (tDWORD)handle.obj, sizeof(dobj), &dobj, "object", pOutputStr, dwMaxSize );
//			if ( SUCCEEDED(hr) && (dobj.data == (((tDWORD)handle.obj) + 5*sizeof(tPTR))) ) { // its debug
//			}
//		}

    if ( handle.iface == 0 ) {
      lstrcpyn( pOutputStr, "iface NULL", dwMaxSize );
      return S_OK;
    }
    
    if ( FAILED(read_debuggee_mem(pHelper,(tDWORD)handle.iface,sizeof(tINTERFACE),&iface,"iface",pOutputStr,dwMaxSize)) )
      return S_OK;
    
    if ( iface.iid == IID_NONE ) {
      lstrcpyn( pOutputStr, "iface NONE", dwMaxSize );
      return S_OK;
    }
    
    if ( iface.iid == IID_ANY ) {
      lstrcpyn( pOutputStr, "iface ANY", dwMaxSize );
      return S_OK;
    }
    
    if ( iface.iid == IID_STRING ) {
      addr = lstrlen( pOutputStr );
			obj = _get_obj( &handle, pHelper, pOutputStr, dwMaxSize );
      if ( obj && (addr < dwMaxSize) )
        return phString( (tDWORD)obj, pHelper, base, ignore, pOutputStr+addr, dwMaxSize-addr, reserved );
    }

		if ( iface.iid == IID_HEAP ) {
			obj = _get_obj( &handle, pHelper, pOutputStr, dwMaxSize );
			if ( obj )
				return phHeap( (tDWORD)obj, pHelper, base, ignore, pOutputStr, dwMaxSize, reserved );
		}
		
    if ( (iface.iid == IID_IO) && (iface.pid == PID_NATIVE_FIO) ) {
			obj = _get_obj( &handle, pHelper, pOutputStr, dwMaxSize );
			if ( obj )
				return phNFIO( (tDWORD)obj, pHelper, base, ignore, pOutputStr, dwMaxSize, reserved );
		}
    
    pname = find_name( iids, iid_count, iface.iid, 0 );
    if ( pname )
      wsprintf( iid, "%s(%d)", pname, iface.iid );
    else
      wsprintf( iid, "iid=%d(unk)", iface.iid );
    
    pname = find_name( pids, pid_count, iface.pid, 0 );
    if ( pname )
      wsprintf( pid, "%s(%d)", pname, iface.pid );
    else
      wsprintf( pid, "pid=%d(unk)", iface.pid );
    
    wsprintf( local_buff, "%s, %s", iid, pid );
    lstrcpyn( pOutputStr, local_buff, dwMaxSize );
    
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
    lstrcpy( pOutputStr, "cannot resolve tHANDLE, exception" );
  }
  return S_OK;
}


// ---
//HRESULT old_handle( t_old_HANDLE* oh, DWORD dwValue, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved ) {
//  tINTERFACE iface;
//  tCHAR*     pname;
//  tCHAR      iid[MAX_PATH];
//  tCHAR      pid[MAX_PATH];
//
//  __try {
//    if ( oh->iface == 0 ) {
//      lstrcpyn( pOutputStr, "iface NULL", dwMaxSize );
//      return S_OK;
//    }
//  
//    if ( oh->iid == IID_NONE ) {
//      lstrcpyn( pOutputStr, "iface NONE", dwMaxSize );
//      return S_OK;
//    }
//  
//    if ( oh->iid == IID_ANY ) {
//      lstrcpyn( pOutputStr, "iface ANY", dwMaxSize );
//      return S_OK;
//    }
//  
//    if ( FAILED(read_debuggee_mem(pHelper,(tDWORD)oh->iface,sizeof(tINTERFACE),&iface,"iface",pOutputStr,dwMaxSize)) )
//      return S_OK;
//  
//    pname = find_name( iids, iid_count, oh->iid, 0 );
//    if ( pname )
//      wsprintf( iid, "%s(%d)", pname, oh->iid );
//    else
//      wsprintf( iid, "iid=%d(unk)", oh->iid );
//  
//    pname = find_name( pids, pid_count, iface.pid, 0 );
//    if ( pname )
//      wsprintf( pid, "%s(%d)", pname, oh->iid );
//    else
//      wsprintf( pid, "pid=%d(unk)", iface.pid );
//  
//    wsprintf( local_buff, "%s, %s", iid, pid );
//    lstrcpyn( pOutputStr, local_buff, dwMaxSize );
//  
//    if ( oh->iid == IID_STRING ) {
//      tDWORD len = lstrlen( pOutputStr );
//      if ( len < dwMaxSize )
//        return phString( dwValue+sizeof(t_old_HANDLE)-sizeof(k_old_OBJECT), pHelper, base, ignore, pOutputStr+len, dwMaxSize-len, reserved );
//    }
//  }
//  __except( EXCEPTION_EXECUTE_HANDLER ) {
//    lstrcpy( pOutputStr, "cannot resolve tHANDLE, exception" );
//  }
//  return S_OK;
//}



