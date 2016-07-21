// Localize.h: interface for the CLocalize class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#if !defined(AFX_LOCALIZE_H__4A74A5A3_4E38_11D3_9C00_0000E8905EBC__INCLUDED_)
#define AFX_LOCALIZE_H__4A74A5A3_4E38_11D3_9C00_0000E8905EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxmt.h"
#include "Reestr.h"

#define _ENGLISH "English"

class CLocalize  
{
public:
	CLocalize(CString SearchMask, CReestr* pReestr, HINSTANCE hInstanceModule);
	virtual ~CLocalize();

	bool IsDefaultLocale();
	BOOL ChangeLocale(CString* pLocale, bool bSave = true);
	
	CString GetLocalizeString(UINT uID);
	BOOL GetLocalizeString(UINT uID, CString* lpstrTarget);

	BOOL GetLMList(CStringArray* pstrArr, CStringArray* ptrLibName);
	BOOL GetModulePath(CString* pstr);
	CString m_strCurrentLocale;
private:
//	CCriticalSection m_CriticalSection;
	HINSTANCE m_hInstanceDefault;
	HINSTANCE m_hLocalizeModule;
	CReestr *m_pReestr;
	CString m_SearchMask;
};

#endif // !defined(AFX_LOCALIZE_H__4A74A5A3_4E38_11D3_9C00_0000E8905EBC__INCLUDED_)
