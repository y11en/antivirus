// Reestr.cpp: implementation of the CReestr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reestr.h"
#include "AutoPTCHAR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReestr::CReestr()
{

}

CReestr::CReestr(CString appname, HKEY hkey, CString subkey, 
	DWORD options, REGSAM regsam)
{
	m_error = 0;
	m_hKey = hkey;
	m_SubKey = subkey;
	m_options = options;
	m_regsam = regsam;
	m_TypeStr = REG_SZ;

	m_Key = m_SubKey + "\\" + appname;
	m_OpenKey = NULL;

	if(ERROR_SUCCESS!= RegOpenKeyEx(m_hKey, m_Key, m_options, m_regsam, &m_OpenKey))
		if(ERROR_SUCCESS != RegCreateKey(m_hKey, m_Key, &m_OpenKey))
			m_error = 1;
}

CReestr::~CReestr()
{
	if (m_OpenKey)
		RegCloseKey(m_OpenKey);
}

void CReestr::SetValue(PTCHAR name, CString& value)
{
	m_options = 0;
	
	AutoPTCHAR value_buf(&value);

	if (RegSetValueEx(m_OpenKey, name, m_options, REG_SZ, 
		(value_buf), value.GetLength() * sizeof(TCHAR)) != ERROR_SUCCESS)
	{
		//::MessageBox(NULL, "Can't set value", "Error", MB_ICONSTOP);
	}
}

CString CReestr::GetValue(PTCHAR name, CString defvalue, bool bSaveNew)
{
	CString tmp;
	m_cbData = sizeof(m_buf);

	if(ERROR_SUCCESS != RegQueryValueEx(m_OpenKey, (name), NULL, &m_TypeStr, m_buf, &m_cbData))
	{
		if (bSaveNew == true)
			SetValue(name, defvalue);

		return defvalue;
	}
	if (m_cbData)
		tmp = (TCHAR*) m_buf;
	else
		tmp.Empty();

	return tmp;
}

void CReestr::SetValue(PTCHAR name, DWORD value)
{
	if(RegSetValueEx(m_OpenKey, (LPCTSTR)name, 0, REG_DWORD, (LPBYTE)&value, sizeof(value)) != ERROR_SUCCESS)
	{
		//::MessageBox(NULL, "Can't set value", "Error", MB_ICONSTOP);
	}
}

DWORD CReestr::GetValue(PTCHAR name, DWORD defvalue)
{
	m_cbData = sizeof(m_dw);
	if(ERROR_SUCCESS != RegQueryValueEx(m_OpenKey, name, NULL, &m_TypeStr, (LPBYTE)&m_dw, &m_cbData))
	{
		SetValue(name, defvalue);

		return defvalue;
	}
	
	return m_dw;
}

void CReestr::SetBinaryValue(PTCHAR name, DWORD value, DWORD size)
{
	if(RegSetValueEx(m_OpenKey, (LPCTSTR)name, 0, REG_BINARY,(LPBYTE)value, size) != ERROR_SUCCESS)
	{
	//	::MessageBox(NULL, "Can't set value", "Error", MB_ICONSTOP);
	}
}

bool CReestr::GetBinaryValue(PTCHAR name, DWORD value, DWORD size)
{
	DWORD type = REG_BINARY;
	if(ERROR_SUCCESS != RegQueryValueEx(m_OpenKey, name, NULL, &type, (LPBYTE)value, &size))
		return false;

	return true;
}

BOOL CReestr::IsKey(PTCHAR name, DWORD nType)
{
	BOOL bRes = FALSE;

	switch (nType)
	{
	case REG_SZ:
		{
			CString tmp;
			m_cbData = sizeof(m_buf);
			if(ERROR_SUCCESS == RegQueryValueEx(m_OpenKey, name, NULL, &m_TypeStr, m_buf, &m_cbData))
				bRes = TRUE;
			if (m_cbData)
				tmp = m_buf;
			else
				tmp.Empty();
		}
		break;
	case REG_DWORD:
		{
			m_cbData = sizeof(m_dw);
			if(ERROR_SUCCESS == RegQueryValueEx(m_OpenKey, name, NULL, &m_TypeStr, (LPBYTE)&m_dw, &m_cbData))
				bRes = TRUE;
		}
		break;
	case REG_BINARY:
		{
			DWORD type = REG_BINARY;
			BYTE value;
			DWORD size;
			size = sizeof(BYTE);
			if(ERROR_SUCCESS == RegQueryValueEx(m_OpenKey, name, NULL, &type, (LPBYTE)&value, &size))
				bRes = TRUE;
		}
		break;
	default:
		break;
	}

	return bRes;
}

DWORD CReestr::DeleteKeyA(TCHAR* KeyName)
{
	return RegDeleteKey(m_OpenKey, KeyName);
}

DWORD CReestr::DeleteValueA(TCHAR* KeyName)
{
	return RegDeleteValue(m_OpenKey, KeyName);
}

