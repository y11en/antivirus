
#include "remote_mbl.h"

static hOBJECT g_hMBL = NULL;
PRRemoteAPI g_RP_API = {0};
HMODULE g_hModuleRemotePrague = NULL;
hROOT g_hRoot = NULL;

#ifndef MAX_PATH
#define MAX_PATH 8192
#endif

tERROR RemoteMBLInit(HMODULE hModule, tCHAR* plugins_list[], tINT plugin_count)
{
	tCHAR szLoadPath[MAX_PATH];
	tCHAR* pModuleName;
	tERROR err = errOK;

	if (!GetModuleFileName(hModule, szLoadPath, sizeof(szLoadPath)))
		*szLoadPath = 0;
	pModuleName = strrchr(szLoadPath, '\\');
	if (!pModuleName)
		pModuleName = szLoadPath;
	else
		pModuleName++;
	
	strcpy(pModuleName, PR_REMOTE_MODULE_NAME);
	g_hModuleRemotePrague = LoadLibraryEx(szLoadPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	
	if (!g_hModuleRemotePrague)
		err = errMODULE_CANNOT_BE_LOADED;
	else
	{
		tERROR ( *hGetAPI )(PRRemoteAPI *api) = NULL;

		*(void**)&hGetAPI = GetProcAddress(g_hModuleRemotePrague, "PRGetAPI");
		if( hGetAPI )
			err = hGetAPI(&g_RP_API);

		if( PR_SUCC(err) )
			err = g_RP_API.Initialize(PR_FLAG_NO_LOAD_PLUGINS);

		if( PR_SUCC(err) )
		{
			hLOADER hLoader;
			err = g_RP_API.GetRoot(&g_hRoot);
			if (PR_SUCC(err))
			{
				if (PR_SUCC(err = g_hRoot->sysCreateObjectQuick((hOBJECT*)&hLoader, IID_LOADER)))
				{
					int i;
					for (i=0; i<plugin_count; i++)
					{
						tCHAR* dummy = szLoadPath;
						tCHAR** dummy2 = &dummy;
						strcpy(pModuleName, plugins_list[i]);
						if (PR_FAIL(err = err = hLoader->IntegrateModules(&dummy, sizeof(tCHAR*), fLOADER_PARAM_IS_STR_POINTER_LIST)))
							break;
					}
				}
			}
		}
	}
	return err;
}

tERROR RemoteMBLTerm()
{
	if ( g_RP_API.ReleaseObjectProxy && g_hMBL ) 
		g_RP_API.ReleaseObjectProxy(g_hMBL);

	if ( g_RP_API.Deinitialize )
		return g_RP_API.Deinitialize();
	else
		return errOK;
}


hOBJECT RemoteMBLGet()
{
	if ( g_hMBL ) 
	{
		if ( g_RP_API.IsValidProxy && PR_SUCC(g_RP_API.IsValidProxy(g_hMBL)) )
			return g_hMBL;
		else
			g_hMBL = NULL;
	}
	
	if ( g_RP_API.GetObjectProxy )
	{
		if ( PR_SUCC(g_RP_API.GetObjectProxy(PR_PROCESS_ANY, "MCTask", &g_hMBL)) )
			return g_hMBL;
	}

	return NULL;
}
