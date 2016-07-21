// CKAHManifest.cpp: implementation of the CCKAHManifest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CKAHManifest.h"
#include <algorithm>
#include "malloc.h"
#include "UnicodeConv.h"
#include "Unpacker.h"
#include "Utils.h"

const WCHAR szwBaseDrvSectionName[] = L"basedrv";
const WCHAR szwPluginSectionName[] = L"plugins";
const WCHAR szwPluginDataSectionName[] = L"plugindata";

const WCHAR szwIDSPluginBaseNameDataSectionName[] = L"base";

#define KLAVA_B2C(c) (((c) == '\0')?('_'):(c))
#define KLAVA_C2B(c) (((c) == '_')?('\0'):(c))

const WCHAR szwIDSCurrBasePrefix[] = L"cur";

//////////////////////////////////////////////////////////////////////
// CCKAHBaseDrv
//////////////////////////////////////////////////////////////////////

CCKAHBaseComp::CCKAHBaseComp ()
{	
}

CCKAHBaseComp::CCKAHBaseComp (const std::wstring &sName) :
	m_sName (sName)
{
}

bool CCKAHBaseComp::operator == (const CCKAHBaseComp &basecmp) const
{
	return m_sName == basecmp.m_sName;
}

void CCKAHBaseComp::Clear ()
{
	m_sName = L"";
}

//////////////////////////////////////////////////////////////////////
// CCKAHBaseDrv
//////////////////////////////////////////////////////////////////////

CCKAHBaseDrv::CCKAHBaseDrv () :
	m_dwVersionMS (0),
	m_dwVersionLS (0)
{
}

CCKAHBaseDrv::CCKAHBaseDrv (const std::wstring &sName, DWORD dwVersionMS, DWORD dwVersionLS) :
	CCKAHBaseComp (sName),
	m_dwVersionMS (dwVersionMS),
	m_dwVersionLS (dwVersionLS)
{
}

bool CCKAHBaseDrv::ReadFromString (LPCWSTR szString)
{
	bool bRetValue = false;

	Clear ();

	try
	{
		WCHAR *token;
		int toknum = 0;

		token = wcstok ((LPWSTR)szString, L",");

		while (token)
		{
			switch (toknum++)
			{
			case 0:
				m_sName = token;
				break;
			case 1:
				{
					DWORD dwMSHi, dwMSLo, dwLSHi, dwLSLo;
					if (swscanf (token, L"%d.%d.%d.%d", &dwMSHi, &dwMSLo, &dwLSHi, &dwLSLo) == 4)
					{
						m_dwVersionMS = MAKELONG (dwMSLo & 0xFFFF, dwMSHi & 0xFFFF);
						m_dwVersionLS = MAKELONG (dwLSLo & 0xFFFF, dwLSHi & 0xFFFF);

						bRetValue = !m_sName.empty (); // имя и версия обязательны
					}
				}
				break;
			}
			
			token = wcstok (NULL, L",");
		}
	}
	catch (...)
	{
		bRetValue = false;
	}

	return bRetValue;
}

int CCKAHBaseDrv::WriteToString (LPWSTR szString) const
{
	*szString = 0;
	
	swprintf (szString, L"%s,%d.%d.%d.%d", m_sName.c_str (), 
						HIWORD(m_dwVersionMS), LOWORD (m_dwVersionMS), 
						HIWORD(m_dwVersionLS), LOWORD (m_dwVersionLS));

	return wcslen (szString);
}

void CCKAHBaseDrv::Clear ()
{
	m_dwVersionMS = 0;
	m_dwVersionLS = 0;
}

bool CCKAHBaseDrv::operator == (const CCKAHBaseDrv &basedrv) const
{
	return CCKAHBaseComp::operator == (basedrv) &&
			m_dwVersionMS == basedrv.m_dwVersionMS &&
			m_dwVersionLS == basedrv.m_dwVersionLS;
}

//////////////////////////////////////////////////////////////////////
// CCKAHPlugin
//////////////////////////////////////////////////////////////////////

CCKAHPlugin::CCKAHPlugin () :
	m_IfaceType (ptUnknown),
	m_LoadGroupType (ptUnknown),
	m_bResumed (false),
	m_dwVersionMS (0),
	m_dwVersionLS (0)
{
}

CCKAHPlugin::~CCKAHPlugin ()
{
}

