// AdminGUIProxy.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <tchar.h>

HMODULE g_hInstance = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = (HMODULE)hModule;
		DisableThreadLibraryCalls(hModule);
		break;
	}
	return TRUE;
}

typedef void (*fnInitFunc)(void);
class IKLAccessor;
typedef IKLAccessor *(*fnGetAccessor)(void);

fnInitFunc gfn_InitPlugin;
fnInitFunc gfn_DeinitPlugin;
fnGetAccessor gfn_GetAccessor;

bool LoadManagee()
{
	static HMODULE g_hManagee = NULL;
	// check if already loaded
	if(g_hManagee)
		return true;
	
	// get our path
	TCHAR szThisModuleFileName [MAX_PATH + 1];
	if (GetModuleFileName(g_hInstance, szThisModuleFileName, MAX_PATH) == 0)
		return false;
	
	// add 'i' to filename
	TCHAR path_buffer[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	_tsplitpath_s(szThisModuleFileName,
					drive, _MAX_DRIVE,
					dir, _MAX_DIR,
					fname, _MAX_FNAME,
					ext, _MAX_EXT );
	_tcscat_s(fname, _MAX_FNAME, _T("i"));
	_tmakepath_s(path_buffer, _MAX_PATH, drive, dir, fname, ext);

	// load managee
	g_hManagee = ::LoadLibraryEx(path_buffer, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if(!g_hManagee)
		return false;

	// get required entry points
	gfn_InitPlugin = (fnInitFunc)GetProcAddress(g_hManagee, "InitPlugin");
	gfn_DeinitPlugin = (fnInitFunc)GetProcAddress(g_hManagee, "DeinitPlugin");
	gfn_GetAccessor = (fnGetAccessor)GetProcAddress(g_hManagee, "GetAccessor");

	return true;
}

extern "C" __declspec(dllexport) void InitPlugin()
{
	LoadManagee();
	if(gfn_InitPlugin)
		gfn_InitPlugin();
}

extern "C" __declspec(dllexport) void DeinitPlugin()
{
	LoadManagee();
	if(gfn_DeinitPlugin)
		gfn_DeinitPlugin();
}

extern "C" __declspec(dllexport) IKLAccessor *GetAccessor()
{
	LoadManagee();
	return gfn_GetAccessor ? gfn_GetAccessor() : NULL;
}