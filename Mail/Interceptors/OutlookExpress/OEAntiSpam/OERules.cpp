// OERules.cpp: implementation of the COERules class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OERules.h"
#include "Utility.h"
#include "StringCollector.h"
#include <boost/scoped_array.hpp>
#include "StringTokenizer.h"
#include "../../TheBatAntispam/EnsureCleanup.h"
#include "Tracing.h"
#include "OETricks.h"

#pragma warning(push)
#pragma warning(disable:4800)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

namespace
{
	LPCTSTR MAIN_IDENTITIES_KEY = _T("Identities");
	LPCTSTR OE_MAIN_KEY = _T("Software\\Microsoft\\Outlook Express\\5.0");	// relative to LastUserID
	LPCTSTR OE_MAIN_KEY_VISTA = _T("Software\\Microsoft\\Windows Mail");	// relative to LastUserID
	LPCTSTR FOLDERSDBX_FILE = _T("folders.dbx");
	LPCTSTR FOLDERSMSGSTORE_FILE = _T("WindowsMail.MSMessageStore");

	LPCTSTR LAST_USERID_VALUE = _T("Last User ID");
	LPCTSTR START_AS_VALUE = _T("Start As");
	LPCTSTR STOREROOT_VALUE = _T("Store Root");
	LPCTSTR EMPTY_IDENTITY = _T("{00000000-0000-0000-0000-000000000000}");
	LPCTSTR RUNNING_VALUE = _T("Running");

	LPCTSTR RULE_FLAGS_VALUE = _T("Flags");
	LPCTSTR RULE_TYPE_VALUE = _T("Type");
	LPCTSTR RULE_VALUE_VALUE = _T("Value");
	LPCTSTR RULE_VALUETYPE_VALUE = _T("ValueType");
	LPCTSTR RULE_LOGIC_VALUE = _T("Logic");

	LPCTSTR RULE_ENABLED_VALUE = _T("Enabled");
	LPCTSTR RULE_NAME_VALUE = _T("Name");
	LPCTSTR RULE_VERSION_VALUE = _T("Version");

	LPCTSTR ORDER_VALUE = _T("Order");
}

DWORD	COERule::m_dwInternal1Vista;
DWORD	COERule::m_dwInternal2Vista;

COERule::COERule(LPCTSTR szRuleName, LPCTSTR szDisplayName) :
	m_bInternalsInited(false), m_bEnabled(true)
{
	if (szRuleName)
		m_strRuleName = szRuleName;

	if (szDisplayName)
		m_strDisplayName = szDisplayName;

	m_bInternalsInited = _ReadInternalValues();
}

bool COERule::_ReadInternalValues() const
{
	// read two DWORDs from folders.dbx, offset 0x024BC
	tstring strKeyName = GetMainOEKey();
	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, strKeyName.c_str(), KEY_READ) != ERROR_SUCCESS)
		return false;

	TCHAR szBuf[MAX_PATH];
	DWORD dwBufSize = MAX_PATH;
	if (key.QueryStringValue(STOREROOT_VALUE, szBuf, &dwBufSize) != ERROR_SUCCESS)
		return false;

	dwBufSize = MAX_PATH;
	TCHAR szExpanded[MAX_PATH];
	ExpandEnvironmentStrings(szBuf, szExpanded, dwBufSize);
	
	tstring strFileName(szExpanded);
	
	// add slash if it's missing
	if ((strFileName.size() > 0) && (*strFileName.rbegin() != '\\') && (*strFileName.rbegin() != '/'))
		strFileName += '\\';
	
	strFileName += FOLDERSDBX_FILE;
	
	CEnsureCloseHandle hFoldersFile = CreateFile(strFileName.c_str(), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFoldersFile.IsInvalid())
	{
		tstring msg(_T("Failed to open folders.dbx: "));
		msg += strFileName;
		ASTRACE(msg.c_str());
		return false;
	}
		
	DWORD arr[2];
	SetFilePointer(hFoldersFile, 0x024BC, NULL, FILE_BEGIN);
	
	DWORD dwRead = 0;
	if (!ReadFile(hFoldersFile, arr, 2 * sizeof(DWORD), &dwRead, NULL))
		return false;

	m_dwInternal1 = arr[0];
	m_dwInternal2 = arr[1];
	return true;
}

