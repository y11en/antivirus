#define PR_IMPEX_DEF


#pragma warning ( disable : 4786 )

#include <atlbase.h>
#include <tchar.h>

#include <map>

#include <service/sa.h>

#include <common/releasetry.h>
#include "../Kav/ver_mod.h"

#include "resource.h"

#include "shellext.h"

#define ARRAY_SIZE(arr)         sizeof(arr) / sizeof(arr[0])
#define GetPIDLItem(pida, i)    (LPITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[i+1])
#define IS_ANSI()               (OS.dwPlatformId != VER_PLATFORM_WIN32_NT)

typedef BOOL (WINAPI *tProcessIdToSessionId)(DWORD dwProcessId,DWORD* pSessionId) ;

extern UINT          g_cRefThisDll ;
extern HINSTANCE     g_hmodThisDll ;
extern OSVERSIONINFO OS;
hROOT                g_root;

//--------------------------- CShellExt -----------------------------

CShellExt::CShellExt()
{
    m_CFIdList = ::RegisterClipboardFormat(CFSTR_SHELLIDLIST) ;

    m_hMenuBmp = (HBITMAP)::LoadImage(g_hmodThisDll, 
                                      MAKEINTRESOURCE(IDB_KAP_KAV_BITMAP), 
                                      IMAGE_BITMAP, 
                                      0, 
                                      0, 
                                      LR_LOADMAP3DCOLORS) ;
    
    m_cRef = 0 ;
    m_pDataObj = NULL ;
    
    ++g_cRefThisDll ;
}

CShellExt::~CShellExt()
{
    if (m_hMenuBmp)
    {
        ::DeleteObject(m_hMenuBmp) ;
    }
    
    if (m_pDataObj)
    {
        m_pDataObj->Release() ;
    }

    --g_cRefThisDll ;
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, 
                                       LPVOID FAR * ppv)
{
    *ppv = NULL ;
    
    if (::IsEqualIID(riid, IID_IShellExtInit) || 
        ::IsEqualIID(riid, IID_IUnknown))
    {
        *ppv = (LPSHELLEXTINIT)this ;
    }
    else if (::IsEqualIID(riid, IID_IContextMenu))
    {
        *ppv = (LPCONTEXTMENU)this ;
    }
    else
    {
        return E_NOINTERFACE ;
    }

    AddRef() ;
    return NOERROR ;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    return ++m_cRef ;
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{
    if (m_cRef)
    {
        if (--m_cRef)
        {
            return m_cRef ;
        }

        delete this ;
    }

    return 0 ;
}

STDMETHODIMP CShellExt::Initialize(LPCITEMIDLIST pIDFolder, 
                                   LPDATAOBJECT pDataObj, 
                                   HKEY hRegKey)
{
    if (pDataObj == NULL)
    {
        return E_FAIL ;
    }
    
    if (m_pDataObj)
    {
        m_pDataObj->Release() ;
    }

    m_pDataObj = pDataObj ;
    m_pDataObj->AddRef() ;

    return NOERROR ;
}

STDMETHODIMP CShellExt::QueryContextMenu(HMENU hMenu, 
                                         UINT indexMenu, 
                                         UINT idCmdFirst, 
                                         UINT idCmdLast, 
                                         UINT uFlags) 
{
    if (IsMyComputer())
    {
        // Must return number of menu items we added.
        return ShellInsertMenu(hMenu, indexMenu, idCmdFirst, true) ;
    }

    CreateFileList() ;

    if (m_aFile.size())
    {
        return ShellInsertMenu(hMenu, indexMenu, idCmdFirst, false) ;
    }

    return 0 ;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR idCmd, 
                                         UINT uFlags, 
                                         UINT FAR * reserved, 
                                         LPSTR pszName, 
                                         UINT cchMax)
{
    return NOERROR ;
}

STDMETHODIMP CShellExt::InvokeCommand ( LPCMINVOKECOMMANDINFO lpcmi )
{
    if  (!HIWORD(lpcmi->lpVerb) && LOWORD(lpcmi->lpVerb) ||
		 HIWORD(lpcmi->lpVerb) && _tcslen(lpcmi->lpVerb))
    {
        return E_INVALIDARG ;
    }

    CScanData* pData = new CScanData ;
    if (!pData)
    {
        return E_UNEXPECTED ;
    }

    if (IsMyComputer ())
    {
        pData->m_blMyComputer = true ;
    }
    else
    {
        pData->m_blMyComputer = false ;
        pData->m_aFile = m_aFile ;

//        DelNetworkPathFromList(pData->m_aFile) ;

//        if (pData->m_aFile.size() == 0)
//        {
//            pData->m_eMsgBoxType = eOnlyNetwork ;
//        }
//        else if (pData->m_aFile.size() != m_aFile.size())
//        {
//            pData->m_eMsgBoxType = eNormalAndNetwork ;
//        }
    }

    DWORD dwThreadID = 0 ;
    HANDLE hThread = ::CreateThread(NULL, 0, StartScanner, pData, 0, &dwThreadID) ;
    
    if (hThread)
    {
        CloseHandle(hThread) ;
    }

    return S_OK ;
}

bool CShellExt::IsGrayedMenu()
{
    HSA hSecure = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL) ;
	HANDLE hBlMutex;

	if( OS.dwMajorVersion >= 5 )
    {
		hBlMutex = ::CreateMutex(SA_Get(hSecure), TRUE, "Global\\AVP.Mutex."VER_PRODUCTNAME_STR ".BL") ;
    }
	else
    {
		hBlMutex = ::CreateMutex(SA_Get(hSecure), TRUE, "AVP.Mutex."VER_PRODUCTNAME_STR ".BL") ;
    }

    bool blResult = true ;

	DWORD dwLastError = GetLastError();
    if (hBlMutex)
    {
		if(dwLastError == ERROR_ALREADY_EXISTS)
		{
			blResult = false ;
		}
	    CloseHandle(hBlMutex) ;
    }

    SA_Destroy(hSecure) ;
    return blResult ;
}

