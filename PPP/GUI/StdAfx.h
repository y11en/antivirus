// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__BC8D35C2_2D0A_492B_8161_5D2B838C7598__INCLUDED_)
#define AFX_STDAFX_H__BC8D35C2_2D0A_492B_8161_5D2B838C7598__INCLUDED_

#define _OWNEROFMEMBER_PTR_EX( _owner_class, _member, _ptr )	\
	( ( _owner_class * )( ( unsigned char * )(_ptr) - ( unsigned char * )&( ( reinterpret_cast<_owner_class *>( 0 ) ) -> _member ) ) )
#define _OWNEROFMEMBER_PTR( _owner_class, _member )	_OWNEROFMEMBER_PTR_EX(_owner_class, _member, this)

#define BASECLASS_OFFSET(_class, _class_base) ((tDWORD)(_class_base *)(_class *)4 - 4)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef ADMINGUI_EXPORTS

#include "../AdminGUI/StdAfx.h"

#else

#pragma warning(disable : 4786) // identifier was truncated to '255' characters in the debug information
#pragma warning(disable : 4355) // 'this' : used in base member initializer list

#ifndef _WIN32_WINNT
#define _WIN32_WINNT  0x0500
#endif
#define WINVER        0x0500
#define _WIN32_IE     0x0500

#include <pr_remote.h>
#include <basegui.h>
#include <binding.h>
#include <gridview.h>
#include <ItemProps.h>
#include <GuiLoader.h>

#include <structs/s_profiles.h>
#include <structs/s_gui.h>
#include <structs/s_taskmanager.h>
#include <structs/s_bl.h>
#include <structs/s_ipresolver.h>
#include <structs/s_qb.h>

#include "ver_mod.h"

extern CRootItem *      g_pRoot;

#include "TaskView.h"
#include "ListsView.h"
#include "FWRules.h"
#include "TaskProfileProduct.h"
#include "avpgui.h"
#include "resource.h"

#if defined (__unix__)
#include <sys/types.h>	
#include <sys/stat.h>
#include <unistd.h>
#include <unix/compatutils.h>
#define INFINITE (-1)
#define GetTickCount() 0
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // ADMINGUI_EXPORTS

#endif // !defined(AFX_STDAFX_H__BC8D35C2_2D0A_492B_8161_5D2B838C7598__INCLUDED)