bool COERule::ReadInternalValuesVista()
{
	// read two DWORDs from folders.dbx, offset 0x024BC
	tstring strKeyName = GetMainOEKey();
	tstring sTrace;
	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, strKeyName.c_str(), KEY_READ) != ERROR_SUCCESS)
	{
		sTrace = _T("Failed to open: ");
		sTrace += strKeyName;
		sTrace += _T(" ");
		sTrace += A2CT((boost::lexical_cast<std::string>(GetLastError())).c_str());
		ASTRACE(sTrace.c_str());
		return false;
	}

	TCHAR szBuf[MAX_PATH];
	DWORD dwBufSize = MAX_PATH;
	if (key.QueryStringValue(STOREROOT_VALUE, szBuf, &dwBufSize) != ERROR_SUCCESS)
	{
		sTrace = _T("Failed to open store root: ");
		sTrace += A2CT((boost::lexical_cast<std::string>(GetLastError())).c_str());
		ASTRACE(sTrace.c_str());
		return false;
	}

	dwBufSize = MAX_PATH;
	TCHAR szExpanded[MAX_PATH];
	ExpandEnvironmentStrings(szBuf, szExpanded, dwBufSize);

	tstring strFileName(szExpanded);

	// add slash if it's missing
	if ((strFileName.size() > 0) && (*strFileName.rbegin() != '\\') && (*strFileName.rbegin() != '/'))
		strFileName += '\\';

	strFileName += FOLDERSMSGSTORE_FILE;

	sTrace = _T("Reading from: ");
	sTrace += strFileName;
	ASTRACE(sTrace.c_str());

	CEnsureCloseHandle hFoldersFile = CreateFile(strFileName.c_str(), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFoldersFile.IsInvalid())
	{
		sTrace = _T("Failed to open WindowsMail.MSMessageStore: ");
		sTrace += strFileName;
		ASTRACE(sTrace.c_str());
		return false;
	}

	DWORD arr[2];
	SetFilePointer(hFoldersFile, 0x0004C069, NULL, FILE_BEGIN);

	DWORD dwRead = 0;
	if (!ReadFile(hFoldersFile, arr, 2 * sizeof(DWORD), &dwRead, NULL))
	{
		sTrace = _T("Failed to read: ");
		sTrace += A2CT((boost::lexical_cast<std::string>(GetLastError())).c_str());
		ASTRACE(sTrace.c_str());
		return false;
	}

	m_dwInternal1Vista = arr[0];
	m_dwInternal2Vista = arr[1];

	sTrace = _T("Int values: ");
	sTrace += A2CT((boost::lexical_cast<std::string>(m_dwInternal1Vista)).c_str());
	sTrace += _T(", ");
	sTrace += A2CT((boost::lexical_cast<std::string>(m_dwInternal2Vista)).c_str());
	ASTRACE(sTrace.c_str());

	return true;
}

bool COERule::AddSubjCriteria(LPCTSTR szSubjString)
{
	m_Criteria.m_strSubjString = szSubjString;
	return true;
}

bool COERule::AddAction(RuleActions nAction, OPTIONAL __int64 dwFolderID, DWORD dwColorID)
{
	TAction action;
	action.m_nType = nAction;
	action.m_dwFolderID = dwFolderID;
	action.m_dwColorID = dwColorID;
	m_arrActions.push_back(action);
	return true;
}

bool COERule::LoadRule()
{
	if (m_strRuleName.size() <= 0)
		return false;

	// ... do we need it?

	return true;
}

