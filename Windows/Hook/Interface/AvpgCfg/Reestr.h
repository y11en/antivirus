// Reestr.h: interface for the CReestr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REESTR_H__8A6A5593_04C1_11D1_8447_0020AFF24F91__INCLUDED_)
#define AFX_REESTR_H__8A6A5593_04C1_11D1_8447_0020AFF24F91__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "winreg.h"

#ifndef __REESTR_H
#define __REESTR_H

class CReestr
{
public:
	CReestr();
	CReestr(CString appname, HKEY hkey = HKEY_CURRENT_USER ,CString subkey = "Software\\KasperskyLab", 
		DWORD options = 0, REGSAM regsam = KEY_ALL_ACCESS );
	inline UINT GetError(void){return m_error;};
	virtual ~CReestr();
public:
	BOOL IsKey(CString name, DWORD nType);
	void SetValue(CString name, CString value);
	CString GetValue(CString name, CString defvalue = "", bool bSaveNew = true);
	void SetValue(CString name, DWORD value);
	DWORD GetValue(CString name, DWORD defvalue = 0);
	void SetBinaryValue(CString name, DWORD value, DWORD size);
	bool GetBinaryValue(CString name, DWORD value, DWORD size);
	DWORD DeleteKeyA(TCHAR* KeyName);
	DWORD DeleteValueA(TCHAR* KeyName);
private:
	HKEY m_hKey;
	CString m_Key;
	CString m_SubKey;
	DWORD m_options;
	REGSAM m_regsam;
	PHKEY m_phkey;
	HKEY m_OpenKey;

	DWORD m_cbData;
	BYTE m_buf[1024];
	DWORD m_dw;
	DWORD m_TypeStr;

	UINT m_error;
};

#endif

#endif // !defined(AFX_REESTR_H__8A6A5593_04C1_11D1_8447_0020AFF24F91__INCLUDED_)
