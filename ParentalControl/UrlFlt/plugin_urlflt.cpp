// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  27 November 2006,  11:50 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Nechaev
// File Name   -- plugin_urlflt.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>

#include <ParentalControl/plugin/p_urlflt.h>
// AVP Prague stamp end

#include <string>

#include "CharsetDecode.h"


// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Task_Register( hROOT root );

// AVP Prague stamp end



#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_loader.h>
IMPORT_TABLE_END

//////////////////////////////////////////////////////////////////////////
#include <boost/config.hpp>
#include <boost/thread/detail/threadmon.hpp>
#include <windows.h>

#include <algorithm>
#include <list>
#include <set>

typedef void (__cdecl * handler)(void);
typedef std::list<handler> exit_handlers;
typedef std::set<exit_handlers*> registered_handlers;

namespace
{
	CRITICAL_SECTION cs;
	DWORD key;
	registered_handlers registry;
}
//////////////////////////////////////////////////////////////////////////

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT  g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pError ) {
	// tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
			InitializeCriticalSection(&cs);
			key = TlsAlloc();
			break;
		case DLL_PROCESS_DETACH:
			{
				// Assume the main thread is ending (call its handlers) and
				// all other threads have already ended.  If this DLL is
				// loaded and unloaded dynamically at run time
				// this is a bad assumption, but this is the best we can do.
				exit_handlers* handlers =
					static_cast<exit_handlers*>(TlsGetValue(key));
				if (handlers)
				{
					for (exit_handlers::iterator it = handlers->begin();
						it != handlers->end(); ++it)
					{
						(*it)();
					}
				}

				// Destroy any remaining exit handlers.  Above we assumed
				// there'd only be the main thread left, but to insure we
				// don't get memory leaks we won't make that assumption
				// here.
				EnterCriticalSection(&cs);
				for (registered_handlers::iterator it = registry.begin();
					it != registry.end(); ++it)
				{
					delete (*it);
				}
				LeaveCriticalSection(&cs);
				DeleteCriticalSection(&cs);
				TlsFree(key);
			}
			break;
		case DLL_THREAD_ATTACH :
			break;
		case DLL_THREAD_DETACH :
			{
				// Call the thread's exit handlers.
				exit_handlers* handlers =
					static_cast<exit_handlers*>(TlsGetValue(key));
				if (handlers)
				{
					for (exit_handlers::iterator it = handlers->begin();
						it != handlers->end(); ++it)
					{
						(*it)();
					}

					// Remove the exit handler list from the registered lists
					// and then destroy it.
					EnterCriticalSection(&cs);
					registry.erase(handlers);
					LeaveCriticalSection(&cs);
					delete handlers;
				}
			}
			break;

		case PRAGUE_PLUGIN_LOAD :
			_ChsetDec_Init();
			
			g_root = (hROOT)hInstance;
			*pError = errOK;
			//resolve  my imports
			if ( PR_FAIL(*pError=CALL_Root_ResolveImportTable(g_root,NULL,import_table,PID_URLFLT)) )
			{
			   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for PID_URLFLT" ));
			   return cFALSE;
			}

			/*if ( PR_FAIL(CALL_Root_ResolveImportTable(g_root,NULL,import_table_win32api,PID_URLFLT)) )
			{
			   PR_TRACE(( g_root, prtERROR, "cannot resolve Win32API import table for PID_URLFLT" ));
			}*/

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_URLFLT );

			//register my custom property ids
			//if ( PR_FAIL(*pError=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pError=Task_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"Task\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			g_root = NULL;

			_ChsetDec_Deinit();
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end
extern "C" BOOST_THREAD_DECL int on_thread_exit(void (__cdecl * func)(void))
{
	// Get the exit handlers for the current thread, creating and registering
	// one if it doesn't exist.
	exit_handlers* handlers = static_cast<exit_handlers*>(TlsGetValue(key));
	if (!handlers)
	{
		try
		{
			handlers = new exit_handlers;
			// Handle broken implementations of operator new that don't throw.
			if (!handlers)
				return -1;
		}
		catch (...)
		{
			return -1;
		}

		// Attempt to set a TLS value for the new handlers.
		if (!TlsSetValue(key, handlers))
		{
			delete handlers;
			return -1;
		}

		// Attempt to register this new handler so that memory can be properly
		// cleaned up.
		try
		{
			EnterCriticalSection(&cs);
			registry.insert(handlers);
			LeaveCriticalSection(&cs);
		}
		catch (...)
		{
			LeaveCriticalSection(&cs);
			delete handlers;
			return -1;
		}
	}

	// Attempt to add the handler to the list of exit handlers. If it's been
	// previously added just report success and exit.
	try
	{
		handlers->push_front(func);
	}
	catch (...)
	{
		return -1;
	}

	return 0;
}