tstring COERule::_SaveAction(CRegKey& key, DWORD dwActionIndex, const TAction& action) const
{
	// form the XXX key name
	TCHAR szKeyName[10];
	_stprintf(szKeyName, _T("%03d"), dwActionIndex);

	// create the subkey
	CRegKey subkey;
	if (subkey.Create(key, szKeyName, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE) != ERROR_SUCCESS)
		return _T("");

	subkey.SetDWORDValue(RULE_FLAGS_VALUE, 0);
	subkey.SetDWORDValue(RULE_TYPE_VALUE, action.m_nType);

	switch (action.m_nType)
	{
	case RA_HILIGHT:
		subkey.SetDWORDValue(RULE_VALUE_VALUE, action.m_dwColorID);
		subkey.SetDWORDValue(RULE_VALUETYPE_VALUE, 19);
		break;

	case RA_MOVETO:
	case RA_COPYTO:
		{
			DWORD dwSize = sizeof(DWORD) + sizeof(DWORD) + sizeof(UINT_PTR);
			if (COETricks::m_bVista)
				dwSize = sizeof(DWORD) + sizeof(DWORD) + sizeof(__int64);

			boost::scoped_array<BYTE> arr(new BYTE[dwSize]);
			memset(arr.get(), 0, dwSize);
			*(DWORD *)arr.get() = COETricks::m_bVista ? m_dwInternal1Vista : m_dwInternal1;
			*(DWORD *)(arr.get()+sizeof(DWORD)) = COETricks::m_bVista ? m_dwInternal2Vista : m_dwInternal2;
			if (COETricks::m_bVista)
				*(__int64 *)(arr.get() + sizeof(DWORD) + sizeof(DWORD)) = action.m_dwFolderID;
			else
				*(UINT_PTR *)(arr.get() + sizeof(DWORD) + sizeof(DWORD)) = (UINT_PTR)action.m_dwFolderID;

			RegSetValueEx(subkey, RULE_VALUE_VALUE, NULL, REG_BINARY, 
				reinterpret_cast<LPCBYTE>(arr.get()), dwSize);
		}

		subkey.SetDWORDValue(RULE_VALUETYPE_VALUE, 65);
		break;
	}

	return szKeyName;
}

