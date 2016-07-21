// Localizer.h: interface for the CLocalizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCALIZER_H__7C5D4F81_47B4_49CC_9883_35E488975581__INCLUDED_)
#define AFX_LOCALIZER_H__7C5D4F81_47B4_49CC_9883_35E488975581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <string>

#ifdef UNICODE
	#define STROBJ_CSTR(_o) (_o).data()
#else
	#define STROBJ_CSTR(_o) (_o).c_str(cCP_ANSI)
#endif // UNICODE

struct cLocalizedResources
{
	cLocalizedResources();
	~cLocalizedResources();

	bool Load(HINSTANCE hInstance, hROOT hRoot);

	std::wstring m_wsCaption;
	std::string m_sCaption;
	std::string m_sStatistics;
	std::string m_sProductNotLoaded;
	std::string m_sViewReport;

	tDWORD      m_nLocale;

protected:
	bool        m_bInited;
};

#endif // !defined(AFX_LOCALIZER_H__7C5D4F81_47B4_49CC_9883_35E488975581__INCLUDED_)
