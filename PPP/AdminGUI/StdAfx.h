// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BA1C0C50_933B_449F_909F_76D8ECD373BE__INCLUDED_)
#define AFX_STDAFX_H__BA1C0C50_933B_449F_909F_76D8ECD373BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning (disable: 4355)

#include <windows.h>
#include <tchar.h>
#include <objbase.h>
#include <string.h>

class cCS : protected CRITICAL_SECTION
{
public:
	cCS()        { InitializeCriticalSection(this); }
	~cCS()       { DeleteCriticalSection(this); }
	void Enter() { EnterCriticalSection(this); }
	void Leave() { LeaveCriticalSection(this); }
};

class cAutoLockerCS
{
public:
	cAutoLockerCS(cCS &cs) : m_cs(cs) { m_cs.Enter(); }
	~cAutoLockerCS()                  { m_cs.Leave(); }

protected:
	cCS & m_cs;
};

// CS includes
#define CLASSES_EXPORT
#include <std\thr\sync.h>
#include <std\par\params.h>
#include <std\trc\trace.h>
#include <std\err\klerrors.h>
#include <gui\ConfiguratorInterfaces.h>
#include <srvp\pkg\pkgcfg.h>

using namespace KLPlugin;
typedef IKLConfigurator::CONFIGURATOR_TYPES CONFIGURATOR_TYPES;
typedef IKLConfigurator::PAGE_TYPES PAGE_TYPES;
typedef KLSTD::CAutoPtr<KLPAR::Params> PParams;
typedef KLSTD::CAutoPtr<KLPAR::Value> PValue;

#define __hGUI_NOT_IMPLEMENTED

// Prague includes
#include <prague.h>
#include <pr_serializable.h>
#include <structs/s_profiles.h>
#include <structs/s_gui.h>
#include <structs/s_taskmanager.h>
#include <structs/s_licensing.h>
#include <structs/s_ids.h>
#include <structs/s_ahfw.h>
#include <structs/s_pdm.h>
#include <structs/s_qb.h>
#include <structs/s_ipresolver.h>
#include <structs/s_mc.h>
#include <structs/s_mc_oe.h>
#include <structs/s_antispam.h>
#include <structs/s_mc_trafficmonitor.h>
#include <structs/s_antidial.h>
#include <common/transport.h>
#include <common/updater.h>

#include "ver_mod.h"

// Skin engine includes
#include <basegui.h>
#include <binding.h>
#include <ItemProps.h>
#include "TaskView.h"

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_string<TCHAR> tString;

typedef std::wstring wstring;
typedef std::vector<wstring> wstrings;

typedef std::string string;
typedef std::vector<string> strings;

// common definitions
#include "TaskProfileAdm.h"
#include "defines.h"
#include "AdminGUI.h"
#include "resource.h"

#if defined(UNICODE) || defined(_UNICODE)
#define cCP_TCHAR cCP_UNICODE
#else
#define cCP_TCHAR cCP_ANSI
#endif

#define lineno_to_str2(x)  #x
#define lineno_to_str(x) lineno_to_str2(x)
#define todo(desc)        message(__FILE__ "(" lineno_to_str(__LINE__) "): TODO: " desc)

#if 1
#undef todo
#define todo(desc)
#endif

#define err2prtl(err) (PR_SUCC(err) ? prtIMPORTANT : prtERROR)

namespace KLSTD
{
	inline void TraceAdmGui(const wchar_t *format, ...)
	{
		va_list arg;
		va_start(arg, format);
		Trace(3, L"kavgui", format, VA_LIST_ADDR(arg));
		va_end(arg);
	}
}

#define ADM_TRACE(expr) KLSTD::TraceAdmGui expr

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BA1C0C50_933B_449F_909F_76D8ECD373BE__INCLUDED_)
