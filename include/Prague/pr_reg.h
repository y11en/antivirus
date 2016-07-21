/*-----------------02.05.99 12:57-------------------
 * Project Prague                                 *
 * Interface Registration definition              *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 * revision 2                                     *
--------------------------------------------------*/

#ifndef __PR_REG_H
#define __PR_REG_H

#if !defined(DLL_PROCESS_ATTACH)
	#define DLL_PROCESS_ATTACH 1
	#define DLL_THREAD_ATTACH  2
	#define DLL_THREAD_DETACH  3
	#define DLL_PROCESS_DETACH 0
#endif

#define PRAGUE_PLUGIN_LOAD       100
#define PRAGUE_PLUGIN_UNLOAD     101
#define PRAGUE_PLUGIN_PRE_UNLOAD 102

// generage compiller error
#define PluginInit  (_ YOU_HAVE_TO_REIMPLEMENT_IT_NOW____SEE__pr_reg_h__file)

#define PLUGIN_INIT_ENTRY "DllMain"
#if defined (_MSC_VER)
typedef tBOOL (__stdcall* tPluginInit)( tPTR hInstance, tDWORD dwReason, tERROR* pERROR );
#else
typedef tBOOL (* tPluginInit)( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) __stdcall;
#endif

// standart Windows DllMain defined as
// BOOL WINAPI DllMain(HINSTANCE hInstance, tDWORD dwReason, LPVOID /*lpReserved*/)
//
// we'll have a
// tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR )
// don't forget set it as Entry point in a project
//
// instead of
// tERROR PluginInit( hROOT root );
//
// dwReason is one of: 
//   DLL_PROCESS_ATTACH
//   DLL_PROCESS_DETACH
//   DLL_THREAD_ATTACH
//   DLL_THREAD_DETACH  -- Windows defined rules
//   PRAGUE_PLUGIN_LOAD -- init plugin ( register interfaces, export/import tables, custom property ids, alloc some resources and so on... )
//   PRAGUE_PLUGIN_UNLOAD -- free all used resources
//
// parameters are: 
//   when dwReason is DLL_... -- follow Windows rules
//   when dwReason is PRAGUE_PLUGIN_...:
//     hInstance -- can be used as ((hROOT)hInstance)
//     pERROR -- pointer to result value
//   
//
// call sequence will be:
//    DLL_PROCESS_ATTACH
//    DLL_THREAD_ATTACH
//    PRAGUE_PLUGIN_LOAD 
//
//    DLL_THREAD_ATTACH - by thread attached to the process
//    DLL_THREAD_DETACH - by thread detached from the process
//
//    PRAGUE_PLUGIN_UNLOAD 
//    DLL_THREAD_DETACH
//    DLL_PROCESS_DETACH
//
// loader will emulate this sequence for any plugin formats (now two formats are supported -- "dll" and "prague")
//
//
//
//
// so an example:
/*
tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	switch( dwReason ) {

		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH : 
			break;

		case PRAGUE_PLUGIN_LOAD :
			// register my interfaces
			if ( PR_FAIL(*pERROR==MyIFace_Register((hROOT)hInstance)) )
				return cFALSE;

			// register my custom property ids
			if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId((hROOT)hInstance,&my_custom_prop_id,"my custom property id",pTYPE_DWORD)) )
				return cFALSE;

			// register my exports
			CALL_Root_RegisterExportTable( (hROOT)hInstance, 0, my_export_table, PID_OF_MY_PLUGIN );

			// resolve  my imports
			if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable((hROOT)hInstance,0,my_import_table,PID_OF_MY_PLUGIN)) )
				return cFALSE;

			// use some system resources
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
*/

#endif



