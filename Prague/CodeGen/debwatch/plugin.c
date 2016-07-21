#include "custview.h"

// ---
typedef struct tag_PluginData {
	tCHAR           m_name[MAX_PATH]; // m_name of the plugin to load
	tPID            m_pid;          // plugin identificator
/*	hPLUGININSTANCE m_instance;     // plugin memory instance
	HDATA           m_mod_info;     // description of the plugin in tree form
	tDWORD          m_prague_format; // is plugin converted to prague format
	tDWORD          m_usage_count;  // usage count
	tDWORD          m_iface_count;  // interface count
	tPluginInit     m_plugin_init;  // plugin initialize function
	tBOOL           m_initialized;  // plugin initialized successfully
	tBOOL           m_unloadable;   // unload flag
	tDWORD          m_export_count; // count of plugin exports*/
} tPluginData;




// ---
HRESULT WINAPI phPlugin( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved ) {
	tPluginData  plugin;
  HRESULT      hr;
  DWORD        dwErr;
  const char*  fmt;
	char*        pid;
	char         lpid[12];

  __try {
    if ( ((DWORD)address) == 0 ) {
      lstrcpyn( pOutputStr, "null pointer", dwMaxSize );
      return S_OK;
    }
    if ( ((DWORD)address) == 0xccccccccl ) {
      lstrcpyn( pOutputStr, "not initialized", dwMaxSize );
      return S_OK;
    }
  
		hr = pHelper->ReadDebuggeeMemory( pHelper, address, sizeof(tPluginData), &plugin, &dwErr );
    if ( FAILED(hr) ) {
      lstrcpyn( pOutputStr, "??? cannot read debuggee memory", dwMaxSize );
      return S_OK;
    }


    if ( (0x80000000l & GetVersion()) ) // 98
      fmt = "PLUGIN(%s,\"%s\")";
    else
      fmt = "PLUGIN(%s,\"%S\")";

		if ( plugin.m_pid == PID_ANY )
			pid = "PID_ANY";
		else if ( 0 == (pid=find_name(pids,pid_count,plugin.m_pid,0)) )
			wsprintf( pid=lpid, "pid=0x%08x", plugin.m_pid );

    wsprintf( local_buff, fmt, pid, plugin.m_name );
    lstrcpyn( pOutputStr, local_buff, dwMaxSize );
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
    lstrcpyn( pOutputStr, "cannot parse address", dwMaxSize );
  }
  return S_OK;
}


