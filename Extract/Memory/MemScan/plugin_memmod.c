// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  03 July 2002,  15:27 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Process memory scan
// Purpose     -- Доступ к памяти процессов для операционных систем Windows 9x/NT
// Author      -- Sobko
// File Name   -- plugin_memmod.h
// Additional info
//    Триада интерфейсов позволяющая енумерировать процессы в памяти, читать и изменять данные в этой памяти
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include "io.h"
#include "objptr.h"
#include "os.h"

#include <windows.h>
#include <stuff/memmanag.h>
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) 
{
	hROOT  root;
	// tDWORD count;

	switch( dwReason ) 
	{
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = root = (hROOT)hInstance;
			*pERROR = errOK;
			// register my interfaces
			{
				OSVERSIONINFO OsVersionInfo;
				ZeroMemory(&OsVersionInfo, sizeof(OSVERSIONINFO));
				OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

				if (GetVersionEx(&OsVersionInfo) == FALSE)
					return cFALSE;

				if (InitWinMemManagement(OsVersionInfo.dwPlatformId, _MM__Manage_Alloc, _MM__Manage_Free) == FALSE)
					return cFALSE;
			}

			if ( PR_FAIL(*pERROR=OS_Register(root)) ) 
			{
				PR_TRACE(( root, prtERROR, "cannot register \"OS\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=ObjPtr_Register(root)) ) 
			{
				PR_TRACE(( root, prtERROR, "cannot register \"ObjPtr\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=IO_Register(root)) ) 
			{
				PR_TRACE(( root, prtERROR, "cannot register \"IO\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



