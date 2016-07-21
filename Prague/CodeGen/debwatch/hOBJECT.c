#include "custview.h"
#include <iface/i_string.h>
#include <iface/i_io.h>

char src[0x4000];
char local_buff[0x4000];


// ---
HRESULT read_debuggee_mem( DEBUGHELPER* pHelper, DWORD addr, DWORD size, tVOID* buff, tSTRING pref, char* out, size_t dwMaxSize ) {

	tDWORD  len;
	tDWORD  dwErr;
	HRESULT hr = pHelper->ReadDebuggeeMemory( pHelper, addr, size, buff, &dwErr );

	if ( FAILED(hr) ) {
		len = wsprintf( local_buff, "%s: ??? debuggee memory(p=0x%08x,s=%u) - 0x%08x ", pref, addr, size, hr );
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, hr, 0, local_buff+len, sizeof(local_buff)-len, 0 );
		lstrcpyn( out, local_buff, dwMaxSize );
	}
	
	else if ( FAILED(dwErr) ) {
		len = wsprintf( local_buff, "%s: ??? debuggee memory ???(p=0x%08x,s=%u) - 0x%08x ", pref, addr, size, dwErr );
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, dwErr, 0, local_buff+len, sizeof(local_buff)-len, 0 );
		lstrcpyn( out, local_buff, dwMaxSize );
	}

	return hr;
}



// ---
HRESULT WINAPI phOBJECT( DWORD dwValue, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved ) {

	__try {
		
		//tHANDLE    handle;
		//tINTERFACE iface;
		//tDWORD     addr;
    //tDWORD     size;
    //tCHAR*     pname;
    //tCHAR      iid[MAX_PATH];
    //tCHAR      pid[MAX_PATH];
		//HRESULT    hr;
		tProveObject prove;
		
		memset( pOutputStr, 0, dwMaxSize );

		if ( ((DWORD)dwValue) == 0 ) {
			lstrcpyn( pOutputStr, "null pointer", dwMaxSize );
			return S_OK;
		}

		if ( ((DWORD)dwValue) == 0xccccccccl ) {
			lstrcpyn( pOutputStr, "not initialized", dwMaxSize );
			return S_OK;
		}

    if ( FAILED(read_debuggee_mem(pHelper,(tDWORD)OBJD(dwValue,1),sizeof(tProveObject),&prove,"prove obj",pOutputStr,dwMaxSize)) )
      return S_OK;

		if ( OBJD(dwValue,2) == prove.cpp_data )
			dwValue = (tDWORD)OBJD(dwValue,-2);
		else if ( OBJD(dwValue,3) == prove.c_data )
			dwValue = (tDWORD)OBJD(dwValue,-1);
		else {
			lstrcpyn( pOutputStr, "not an object", dwMaxSize );
			return S_OK;
		}

    if ( FAILED(read_debuggee_mem(pHelper,dwValue,sizeof(tPTR),&dwValue,"handle ptr",pOutputStr,dwMaxSize)) )
      return S_OK;

		return phandle( dwValue, pHelper, base, ignore, pOutputStr, dwMaxSize, reserved );

//		addr = dwValue - sizeof(tPTR);
//    size = sizeof(tHANDLE);
//    hr = read_debuggee_mem( pHelper, addr, size, &handle, "handle", pOutputStr, dwMaxSize );
//    if ( FAILED(hr) )
//			return S_OK;
//
//		if ( handle.iface == 0 ) {
//			lstrcpyn( pOutputStr, "iface NULL", dwMaxSize );
//			return S_OK;
//		}
//		
//		if ( handle.iid == IID_NONE ) {
//			lstrcpyn( pOutputStr, "iface NONE", dwMaxSize );
//			return S_OK;
//		}
//		
//		if ( handle.iid == IID_ANY ) {
//			lstrcpyn( pOutputStr, "iface ANY", dwMaxSize );
//			return S_OK;
//		}
//		
//		if ( handle.iid == IID_STRING )
//			return phString( dwValue, pHelper, base, ignore, pOutputStr, dwMaxSize, reserved );
//
//		if ( handle.iid == IID_HEAP ) {
//			tDWORD addr = ((tDWORD)handle.obj) + sizeof(tHANDLE*);
//			return phHeap( addr, pHelper, base, ignore, pOutputStr, dwMaxSize, reserved );
//		}
//		
//		//if ( FAILED(read_debuggee_mem(pHelper,(tDWORD)handle.iface,sizeof(tINTERFACE),&iface,"iface",pOutputStr,dwMaxSize)) )
//		//	return S_OK;
//
//    if ( handle.iid == IID_IO && handle.pid == PID_NATIVE_FIO )
//      return phNFIO( dwValue, pHelper, base, ignore, pOutputStr, dwMaxSize, reserved );
//
//    pname = find_name( iids, iid_count, handle.iid, 0 );
//    if ( pname )
//      wsprintf( iid, "%s(%d)", pname, handle.iid );
//    else
//      wsprintf( iid, "iid=%d(unk)", handle.iid );
//
//    pname = find_name( pids, pid_count, iface.pid, 0 );
//    if ( pname )
//      wsprintf( pid, "%s(%d)", pname, iface.pid );
//    else
//      wsprintf( pid, "pid=%d(unk)", iface.pid );
//    
//    wsprintf( local_buff, "%s, %s", iid, pid );
//		lstrcpyn( pOutputStr, local_buff, dwMaxSize );

	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pOutputStr, "cannot resolve hOBJECT, exception" );
	}
	return S_OK;
}


// ---
tName iids[] = {
  #define DEF_IDENTIFIER(name, val)	{val, #name},
  #include "../../include/pr_d_iid.h"
  #undef DEF_IDENTIFIER
};

tUINT iid_count = countof(iids);
