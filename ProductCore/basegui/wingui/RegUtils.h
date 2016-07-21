////////////////////////////////////////////////////////////////////
//
//  RegUtils.h
//  Interface for the CRegUtils class.
//  AVP generic purposes stuff
//  Mezhuev Pavel, Kaspersky Labs. 2002
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#if !defined(__REGUTILS_H__)
#define __REGUTILS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRegKeyNode
{
public:
	CRegKeyNode(HKEY key = NULL, LPCTSTR sub_key = NULL, BOOL fCreate = FALSE) : m_cur_key(0), m_cur_val(0)
	{
		if( !key )
			m_key = NULL;
		else if( !sub_key )
			m_key = key;
		else if( fCreate )
			RegCreateKeyEx(key, sub_key, 0, 0, 0, KEY_ALL_ACCESS, NULL, &m_key, NULL);
		else
			RegOpenKeyEx(key, sub_key, 0, KEY_ALL_ACCESS, &m_key);
	}
	~CRegKeyNode()
	{
		if( m_key )
		{
			RegCloseKey(m_key);
			m_key = NULL;
		}
	}

	CRegKeyNode(CRegKeyNode &c){ *this = c; }
	operator = (CRegKeyNode &c){ Close(); memcpy(this, &c, sizeof(CRegKeyNode)); c.m_key = NULL; }

	bool IsValid(){ return m_key != NULL; }
	void Close(){ if( m_key ) RegCloseKey(m_key); }

	CRegKeyNode GetKey( LPCTSTR sub_key, BOOL fCreate = FALSE ){ return CRegKeyNode(m_key, sub_key, fCreate); }
	CRegKeyNode AddKey( LPCTSTR sub_key ){ return CRegKeyNode(m_key, sub_key, TRUE); }

	void DelKey(LPCTSTR sub_key)
	{
		GetKey(sub_key).DelKeys();
		RegDeleteKey(m_key, sub_key);
	}

	void DelKeys()
	{
		if( m_key )
		{
			char buffer[MAX_PATH];
			while( RegEnumKey(m_key, 0, buffer, MAX_PATH ) == ERROR_SUCCESS )
				DelKey(buffer);
		}
	}

	CRegKeyNode GetNextKey()
	{
		if( m_key )
		{
			char buffer[MAX_PATH];
			if( RegEnumKey(m_key, m_cur_key++, buffer, MAX_PATH ) == ERROR_SUCCESS )
				return GetKey(buffer);
		}
		return CRegKeyNode();
	}

	bool GetNextValueName(LPTSTR buffer, DWORD size)
	{
		if( !m_key ) return false;
		LONG res = RegEnumValue(m_key, m_cur_val++, buffer, &size, NULL, NULL, NULL, NULL);
		return res == ERROR_SUCCESS;
	}

	bool GetStringValue( LPCTSTR name, LPTSTR buffer, DWORD size )
	{
		if( !m_key ) return false;
		DWORD type;
		LONG res = RegQueryValueEx(m_key, name, 0, &type, (LPBYTE)buffer, &size);
		return res == ERROR_SUCCESS && type == REG_SZ;
	}

	DWORD GetDwordValue( LPCTSTR name, DWORD def_value = 0 )
	{
		if( !m_key ) return def_value;
		DWORD type, value = def_value, size = sizeof(DWORD);
		RegQueryValueEx(m_key, name, 0, &type, (LPBYTE)&value, &size);
		return value;
	}

	bool SetStringValue( LPCTSTR name, LPCTSTR buffer )
	{
		if( !m_key ) return false;
		LONG res = RegSetValueEx(m_key, name, 0, REG_SZ, (LPBYTE)buffer, strlen(buffer)+1);
		return res == ERROR_SUCCESS;
	}

	bool SetDwordValue( LPCTSTR name, DWORD value )
	{
		if( !m_key ) return false;
		LONG res = RegSetValueEx(m_key, name, 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));
		return res == ERROR_SUCCESS;
	}

	bool GetBinaryValue( LPCTSTR name, LPBYTE data, DWORD *size )
	{
		if( !m_key ) return false;
		DWORD type;
		LONG res = RegQueryValueEx(m_key, name, 0, &type, (LPBYTE)data, size);
		return type == REG_BINARY && res == ERROR_SUCCESS;
	}

	bool SetBinaryValue( LPCTSTR name, LPBYTE data, DWORD size )
	{
		if( !m_key ) return false;
		LONG res = RegSetValueEx(m_key, name, 0, REG_BINARY, data, size);
		return res == ERROR_SUCCESS;
	}

private:
	HKEY	m_key;
	DWORD	m_cur_val;
	DWORD	m_cur_key;
};

#endif // !defined(__REGUTILS_H__)
