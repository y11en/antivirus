#ifdef _WIN32    
    #include "std/win32/klos_win32v50.h"
#endif

#include <std/base/klbase.h>
#include <std/base/klstdutils.h>
#include <std/conv/klconv.h>
#include <std/fmt/stdfmtstr.h>
#include <std/err/error.h>
#include <std/err/errlocmodule.h>
#include <std/trc/trace.h>

#define KLCS_MODULENAME L"KLERR"

using namespace KLSTD;

namespace KLERR
{
#ifdef _WIN32

    static HRESULT LoadLocalizedString(
                        HMODULE             hModule,
                        UINT                uId,
                        KLSTD::klwstr_t&    wstrString)
    {
        wstrString.clear();
	    UINT block = (uId >> 4) + 1;
	    UINT num = uId & 0xf;
	    DWORD dwLen=0;
	    HRSRC hRsrc=NULL;
	    LPWSTR  str=NULL;

        HRESULT hResult = S_OK;

	    if(	!(hRsrc=FindResource(hModule, MAKEINTRESOURCE(block), RT_STRING)) ||
		    !(str=(LPWSTR)LockResource(LoadResource(hModule, hRsrc))))
	    {
            DWORD dwError = GetLastError();
            hResult = dwError?HRESULT_FROM_WIN32(dwError):E_FAIL;		
	    }
        else
        {
            for(unsigned int i=0; i<num; i++)
                str+=*str+1;
	        dwLen=*str;
	        if(!dwLen)
                hResult = S_FALSE;
            else
                wstrString.assign(str+1, dwLen);
        };
	    return hResult;
    };

    KLSTD_NOTHROW KLCSC_DECL HLOCMODULE LoadLocModule(
                            const wchar_t* szwFileName) throw()
    {
        HLOCMODULE hResult = NULL;
        hResult = (HLOCMODULE)(void*)LoadLibraryEx(
                        KLSTD_W2CT2(szwFileName), 
                        NULL, 
                        LOAD_WITH_ALTERED_SEARCH_PATH|
                            LOAD_LIBRARY_AS_DATAFILE);
        return hResult;
    };
    
    KLSTD_NOTHROW KLCSC_DECL void UnloadLocModule(
                            HLOCMODULE hModule) throw()
    {
        if(hModule)
            FreeLibrary((HMODULE)(void*)hModule);
    };    

    KLSTD_NOTHROW KLCSC_DECL bool GetLocString(
                            HLOCMODULE      hModule,
                            unsigned        nID,
                            const wchar_t*  szwDefault,
                            KLSTD::AKWSTR&  wstrResult) throw()
    {
        bool bResult = false;
        KLSTD::klwstr_t wstrRes;
        if(S_OK != LoadLocalizedString(
                    (HMODULE)(void*)hModule,
                    nID,
                    wstrRes))
        {
            wstrRes = KLSTD::FixNullString(szwDefault);
            bResult = true;
        };
        wstrResult = wstrRes.detach();
        return bResult;
    };

#else
    KLSTD_NOTHROW KLCSC_DECL HLOCMODULE LoadLocModule(
                            const wchar_t* szwFileName) throw()
    {
        return NULL;
    };
    
    KLSTD_NOTHROW KLCSC_DECL void UnloadLocModule(
                            HLOCMODULE hModule) throw()
    {
        ;
    };    

    KLSTD_NOTHROW KLCSC_DECL bool GetLocString(
                            HLOCMODULE      hModule,
                            unsigned        nID,
                            const wchar_t*  szwDefault,
                            KLSTD::AKWSTR&  wstrResult) throw()
    {
        wstrResult = KLSTD::klwstr_t(KLSTD::FixNullString(szwDefault)).detach();
        return false;
    };
#endif
};