bool CShellExt::GetObjectType(const wchar_t* pszPath, enScanObjType& rObjectType)
{
    DWORD dwAttr;
	
	if( IS_ANSI() )
	{
		USES_CONVERSION;
		dwAttr = ::GetFileAttributes(W2T(pszPath));
	}
	else
		dwAttr = ::GetFileAttributesW(pszPath);
    
	if( dwAttr == INVALID_FILE_ATTRIBUTES )
        return false ;

    rObjectType = OBJECT_TYPE_FILE ;
    if( wcslen(pszPath) == 3 && 
        *(pszPath + 1) == L':' && 
        *(pszPath + 2) == L'\\' )
    {
        rObjectType = OBJECT_TYPE_DRIVE;
    }
    else if( dwAttr & FILE_ATTRIBUTE_DIRECTORY )
    {
        rObjectType = OBJECT_TYPE_FOLDER ;
    }

    return true;
}

void CShellExt::CreateFileList()
{
    m_aFile.clear();

    if( !m_pDataObj )
        return;

    STGMEDIUM medium ;
    FORMATETC fmteDrop = { CF_HDROP, 
                           (DVTARGETDEVICE FAR *)NULL, 
                           DVASPECT_CONTENT, 
                           -1, 
                           TYMED_HGLOBAL } ;

    FileList_t aDropFile ;
    if (SUCCEEDED(m_pDataObj->GetData(&fmteDrop, &medium)))
    {
        GetFileListFromHDROP(reinterpret_cast<HDROP>(medium.hGlobal), aDropFile) ;
        ::ReleaseStgMedium(&medium) ;
    }

    FORMATETC fmteIdList = { USHORT(m_CFIdList), 
                             (DVTARGETDEVICE FAR *)NULL, 
                             DVASPECT_CONTENT, 
                             -1, 
                             TYMED_HGLOBAL } ;

    FileList_t aCidaFile ;
    if (SUCCEEDED(m_pDataObj->GetData(&fmteIdList, &medium)))
    {
        GetFileListFromCida(reinterpret_cast<CIDA*>(medium.hGlobal), aCidaFile) ;
        ::ReleaseStgMedium(&medium) ;
    }

    MergeFileLists(aDropFile, aCidaFile, m_aFile) ;
}

void CShellExt::CreateNotNetworkDiskList(FileList_t& rResult)
{
    rResult.clear() ;

    int nStringSize = ::GetLogicalDriveStrings(0, NULL) ;
    if (!nStringSize)
    {
        return ;
    }

    LPTSTR pszDrives = new TCHAR[nStringSize + 2] ;
    ::GetLogicalDriveStrings(nStringSize, pszDrives) ;

    for (LPTSTR pszDrive = pszDrives; *pszDrive;)
    {
        UINT unDriveType = ::GetDriveType(pszDrive) ;
        
        if (unDriveType != DRIVE_REMOTE)
        {
            _tcsupr(pszDrive) ;

            USES_CONVERSION ;
            rResult.push_back(T2W(pszDrive)) ;
        }

        while (*(pszDrive++)) ;
    }

    delete [] pszDrives ;
}

