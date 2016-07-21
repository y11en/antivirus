
#include "remote_mbl.h"
#include "PragueHelper.h"
#include <atlconv.h>
#include <Prague/pr_remote.h>

static hOBJECT g_hMBL = NULL;
PRRemoteAPI g_RP_API = {0};
HMODULE g_hModuleRemotePrague = NULL;
hROOT g_root = NULL;
tPROPID g_propMailerPID = 0;
tPROPID g_propMailMessageOS_PID = 0;
tPROPID g_propMessageIsIncoming = 0;
tPROPID g_propCheckOnly = 0;
tPROPID g_propVirtualName = 0;
bool g_bInitSucceeded = false;
hPLUGIN g_pxstub_ppl = NULL;
hPLUGIN g_comstream_ppl = NULL;


#ifndef MAX_PATH
#define MAX_PATH 8192
#endif


tERROR RemoteMBLInit(HMODULE hModule, const PLUGIN_LIST& plugins_list)
{
	tERROR err = errOK;

	// Prague can only work with char*-strings, so we use std::string
	{	
		tCHAR szLoadPath[MAX_PATH];
		if (!GetModuleFileName(hModule, szLoadPath, sizeof(szLoadPath)))
			*szLoadPath = 0;
		tCHAR* pModuleName = strrchr(szLoadPath, '\\');
		if (!pModuleName)
			pModuleName = szLoadPath;
		else
			pModuleName++;

		strcpy(pModuleName, PR_REMOTE_MODULE_NAME);
		g_hModuleRemotePrague = LoadLibraryEx(szLoadPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	}
	
	if (!g_hModuleRemotePrague)
		err = errMODULE_CANNOT_BE_LOADED;
	else
	{
		tERROR ( *hGetAPI )(PRRemoteAPI *api) = NULL;

		*(void**)&hGetAPI = GetProcAddress(g_hModuleRemotePrague, "PRGetAPI");
		if( hGetAPI )
			err = hGetAPI(&g_RP_API);

		if( PR_SUCC(err) )
			err = g_RP_API.Initialize(PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH, NULL);

		if( PR_SUCC(err) )
		{
			hLOADER hLoader;
			err = g_RP_API.GetRoot(&g_root);
			if( PR_SUCC(err) )
			{
				if (PR_FAIL(err = g_root->LoadModule(&g_pxstub_ppl,"pxstub.ppl",(tDWORD)strlen("pxstub.ppl"),cCP_ANSI)))
				{
					OutputDebugString("KAV TheBatPlugin: ");
					OutputDebugString("cannot load module ");
					OutputDebugString("pxstub.ppl");
					OutputDebugString("\n");
				}
			}
			
			if (PR_FAIL(err = g_root->LoadModule(&g_comstream_ppl, "ComStmIO.ppl",
				(tDWORD)strlen("ComStmIO.ppl"), cCP_ANSI)))
			{
				OutputDebugString("KAV TheBatPlugin: ");
				OutputDebugString("cannot load module ");
				OutputDebugString("ComStmIO.ppl");
				OutputDebugString("\n");
			}

			if (PR_SUCC(err))
			{
				if (PR_SUCC(err = g_root->sysCreateObjectQuick((hOBJECT*)&hLoader, IID_LOADER)))
				{
					for (PLUGIN_LIST::const_iterator it = plugins_list.begin(); it != plugins_list.end(); ++it)
					{
						tCHAR* dummy = const_cast<tCHAR*>(it->c_str());
						tCHAR** dummy2 = &dummy;
						if (PR_FAIL(err = hLoader->IntegrateModules(&dummy, sizeof(tCHAR*), fLOADER_PARAM_IS_STR_POINTER_LIST)))
							break;
					}
				}
			}
		}
	}
	if( PR_SUCC(err) )
		g_bInitSucceeded = true;
	return err;
}

tERROR RemoteMBLTerm()
{
	if ( g_RP_API.ReleaseObjectProxy && g_hMBL ) 
		g_RP_API.ReleaseObjectProxy(g_hMBL);
	if ( g_pxstub_ppl )
		g_pxstub_ppl->Unload();

	if (g_comstream_ppl)
		g_comstream_ppl->Unload();

	if ( g_RP_API.Deinitialize )
		g_RP_API.Deinitialize();

	g_hMBL = NULL;
	g_root = NULL;
	g_pxstub_ppl = NULL;
	g_comstream_ppl = NULL;
	g_bInitSucceeded = false;
	g_hModuleRemotePrague = NULL;
	g_propMailerPID = 0;
	g_propMailMessageOS_PID = 0;
	g_propMessageIsIncoming = 0;
	g_propCheckOnly = 0;
	g_propVirtualName = 0;
	return errOK;
}

inline tERROR _GetProxy(hOBJECT& hMBL, char* szId)
{
	if ( !g_bInitSucceeded )
		return errHANDLE_INVALID;

	tERROR err = errOK;

	if ( hMBL ) 
	{
		if ( g_RP_API.IsValidProxy )
		{
			if ( PR_FAIL(err= g_RP_API.IsValidProxy(hMBL)) )
			{
				g_RP_API.ReleaseObjectProxy(hMBL);
				hMBL = NULL;
			}
		}
	}
	
	if ( g_RP_API.GetObjectProxy && !hMBL )
	{
		if ( PR_FAIL(err= g_RP_API.GetObjectProxy(PR_PROCESS_ANY, szId, &hMBL)) ) 
			hMBL = NULL;
	}

	return err;
}

hOBJECT RemoteMBLGet(tERROR* pERROR)
{
	tERROR err = _GetProxy(g_hMBL, "TaskManager");
	if (pERROR)
		*pERROR = err;

	if ( PR_SUCC(err) )
		return g_hMBL;
	return NULL;
}

