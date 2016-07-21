#include "NetWatcher.h"
#include <Prague/iface/i_root.h>
tERROR pr_call IDL_NAME(cNetWatcher)::Register(hROOT root) {
	mPROPERTY_TABLE(NetWatcher_PropTable)		
		PROPERTY_MAKE(Task::IDL_NAME(pgTASK_TM), offsetof(IDL_NAME(cNetWatcher), m_tm) - sizeof(cObjImpl), sizeof(((IDL_NAME(cNetWatcher)*)0)->m_tm), 0 | cPROP_BUFFER_READ | cPROP_BUFFER_WRITE, NULL, NULL)
		PROPERTY_MAKE(Task::IDL_NAME(pgTASK_STATE), offsetof(IDL_NAME(cNetWatcher), m_task_state) - sizeof(cObjImpl), sizeof(((IDL_NAME(cNetWatcher)*)0)->m_task_state), 0 | cPROP_BUFFER_READ, NULL, NULL)
		PROPERTY_MAKE(Task::IDL_NAME(pgTASK_SESSION_ID), offsetof(IDL_NAME(cNetWatcher), m_task_session_id) - sizeof(cObjImpl), sizeof(((IDL_NAME(cNetWatcher)*)0)->m_task_session_id), 0 | cPROP_BUFFER_READ, NULL, NULL)
		PROPERTY_MAKE(Task::IDL_NAME(pgTASK_ID), offsetof(IDL_NAME(cNetWatcher), m_task_id) - sizeof(cObjImpl), sizeof(((IDL_NAME(cNetWatcher)*)0)->m_task_id), 0 | cPROP_BUFFER_READ, NULL, NULL)
		PROPERTY_MAKE(Task::IDL_NAME(pgTASK_PERSISTENT_STORAGE), offsetof(IDL_NAME(cNetWatcher), m_task_persistent_storage) - sizeof(cObjImpl), sizeof(((IDL_NAME(cNetWatcher)*)0)->m_task_persistent_storage), 0 | cPROP_BUFFER_READ, NULL, NULL)
		PROPERTY_MAKE(Task::IDL_NAME(pgTASK_PARENT_ID), offsetof(IDL_NAME(cNetWatcher), m_task_parent_id) - sizeof(cObjImpl), sizeof(((IDL_NAME(cNetWatcher)*)0)->m_task_parent_id), 0 | cPROP_BUFFER_READ, NULL, NULL)
		PROPERTY_MAKE(NetWatcher::IDL_NAME(pgINTERFACE_COMPATIBILITY), 23, sizeof(tIID), cPROP_BUFFER_SHARED, NULL, NULL)
	mPROPERTY_TABLE_END(NetWatcher_PropTable)	
	tERROR error;

	PR_TRACE_A0( root, "Enter IDL_NAME(cNetWatcher)::Register method" );

	{
		error = CALL_Root_RegisterIFace(
			root,                                // root object
	
			IDL_NAME(cNetWatch)::IID,                     // interface identifier
			PID,                                 // plugin identifier
			0 /*ImplID*/,                        // implementation identifier
	
			NetWatchImpl_Version,                         // plugin version
			VID /*Version*/,                     // vendor id (must be: implementation version)
	
			&i_vtbl,                             // internal(kernel called) function table
			(sizeof(i_vtbl)/sizeof(tPTR)),       // internal function table size
			&e_vtbl,                             // external function table
			(sizeof(e_vtbl)/sizeof(tPTR)),       // external function table size
			NetWatcher_PropTable,                       // property table
			mPROPERTY_TABLE_SIZE(NetWatcher_PropTable), // property table size
			ActualObjectSize-sizeof(cObjImpl),   // memory size
			0                                    // interface flags
		);
	
		#ifdef _PRAGUE_TRACE_
			if ( PR_FAIL(error) )
				PR_TRACE( (root,prtDANGER,"IDL_NAME(cNetWatcher)(IDL_NAME(cNetWatch)) failed to register [err]",error) );
		#endif // _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			return error;
	}
	{
		error = CALL_Root_RegisterIFace(
			root,                                // root object
	
			IDL_NAME(cTask)::IID,                     // interface identifier
			PID,                                 // plugin identifier
			0 /*ImplID*/,                        // implementation identifier
	
			NetWatchImpl_Version,                         // plugin version
			VID /*Version*/,                     // vendor id (must be: implementation version)
	
			&i_vtbl,                             // internal(kernel called) function table
			(sizeof(i_vtbl)/sizeof(tPTR)),       // internal function table size
			&e_vtbl,                             // external function table
			(sizeof(e_vtbl)/sizeof(tPTR)),       // external function table size
			NetWatcher_PropTable,                       // property table
			mPROPERTY_TABLE_SIZE(NetWatcher_PropTable), // property table size
			ActualObjectSize-sizeof(cObjImpl),   // memory size
			0                                    // interface flags
		);
	
		#ifdef _PRAGUE_TRACE_
			if ( PR_FAIL(error) )
				PR_TRACE( (root,prtDANGER,"IDL_NAME(cNetWatcher)(IDL_NAME(cTask)) failed to register [err]",error) );
		#endif // _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			return error;
	}
	{
		error = CALL_Root_RegisterIFace(
			root,                                // root object
	
			IDL_NAME(cStdBase)::IID,                     // interface identifier
			PID,                                 // plugin identifier
			0 /*ImplID*/,                        // implementation identifier
	
			NetWatchImpl_Version,                         // plugin version
			VID /*Version*/,                     // vendor id (must be: implementation version)
	
			&i_vtbl,                             // internal(kernel called) function table
			(sizeof(i_vtbl)/sizeof(tPTR)),       // internal function table size
			&e_vtbl,                             // external function table
			(sizeof(e_vtbl)/sizeof(tPTR)),       // external function table size
			NetWatcher_PropTable,                       // property table
			mPROPERTY_TABLE_SIZE(NetWatcher_PropTable), // property table size
			ActualObjectSize-sizeof(cObjImpl),   // memory size
			0                                    // interface flags
		);
	
		#ifdef _PRAGUE_TRACE_
			if ( PR_FAIL(error) )
				PR_TRACE( (root,prtDANGER,"IDL_NAME(cNetWatcher)(IDL_NAME(cStdBase)) failed to register [err]",error) );
		#endif // _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			return error;
	}
	PR_TRACE_A1( root, "Leave IDL_NAME(cNetWatcher)::Register method, ret err", error );
	return error;
};

tERROR pr_call NetWatcher_Register(hROOT root) {
	return IDL_NAME(cNetWatcher)::Register(root);
}