void CShellExt::DelNetworkPathFromList(FileList_t& rList)
{
    FileList_t aNotNetworkDisk ;
    CreateNotNetworkDiskList(aNotNetworkDisk) ;

	size_t unIndex, unICount;
    for (unIndex = 0, unICount = rList.size(); unIndex < unICount; ++unIndex)
    {
		size_t unJndex, unJCount;
        for (unJndex = 0, unJCount = aNotNetworkDisk.size(); unJndex < unJCount; ++unJndex)
        {
            if (_wcsnicmp(rList[unIndex].c_str(), 
                          aNotNetworkDisk[unJndex].c_str(), 
                          aNotNetworkDisk[unJndex].size()) == 0)
            {
                break ;
            }
        }

        if (unJndex != unJCount)
        {
            continue ;
        }

        FileList_t::iterator It = rList.begin() ;
        for (UINT unKndex = 0; unKndex < unIndex; ++unKndex)
        {
            ++It ;
        }

        rList.erase(It) ;

        --unIndex ;
        --unICount ;
    }
}

void CShellExt::MergeFileLists(FileList_t& rList1, 
                               FileList_t& rList2, 
                               FileList_t& rResult)
{
    rResult.clear() ;
    
	size_t unIndex, unICount;
    for (unIndex = 0, unICount = rList1.size(); unIndex < unICount; ++unIndex)
    {
        rResult.push_back(rList1[unIndex]) ;
    }

    for (size_t unJndex = 0, unJCount = rList2.size(); unJndex < unJCount; ++unJndex)
    {
        for (unIndex = 0; unIndex < unICount; ++unIndex)
        {
            if (_wcsicmp(rList1[unIndex].c_str(), rList2[unJndex].c_str()) == 0)
            {
                break ;
            }
        }

        if (unIndex != unICount)
        {
            continue ;
        }

        rResult.push_back(rList2[unJndex]) ;
    }
}

void CShellExt::GetFileListFromHDROP(HDROP hDrop, 
                                     FileList_t& rFileList)
{
	if( !hDrop )
		return;

	UINT unFileCount = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	for(UINT unIndex = 0; unIndex < unFileCount; ++unIndex)
	{
		if( IS_ANSI() )
		{
			TCHAR pszFilePath[3 * _MAX_PATH] = _T("");
			::DragQueryFile(hDrop, unIndex, pszFilePath, ARRAY_SIZE(pszFilePath));
			if( _tcslen(pszFilePath) == 0 )
				continue;
			
			if( ::GetFileAttributes(pszFilePath) == INVALID_FILE_ATTRIBUTES )
				continue;
			
			USES_CONVERSION;
			rFileList.push_back(T2W(pszFilePath));
			continue;
		}

		WCHAR pszFilePath[3 * _MAX_PATH] = L"";
		::DragQueryFileW(hDrop, unIndex, pszFilePath, ARRAY_SIZE(pszFilePath));
		if( wcslen(pszFilePath) == 0 )
			continue;
		
		if( ::GetFileAttributesW(pszFilePath) == INVALID_FILE_ATTRIBUTES )
			continue;
		
		rFileList.push_back(pszFilePath);
	}
}

void CShellExt::GetFileListFromCida(CIDA* pCida, 
                                    FileList_t& rFileList)
{
    if( !pCida )
        return;

    for(UINT unIndex = 0; unIndex < pCida->cidl; ++unIndex)
    {
        std::wstring strFilePath = GetObjectPathFromItemIDList(GetPIDLItem(pCida, unIndex));
        if( !strFilePath.size() )
            continue;

        rFileList.push_back(strFilePath);
    }
}

