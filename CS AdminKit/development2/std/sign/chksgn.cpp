/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	chksgn.cpp
 * \author	Andrew Kazachkov
 * \date	29.09.2003 15:17:22 - 16.10.2007 12:31:34
 * \brief	Checking file signatures
 * 
 */

#include <std/base/klstd.h>
#include <std/sign/chksgn.h>
#include <common/measurer.h>

#include <std/sign/sign_opt.h>

#ifdef _WIN32
    #include <std/win32/klos_win32v40.h>
    #include <std/win32/klos_win32_errors.h>
    #include <common/klregistry.h>
#endif


#define KLCS_MODULENAME L"KLSTD"

using namespace KLSTD;
using namespace KLERR;

namespace KLSTDSIGN
{
    void DoCheckFileSignature(const wchar_t* szwFileName);
};

KLCSC_DECL bool KLSTD_IfExists2(const wchar_t* szwFileName);

KLCSC_DECL bool KLSTD_CheckFileSignature(const wchar_t* szwFileName, bool bThrowIfMismatch)
{    
    KLSTD_CHK(szwFileName, szwFileName && szwFileName[0]);
    bool bResult = true;
KL_TMEASURE_BEGIN(L"checking signature of file", 4)
    KLERR_TRY

    #ifdef KLSTD_DO_CHECK_SIGNATURE

        KLSTDSIGN::DoCheckFileSignature(szwFileName);

    #else

        KLSTD_IfExists2(szwFileName);

    #endif

    KLERR_CATCH(pError)
        bResult = false;
        KLERR_SAY_FAILURE(1, pError);
    KLERR_FINALLY
        if(bThrowIfMismatch)
        {
            KLERR_RETHROW();
        };
    KLERR_ENDTRY
KL_TMEASURE_END()
    return bResult;
};


KLCSC_DECL void KLSTD_CheckDllBeforeLoad(const wchar_t* szwDll)
{
    KLSTD_CheckFileSignature(szwDll, true);
}

KLCSC_DECL bool KLSTD_CheckModuleSignature(const wchar_t* szwCmdLine, bool bThrowIfMismatch)
{
    KLSTD_CHK(szwCmdLine, szwCmdLine && szwCmdLine[0]);
	
	wchar_t **argv=NULL;
	wchar_t *args=NULL;
	int numargs=0;
	int numchars=0;
    bool bResult = true;

    KLERR_TRY
        const wchar_t* pModuleName = szwCmdLine;
#ifdef _WIN32
        if(szwCmdLine[0] == _T('\"'))
#else
		if(szwCmdLine[0] == L'\"') 
#endif
        {
            KLSTD_ParseCmdlineW(szwCmdLine, argv, args, &numargs, &numchars);
	        
            argv = (wchar_t**)malloc(sizeof(wchar_t*)*numargs);
            KLSTD_CHKMEM(argv);
	        args = (wchar_t*)malloc(sizeof(wchar_t)*numchars);
            KLSTD_CHKMEM(args);
	        
            KLSTD_ParseCmdlineW(szwCmdLine, argv, args, &numargs, &numchars);
            KLSTD_CHK(szwCmdLine, numargs > 0 && argv[0] && argv[0][0]);
            
            pModuleName = argv[0];
        };
        bResult = KLSTD_CheckFileSignature(pModuleName, bThrowIfMismatch);
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(3, pError);
    KLERR_FINALLY
        if(argv)free(argv);
	    if(args)free(args);
        KLERR_RETHROW();
    KLERR_ENDTRY
    return bResult;
}

#ifdef _WIN32

    /*!
      \brief	KLSTD_CheckLoadedModules

      \param	onst wchar_t * const * szwModules
      \param	size_t                  nModules
      \return	KLCSC_DECL void 
    */
    KLCSC_DECL void KLSTD_CheckLoadedModules(
                        const wchar_t * const * szwModules, 
                        size_t                  nModules)
    {
    KL_TMEASURE_BEGIN(L"KLSTD_CheckLoadedDlls", 4)
        KLSTD_CHK(szwModules, (!!szwModules) == (!!nModules) );
        std::vector<TCHAR> vecBuffer;
        vecBuffer.resize(MAX_PATH*4+1);
        for(size_t i = 0; i < nModules; ++i)
        {
            HMODULE hModule = NULL;
            if(szwModules[i] && szwModules[i][0])
            {
                KLSTD_TRACE1(4, L"module: '%ls'\n", szwModules[i]);
                hModule = GetModuleHandle(KLSTD_W2CT2(szwModules[i]));            
                if(!hModule)
                    continue;
            };
            KLSTD_LECHK(GetModuleFileName(
                                hModule, 
                                &vecBuffer[0], 
                                vecBuffer.size()));
            KLSTD_TRACE2(4, L"module: '%ls', filename='%ls'\n", szwModules[i], &vecBuffer[0]);
            KLSTD_CheckFileSignature(KLSTD_T2CW2(&vecBuffer[0]), true);
        };
    KL_TMEASURE_END()
    };
