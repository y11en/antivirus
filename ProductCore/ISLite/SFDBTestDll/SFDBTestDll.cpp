// SFDBTestDll.cpp : Defines the entry point for the DLL application.
//

#include "windows.h"
#include "prague.h"
#include "sfdbtest.h"

#include <pr_loadr.h>
#include <iface/e_loader.h>
#include "IFace/e_ktime.h"
#include <iface/i_root.h>
#include <iface/i_plugin.h>
#include <iface/i_sfdb.h>
#include <iface/i_io.h>
#include <iface/i_ifilesecurestatus.h>
#include "../ichecker/ifilesec.h"
#include "../../nfio/win32_io.h"

hSECUREFILEDB		hSFDB = NULL;
hIFILESECURESTATUS	hChecker = NULL;
hROOT				hRoot = 0;

PragueLoadFunc    plf;
PragueUnloadFunc  puf;

tBOOL bInited = cFALSE;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

// -----------------------------------------------------------------------------------------
PR_MAKE_TRACE_FUNC;

// ---
void Trace( tSTRING str, hOBJECT hObject, tTRACE_LEVEL dwLevel) {
	OutputDebugString(str);
	OutputDebugString("\n");
}

tERROR pr_call RootCheck(hROOT hRoot, tVOID* param)
{
    tDWORD tr_ptr = (tDWORD)Trace;
	tERROR error;
	
	CALL_SYS_PropertySet( hRoot, 0, pgOUTPUT_FUNC, &tr_ptr, sizeof(tr_ptr) );
	error = CALL_SYS_TraceLevelSet(hRoot, tlsALL_OBJECTS, prtNOTIFY, prtMIN_TRACE_LEVEL);
	
	return errOK;
}

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(imports)
#include "IFace/e_loader.h"
#include "IFace/e_ktime.h"
IMPORT_TABLE_END

DWORD __declspec(dllexport) __cdecl Init(char* szSFDBPathName)
{
	tERROR error = errUNEXPECTED;
	char buffer[5*MAX_PATH];
	char* ptr;

	__asm int 3;

	GetModuleFileName( 0, (LPSTR)buffer, sizeof(buffer) );
	
	// loader module name
	ptr = strrchr( (const char *)buffer, '\\' );
	if ( ptr ) 
		++ptr;
	else 
		ptr = buffer;
	
	lstrcpy( (LPSTR)ptr, PR_LOADER_MODULE_NAME );

	HINSTANCE loader = LoadLibrary( buffer );
	if ( loader ) {
		plf = (PragueLoadFunc)   GetProcAddress( loader, PR_LOADER_LOAD_FUNC );
		puf = (PragueUnloadFunc) GetProcAddress( loader, PR_LOADER_UNLOAD_FUNC );
		
		if ( plf ) {

			error = plf( &hRoot, PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH/*|PR_LOADER_FLAG_USE_TRY_CATCH*/, RootCheck, NULL ); 
			
			if ( PR_SUCC(error) && hRoot ) {

				if (PR_FAIL(error = CALL_Root_ResolveImportTable(hRoot, NULL, imports, PID_APPLICATION)))
					OutputDebugString("Cannot import extern functions\n");
			
				error = CALL_SYS_ObjectCreate(hRoot,&hChecker,IID_IFILESECURESTATUS, PID_ANY, SUBTYPE_ANY);
				if ( PR_SUCC(error) )
					error = CALL_SYS_TraceLevelSet(hChecker, tlsALL_OBJECTS, prtNOT_IMPORTANT, prtMIN_TRACE_LEVEL);
				if ( PR_SUCC(error) )
					error = CALL_SYS_PropertySetStr(hChecker, NULL, plDB_PATHNAME, szSFDBPathName, strlen(szSFDBPathName)+1, cCP_ANSI);
				if ( PR_SUCC(error) )
					error = CALL_SYS_ObjectCreateDone( hChecker );
				

				if (PR_FAIL(error))
				{
					OutputDebugString("Cannot init iChecker\n");
					if (hChecker != NULL)
						CALL_SYS_ObjectClose(hChecker);
				}
				else
				{
					OutputDebugString("iChecker inited OK\n");
				}

			}
			else
				OutputDebugString("Cannot initialize hRoot object\n");
		}
		else
			OutputDebugString("Loader doesn't export init function\n" );
	}
  else
  {
	  char s[6*MAX_PATH];
	  wsprintf(s, "Cannot start loader module '%s', error %08X\n", buffer, GetLastError());
	  OutputDebugString(s);
  }

  if (PR_SUCC(error))
	  bInited = cTRUE;

  return error;
}