bool COERule::SaveRule() const
{
	USES_CONVERSION;
	tstring msg(_T("Saving rule '"));
	msg += m_strRuleName;
	msg += _T("' (actions: ");
	msg += A2CT((boost::lexical_cast<std::string>(m_arrActions.size())).c_str());
	msg += _T(")");
	ASTRACE(msg.c_str());

	if (m_strRuleName.size() <= 0)
		return false;

	//if (!m_bInternalsInited)
	m_bInternalsInited = _ReadInternalValues();

	tstring strOEKey = GetMainOEKey();
	if (strOEKey.size() <= 0)
		return false;
	
	tstring strKeyName;

	// 0. Modify our Rule key
	strKeyName = strOEKey + _T("\\Rules\\Mail\\");
	strKeyName += m_strRuleName;
	CRegKey keyRule;
	if (keyRule.Create(HKEY_CURRENT_USER, strKeyName.c_str(), REG_NONE, REG_OPTION_NON_VOLATILE, 
		KEY_READ | KEY_WRITE) != ERROR_SUCCESS)
	{
		ASTRACE(_T("Error creating rule key"));
		return false;
	}

	keyRule.SetDWORDValue(RULE_ENABLED_VALUE, m_bEnabled ? 1 : 0);
	keyRule.SetDWORDValue(RULE_VERSION_VALUE, 1);
	keyRule.SetStringValue(RULE_NAME_VALUE, m_strDisplayName.c_str());

	// 1. Create/Open Actions key
	strKeyName = strOEKey + _T("\\Rules\\Mail\\");
	strKeyName += m_strRuleName + _T("\\Actions");
	CRegKey keyActions;
	if (keyActions.Create(HKEY_CURRENT_USER, strKeyName.c_str(), REG_NONE, REG_OPTION_NON_VOLATILE, 
		KEY_READ | KEY_WRITE) != ERROR_SUCCESS)
	{
		ASTRACE(_T("Error creating actions key"));
		return false;
	}

	// 2. Delete all subkeys in Actions key
	Utility::DeleteRegSubkeys(keyActions);

	// 3. Add subkeys one by one (if there are any)
	CStringCollector col(" ");
	for (ACTIONS_ARRAY::size_type n = 0; n < m_arrActions.size(); ++n)
	{
		tstring strActionKey = _SaveAction(keyActions, (DWORD)n, m_arrActions[n]);
		if (strActionKey.size() > 0)
			col.AddItem(strActionKey.c_str());
	}

	// 4. Modify the Actions order
	keyActions.SetStringValue(ORDER_VALUE, col.GetResult().c_str());

	// 5. Open Criteria key
	strKeyName = strOEKey + _T("\\Rules\\Mail\\");
	strKeyName += m_strRuleName + _T("\\Criteria");
	CRegKey keyCriteria;
	if (keyCriteria.Create(HKEY_CURRENT_USER, strKeyName.c_str(), REG_NONE, REG_OPTION_NON_VOLATILE, 
		KEY_READ | KEY_WRITE) == ERROR_SUCCESS)
	{
		// 6. Delete all subkeys in Criteria key
		Utility::DeleteRegSubkeys(keyCriteria);

		// 7. Add '000'-subkey (we support only one criteria)
		CRegKey subkey;
		if (subkey.Create(keyCriteria, _T("000"), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE) == ERROR_SUCCESS)
		{
			subkey.SetDWORDValue(RULE_FLAGS_VALUE, 0);
			subkey.SetDWORDValue(RULE_LOGIC_VALUE, 0);
			subkey.SetDWORDValue(RULE_TYPE_VALUE, 8);
			subkey.SetDWORDValue(RULE_VALUETYPE_VALUE, 65);
			
			DWORD dwSize = (DWORD)(m_Criteria.m_strSubjString.size() + 3);

			if (COETricks::m_bVista)
				dwSize *= 2;

			boost::scoped_array<BYTE> arr(new BYTE[dwSize]);
			memset(arr.get(), 0, dwSize);

			if (COETricks::m_bVista)
				::MultiByteToWideChar(CP_ACP, 0, m_Criteria.m_strSubjString.c_str(), (int)m_Criteria.m_strSubjString.size(), (LPWSTR)arr.get(), dwSize/2);
			else
				memcpy(arr.get(), m_Criteria.m_strSubjString.c_str(), m_Criteria.m_strSubjString.size());

			RegSetValueEx(subkey, RULE_VALUE_VALUE, NULL, REG_BINARY, 
				arr.get(), dwSize);
		}
		else
			ASTRACE(_T("Error creating criteria subkey"));

		// 8. Modify the Criteria order
		keyCriteria.SetStringValue(ORDER_VALUE, _T("000"));
	}
	else
		ASTRACE(_T("Error creating criteria key"));
	
	// 9. Modify 'Order' value for the keyRules (take m_arrActions into account)
	if (m_arrActions.size() > 0)
	{
		strKeyName = strOEKey + _T("\\Rules\\Mail");
		CRegKey keyRules;
		if (keyRules.Open(HKEY_CURRENT_USER, strKeyName.c_str(), KEY_READ | KEY_WRITE) != ERROR_SUCCESS)
			return false;

		DWORD dwBufSize = 0;
		LONG query_result;
		if((query_result = keyRules.QueryStringValue(ORDER_VALUE, NULL, &dwBufSize)) == ERROR_SUCCESS)
		{
			TCHAR *szBuf = new TCHAR[dwBufSize];
			if(szBuf)
			{
				if (keyRules.QueryStringValue(ORDER_VALUE, szBuf, &dwBufSize) == ERROR_SUCCESS)
				{
					bool bOurRuleIsThere = false;
					tstring str;
					CStringTokenizer tok(szBuf, _T(" "));
					while (tok.GetNext(str))
					{
						if (str == m_strRuleName)
						{
							bOurRuleIsThere = true;
							break;
						}
					}

					if (!bOurRuleIsThere)
					{
						tstring strOrder(szBuf);

						if (strOrder.size() > 0)
							strOrder += _T(" ");

						strOrder += m_strRuleName;
						keyRules.SetStringValue(ORDER_VALUE, strOrder.c_str());
					}			
				}
			}
			delete []szBuf;
		}
		else if(query_result == ERROR_FILE_NOT_FOUND)
		{
			keyRules.SetStringValue(ORDER_VALUE, m_strRuleName.c_str());
		}
	}

	return true;
}

tstring COERule::GetMainOEKey()
{
	if (COETricks::m_bVista)
		return OE_MAIN_KEY_VISTA;

	tstring str(MAIN_IDENTITIES_KEY);
	str += _T("\\");
	str += GetCurrentIdentityName();
	str += _T("\\");	
	str += OE_MAIN_KEY;

	return str;
}

