#include "custview.h"


#define DEF_IDENTIFIER( name, val )  { name::eIID, #name },

cRoot* g_root;

tName g_serIds[] = {
	#undef DEF_IDENTIFIER
	#define DEF_IDENTIFIER(name, val)	{val, #name},
	#include "../../include/pr_d_sid.h"
};



extern "C" HRESULT read_debuggee_mem( DEBUGHELPER* pHelper, DWORD addr, DWORD size, tVOID* buff, tSTRING pref, char* out, size_t dwMaxSize );

// ---
extern "C" HRESULT WINAPI serializableId( DWORD dwValue, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved ) {
  __try {
		if ( (dwValue == 0xccccccccl) || (dwValue == 0xcdcdcdcdl) ) {
			strcpy( pOutputStr, "SER:ID not initialized" );
			return S_OK;
		}
		if ( dwValue == 0xffffffffl ) {
			strcpy( pOutputStr, "SER: -1 (?-?-?)" );
			return S_OK;
		}
		memset( pOutputStr, 0, dwMaxSize );
		for( tUINT i=0; i<countof(g_serIds); ++i ) {
			if ( g_serIds[i].id == dwValue ) {
				wsprintf( pOutputStr, "SER:%s", g_serIds[i].name );
				return S_OK;
			}
		}
		tUINT pluginId = dwValue >> 16;
		tUINT serId    = (dwValue - (pluginId<<16)) >> 8;
		const tCHAR* pluginName = find_name( pids, pid_count, pluginId, 0 );
		if ( pluginName )
			wsprintf( pOutputStr, "SER:0x%08x (plg:%s,ser:%u) - unknown serializable id", dwValue, pluginName, serId );
		else
			wsprintf( pOutputStr, "SER:0x%08x (plg:%u,ser:%u) - unknown serializable id", dwValue, pluginId, serId );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pOutputStr, "cannot resolve serializable id, exception" );
	}
	return S_OK;
}



// ---
//extern "C" HRESULT WINAPI serializable( DWORD dwValue, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved ) {
//	__try{
//		cSerializable ser;
//		if ( (dwValue == 0xccccccccl) || (dwValue == 0xcdcdcdcdl) ) {
//			strcpy( pOutputStr, "SER:pointer not initialized" );
//			return S_OK;
//		}
//    if ( FAILED(read_debuggee_mem(pHelper,(tDWORD)dwValue,sizeof(cSerializable),&ser,"cSerializable",pOutputStr,dwMaxSize)) )
//      return S_OK;
//		return serializableId( ser.m_unique, pHelper, base, ignore, pOutputStr, dwMaxSize, reserved );
//	}
//	__except( EXCEPTION_EXECUTE_HANDLER ) {
//		lstrcpy( pOutputStr, "cannot resolve serializable id, exception" );
//	}
//	return S_OK;
//}