DWORD __declspec(dllexport) __cdecl GetStatus(char* szFileName, DWORD* pdwData1, DWORD* pdwData2)
{
	tERROR error;
	hIO hIo = NULL;
	
	if (!bInited)
		return errOBJECT_NOT_INITIALIZED;

	error = CALL_SYS_ObjectCreate(hRoot, &hIo, IID_IO, PID_WIN32_NFIO, 0);
	if (PR_SUCC(error))
	{
		error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST);
		if (PR_SUCC(error))
			error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_ACCESS_MODE, fACCESS_READ);
		if (PR_SUCC(error))
			error = CALL_SYS_PropertySetStr(hIo, 0, pgOBJECT_NAME, szFileName, lstrlen(szFileName) + 1, cCP_ANSI);
		if (PR_SUCC(error))
			error = CALL_SYS_ObjectCreateDone(hIo);
		if (PR_FAIL(error))
			CALL_SYS_ObjectClose(hIo);
	}
	if (PR_FAIL(error))
	{
		OutputDebugString("Error: Cannot open file '");
		OutputDebugString(szFileName);
		OutputDebugString("'\n");
		return error;
	}

	if (PR_FAIL(error = CALL_iFileSecureStatus_GetStatus(hChecker, hIo, NULL, 0, pdwData1, pdwData2)))
	{
		if (error = errMODULE_UNKNOWN_FORMAT)
			OutputDebugString("Unknown format for file: ");
		else
			OutputDebugString("Unknown error while GetStatus for file: ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
	}
	CALL_SYS_ObjectClose(hIo);
	return error;
}

DWORD __declspec(dllexport) __cdecl GetStatusEx(char* szFileName, char** pszDate1, char** pszDate2)
{
	tERROR error;
	tDWORD dwData1, dwData2;
	tDT	dt;
	tDWORD year, month, day, hour, minute, second;

	char str[0x100];
	wsprintf(str, "GetStatusEx(szFileName=%s, char** = %08X(%08X), char** = %08X(%08X)\n", pszDate1, pszDate2, *pszDate1, *pszDate2);
	OutputDebugString(str);

	if (!bInited)
		return errOBJECT_NOT_INITIALIZED;

	if (PR_FAIL(error = GetStatus(szFileName, &dwData1, &dwData2)))
	{
		strcpy(*pszDate1, "<undefined>");
		strcpy(*pszDate2, "<undefined>");
		return error;
	}

	wsprintf(*pszDate1, "%08X", dwData1);
	wsprintf(*pszDate2, "%08X", dwData2);

	if (DTGet)
	{
		if (dwData1)
		{
			*(tQWORD*)&dt = (tQWORD)dwData1;
			*(tQWORD*)&dt *= 6000000000;
			if (PR_SUCC(DTGet(&dt, &year, &month, &day, &hour, &minute, &second, NULL)))
				wsprintf(*pszDate1, "%02d.%02d.%04d %02d:%02d:%02d", day, month, year, hour, minute, second);
		}
		if (dwData2) 
		{
			*(tQWORD*)&dt = (tQWORD)dwData2;
			*(tQWORD*)&dt *= 6000000000;
			if (PR_SUCC(DTGet(&dt, &year, &month, &day, &hour, &minute, &second, NULL)))
				wsprintf(*pszDate2, "%02d.%02d.%04d %02d:%02d:%02d", day, month, year, hour, minute, second);
		}
	}

	return error;
}

DWORD __declspec(dllexport) __cdecl Done()
{
	if (!bInited)
		return errOBJECT_NOT_INITIALIZED;

	if ( puf )
		puf( hRoot );
	else
		CALL_SYS_ObjectClose( hRoot );

	return errOK;
}