bool CCKAHPlugin::ReadFromString (LPCWSTR szString)
{
	bool bRetValue = false;

	Clear ();

	try
	{
		WCHAR *token;
		int toknum = 0;

		token = wcstok ((LPWSTR)szString, L",");

		while (token)
		{
			switch (toknum++)
			{
			case 0:
				m_sName = token;
				break;
			case 1:
				m_IfaceType = (PluginType)_wtoi (token);
				if (m_IfaceType < ptUnknown || m_IfaceType >= ptLast)
					m_IfaceType = ptUnknown;
				bRetValue = !m_sName.empty ();
				break;
			case 2:
				// необязательный парамтр
				m_LoadGroupType = (PluginType)_wtoi (token);
				if (m_LoadGroupType < ptUnknown || m_LoadGroupType >= ptLast)
					m_LoadGroupType = ptUnknown;
				break;
			case 3:
				{
					DWORD dwMSHi, dwMSLo, dwLSHi, dwLSLo;
					if (swscanf (token, L"%d.%d.%d.%d", &dwMSHi, &dwMSLo, &dwLSHi, &dwLSLo) == 4)
					{
						m_dwVersionMS = MAKELONG (dwMSLo & 0xFFFF, dwMSHi & 0xFFFF);
						m_dwVersionLS = MAKELONG (dwLSLo & 0xFFFF, dwLSHi & 0xFFFF);
					}
				}
				break;
			}
			
			token = wcstok (NULL, L",");
		}
	}
	catch (...)
	{
		bRetValue = false;
	}

	switch (m_IfaceType)
	{
	case ptIDS:
		m_LoadGroupType = ptIDS;
		break;
	case ptStealthMode:
		m_LoadGroupType = ptStealthMode;
		break;
	case ptFirewall:
		m_LoadGroupType = ptFirewall;
		break;
	// про остальные ничего определенного сказать нельзя
	}

	return bRetValue;
}

int CCKAHPlugin::WriteToString (LPWSTR szString) const
{
	*szString = 0;

	swprintf (szString, L"%s,%d,%d", m_sName.c_str (), m_IfaceType, m_LoadGroupType);

	WCHAR szTmp[128];

	if(m_dwVersionMS != 0 || m_dwVersionLS != 0)
	{
		swprintf (szTmp, L",%d.%d.%d.%d", HIWORD(m_dwVersionMS), LOWORD (m_dwVersionMS), 
											HIWORD(m_dwVersionLS), LOWORD (m_dwVersionLS));
		wcscat (szString, szTmp);
	}
	else
		wcscat (szString, L",");

	return wcslen (szString);
}

void CCKAHPlugin::Clear ()
{
}

//////////////////////////////////////////////////////////////////////
// CCKAHIDSPlugin
//////////////////////////////////////////////////////////////////////