std::wstring CShellExt::GetObjectPathFromItemIDList(LPITEMIDLIST pidlOriginal)
{
	if( IS_ANSI() )
	{
		TCHAR pszFile[3 * _MAX_PATH] = _T("");
		if( !::SHGetPathFromIDList(pidlOriginal, pszFile) )
			return L"";
		
		TCHAR pszPathPart[3 * MAX_PATH] = _T("");
		TCHAR * pszFilePart = NULL ;
		if( !::GetFullPathName(pszFile, ARRAY_SIZE(pszPathPart), pszPathPart, &pszFilePart) )
			return L"";
		
		if( ::GetFileAttributes(pszPathPart) == INVALID_FILE_ATTRIBUTES )
			return L"";
		
		USES_CONVERSION ;
		return T2W(pszPathPart);
	}

	WCHAR pszFile[3 * _MAX_PATH] = L"";
	if( !::SHGetPathFromIDListW(pidlOriginal, pszFile) )
		return L"";
	
	WCHAR pszPathPart[3 * MAX_PATH] = L"";
	WCHAR * pszFilePart = NULL ;
	if( !::GetFullPathNameW(pszFile, ARRAY_SIZE(pszPathPart), pszPathPart, &pszFilePart) )
		return L"";
	
	if( ::GetFileAttributesW(pszPathPart) == INVALID_FILE_ATTRIBUTES )
		return L"";
	
	return pszPathPart;
}

bool CShellExt::IsMyComputer(LPITEMIDLIST pidlOriginal)
{
    CComPtr<IShellFolder> pShellFolder;
    if( FAILED(::SHGetDesktopFolder(&pShellFolder)) )
        return false;

    CComPtr<IMalloc> pMalloc;
    if( FAILED(::SHGetMalloc(&pMalloc)) )
        return false;

    LPITEMIDLIST pidl = NULL ;
    if( ::SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidl) != NOERROR )
        return false;

    HRESULT hResult = pShellFolder->CompareIDs(0, pidlOriginal, pidl);
    pMalloc->Free(pidl);
    return (short)HRESULT_CODE(hResult) == 0;
}

bool CShellExt::IsMyComputer ()
{
    if( !m_pDataObj )
        return false;

    STGMEDIUM medium ;
    FORMATETC fmteIdList = { USHORT(m_CFIdList), 
                             (DVTARGETDEVICE FAR *)NULL, 
                             DVASPECT_CONTENT, 
                             -1, 
                             TYMED_HGLOBAL } ;

    FileList_t aCidaFile ;
    if (FAILED(m_pDataObj->GetData(&fmteIdList, &medium)))
    {
        return false ;
    }

    CIDA * pCida = reinterpret_cast<CIDA*>(medium.hGlobal) ;
    for (UINT unIndex = 0; unIndex < pCida->cidl; ++unIndex)
    {
        if (IsMyComputer(GetPIDLItem(pCida, unIndex)))
        {
            ::ReleaseStgMedium (&medium) ;
            return true ;
        }
    }

    ::ReleaseStgMedium (&medium) ;
    return false ;
}