#endif

#ifdef KLSTD_WINNT
    KLCSC_DECL bool KLSTD_CheckServiceSignature(const wchar_t* szwServiceNameName, bool bThrowIfMismatch)
    {
        KLSTD_CHK(szwServiceNameName, szwServiceNameName && szwServiceNameName[0]);
        const std::wstring wstrKey = std::wstring(L"SYSTEM\\CurrentControlSet\\Services\\") + szwServiceNameName;

        KlRegKey key;
        KLSTD_RGCHK(key.Open(
                                HKEY_LOCAL_MACHINE,
                                wstrKey.c_str(),
                                KEY_READ));
        
        DWORD dwType=0;
        TCHAR*  pBuffer=0;
        DWORD   cbBuffer=0;
        long lResult=RegQueryValueExW(key, L"ImagePath", NULL, &dwType, NULL, &cbBuffer);
        if(lResult != NO_ERROR)
            KLSTD_THROW_LASTERROR_CODE2(lResult);

        if(dwType != REG_EXPAND_SZ && dwType != REG_SZ)
            KLSTD_THROW(STDE_BADFORMAT);

        CAutoPtr<MemoryChunk> pChunk;
        KLSTD_AllocMemoryChunk(cbBuffer, &pChunk);

        pBuffer=(TCHAR*)pChunk->GetDataPtr();
        
        lResult=RegQueryValueExW(key, L"ImagePath", NULL, &dwType, (LPBYTE)pBuffer, &cbBuffer);
        if(lResult != NO_ERROR)
            KLSTD_THROW_LASTERROR_CODE2(lResult);

        if(dwType == REG_EXPAND_SZ)
        {
            WCHAR* pExpandedBuffer = NULL;
            DWORD dwExpandedBuffer = ExpandEnvironmentStringsW(pBuffer, NULL, 0);
            if(dwExpandedBuffer == 0)
                KLSTD_THROW_LASTERROR2();
            
            CAutoPtr<MemoryChunk> pExpandedChunk;
            KLSTD_AllocMemoryChunk(dwExpandedBuffer*sizeof(WCHAR), &pExpandedChunk);
            pExpandedBuffer=(WCHAR*)pExpandedChunk->GetDataPtr();
            if(!ExpandEnvironmentStringsW(pBuffer, pExpandedBuffer, dwExpandedBuffer))
                KLSTD_THROW_LASTERROR2();

            pBuffer=pExpandedBuffer;
            pChunk=pExpandedChunk;
        };        
        return KLSTD_CheckModuleSignature(pBuffer, bThrowIfMismatch);
    };
#else
    KLCSC_DECL bool KLSTD_CheckServiceSignature(const wchar_t* szwServiceNameName, bool bThrowIfMismatch)
    {
        return false;
    };
#endif

#define PCL_T(_x)   _x
#define PCL_TSCHAR  char
#define PCL_TUCHAR  unsigned char

#include "./prsecmdl.h"

KLCSC_DECL void KLSTD_ParseCmdlineA(
					    const char* cmdstart,
					    char**      argv,
					    char*       args,
					    int*        numargs,
					    int*        numchars)
{
    parse_cmdline(
                cmdstart,        
                argv,        
                args,        
                numargs,        
                numchars);
};

#undef PCL_T       
#undef PCL_TSCHAR  
#undef PCL_TUCHAR  


#define PCL_T(_x)   L##_x
#define PCL_TSCHAR  wchar_t
#define PCL_TUCHAR  wchar_t

#include "./prsecmdl.h"

KLCSC_DECL void KLSTD_ParseCmdlineW(
					    const wchar_t*  cmdstart,
					    wchar_t**       argv,
					    wchar_t*        args,
					    int*            numargs,
					    int*            numchars)
{
    parse_cmdline(
                cmdstart,        
                argv,        
                args,        
                numargs,        
                numchars);
};

#undef PCL_T       
#undef PCL_TSCHAR  
#undef PCL_TUCHAR  