DWORD IOSGetPrivateProfileStringW (
	IN LPCWSTR lpAppName,
    IN LPCWSTR lpKeyName,
    IN LPCWSTR lpDefault,
    OUT LPWSTR lpReturnedString,
    IN DWORD nSize,
    IN LPCWSTR lpFileName
 )
{
#if defined(_UNICODE)
	return ::GetPrivateProfileString (lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
#else
	if ( g_bIsNT ) {
		DWORD dwResult = ::GetPrivateProfileStringW (lpAppName, lpKeyName, lpDefault,
														lpReturnedString, nSize, lpFileName);
		return dwResult;
	}
	else {
		LPSTR szAnsiName = (LPSTR)_alloca (nSize);

		DWORD dwResult = ::GetPrivateProfileStringA (__LPSTR (lpAppName), __LPSTR (lpKeyName), __LPSTR (lpDefault),
														szAnsiName, nSize,
														__LPSTR (lpFileName));
		memset (lpReturnedString, 0, nSize * sizeof (WCHAR));
		wcsncpy (lpReturnedString, __LPWSTR (szAnsiName), nSize );
		return dwResult;
	}
#endif
}

CCKAHIDSPlugin::CCKAHIDSPlugin ()
{
	m_IfaceType = ptIDS;
}

CCKAHIDSPlugin::CCKAHIDSPlugin (const CCKAHPlugin &plugin) :
	CCKAHPlugin (plugin)
{
	m_IfaceType = ptIDS;
}

CCKAHIDSPlugin::~CCKAHIDSPlugin ()
{
}

bool CCKAHIDSPlugin::ReadDataFromFile (LPCWSTR szwManifestFilePath)
{
	if (m_IfaceType != ptIDS || szwManifestFilePath == NULL || wcslen (szwManifestFilePath) == 0 || m_sName.empty ())
		return false;

	int i;
	WCHAR szI[10], szValue[0x1000];

	m_IDSBaseName = L"";
	m_Data.clear ();

	std::wstring sName = RemoveExtension (m_sName);

	IOSGetPrivateProfileStringW (sName.c_str (), szwIDSPluginBaseNameDataSectionName, L"", szValue, 0xFFF, szwManifestFilePath);
	if (szValue[0])
		m_IDSBaseName = szValue;

	for (i = 0; ; ++i)
	{
		IOSGetPrivateProfileStringW (sName.c_str (), _itow (i, szI, 10), L"", szValue, 0xFFF, szwManifestFilePath);
		if (szValue[0] == 0) 
			break;

		CCKAHPluginDataItem item;

		if (item.ReadFromString (szValue))
			m_Data.push_back (item);
	}

	return true;
}

bool CCKAHIDSPlugin::ReadDataFromStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName)
{
	if (m_IfaceType != ptIDS || lpStorage == NULL ||
			szwRootName == NULL || wcslen (szwRootName) == 0 || m_sName.empty ())
	{
		return false;
	}

	std::wstring sDataRoot = std::wstring (szwRootName) + L"\\" + RemoveExtension (m_sName);

	WCHAR szI[10];
	WCHAR szValue[0x1000];
	int i;

	m_IDSBaseName = L"";
	m_Data.clear ();

	if (lpStorage->GetParam ((sDataRoot + L'\\' + szwIDSPluginBaseNameDataSectionName).c_str (), sizeof (szValue), szValue))
		m_IDSBaseName = szValue;		

	for (i = 0; ; ++i)
	{
		if (lpStorage->GetParam ((sDataRoot + L'\\' + _itow (i, szI, 10)).c_str (), sizeof (szValue), szValue) == 0)
			break;
	
		CCKAHPluginDataItem item;

		if (item.ReadFromString (szValue))
			m_Data.push_back (item);
	}

	return true;
}

bool CCKAHIDSPlugin::WriteDataToStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName)
{
	if (lpStorage == NULL || szwRootName == NULL || wcslen (szwRootName) == 0)
		return false;

	std::wstring szwParamPath = std::wstring (szwRootName) + L'\\' + RemoveExtension (m_sName);
	lpStorage->ClearParams (szwParamPath.c_str ());

	lpStorage->SetParam ((szwParamPath + L'\\' + szwIDSPluginBaseNameDataSectionName).c_str (), m_IDSBaseName.c_str ());

	if (m_Data.empty ())
		return true;

	unsigned i;
	WCHAR szI[10], szValue[0x400]; // 1024 is maximum buffer size for wsprintf

	for (i = 0; i < m_Data.size (); ++i)
	{
		if (m_Data[i].WriteToString (szValue) > 0)
			lpStorage->SetParam ((szwParamPath + L'\\' + _itow (i, szI, 10)).c_str (), szValue);
	}

	return true;
}

bool CCKAHIDSPlugin::CopyData (LPCWSTR szwDirectory)
{
	if (m_Data.empty ())
		return true;

	log.WriteFormat (_T("[UNPACKPLUGINDATA] Copying '%ls' plugin data..."), PEL_INFO, m_sName.c_str ());

	if (m_IDSBaseName.empty ())
	{
		log.WriteFormat (_T("[UNPACKPLUGINDATA] No base provided"), PEL_ERROR);
		return false;
	}

    std::wstring sSrcFile = szwDirectory + m_IDSBaseName;
    std::wstring sDstFile = szwDirectory + std::wstring(szwIDSCurrBasePrefix) + m_IDSBaseName;

    LPCWSTR szwSrcFile = sSrcFile.c_str();
    LPCWSTR szwDstFile = sDstFile.c_str();

    IOSFILEINFO si, di;

    memset(&si, 0, sizeof(si));
    memset(&di, 0, sizeof(di));

    if ( IOSGetFileInfoW(szwSrcFile,&si) == IOS_ERR_OK && 
         IOSGetFileInfoW(szwDstFile,&di) == IOS_ERR_OK )
    {
        if (memcmp(&si, &di, sizeof(IOSFILEINFO)) == 0) 
        {
		    log.WriteFormat (_T("[UNPACKPLUGINDATA] Plugin '%ls' data not changed"), PEL_INFO, m_sName.c_str ());
            return true;
        }
    }

    bool bCopyResult = IOSCopyFileW(szwSrcFile, szwDstFile, FALSE) == IOS_ERR_OK;

	if(bCopyResult)
		log.WriteFormat (_T("[UNPACKPLUGINDATA] Plugin '%ls' data successfully copied"), PEL_INFO, m_sName.c_str ());
	else
		log.WriteFormat (_T("[UNPACKPLUGINDATA] Failed to copy plugin '%ls' data"), PEL_ERROR, m_sName.c_str ());

	return bCopyResult;
}

