//
// ProductInfoCab.cpp
//

#include "StdAfx.h"

#include <prague.h>
#include <pr_cpp.h>

#include <iface/i_root.h>
#include <iface/i_io.h>

#include <plugin/p_miniarc.h>
#include <plugin/p_minizip.h>

#define IMPEX_IMPORT_LIB
    #include <plugin/p_win32_nfio.h>


#if defined (_WIN32)

#include <windows.h>
#include <tchar.h>

tERROR GetPragueTempFile(tSTRING p_pszPrefix, tSTRING p_pszExt, cStrObj& p_strTempFilePath);

static LPCSTR s_pszRootRegKey = _T(VER_PRODUCT_REGISTRY_PATH);

class cAutoHandle
{
public:

    cAutoHandle(HANDLE hHandle = NULL)
    {
        m_hHandle = hHandle ;
    }

    ~cAutoHandle()
    {
        if (m_hHandle)
        {
            ::CloseHandle(m_hHandle) ;
        }
    }

    HANDLE operator =(HANDLE hHandle)
    {
        return m_hHandle = hHandle ;
    }

    operator HANDLE()
    {
        return m_hHandle ;
    }

    operator HANDLE*()
    {
        return &m_hHandle ;
    }

private:
    HANDLE m_hHandle ;
};

tERROR CreateRegFile(cStrObj& p_strRegFile)
{
    tERROR error = errOK ;

    cStrObj strRegFile ;
    error = GetPragueTempFile("kav_reg", ".reg", strRegFile) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    HKEY hKey = NULL ;
    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, s_pszRootRegKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
        return errNOT_FOUND ;
    }

    cAutoHandle hSafeToken = NULL ;
    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, hSafeToken))
    {
        return errUNEXPECTED ;
    }

    LUID luid ;
    TOKEN_PRIVILEGES tp ;
    if (!::LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &luid))
    {
        return errUNEXPECTED ;
    }

    tp.PrivilegeCount           = 1 ;
    tp.Privileges[0].Luid       = luid ;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED ;
    ::AdjustTokenPrivileges(hSafeToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL) ;

    if (::RegSaveKey(hKey, (tSTRING)strRegFile.c_str(cCP_ANSI), NULL) != ERROR_SUCCESS)
    {
        return errUNEXPECTED ;
    }

    p_strRegFile = strRegFile ;
    return error ;
}

tERROR AddRegFile(hOS p_hZip)
{
    tERROR error = errOK ;

    cStrObj strRegFile ;
    error = CreateRegFile(strRegFile) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    cStrObj strRegFileName("kav2006.reg") ;

    hIO hZipIO = NULL ;
    error = p_hZip->IOCreate(&hZipIO, cAutoString (strRegFileName), fACCESS_RW, fOMODE_CREATE_ALWAYS) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = PrCopy(cAutoString (strRegFile), (hOBJECT)hZipIO, fPR_FILE_REPLACE_EXISTING) ;

    cAutoObj<cOS> hWin ;
    if (PR_SUCC(g_root->sysCreateObjectQuick(hWin, IID_OS, PID_NATIVE_FIO)))
    {
        hWin->Delete(cAutoString (strRegFile)) ;
    }

    return error ;
}

tERROR AddSkinFile(hOS p_hZip)
{
    tERROR error = errOK ;

    return error ;
}

tERROR AddAntispamFile(const cStrObj& p_strSfdbFile, hOS p_hZip)
{
    tERROR error = errOK ;

    cStrObj strRegFileName("antispam.sfdb") ;

    hIO hZipIO = NULL ;
    error = p_hZip->IOCreate(&hZipIO, cAutoString (strRegFileName), fACCESS_RW, fOMODE_CREATE_ALWAYS) ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tAddAntispamFile(): IOCreate fails 0x%08x\n", error));
        return error ;
    }

    error = PrCopy(cAutoString (p_strSfdbFile), (hOBJECT)hZipIO, fPR_FILE_REPLACE_EXISTING) ;
    
	if(PR_FAIL(error))
		PR_TRACE((g_root, prtERROR, "gui\tAddAntispamFile(): Can't copy to file %s 0x%08x\n", p_strSfdbFile, error));

    return error ;
}

tERROR CreateRegInfoCab(cStrObj& strCabFile)
{
    tERROR error = errOK ;

    error = GetPragueTempFile("kav_reg", ".zip", strCabFile) ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateRegInfoCab(): Can't create temp file 0x%08x\n", error));
        return error ;
    }

    cAutoObj<cIO> hNFIO = NULL ;
    error = g_root->sysCreateObject(hNFIO, IID_IO, PID_WIN32_NFIO) ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateRegInfoCab(): Can't create hIO 0x%08x\n", error));
        return error ;
    }

    error = strCabFile.copy(hNFIO, pgOBJECT_FULL_NAME) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hNFIO->set_pgOBJECT_OPEN_MODE(fOMODE_CREATE_ALWAYS) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hNFIO->set_pgOBJECT_ACCESS_MODE(fACCESS_RW) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hNFIO->sysCreateObjectDone() ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateRegInfoCab(): Can't init hIO 0x%08x\n", error));
        return error ;
    }

    hOS hZip = NULL ;
    error = hNFIO->sysCreateObject((hOBJECT*)&hZip, IID_OS, PID_UNIVERSAL_ARCHIVER) ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateRegInfoCab(): Can't create PID_UNIVERSAL_ARCHIVER 0x%08x\n", error));
        return error ;
    }

    error = hZip->propSetDWord(ppMINI_ARCHIVER_PID, PID_MINIZIP) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hZip->sysCreateObjectDone() ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateRegInfoCab(): Can't init PID_UNIVERSAL_ARCHIVER 0x%08x\n", error));
        return error ;
    }

    error = AddRegFile(hZip) ;

	if (PR_FAIL(error))
		PR_TRACE((g_root, prtERROR, "gui\tCreateRegInfoCab(): AddRegFile fails 0x%08x\n", error));

    return error ;
}

