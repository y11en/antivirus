// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__BC8D35C2_2D0A_492B_8161_5D2B838C7598__INCLUDED_)
#define AFX_STDAFX_H__BC8D35C2_2D0A_492B_8161_5D2B838C7598__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#include <stdlib.h>

#ifndef _DEBUG
	#define _SECURE_SCL 0
#endif

#ifdef WIN32
	#include <malloc.h>
#endif // !WIN32
#include <ProductCore/basegui.h>
#include <ProductCore/ItemProps.h>
#include <ProductCore/binding.h>
#include <ProductCore/gridview.h>
#include "Formats.h"

using namespace std;

#include <Prague/pr_serializable.h>
#include <Prague/pr_serdescriptor.h>

#ifndef WIN32

#include <unix/compatutils/compatutils.h>

#endif // !WIN32

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define OWNEROFMEMBER_PTR( _owner_class, _member )	\
	((_owner_class *)((unsigned char *)this - (unsigned char *)&((reinterpret_cast<_owner_class *>(0))->_member)))
#define BASECLASS_OFFSET(_class, _class_base) ((tDWORD)(_class_base *)(_class *)4 - 4)

//////////////////////////////////////////////////////////////////////

extern tString	GetNextToken(LPCSTR &strText, bool bBraced=true);
extern tDWORD   GetAlign( const tString& strProp );

//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BC8D35C2_2D0A_492B_8161_5D2B838C7598__INCLUDED)
