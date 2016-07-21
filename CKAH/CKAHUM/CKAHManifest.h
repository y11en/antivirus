// CKAHManifest.h: interface for the CCKAHManifest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CKAHMANIFEST_H__C37AAC7C_29A0_4726_BE74_28FF9F0B8C0C__INCLUDED_)
#define AFX_CKAHMANIFEST_H__C37AAC7C_29A0_4726_BE74_28FF9F0B8C0C__INCLUDED_

#include "CKAHStorage.h"

#define USER_MODE
#include "winioctl.h"
#include "../../windows/Hook/klsdk/klstatus.h"
#include "../../windows/Hook/klsdk/PlugAPI.h"
#include "../../windows/Hook/klsdk/BaseAPI.h"
#include "../../windows/Hook/Plugins/Include/ids_kernel_api.h"
#include <stuff/cpointer.h>

#define MANIFEST_VERSION 1

const WCHAR szwPersistentManifestRoot[] = L"LastSet";
const WCHAR szwPersistentRebootManifestRoot[] = L"RebootSet";

static bool ClearRegistryKey(HKEY hKey)
{
	static TCHAR szValue[0x1000];
	DWORD dwSize;
	bool bRes = true;

	// If we cannot delete at least one value - try to delete all others and return false.
	for (int i=0; RegEnumValue(hKey, i, szValue, &(dwSize = sizeof (szValue)), 0, 0, 0, 0) == ERROR_SUCCESS; )
	{
		if (RegDeleteValue (hKey, szValue) != ERROR_SUCCESS)
		{
			bRes = false; ++i;
		}
	}
	return bRes;
}

static bool RecurClearRegistryKey(HKEY hKey, LPCTSTR szSubKey)
{
	HKEY hSubKey = NULL;

	if (RegOpenKeyEx (hKey, szSubKey, 0, KEY_READ | KEY_WRITE, &hSubKey) != ERROR_SUCCESS)
		return false;
	
	TCHAR szKey[0x200];
	DWORD dwKeySize;
	FILETIME ft;
	bool bRes = true;

	// If we cannot delete at least one key - try to delete all others and return false.
	for (int i=0; RegEnumKeyEx (hSubKey, i, szKey, &(dwKeySize = sizeof (szKey) / sizeof (szKey[0])), 0, 0, 0, &ft) == ERROR_SUCCESS; )
	{
		if (!RecurClearRegistryKey(hSubKey, szKey))
		{
			bRes = false; ++i;
		}
	}

	bRes &= ClearRegistryKey(hSubKey);
	bRes &= (RegCloseKey(hSubKey) == ERROR_SUCCESS);
	bRes &= (RegDeleteKey(hKey, szSubKey) == ERROR_SUCCESS);
	return bRes;
}

class CCKAHBaseComp
{
public:
	CCKAHBaseComp ();
	CCKAHBaseComp (const std::wstring &sName);

	virtual bool ReadFromString (LPCWSTR szString) = 0;
	virtual int WriteToString (LPWSTR szString) const = 0;
	virtual void Clear () = 0;

	bool operator == (const CCKAHBaseComp &basecmp) const;

	// common members
	std::wstring m_sName;
};

class CCKAHBaseDrv : public CCKAHBaseComp
{
public:
	CCKAHBaseDrv ();
	CCKAHBaseDrv (const std::wstring &sName, DWORD dwVersionMS, DWORD dwVersionLS);

	virtual bool ReadFromString (LPCWSTR szString);
	virtual int WriteToString (LPWSTR szString) const;
	virtual void Clear ();

	bool operator == (const CCKAHBaseDrv &basedrv) const;

	DWORD m_dwVersionMS;
	DWORD m_dwVersionLS;
};

class CCKAHPlugin : public CCKAHBaseComp
{
public:
	enum PluginType
	{
		ptUnknown = 0,
		ptIDS,			// 1 - IDS
		ptStealthMode,	// 2 - Stealth Mode
		ptFirewall,		// 3 - Firewall
        ptRedirector,   // 4 - Redirector
		
