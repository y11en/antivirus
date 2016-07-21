#include "stdafx.h"
#include <SelfDefence/SelfDefence.cpp>
#include "msi_misc.h"
#pragma warning(disable : 4786)
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"

// sync with selfdef.wxi and msi_misc.def
#define SELFPROTECTION              "RMLOC"
#define InitProtectionProperty      SetSTATUSProp
#define RbFileProtectionDef         RbInitCAServerDef
#define EnableFileProtectionDef     InitCAServerDef
#define EnableFileProtectionImm     InitCAServerImm
#define EnableFullProtectionDef     RegisterRMLoc
#define RbFullProtectionDef         RbRegisterRMLoc
#define DisableProtectionDef        SetIsInstalled
#define DisableFidboxDef            DisableFidbox

#define LSTR(A) L ## #A

MSI_MISC_API UINT __stdcall InitProtectionProperty(MSIHANDLE hInstall)
{
#define SCRAMBLED_SELFPROTECTION  "fryscebgrpgvba"

    const char *scrambled = SCRAMBLED_SELFPROTECTION;
    char str[sizeof(SCRAMBLED_SELFPROTECTION) + 1];
    for (int i = strlen(scrambled); i >= 0;--i)
    {
        int c = scrambled[i];
        if (c >= 'a' && c <= 'z') {
          c += 13;
          if (c > 'z') c -= 26; 
          str[i] = c - 'a' + 'A';
        }
        else
          str[i] = c;
    }

    //installLog(hInstall, "InitProtectionProperty: str=%s", str);
    if (GetStrPropValue(hInstall, str) == "1")
    {
        MsiSetProperty(hInstall, SELFPROTECTION, "1");
    }
    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall RbFileProtectionDef(MSIHANDLE hInstall)
{
    installLog(hInstall, "RbInitCAServerDef");
    // turn off protection
	Protect( _eTS_Off /*active*/, _eTS_Off/*files*/, NULL, _eTS_Off/*reg*/, _eTS_Off/*service*/, TRUE, FALSE );
    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall EnableFileProtectionDef(MSIHANDLE hInstall)
{
    using namespace MsiSupport::Utility;
    wstring installdir;
    CAGetDataW(hInstall, installdir);
    //installLogW(hInstall, L"EnableFileProtectionDef: str=%ls", installdir.c_str());
    // turn on file protection only
	Protect( _eTS_On /*active*/, _eTS_On/*files*/, (PWCHAR)installdir.c_str(), _eTS_Off/*reg*/, _eTS_Off/*service*/, TRUE, FALSE );
    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall EnableFileProtectionImm(MSIHANDLE hInstall)
{
    wstring installdir = GetStrPropValueW(hInstall, L"INSTALLDIR");
    // register install 
	Protect( _eTS_None /*active*/, _eTS_None/*files*/, NULL, _eTS_None/*reg*/, _eTS_None/*service*/, TRUE, FALSE );
    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall EnableFullProtectionDef(MSIHANDLE hInstall)
{
    using namespace MsiSupport::Utility;
    wstring installdir;
    CAGetDataW(hInstall, installdir);
    //installLogW(hInstall, L"EnableFullProtectionDef: str=%ls", installdir.c_str());
    // turn on full protection
	Protect( _eTS_On /*active*/, _eTS_On/*files*/, (PWCHAR)installdir.c_str(), _eTS_On/*reg*/, _eTS_On/*service*/, TRUE, FALSE );
    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall RbFullProtectionDef(MSIHANDLE hInstall)
{
    installLog(hInstall, "RbRegisterRMLoc");
    // turn off protection
	Protect( _eTS_Off /*active*/, _eTS_Off/*files*/, NULL, _eTS_Off/*reg*/, _eTS_Off/*service*/, TRUE, FALSE );
    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall DisableProtectionDef(MSIHANDLE hInstall)
{
    installLog(hInstall, "SetIsInstalled");
    // turn off protection
	Protect( _eTS_Off /*active*/, _eTS_Off/*files*/, NULL, _eTS_Off/*reg*/, _eTS_Off/*service*/, TRUE, FALSE );
    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall DisableFidboxDef(MSIHANDLE hInstall)
{
    installLog(hInstall, "DisableFidbox");
    // delete fidbox
	DelDrvFiles();
    return ERROR_SUCCESS;
}

