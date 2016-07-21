// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
//#define WINVER		0x0400
//#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0500
#define _RICHEDIT_VER	0x0100

#include <pr_loadr.h>
#include <basegui.h>
#include <guiloader.h>
#include <itemprops.h>

#include <atlbase.h>
#include <atlcoll.h>
#include <atlstr.h>

#include "atlapp.h"
#include <atlwin.h>
#include <atlddx.h>
#include <atltypes.h>

#include <atlapp.h>

#include <atlframe.h>
#include <atldlgs.h>

extern CAppModule	_Module;
extern hROOT		g_root;

#include <atlsplit.h>
#include <atlgdi.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlctrlw.h>

// ########################################################################
// Prague

#include <pr_cpp.h>

// ########################################################################

#define NOTIFY_HANDLER_EX(hwnd, cd, func) \
	if(uMsg == WM_NOTIFY && hwnd == ((LPNMHDR)lParam)->hwndFrom && cd == ((LPNMHDR)lParam)->code) \
	{ \
		bHandled = TRUE; \
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define _OWNEROFMEMBER_PTR_EX( _owner_class, _member, _ptr )	\
	( ( _owner_class * )( ( unsigned char * )(_ptr) - ( unsigned char * )&( ( reinterpret_cast<_owner_class *>( 0 ) ) -> _member ) ) )
#define _OWNEROFMEMBER_PTR( _owner_class, _member )	_OWNEROFMEMBER_PTR_EX(_owner_class, _member, this)

#define _USES_CONVERSION(_cp)	USES_CONVERSION; _acp = _cp;

#define _REFCLASS_NULL(_c)	(_c&)*(_c *)NULL