bool CCKAHIDSPlugin::LoadData (LPCWSTR szwDirectory)
{
	if (m_Data.empty ())
		return true;

	log.WriteFormat (_T("[LOADPLUGINDATA] Loading '%ls' plugin data..."), PEL_INFO, m_sName.c_str ());

	if (m_IDSBaseName.empty ())
	{
		log.WriteFormat (_T("[LOADPLUGINDATA] No base provided"), PEL_ERROR);
		return false;
	}

    std::wstring sIDSBasePath = std::wstring (szwDirectory) + std::wstring (szwIDSCurrBasePrefix) + m_IDSBaseName;

	std::wstring sName = RemoveExtension (m_sName);

	HANDLE hPlugin = NULL;

	TCHAR szName [MAX_PATH + 5];
	wsprintf (szName, _T("\\\\.\\%s"), __LPCTSTR (sName.c_str ()));

	if (g_bIsNT)
		hPlugin = CreateFile (szName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		hPlugin = CreateFile (szName, 0, 0, NULL, 0, 0, NULL);

	if (hPlugin == INVALID_HANDLE_VALUE)
	{
		log.WriteFormat (_T("[LOADPLUGINDATA] Unable to get '%ls' plugin handle (0x%08x)"), PEL_ERROR, sName.c_str (), GetLastError ());
		return false;
	}

	std::vector<BYTE> vec_buf (sizeof (PLUGIN_MESSAGE) + sizeof (IDSKERNELDB), 0);

	{
		PLUGIN_MESSAGE *pPluginMessage = (PLUGIN_MESSAGE *)&vec_buf[0];

		strcpy (pPluginMessage->PluginName, IDS_KERNEL_PLUGINNAME);
		pPluginMessage->MessageID = IDS_MSGID_LOAD;
		pPluginMessage->MessageSize = sizeof (IDSKERNELDB);
	}

	for (unsigned i = 0; i < m_Data.size (); ++i)
	{
		log.WriteFormat (_T("[LOADPLUGINDATA] Loading data file '%ls' with section name '%ls' into '%ls' plugin"), PEL_INFO,
								m_Data[i].m_sName.c_str (), m_Data[i].m_SectionName.c_str (), sName.c_str ());

		std::wstring sDataFileName = std::wstring (szwDirectory) + m_Data[i].m_sName;

        const char * buffer;
        int size;
        if (UnpackIsFile(sIDSBasePath.c_str(), m_Data[i].m_sName.c_str()))
        {
            if (UnpackFileToMemory(sIDSBasePath.c_str(), m_Data[i].m_sName.c_str(), buffer, size))
            {
			    std::vector<BYTE> vec_fragment (sizeof (IDSKERNELDBFRAGMENT) + size, 0);

                memcpy(&vec_fragment[sizeof (IDSKERNELDBFRAGMENT)], buffer, size);

			    IDSKERNELDBFRAGMENT *pFragment = (IDSKERNELDBFRAGMENT *)&vec_fragment[0];
			    strncpy (pFragment->SectionName, __LPSTR(m_Data[i].m_SectionName), sizeof (pFragment->SectionName));
			    pFragment->SectionName[sizeof (pFragment->SectionName) - 1] = '\0';
			    pFragment->FragmentSize = size;

			    vec_buf.insert (vec_buf.end (), vec_fragment.begin (), vec_fragment.end ());

			    IDSKERNELDB *pIDSKernelDB = (IDSKERNELDB *)&vec_buf[sizeof (PLUGIN_MESSAGE)];

			    pIDSKernelDB->DBSize += vec_fragment.size ();
			    pIDSKernelDB->FragmentCount++;

			    PLUGIN_MESSAGE *pPluginMessage = (PLUGIN_MESSAGE *)&vec_buf[0];
			    pPluginMessage->MessageSize += vec_fragment.size ();

			    log.WriteFormat (_T("[LOADPLUGINDATA]   Data file '%ls' added"), PEL_INFO, sDataFileName.c_str ());

                UnpackFreeMemory(buffer);
		    }
		    else
		    {
			    log.WriteFormat (_T("[LOADPLUGINDATA] Failed to open data file '%ls' (0x%08x)"), PEL_INFO, sDataFileName.c_str (), GetLastError ());
		    }
        }
	}

	IDSKERNELDB *pIDSKernelDB = (IDSKERNELDB *)&vec_buf[sizeof (PLUGIN_MESSAGE)];
	if (pIDSKernelDB->FragmentCount > 0)
	{
		IDSKERNELLOADRET LoadRet;
		ZeroMemory (&LoadRet, sizeof (LoadRet));
		DWORD dwBytesReturned = 0;
		if (!DeviceIoControl (hPlugin, PLUG_IOCTL, &vec_buf[0], vec_buf.size (), &LoadRet, sizeof (LoadRet), &dwBytesReturned, NULL))
		{
			log.WriteFormat (_T("[LOADPLUGINDATA] DeviceIoControl failed (0x%08x)"), PEL_ERROR, GetLastError ());
		}
		else
		{
			if (dwBytesReturned != sizeof (LoadRet) || !LoadRet.Status)
				log.WriteFormat (_T("[LOADPLUGINDATA] Failed to load data files into '%ls' plugin"), PEL_ERROR, sName.c_str ());
			else
				log.WriteFormat (_T("[LOADPLUGINDATA] Data files into '%ls' plugin successfully loaded"), PEL_INFO, sName.c_str ());
		}
	}

	CloseHandle (hPlugin);

	log.WriteFormat (_T("[LOADPLUGINDATA] Loading '%ls' plugin data done"), PEL_INFO, sName.c_str ());

	return true;
}

bool CCKAHIDSPlugin::RemoveData (LPCWSTR szwDirectory)
{
	if (m_Data.empty ())
		return true;

    RemoveAttribsDeleteFileW ((std::wstring (szwDirectory) + std::wstring(szwIDSCurrBasePrefix) + m_IDSBaseName).c_str());

	for (unsigned i = 0; i < m_Data.size (); ++i)
		RemoveAttribsDeleteFileW ((std::wstring (szwDirectory) + m_Data[i].m_sName).c_str ());

	return true;
}

//////////////////////////////////////////////////////////////////////
// CCKAHPluginDataItem
//////////////////////////////////////////////////////////////////////

CCKAHPluginDataItem::CCKAHPluginDataItem ()
{
}

CCKAHPluginDataItem::~CCKAHPluginDataItem ()
{
}

bool CCKAHPluginDataItem::ReadFromString (LPCWSTR szString)
{
	bool bRetValue = false;

	Clear ();

	try
	{
		WCHAR *token;
		int toknum = 0;

		token = wcstok ((LPWSTR)szString, L",");

		while (token)
		{
			switch (toknum++)
			{
			case 0:
				m_sName = token;
				break;
			case 1:
				{
					m_SectionName = token;
					bRetValue = !m_sName.empty ();
				}

				break;
			}
			
			token = wcstok (NULL, L",");
		}
	}
	catch (...)
	{
		bRetValue = false;
	}

	return bRetValue;
}

int CCKAHPluginDataItem::WriteToString (LPWSTR szString) const
{
	*szString = 0;
	
	swprintf (szString, L"%s,%s", m_sName.c_str (), m_SectionName.c_str ());

	return wcslen (szString);
}

void CCKAHPluginDataItem::Clear ()
{
	m_SectionName = L"";
}

//////////////////////////////////////////////////////////////////////
// CCKAHManifest
//////////////////////////////////////////////////////////////////////

CCKAHManifest::CCKAHManifest () :
	m_Version (MANIFEST_VERSION)
{

}

CCKAHManifest::~CCKAHManifest ()
{

}

//////////////////////////////////////////////////////////////////////////

template<class _Ty>
inline bool BaseCompNameEq (CEnvelope <_Ty> &a, CEnvelope<_Ty> &b);

template<>
inline bool BaseCompNameEq<CCKAHBaseDrv> (CEnvelope <CCKAHBaseDrv> &a, CEnvelope<CCKAHBaseDrv> &b)
{
	return a->m_sName == b->m_sName;
}

template<>
inline bool BaseCompNameEq<CCKAHPlugin> (CEnvelope <CCKAHPlugin> &a, CEnvelope<CCKAHPlugin> &b)
{
	return a->m_sName == b->m_sName;
}

template<class _Ty>
inline bool BaseCompNameLt (CEnvelope <_Ty> &a, CEnvelope<_Ty> &b);

template<>
inline bool BaseCompNameLt<CCKAHBaseDrv> (CEnvelope <CCKAHBaseDrv> &a, CEnvelope<CCKAHBaseDrv> &b)
{
	return a->m_sName < b->m_sName;
}

template<>
inline bool BaseCompNameLt<CCKAHPlugin> (CEnvelope <CCKAHPlugin> &a, CEnvelope<CCKAHPlugin> &b)
{
	return a->m_sName < b->m_sName;
}

template<class _Ty>
static void UniqueBaseComp (std::vector<CEnvelope<_Ty> > &vector)
{
	std::sort (vector.begin (), vector.end (), BaseCompNameLt<_Ty>);
	vector.erase (std::unique (vector.begin (), vector.end (), BaseCompNameEq<_Ty>), vector.end ());
}

template<class _Ty>
static void EliminateIdenticalBaseComp (std::vector<CEnvelope<_Ty> > &vector1,
										std::vector<CEnvelope<_Ty> > &vector2,
										std::vector<CEnvelope<_Ty> > &eliminated_items)
{
	UniqueBaseComp<_Ty> (vector1);
	UniqueBaseComp<_Ty> (vector2);

	unsigned i, j;

	eliminated_items.clear ();

	if (vector1.size () > 0 && vector2.size () > 0)
	{
		for (i = 0; i < vector1.size (); ++i)
		{
			for (j = 0; j < vector2.size (); ++j)
			{
				if (*vector1[i] == *vector2[j])
				{
					eliminated_items.push_back (vector1[i]);
					vector1.erase (vector1.begin () + i--);
					vector2.erase (vector2.begin () + j);
					break;
				}
			}
		}
	}
}

bool CCKAHManifest::EliminateIdentical (CCKAHManifest &manifest, 
											std::vector<CEnvelope<CCKAHBaseDrv> > &eliminated_basedrivers,
											std::vector<CEnvelope<CCKAHPlugin> > &eliminated_plugins)
{
	EliminateIdenticalBaseComp<CCKAHBaseDrv> (m_BaseDrvs, manifest.m_BaseDrvs, eliminated_basedrivers);
	EliminateIdenticalBaseComp<CCKAHPlugin> (m_Plugins, manifest.m_Plugins, eliminated_plugins);

	return true;
}

bool CCKAHManifest::EliminateUnused(LPCKAHSTORAGE lpStorage)
{
    unsigned i;
    WCHAR szI[10], szValue[0x1000];
    
    std::wstring sDataRoot = L"\\Required\\";
    std::list<std::wstring> required_drv;

    for (i = 0; ; ++i)
    {
        if (lpStorage->GetParam ((sDataRoot + _itow (i, szI, 10)).c_str (), sizeof (szValue), szValue) == 0)
            break;
        required_drv.push_back( szValue );
    }

    for ( i = 0; i < m_BaseDrvs.size(); i++ )
    {
        bool bRequired = false;

        std::list<std::wstring>::iterator ii;
        
        for ( ii = required_drv.begin(); ii != required_drv.end(); ii++ )
        {
            std::wstring s1 = *ii;
            std::wstring s2 = m_BaseDrvs[i]->m_sName;

            if ( *ii == m_BaseDrvs[i]->m_sName )
                bRequired = true;
        }        

        if ( !bRequired )
        {
            m_BaseDrvs.erase (m_BaseDrvs.begin () + i--);
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////

template<class _Ty>
static bool ReadBaseCompListFromFile (std::vector<CEnvelope<_Ty> > &vector,
									  LPCWSTR szwManifestFilePath,
									  LPCWSTR szwSectionName)
{
	WCHAR szI[10], szValue[0x1000];
	int i;

	vector.clear ();

	for (i = 0; ; ++i)
	{
		IOSGetPrivateProfileStringW (szwSectionName, _itow (i, szI, 10), L"", szValue, 0xFFF, szwManifestFilePath);
		if (szValue[0] == 0) 
			break;
		
		_Ty *component = new _Ty;

		if (component && component->ReadFromString (szValue))
			vector.push_back (CEnvelope <_Ty> (component));
	}

	UniqueBaseComp<_Ty> (vector);

	return true;
}

static CCKAHPlugin *AddPluginToVecFromString (LPCWSTR szwString, std::vector<CEnvelope<CCKAHPlugin> > &Plugins)
{
	CCKAHPlugin plugin, *pPlugin = NULL;

	if (plugin.ReadFromString (szwString))
	{
		switch (plugin.GetIfaceType ())
		{
		case CCKAHPlugin::ptIDS:
			pPlugin = new CCKAHIDSPlugin (plugin);
			break;
		default:
			pPlugin = new CCKAHPlugin (plugin);
		}

		if (pPlugin)
			Plugins.push_back (CEnvelope<CCKAHPlugin> (pPlugin));
	}

	return pPlugin;
}

static bool ReadPluginFromFile (std::vector<CEnvelope<CCKAHPlugin> > &Plugins,
									  LPCWSTR szwManifestFilePath,
									  LPCWSTR szwSectionName)
{
	WCHAR szI[10], szValue[0x1000];
	int i;

	Plugins.clear ();

	for (i = 0; ; ++i)
	{
		IOSGetPrivateProfileStringW (szwSectionName, _itow (i, szI, 10), L"", szValue, 0xFFF, szwManifestFilePath);
		if (szValue[0] == 0) 
			break;

		CCKAHPlugin *pPlugin = AddPluginToVecFromString (szValue, Plugins);

		if (pPlugin)
			pPlugin->ReadDataFromFile (szwManifestFilePath);
	}

	UniqueBaseComp<CCKAHPlugin> (Plugins);

	return true;
}

bool CCKAHManifest::ReadFromFile (LPCWSTR szwManifestFilePath)
{
	if (szwManifestFilePath == NULL)
		return false;

	m_Directory = L"";

	LPCWSTR pLastSlash = wcsrchr (szwManifestFilePath, L'\\');
	if (pLastSlash)
		m_Directory = std::wstring (szwManifestFilePath, pLastSlash);

    if (!m_Directory.empty () && *m_Directory.rbegin() != L'\\')
		m_Directory += L'\\';

	//TODO считать и проверить версию manifest

	bool bBaseDrvs = ReadBaseCompListFromFile<CCKAHBaseDrv> (m_BaseDrvs, szwManifestFilePath, szwBaseDrvSectionName);
	bool bPlugins = ReadPluginFromFile (m_Plugins, szwManifestFilePath, szwPluginSectionName);

	return bBaseDrvs && bPlugins;
}

//////////////////////////////////////////////////////////////////////////

template<class _Ty>
static bool ReadBaseCompListFromStorage (std::vector<CEnvelope <_Ty> > &vector,
											LPCKAHSTORAGE lpStorage,
											const std::wstring &szwParamPath)
{
	vector.clear ();

	WCHAR szI[10];
	WCHAR szValue[0x1000];
	int i;

	for (i = 0; ; ++i)
	{
		if (lpStorage->GetParam ((szwParamPath + L'\\' + _itow (i, szI, 10)).c_str (), sizeof (szValue), szValue) == 0)
			break;

		_Ty *component = new _Ty;

		if (component && component->ReadFromString (szValue))
			vector.push_back (CEnvelope <_Ty> (component));
	}

	UniqueBaseComp<_Ty> (vector);

	return true;
}

static bool ReadPluginFromStorage (std::vector <CEnvelope<CCKAHPlugin> > &Plugins,
									LPCKAHSTORAGE lpStorage,
									LPCWSTR szwRootName)
{
	Plugins.clear ();

	WCHAR szI[10];
	WCHAR szValue[0x1000];
	int i;

	std::wstring szwParamPath = std::wstring (szwRootName) + L'\\' + szwPluginSectionName;

	for (i = 0; ; ++i)
	{
		if (lpStorage->GetParam ((szwParamPath + L'\\' + _itow (i, szI, 10)).c_str (), sizeof (szValue), szValue) == 0)
			break;

		CCKAHPlugin *pPlugin = AddPluginToVecFromString (szValue, Plugins);

		if (pPlugin)
			pPlugin->ReadDataFromStorage (lpStorage, szwRootName);
	}

	UniqueBaseComp<CCKAHPlugin> (Plugins);

	return true;
}

bool CCKAHManifest::ReadFromStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName)
{
	if (lpStorage == NULL || szwRootName == NULL || wcslen (szwRootName) == 0)
		return false;

	m_Directory = L"";

	std::wstring sDirParamName = szwRootName;
	sDirParamName += L"\\Directory";

	DWORD dwDirParamSize = lpStorage->GetParam (sDirParamName.c_str (), 0, NULL);
	
	if (dwDirParamSize)
	{
		LPWSTR szwDirParamValue = (LPWSTR)_alloca (dwDirParamSize);
		if (lpStorage->GetParam (sDirParamName.c_str (), dwDirParamSize, szwDirParamValue))
		{
			m_Directory = szwDirParamValue;
		}
	}

	if (!m_Directory.empty () && *m_Directory.rbegin() != L'\\')
		m_Directory += L'\\';

	//TODO считать и проверить версию manifest

	bool bBaseDrvs = ReadBaseCompListFromStorage<CCKAHBaseDrv> (m_BaseDrvs, lpStorage, std::wstring (szwRootName) + L'\\' + szwBaseDrvSectionName);
	bool bPlugins = ReadPluginFromStorage (m_Plugins, lpStorage, szwRootName);

	return bBaseDrvs && bPlugins;
}

//////////////////////////////////////////////////////////////////////////

template<class _Ty>
static bool WriteBaseCompListToStorage (std::vector<CEnvelope<_Ty> > &vector,
											LPCKAHSTORAGE lpStorage,
											const std::wstring &szwParamPath)
{
	lpStorage->ClearParams (szwParamPath.c_str ());

	if (vector.empty ())
		return true;

	unsigned i;
	WCHAR szI[10], szValue[0x400]; // 1024 is maximum buffer size for wsprintf

	for (i = 0; i < vector.size (); ++i)
	{
		if (vector[i]->WriteToString (szValue) > 0)
			lpStorage->SetParam ((szwParamPath + L'\\' + _itow (i, szI, 10)).c_str (), szValue);
	}

	return true;
}

static bool WritePluginToStorage (std::vector<CEnvelope<CCKAHPlugin> > &Plugins,
											LPCKAHSTORAGE lpStorage,
											LPCWSTR szwRootName)
{
	std::wstring szwParamPath = std::wstring (szwRootName) + L'\\' + szwPluginSectionName;
	lpStorage->ClearParams (szwParamPath.c_str ());

	if (Plugins.empty ())
		return true;

	unsigned i;
	WCHAR szI[10], szValue[0x400]; // 1024 is maximum buffer size for wsprintf

	for (i = 0; i < Plugins.size (); ++i)
	{
		if (Plugins[i]->WriteToString (szValue) > 0)
		{
			lpStorage->SetParam ((szwParamPath + L'\\' + _itow (i, szI, 10)).c_str (), szValue);
			Plugins[i]->WriteDataToStorage (lpStorage, szwRootName);
		}
	}

	return true;
}

bool CCKAHManifest::WriteToStorage (LPCKAHSTORAGE lpStorage, LPCWSTR szwRootName)
{
	if (lpStorage == NULL || szwRootName == NULL || wcslen (szwRootName) == 0)
		return false;

	if (!m_Directory.empty ())
		lpStorage->SetParam ((std::wstring (szwRootName) + L"\\Directory").c_str (), m_Directory.c_str ());

	WCHAR wszVerStr[32];
	lpStorage->SetParam ((std::wstring (szwRootName) + L"\\Version").c_str (), _itow (m_Version, wszVerStr, 10));

	bool bBaseDrvs = WriteBaseCompListToStorage<CCKAHBaseDrv> (m_BaseDrvs, lpStorage, std::wstring (szwRootName) + L'\\' + szwBaseDrvSectionName);
	bool bPlugins = WritePluginToStorage (m_Plugins, lpStorage, szwRootName);

	return bBaseDrvs && bPlugins;
}
