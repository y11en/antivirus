// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  25 April 2005,  13:20 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Ovcharik
// File Name   -- plugin_httpprotocoller.cpp
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>

#include <AntiPhishing/plugin/p_antiphishingtask.h>
#include <AV/iface/i_engine.h>
#include <Mail/plugin/p_mctask.h>
#include <ProductCore/structs/s_profiles.h>
// AVP Prague stamp end


#include <boost/config.hpp>
#include <boost/thread/detail/threadmon.hpp>
#include <windows.h>
#pragma warning(disable : 4786)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Task_Register( hROOT root );

// AVP Prague stamp end

#include <Mail/plugin/p_mctask.h>

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

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT  g_root = NULL;

///////////////////////////////////////////////////////////////////////////////
tPROPID g_propMessageIsIncoming;
tPROPID g_propMailerPID;
tPROPID g_propVirtualName;
tPROPID g_propContentType;
tPROPID g_propStartScan;
tPROPID g_propMessageCheckOnly;
tPROPID g_propMsgDirection;
///////////////////////////////////////////////////////////////////////////////

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
			g_root = (hROOT)hInstance;
			*pError = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pError=CALL_Root_ResolveImportTable(g_root,&count,import_table_variable...,PID_HTTPPROTOCOLLER)) ) {
			//   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_HTTPPROTOCOLLER );

			//register my custom property ids
			//if ( PR_FAIL(*pError=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}
			g_root->RegisterCustomPropId(&g_propMailerPID, npMAILER_PID, pTYPE_DWORD | pCUSTOM_HERITABLE);
			g_root->RegisterCustomPropId(&g_propMessageIsIncoming, npMESSAGE_IS_INCOMING, pTYPE_BOOL | pCUSTOM_HERITABLE);
			g_root->RegisterCustomPropId(&g_propVirtualName, npENGINE_VIRTUAL_OBJECT_NAME, pTYPE_STRING);
			g_root->RegisterCustomPropId(&g_propContentType, npCONTENT_TYPE, pTYPE_STRING);
			g_root->RegisterCustomPropId(&g_propStartScan, "START_HTTP_SCAN", pTYPE_DWORD);
			g_root->RegisterCustomPropId(&g_propMsgDirection, "HTTP_MSG_DIRECTION", pTYPE_DWORD);
			g_root->RegisterCustomPropId(&g_propMessageCheckOnly, npMESSAGE_CHECK_ONLY, pTYPE_DWORD | pCUSTOM_HERITABLE);
			// register my interfaces
			if ( PR_FAIL(*pError=Task_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"TaskEx\" interface"));
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
