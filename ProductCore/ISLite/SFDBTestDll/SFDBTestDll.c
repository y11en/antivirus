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

tBOOL bPragueLoaded = cFALSE;
tBOOL bInited = cFALSE;

#define npKAVI_HASH_AND_DATA   ((tSTRING)("npKAVI_HASH_AND_DATA"))
static tPROPID propid_hash;
typedef struct _tag_HashAndData_
{
	ULARGE_INTEGER byHash;
	tDWORD dwData1;
	tDWORD dwData2;
}HashAndData;


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

DWORD __declspec(dllexport) __cdecl Init()
{
	tERROR error = errUNEXPECTED;
	char buffer[5*MAX_PATH];
	char* ptr;
	HINSTANCE loader;

	GetModuleFileName( 0, (LPSTR)buffer, sizeof(buffer) );
	
	// loader module name
	ptr = strrchr( (const char *)buffer, '\\' );
	if ( ptr ) 
		++ptr;
	else 
		ptr = buffer;
	
	lstrcpy( (LPSTR)ptr, PR_LOADER_MODULE_NAME );

	loader = LoadLibrary( buffer );
	if ( loader ) {
		plf = (PragueLoadFunc)   GetProcAddress( loader, PR_LOADER_LOAD_FUNC );
		puf = (PragueUnloadFunc) GetProcAddress( loader, PR_LOADER_UNLOAD_FUNC );
		
		if ( plf ) {

			error = plf( &hRoot, PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH/*|PR_LOADER_FLAG_USE_TRY_CATCH*/, RootCheck, NULL ); 
			
			if ( PR_SUCC(error) && hRoot ) {

				if (PR_FAIL(error = CALL_Root_ResolveImportTable(hRoot, NULL, imports, PID_APPLICATION)))
					OutputDebugString("Init: Cannot import extern functions\n");
			}
			else
				OutputDebugString("Init: Cannot initialize hRoot object\n");
		}
		else
			OutputDebugString("Init: Loader doesn't export init function\n" );
	}
  else
  {
	  char s[6*MAX_PATH];
	  wsprintf(s, "Init: Cannot start loader module '%s', error %08X\n", buffer, GetLastError());
	  OutputDebugString(s);
  }

  if (PR_SUCC(error))
	  bPragueLoaded = cTRUE;

  return error;
}


DWORD __declspec(dllexport) __cdecl InitSFDB(char* szSFDBPathName)
{
	tERROR error;

	if (!bPragueLoaded)
		return errOBJECT_NOT_INITIALIZED;

	if (bInited && hChecker != NULL)
	{
		CALL_SYS_ObjectClose(hChecker);
		hChecker = NULL;
		bInited = cFALSE;
	}

	error = CALL_SYS_ObjectCreate(hRoot,&hChecker,IID_IFILESECURESTATUS, PID_ANY, SUBTYPE_ANY);
	if ( PR_SUCC(error) )
		error = CALL_SYS_TraceLevelSet(hChecker, tlsALL_OBJECTS, prtNOT_IMPORTANT, prtMIN_TRACE_LEVEL);
	if ( PR_SUCC(error) )
		error = CALL_SYS_PropertySetStr(hChecker, NULL, plDB_PATHNAME, szSFDBPathName, strlen(szSFDBPathName)+1, cCP_ANSI);
	if ( PR_SUCC(error) )
		error = CALL_SYS_ObjectCreateDone( hChecker );
	
	if ( PR_SUCC(error) )
		error = CALL_Root_RegisterCustomPropId(hRoot, &propid_hash, npKAVI_HASH_AND_DATA, pTYPE_BINARY );

	if (PR_FAIL(error))
	{
		OutputDebugString("Init: Cannot init iChecker\n");
		if (hChecker != NULL)
		{
			CALL_SYS_ObjectClose(hChecker);
			hChecker = NULL;
		}
	}
	else
	{
		OutputDebugString("Init: iChecker inited OK\n");
	}

	if (PR_SUCC(error))
		bInited = cTRUE;
	
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

DWORD __declspec(dllexport) __cdecl GetStatus(char* szFileName, tBYTE* pOptionsData, DWORD dwOptionsSize, DWORD* pdwData1, DWORD* pdwData2, tQWORD *pqwHash)
{
	HashAndData data;
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
		OutputDebugString("GetStatus: Cannot open file '");
		OutputDebugString(szFileName);
		OutputDebugString("'\n");
		return error;
	}

	if (PR_FAIL(error = CALL_iFileSecureStatus_GetStatus(hChecker, hIo, pOptionsData, dwOptionsSize, pdwData1, pdwData2)))
	{
		if (error = errMODULE_UNKNOWN_FORMAT)
			OutputDebugString("GetStatus: Unknown format for file: ");
		else
			OutputDebugString("GetStatus: Unknown error while GetStatus for file: ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
	}
	else
	{
		if (pqwHash)
		{
			if (PR_FAIL(error = CALL_SYS_PropertyGet(hIo, NULL, propid_hash, &data, sizeof(data))))
			{
				OutputDebugString("GetStatus: Cannot retrive hash for file: ");
				OutputDebugString(szFileName);
				OutputDebugString("\n");
			}
			else
			{
				*pqwHash = *(tQWORD*)&data.byHash;
			}
		}
	}
	CALL_SYS_ObjectClose(hIo);
	return error;
}

DWORD __declspec(dllexport) __cdecl GetStatusEx(char* szFileName, tBYTE* pOptionsData, DWORD dwOptionsSize, char** pszDate1, char** pszDate2, tQWORD* pqwHash)
{
	tERROR error;
	tDWORD dwData1, dwData2;
	tDT	dt;
	tDWORD year, month, day, hour, minute, second;

	char str[0x100];
	wsprintf(str, "GetStatusEx(szFileName=%s, char** = %08X(%08X), char** = %08X(%08X)\n", szFileName, pszDate1, pszDate2, *pszDate1, *pszDate2);
	OutputDebugString(str);

	if (!bInited)
		return errOBJECT_NOT_INITIALIZED;

	if (PR_FAIL(error = GetStatus(szFileName, pOptionsData, dwOptionsSize, &dwData1, &dwData2, pqwHash)))
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

DWORD __declspec(dllexport) __cdecl DeleteStatus(char* szFileName, tBYTE* pOptionsData, DWORD dwOptionsSize)
{
	tERROR error;
	hIO hIo = NULL;
	tDWORD dwData1;
	tDWORD dwData2;
	
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
		OutputDebugString("DeleteStatus: Cannot open file '");
		OutputDebugString(szFileName);
		OutputDebugString("'\n");
		return error;
	}

	if (PR_FAIL(error = CALL_iFileSecureStatus_GetStatus(hChecker, hIo, pOptionsData, dwOptionsSize, &dwData1, &dwData2)))
	{
		if (error = errMODULE_UNKNOWN_FORMAT)
			OutputDebugString("Unknown format for file: ");
		else
		{
			OutputDebugString("DeleteStatus: Unknown error while GetStatus for file: ");
			OutputDebugString(szFileName);
			OutputDebugString("\n");
		}
	}
	else
	{
		if (PR_FAIL(error = CALL_iFileSecureStatus_DeleteStatus(hChecker, hIo)))
		{
			OutputDebugString("DeleteStatus: Cannot delete status for file: ");
			OutputDebugString(szFileName);
			OutputDebugString("\n");
		}
		else
		{
			OutputDebugString("DeleteStatus: Deleted status for file: ");
			OutputDebugString(szFileName);
			OutputDebugString("\n");
		}
	}
	CALL_SYS_ObjectClose(hIo);
	return error;
}

