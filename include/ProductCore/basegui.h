// basegui.h: interface for the Cbasegui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEGUI_H__ACE32930_00DB_4B40_BD2D_AD0EE47B244F__INCLUDED_)
#define AFX_BASEGUI_H__ACE32930_00DB_4B40_BD2D_AD0EE47B244F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _BASEGUI_VER 0x0200

#pragma warning(disable:4786)

#include <vector>
#include <map>
#include <set>

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>
#include <Prague/pr_serializable.h>
#include <Prague/pr_registry.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_io.h>

#include <ProductCore/basedefs.h>

#ifndef TODO
	#define MAKESTR(x)		#x
	#define MAKESTR2(x)		MAKESTR(x)
	#define TODO(msg) message(__FILE__"(" MAKESTR2(__LINE__) "): TODO: " msg)
#endif

//////////////////////////////////////////////////////////////////////

class tIniKey : public tString { public: tString val; };

typedef std::vector<tIniKey> tKeys;
typedef tString              tStrText;
typedef cStrObj              tObjPath;
typedef std::vector<tString> tSections;

#define RECT_CX(_rc)	((_rc).right-(_rc).left)
#define RECT_CY(_rc)	((_rc).bottom-(_rc).top)

#define sswitch(str)    { const tCHAR* __str = str;
#define stcase(str)     if( __str && !strcmp(__str,#str) ) { __str = NULL;
#define scase(str)      if( __str && !strcmp(__str,str) ) { __str = NULL;
#define sdefault()      if( __str ) {
#define sbreak          }
#define send            }

#define __itoa(num, base) _itoa(num, (char *)alloca(33), base)
#define __itoa10(num)     __itoa(num, 10)
#define __itoa16(num)     __itoa(num, 16)

#define __itow(num, base) _itow(num, (wchar_t *)alloca(65), base)
#define __itow10(num)     __itow(num, 10)
#define __itow16(num)     __itow(num, 16)

#define istcsym(c)      (_istalnum(c)||((c) == '_'))

#define cCP_TCHAR        cCP_ANSI

#define IS_TIME_VALID(_time)       ((_time)!=-1)
#define IS_DATETIME_VALID(_time)   ((_time)!=-1&&(_time)!= 0)

class CRootItem;
class CPlaceHolder;

//////////////////////////////////////////////////////////////////////

#include "ItemBase.h"
#include "ItemRoot.h"

#define PR_BASE_GUI_PROP	"PR_BASE_GUI_PROP"

typedef tBOOL (* tGUICreateRoot)(hROOT hRoot, tDWORD nFlags, CRootSink * pSink, CRootItem ** pRoot);

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_BASEGUI_H__ACE32930_00DB_4B40_BD2D_AD0EE47B244F__INCLUDED_)
