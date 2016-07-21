// basegui.cpp: implementation of the Cbasegui class.
//
//////////////////////////////////////////////////////////////////////

#define PR_IMPEX_DEF

#include "StdAfx.h"

#define IMPEX_IMPORT_LIB
	#include <Prague/plugin/p_win32_nfio.h>
	#include <Prague/iface/e_ktrace.h>
	#include <Prague/iface/e_loader.h>
	#include <Prague/iface/e_ktime.h>
	#include <Prague/plugin/p_string.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_ktrace.h>
	#include <Prague/iface/e_loader.h>
	#include <Prague/iface/e_ktime.h>
	#include <Prague/plugin/p_win32_nfio.h>
IMPORT_TABLE_END

cSerRegistrar * g_serializable_registrar_head = NULL;
hROOT           g_root;

PR_MAKE_TRACE_FUNC;

//////////////////////////////////////////////////////////////////////

tDWORD GUICreateRoot(hROOT hRoot, tDWORD nFlags, CRootSink * pSink, CRootItem ** pRoot)
{
	*pRoot = CRootItem::CreateInstance(nFlags, pSink);
	return cTRUE;
}

//////////////////////////////////////////////////////////////////////

extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR )
{
	switch( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		CRootItem::InitInstance((HINSTANCE)hInstance);
		break;
	case DLL_PROCESS_DETACH:
		CRootItem::ExitInstance();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		CRootItem::ThreadDetach();
		break;

	case PRAGUE_PLUGIN_LOAD:
		{
			g_root = (cRoot *)hInstance;
			
			tPROPID prop;
			if( PR_FAIL(g_root->RegisterCustomPropId(&prop, PR_BASE_GUI_PROP, pTYPE_PTR)) )
				return cFALSE;
			
			g_root->propSetPtr(prop, (tPTR)GUICreateRoot);
			
			g_root->ResolveImportTable(NULL, import_table, PID_APPLICATION);
			
			for(const cSerRegistrar * tmp = g_serializable_registrar_head; tmp; tmp = tmp->m_next)
				g_root->RegisterSerializableDescriptor(tmp->m_dsc);
		}
		break;
	
	case PRAGUE_PLUGIN_UNLOAD:
		{
			for(const cSerRegistrar * tmp = g_serializable_registrar_head; tmp; tmp = tmp->m_next)
				g_root->UnregisterSerializableDescriptor(tmp->m_dsc);
		}
		break;
	}
	
	return cTRUE;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#undef  __pr_serializable_h
#define __DECLARE_SERIALIZABLE

#include <Prague/pr_serializable.h>

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiParams, 0 )
	SER_VALUE_M( nRes,       tid_PTR )
	SER_VALUE_M( nVal1,      tid_PTR )
	SER_VALUE_M( nVal2,      tid_PTR )
	SER_VALUE_M( nVal3,      tid_PTR )
	SER_VALUE_M( nVal4,      tid_PTR )
	SER_VALUE_M( nVal5,      tid_PTR )
	SER_VALUE_M( strVal1,    tid_STRING_OBJ )
	SER_VALUE_M( strVal2,    tid_STRING_OBJ )
	SER_VALUE_M( strVal3,    tid_STRING_OBJ )
	SER_VALUE_M( strVal4,    tid_STRING_OBJ )
	SER_VALUE_M( strVal5,    tid_STRING_OBJ )
	SER_VALUE_M( strVal6,    tid_STRING_OBJ )
	SER_VALUE_M( strVal7,    tid_STRING_OBJ )
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( CAttachedFile, 0 )
	SER_VALUE_M( strObjPath,            tid_STRING_OBJ)
	SER_VALUE_M( strName,               tid_STRING_OBJ)
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( CDeviceAction, 0 )
	SER_VALUE_M( cDevice,               tid_CHAR)
	SER_VALUE_M( nAction,               tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( CSendMailInfo, 0 )
	SER_VECTOR_M(aAddresses,            tid_STRING_OBJ)
	SER_VALUE_M( strSubject,            tid_STRING_OBJ)
	SER_VALUE_M( strBody,               tid_STRING_OBJ)
	SER_VECTOR_M(aAattaches,            CAttachedFile::eIID)
	SER_VALUE_M( strFrom,               tid_STRING_OBJ)
	SER_VALUE_M( strUserName,           tid_STRING_OBJ)
	SER_VALUE_M( strPassword,           tid_STRING_OBJ)
	SER_VALUE_M( strSMTPServer,         tid_STRING_OBJ)
	SER_VALUE_M( nSMPTPort,             tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiMsgBox, 0 )
	SER_VALUE( m_strCaption,      tid_STRING_OBJ, "Caption")
	SER_VALUE( m_strText,         tid_STRING_OBJ, "Text")
	SER_VALUE( m_dwIcon,          tid_DWORD, "Icon")
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( cGridViewTemplate, 0 )
	SER_VALUE( m_sViewName,       tid_STRING_OBJ, "ViewName")
	SER_VALUE( m_sViewData,       tid_STRING_OBJ, "ViewData")
IMPLEMENT_SERIALIZABLE_END()

#undef __DECLARE_SERIALIZABLE

//////////////////////////////////////////////////////////////////////