tERROR CreateAntispamInfoCab(const cStrObj& p_strSfdbFile, cStrObj& strCabFile)
{
    tERROR error = errOK ;

    error = GetPragueTempFile("kav_antispam", ".zip", strCabFile) ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateAntispamInfoCab(): Can't create temp file 0x%08x\n", error));
        return error ;
    }

    cAutoObj<cIO> hNFIO = NULL ;
    error = g_root->sysCreateObject(hNFIO, IID_IO, PID_WIN32_NFIO) ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateAntispamInfoCab(): Can't create hIO 0x%08x\n", error));
        return error ;
    }

    error = strCabFile.copy(hNFIO, pgOBJECT_FULL_NAME) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hNFIO->set_pgOBJECT_OPEN_MODE(fOMODE_CREATE_ALWAYS) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hNFIO->set_pgOBJECT_ACCESS_MODE(fACCESS_RW) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hNFIO->sysCreateObjectDone() ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateAntispamInfoCab(): Can't init hIO 0x%08x\n", error));
        return error ;
    }

    hOS hZip = NULL ;
    error = hNFIO->sysCreateObject((hOBJECT*)&hZip, IID_OS, PID_UNIVERSAL_ARCHIVER) ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateAntispamInfoCab(): Can't create PID_UNIVERSAL_ARCHIVER 0x%08x\n", error));
        return error ;
    }

    error = hZip->propSetDWord(ppMINI_ARCHIVER_PID, PID_MINIZIP) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hZip->sysCreateObjectDone() ;
    if (PR_FAIL(error))
    {
		PR_TRACE((g_root, prtERROR, "gui\tCreateAntispamInfoCab(): Can't init PID_UNIVERSAL_ARCHIVER 0x%08x\n", error));
        return error ;
    }

    error = AddAntispamFile(p_strSfdbFile, hZip) ;
    
	if (PR_FAIL(error))
		PR_TRACE((g_root, prtERROR, "gui\tCreateAntispamInfoCab(): AddAntispamFile fails 0x%08x\n", error));


    return error ;
}

tERROR CreateProductInfoCab(cStrObj& strCabFile)
{
    tERROR error = errOK ;

    error = GetPragueTempFile("kav", ".zip", strCabFile) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    cAutoObj<cIO> hNFIO = NULL ;
    error = g_root->sysCreateObject(hNFIO, IID_IO, PID_WIN32_NFIO) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = strCabFile.copy(hNFIO, pgOBJECT_FULL_NAME) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hNFIO->set_pgOBJECT_OPEN_MODE(fOMODE_CREATE_ALWAYS) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hNFIO->set_pgOBJECT_ACCESS_MODE(fACCESS_RW) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hNFIO->sysCreateObjectDone() ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    hOS hZip = NULL ;
    error = hNFIO->sysCreateObject((hOBJECT*)&hZip, IID_OS, PID_UNIVERSAL_ARCHIVER) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hZip->propSetDWord(ppMINI_ARCHIVER_PID, PID_MINIZIP) ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = hZip->sysCreateObjectDone() ;
    if (PR_FAIL(error))
    {
        return error ;
    }

    error = AddRegFile(hZip) ;

    tERROR err = errOK ;
    err = AddSkinFile(hZip) ;
    if (PR_FAIL(error))
    {
        error = err ;
    }

    return error ;
}
#endif //_WIN32


tERROR GetPragueTempFile(tSTRING p_pszPrefix, tSTRING p_pszExt, cStrObj& p_strTempFilePath)
{
    tERROR error = errOK ;

    cStrObj strTempFileName ;
    strTempFileName.init() ;
    error = PrGetTempFileName((hOBJECT)cAutoString (strTempFileName), p_pszPrefix, cCP_ANSI) ;
    if (PR_SUCC(error))
    {
 		if ( p_pszExt )
		{
			// DVi: удаляем паразитный темповый файл, созданный в PrGetTempFileName
			cAutoObj<cOS> hWin ;
			if (PR_SUCC(g_root->sysCreateObjectQuick(hWin, IID_OS, PID_NATIVE_FIO)))
				hWin->Delete(cAutoString(strTempFileName)) ;
		}
		strTempFileName += p_pszExt ;
		p_strTempFilePath = strTempFileName ;
    }

    return error ;
}