		ptLast
	};

	CCKAHPlugin ();
	virtual ~CCKAHPlugin ();

	virtual bool ReadFromString (LPCWSTR szString);
	virtual int WriteToString (LPWSTR szString) const;
	virtual void Clear ();

	virtual bool ReadDataFromFile (LPCWSTR szwManifestFilePath)
	{
		return false;
	}
	virtual bool ReadDataFromStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName)
	{
		return false;
	}
	virtual bool WriteDataToStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName)
	{
		return false;
	}

	virtual bool CopyData (LPCWSTR szwDirectory)
	{
		return true;
	}

	virtual bool LoadData (LPCWSTR szwDirectory)
	{
		return true;
	}

	virtual bool RemoveData (LPCWSTR szwDirectory)
	{
		return true;
	}

	PluginType GetIfaceType () const
	{
		return m_IfaceType;
	}

	PluginType GetGroupType () const
	{
		return m_LoadGroupType;
	}

	bool IsResumed () const
	{
		return m_bResumed;
	}

	bool MarkResumed (bool bResumed)
	{
		bool bPrevValue = m_bResumed;
		m_bResumed = bResumed;
		return bPrevValue;
	}

	DWORD m_dwVersionMS;
	DWORD m_dwVersionLS;

protected:	
	PluginType m_IfaceType;		// тип плагина по интерфейсу
	PluginType m_LoadGroupType;	// тип плагина по группе загрузки


	// несериализуемые члены
	bool m_bResumed;
};

class CCKAHPluginDataItem : public CCKAHBaseComp
{
public:
	CCKAHPluginDataItem ();
	virtual ~CCKAHPluginDataItem ();
		
	virtual bool ReadFromString (LPCWSTR szString);
	virtual int WriteToString (LPWSTR szString) const;
	virtual void Clear ();

	std::wstring m_SectionName;
};

class CCKAHIDSPlugin : public CCKAHPlugin
{
public:
	CCKAHIDSPlugin ();
	CCKAHIDSPlugin (const CCKAHPlugin &plugin);
	virtual ~CCKAHIDSPlugin ();

	virtual bool ReadDataFromFile (LPCWSTR szwManifestFilePath);
	virtual bool ReadDataFromStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName);
	virtual bool WriteDataToStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName);
	virtual bool CopyData (LPCWSTR szwDirectory);
	virtual bool LoadData (LPCWSTR szwDirectory);
	virtual bool RemoveData (LPCWSTR szwDirectory);

protected:
	std::wstring m_IDSBaseName;
	std::vector<CCKAHPluginDataItem> m_Data;
};

class CCKAHManifest
{
public:
	CCKAHManifest ();
	virtual ~CCKAHManifest ();

	bool ReadFromFile (LPCWSTR szwManifestFilePath);

	bool ReadFromStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName);
	bool WriteToStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName);
	
	bool EliminateIdentical (CCKAHManifest &manifest, 
								std::vector<CEnvelope<CCKAHBaseDrv> > &eliminated_basedrivers,
								std::vector<CEnvelope<CCKAHPlugin> > &eliminated_plugins);

    bool EliminateUnused( LPCKAHSTORAGE lpStorage );

	bool IsEmpty ()
	{
		return m_Directory.empty () && m_BaseDrvs.size () == 0 && m_Plugins.size () == 0;
	}

	std::vector <CEnvelope<CCKAHBaseDrv> > m_BaseDrvs;
	std::vector <CEnvelope<CCKAHPlugin> > m_Plugins;

	std::wstring m_Directory;
	ULONG m_Version;
};

#endif // !defined(AFX_CKAHMANIFEST_H__C37AAC7C_29A0_4726_BE74_28FF9F0B8C0C__INCLUDED_)
