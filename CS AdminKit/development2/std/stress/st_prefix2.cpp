/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	st_prefix.cpp
 * \author	Andrew Kazachkov
 * \date	29.10.2003 15:36:39
 * \brief	internal support for stress tests
 * 
 */

#ifdef _WIN32
#include <atlbase.h>
#include <io.h>
#endif

#include "avp/avp_data.h"

#include "std/base/klbase.h"
#include "std/io/klio.h"
#include "std/err/error.h"
#include "std/err/klerrors.h"
#include "std/conv/klconv.h"

#include "std/stress/st_prefix.h"
#include "std/stress/st_prefix2.h"

#include <kca/prss/settingsstorage.h>
#include "kca/prss/prssconst.h"

using namespace KLSTD;
using namespace KLERR;
using namespace std;

#define KLCS_DEFAULT_SETTINGS_PATHA  "c:\\KlDbgKca"
#define KLCS_DEFAULT_SETTINGS_PATHW  L"c:\\KlDbgKca"

void StCallRegProduct(bool bForce)
{
    KLSTD_USES_CONVERSION;
    bool bRegRequired=false;
    if(bForce)
        bRegRequired=true;
    else
    {
        CRegKey key;
        long lResult=key.Open(
                        HKEY_LOCAL_MACHINE,
                        KLSTD_StGetRootKey().c_str(),
                        KEY_READ);
        if(lResult == ERROR_FILE_NOT_FOUND)
            bRegRequired=true;
        else if(lResult)
            KLSTD_THROW_LASTERROR_CODE2(lResult);
        if(!bRegRequired)        
            bRegRequired=(_taccess(KLSTD_StGetDefDir().c_str(), 00) == -1);
    };
    if(bRegRequired)
    {
        const std::wstring& wstrRoot=KLSTD_StGetDefDirW();
        KLSTD_CreatePath(wstrRoot.c_str());
        
        {
            std::wstring wstrSsSettings, wstrSsProdinfo, wstrSsRuntime, wstrLicKeys;
            KLSTD_MakePath(wstrRoot, KLPRSS::c_szwSST_HostSS, KLPRSS::c_szwFileExt_Data, wstrSsSettings);
            KLSTD_MakePath(wstrRoot, KLPRSS::c_szwSST_ProdinfoSS, KLPRSS::c_szwFileExt_Data, wstrSsProdinfo);
            KLSTD_MakePath(wstrRoot, KLPRSS::c_szwSST_RuntimeSS, KLPRSS::c_szwFileExt_Data, wstrSsRuntime);
            KLSTD_MakePath(wstrRoot, KLPRSS::c_szwSST_LicKeys, KLPRSS::c_szwFileExt_Data, wstrLicKeys);

            CRegKey key;
            long lResult;
            lResult=key.Create(
                            HKEY_LOCAL_MACHINE,
                            KLSTD_StGetRootKey().c_str(),
                            NULL,
                            REG_OPTION_NON_VOLATILE,
                            KEY_WRITE);
            if(lResult)KLSTD_THROW_LASTERROR_CODE2(lResult);
            ;
            key.SetValue(KLSTD_W2CT(wstrSsSettings.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_HostSS));
            key.SetValue(KLSTD_W2CT(wstrSsProdinfo.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_ProdinfoSS));
            key.SetValue(KLSTD_W2CT(wstrSsRuntime.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_RuntimeSS));
            key.SetValue(KLSTD_W2CT(wstrLicKeys.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_LicKeys));
            ;
            CAutoPtr<File> pFile;
            KLSTD_CreateFile(wstrSsSettings, 0, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, 0, &pFile);
            pFile=NULL;
            KLSTD_CreateFile(wstrSsProdinfo, 0, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, 0, &pFile);
            pFile=NULL;
            KLSTD_CreateFile(wstrSsRuntime, 0, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, 0, &pFile);
            pFile=NULL;
        };
        {
            CRegKey key;
            long lResult;
            lResult=key.Create(
                            HKEY_LOCAL_MACHINE,
                            KLSTD_StGetProductKey(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT).c_str(),
                            NULL,
                            REG_OPTION_NON_VOLATILE,
                            KEY_WRITE);
            if(lResult)KLSTD_THROW_LASTERROR_CODE2(lResult);
            ;
            std::wstring wstrPolicy, wstrTasks;
            KLSTD_MakePath(wstrRoot, L"unipl", KLPRSS::c_szwFileExt_Data, wstrPolicy);
            KLSTD_MakePath(wstrRoot, L"unitsk", L"", wstrTasks);
            key.SetValue(KLSTD_W2CT(wstrPolicy.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_HostSP));
            key.SetValue(KLSTD_W2CT(wstrTasks.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_TasksSS));
            ;
            KLSTD_CreatePath(wstrTasks.c_str());
            CAutoPtr<File> pFile;
            KLSTD_CreateFile(wstrPolicy, 0, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, 0, &pFile);
            pFile=NULL;
        };
    };

}