bool COERule::Initialize()
{
	USES_CONVERSION;
	tstring msg(_T("Initialized rule '"));
	msg += m_strRuleName + _T("' ('");
	msg += m_strDisplayName + _T("'), dwInt1 = ");
	msg += A2CT((boost::lexical_cast<std::string>(m_dwInternal1)).c_str());
	msg += _T(", dwInt2 = ");
	msg += A2CT((boost::lexical_cast<std::string>(m_dwInternal2)).c_str());
	ASTRACE(msg.c_str());

	return true;
}

tstring COERule::GetCurrentIdentityName()
{
	// see which Identity has Running property set to 1
	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, MAIN_IDENTITIES_KEY, KEY_READ) != ERROR_SUCCESS)
		return _T("");

	tstring strRet;

	DWORD dwIndex = 0;
	TCHAR szName[MAX_PATH];
	DWORD dwBufSize = MAX_PATH;
	FILETIME ftDummy;

	while (RegEnumKeyEx(key, dwIndex, szName, &dwBufSize, NULL, NULL, NULL, &ftDummy) == ERROR_SUCCESS)
	{
		tstring strTemp;
		if (szName)
			strTemp = szName;

		strTemp += _T("\\");	
		strTemp += OE_MAIN_KEY;

		CRegKey keyIdentity;
		if (keyIdentity.Open(key, strTemp.c_str(), KEY_READ) == ERROR_SUCCESS)
		{
			DWORD dwRunning = 0;
			keyIdentity.QueryDWORDValue(RUNNING_VALUE, dwRunning);
			if (dwRunning)
			{
				strRet = szName;
				break;
			}
		}

		++dwIndex;
		dwBufSize = MAX_PATH;
	}

	if (strRet.size() <= 0)
	{
		// we're probably starting up (not switching identities)
		strRet = Utility::GetRegStringValue(key, START_AS_VALUE);
		if ((strRet.size() <= 0) || (strRet == EMPTY_IDENTITY))
			strRet = Utility::GetRegStringValue(key, LAST_USERID_VALUE);
	}

	return strRet;
}

bool COERule::DeleteRule()
{
	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, MAIN_IDENTITIES_KEY, KEY_READ) != ERROR_SUCCESS)
		return false;

	// iterate through all identities to delete their rules
	DWORD dwIndex = 0;
	TCHAR szName[MAX_PATH];
	DWORD dwBufSize = MAX_PATH;
	FILETIME ftDummy;
	
	while (RegEnumKeyEx(key, dwIndex, szName, &dwBufSize, NULL, NULL, NULL, &ftDummy) == ERROR_SUCCESS)
	{
		_ClearRuleInIdentity(szName);

		++dwIndex;
		dwBufSize = MAX_PATH;
	}

	return true;
}

bool COERule::_ClearRuleInIdentity(LPCTSTR szIdentityName)
{
	if (!szIdentityName)
		return false;

	tstring str(MAIN_IDENTITIES_KEY);
	str += _T("\\");
	str += szIdentityName;
	str += _T("\\");	
	str += OE_MAIN_KEY;
	str += _T("\\Rules\\Mail");

	//////////////////////////////////////////////////////////////////////////
	// modify the Order value
	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, str.c_str(), KEY_READ | KEY_WRITE) != ERROR_SUCCESS)
		return false;

	TCHAR szBuf[MAX_PATH];
	DWORD dwBufSize = MAX_PATH;
	
	if (key.QueryStringValue(ORDER_VALUE, szBuf, &dwBufSize) == ERROR_SUCCESS)
	{
		tstring str;
		CStringCollector col(_T(" "));
		CStringTokenizer tok(szBuf, _T(" "));
		while (tok.GetNext(str))
		{
			if (str != m_strRuleName)
				col.AddItem(str.c_str());
		}
		
		key.SetStringValue(ORDER_VALUE, col.GetResult().c_str());
	}

	//////////////////////////////////////////////////////////////////////////
	// delete the rule subkey
	str += _T("\\");
	str += m_strRuleName;
	
	CRegKey keyRule;
	if (keyRule.Open(HKEY_CURRENT_USER, str.c_str(), KEY_READ | KEY_WRITE) != ERROR_SUCCESS)
		return false;

	bool bRet = Utility::DeleteRegSubkeys(keyRule);
	RegDeleteKey(HKEY_CURRENT_USER, str.c_str());

	return bRet;
}
