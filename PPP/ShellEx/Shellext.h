#ifndef __SHELLEXT_H__
#define __SHELLEXT_H__

#include <windows.h>
#include <shlobj.h>

#include <vector>

#include <pr_remote.h>
#include <iface/i_root.h>
#include <structs/s_profiles.h>
#include <structs/s_gui.h>

//--------------------------- CShellExt -----------------------------

class CShellExt : public IContextMenu, 
                  public IShellExtInit 
{

    typedef std::vector<std::wstring> FileList_t ;

public:
    
    enum eMsgBoxType{
        eOnlyNormal         = 0x0,
        eOnlyNetwork        = 0x1,
        eNormalAndNetwork   = 0x2,
    };

public:

    struct CScanData
    {
        eMsgBoxType m_eMsgBoxType ;
        bool        m_blMyComputer ;
        FileList_t  m_aFile ;

        CScanData()
        {
            m_eMsgBoxType   = eOnlyNormal ;
            m_blMyComputer  = false ;
        }
    };

public:
	CShellExt() ;

	~CShellExt() ;

	//IUnknown members
    STDMETHODIMP            QueryInterface(REFIID riid, 
                                           LPVOID FAR * ppv) ;

    STDMETHODIMP_(ULONG)    AddRef() ;

    STDMETHODIMP_(ULONG)    Release() ;

	//IContextMenu members
    STDMETHODIMP            QueryContextMenu(HMENU hMenu,
                                             UINT indexMenu, 
                                             UINT idCmdFirst,
                                             UINT idCmdLast, 
                                             UINT uFlags) ;

    STDMETHODIMP            InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi) ;

	//IShellExtInit methods
    STDMETHODIMP		    Initialize(LPCITEMIDLIST pIDFolder, 
                                       LPDATAOBJECT pDataObj, 
                                       HKEY hKeyID) ;

    STDMETHODIMP            GetCommandString(UINT_PTR idCmd, 
                                             UINT uFlags, 
                                             UINT FAR *reserved, 
                                             LPSTR pszName, 
                                             UINT cchMax) ;
	
private:
    void CreateFileList() ;
    
    void CreateNotNetworkDiskList(FileList_t& rResult) ;

    void DelNetworkPathFromList(FileList_t& rList) ;

    void MergeFileLists(FileList_t& rList1, 
                        FileList_t& rList2, 
                        FileList_t& rResult) ;

    void GetFileListFromHDROP(HDROP hDrop, 
                              FileList_t& rFileList) ;

    void GetFileListFromCida(CIDA* pCida, 
                             FileList_t& rFileList) ;

    std::wstring GetObjectPathFromItemIDList(LPITEMIDLIST pidlOriginal) ;

    int ShellInsertMenu(HMENU hMenu, 
                        UINT indexMenu, 
                        UINT idCmdFirst, 
                        bool bMyComputer) ;

    bool IsGrayedMenu() ;

    
    bool IsMyComputer(LPITEMIDLIST pidlOriginal) ;
    bool IsMyComputer() ;
    
private:
    static bool GetObjectType(const wchar_t* pszPath, enScanObjType& rObjectType) ;
    static DWORD WINAPI StartScanner(LPVOID lpParameter) ;

private:
    UINT            m_CFIdList ;
    ULONG           m_cRef ;
	LPDATAOBJECT    m_pDataObj ;
	HBITMAP         m_hMenuBmp ;
    FileList_t      m_aFile ;
} ;

typedef CShellExt* LPCSHELLEXT ;

//-------------------------------------------------------------------

#endif // __SHELLEXT_H__