static std::wstring LoadString(LPCSTR strKey, LPCSTR strDefault)
{
	static std::string g_strLocFile;
	static UINT g_nACP = CP_ACP;

	if( g_strLocFile.empty() )
	{
		CHAR strLocName[MAX_PATH] = "";
		CHAR strSkinPath[MAX_PATH];
		DWORD size = sizeof(strLocName);

		CRegKey pLocKey;
		bool bWOWFlagsAvailable = (DWORD)(((LOBYTE(LOWORD(GetVersion()))) << 8) | (HIBYTE(LOWORD(GetVersion())))) > 0x0500;
		pLocKey.Open(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\environment", KEY_READ | (bWOWFlagsAvailable ? KEY_WOW64_32KEY : 0));
		if( pLocKey )
		{
			pLocKey.QueryStringValue("Localization", strLocName, &size);
			size = sizeof(strSkinPath);
			pLocKey.QueryStringValue("ProductRoot", strSkinPath, &size);
			strcat(strSkinPath, "\\");
		}
		if( !*strLocName )
			strcpy(strLocName, "en");
		if( !*strSkinPath )
		{
			GetModuleFileName((HMODULE)g_hmodThisDll, strSkinPath, MAX_PATH);
			CHAR* find = strrchr(strSkinPath, '\\');
			if( find ) *++find = 0;
		}
		strcat(strSkinPath, "skin\\");
		strcat(strSkinPath, strLocName);
		strcat(strSkinPath, "\\main.loc");
		g_strLocFile = strSkinPath;

		{
			CHAR strLocale[100] = {""};
			GetPrivateProfileString("Locale", "Value", "", strLocale, sizeof(strLocale), g_strLocFile.c_str());
			
			if( *strLocale )
			{
				CHAR szLInfo[10];
				if( ::GetLocaleInfo(atol(strLocale), LOCALE_IDEFAULTANSICODEPAGE, szLInfo, sizeof(szLInfo)) )
					g_nACP = atol(szLInfo);
			}
		}
	}

	CHAR str[MAX_PATH];
	GetPrivateProfileString("Global", strKey, strDefault, str, sizeof(str), g_strLocFile.c_str());

	USES_CONVERSION; _acp = g_nACP;
	return T2W(str);
}

int CShellExt::ShellInsertMenu(HMENU hMenu, 
                               UINT indexMenu, 
                               UINT idCmdFirst, 
                               bool bMyComputer)
{

    ::InsertMenu(hMenu, indexMenu++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL) ;

	std::wstring& strMenu = LoadString("ShellMenu", "Check for viruses");
    if( IS_ANSI() )
	{
		USES_CONVERSION;
		::InsertMenu(hMenu, indexMenu, MF_STRING|MF_BYPOSITION|(IsGrayedMenu() ? MF_GRAYED : 0), idCmdFirst, W2T(strMenu.c_str()));
	}
	else
		::InsertMenuW(hMenu, indexMenu, MF_STRING|MF_BYPOSITION|(IsGrayedMenu() ? MF_GRAYED : 0), idCmdFirst, strMenu.c_str());
    
    if( m_hMenuBmp )
        ::SetMenuItemBitmaps(hMenu, indexMenu, MF_BITMAP|MF_BYPOSITION, (HBITMAP)m_hMenuBmp, (HBITMAP)m_hMenuBmp);
    
    ::InsertMenu(hMenu, indexMenu + 1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL) ;
    return 2 ;
}

DWORD CShellExt::StartScanner(LPVOID lpParameter)
{
    if (!lpParameter)
        return 0 ;

    std::auto_ptr<CScanData> pData((CScanData*)lpParameter) ;

    if (pData->m_eMsgBoxType == eOnlyNetwork)
    {
        ::MessageBox(NULL, "IDS_SHELLEX_COULD_NOT_SCAN_NETWORK_DRIVE", "Error", MB_OK | MB_ICONWARNING) ;
        return 0 ;
    }

    if (pData->m_eMsgBoxType == eNormalAndNetwork)
    {
        if( ::MessageBox(NULL, "IDS_SHELLEX_SCAN_NETWORK_DRIVE_WARNING", "Error", MB_OKCANCEL | MB_ICONWARNING) != IDOK )
            return 0 ;
    }
    
    RELEASE_TRY

		CPRRemotePrague pRemote(g_hmodThisDll);
		if( PR_FAIL( pRemote.GetError() ) )
			return 0;

		CPRRemoteProxy<cTaskManager*> pProxy(pRemote, "TaskManager");
		tProcessIdToSessionId gfnProcessToSession = (tProcessIdToSessionId)::GetProcAddress(GetModuleHandle("Kernel32.dll"), "ProcessIdToSessionId") ;

		tERROR error = pRemote.LoadProxyStub("pxstub.ppl");
		if( PR_FAIL(error) || !pProxy.Attach())
			return 0;

		cAskScanObjects objAskScanObjects ;
		if (gfnProcessToSession)
        {
			gfnProcessToSession(GetCurrentProcessId(), (DWORD*)&objAskScanObjects.m_nSessionId) ;
        }

		if (pData->m_blMyComputer)
		{
			cScanObject &obj = objAskScanObjects.m_aScanObjects.push_back() ;
			obj.m_nObjType = OBJECT_TYPE_MY_COMPUTER;
		}
		else
		{
			for (size_t unIndex = 0, unCount = pData->m_aFile.size(); unIndex < unCount; ++unIndex)
			{
				cScanObject objScanObject ;
				objScanObject.m_strObjName  = pData->m_aFile[unIndex].c_str() ;
				objScanObject.m_bEnabled    = true ;
				if (!GetObjectType(pData->m_aFile[unIndex].c_str(), (enScanObjType&)objScanObject.m_nObjType))
				{
					continue ;
				}
				objAskScanObjects.m_aScanObjects.push_back(objScanObject) ;
			}
		}
		if (objAskScanObjects.m_aScanObjects.size() && pProxy.IsValid())
		{
			pProxy->AskAction(TASKID_TM_ITSELF, cAskScanObjects::eIID, &objAskScanObjects) ;
		}

    RELEASE_CATCH
    RELEASE_ENDTRY

    return 0 ;
}

//-------------------------------------------------------------------