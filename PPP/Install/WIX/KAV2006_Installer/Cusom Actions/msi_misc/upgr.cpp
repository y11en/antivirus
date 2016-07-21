#include "upgrsett.h"
#include "../common/cregreg.h"
#include "stdafx.h"
#include "msi_misc.h"
#pragma warning(disable : 4786)
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"

using namespace AVPUpgradeSettings;

class CUpgrTracer : public IUpgrTracer
{
    MSIHANDLE m_hinstall;

public:
    CUpgrTracer(MSIHANDLE hinstall) : m_hinstall(hinstall) {}

    virtual void printf(const char * str...)
    {
        va_list list;
        va_start(list, str);
        vinstallLog(m_hinstall, str, list);
        va_end(str);
    }

};

MSI_MISC_API UINT __stdcall AVP6UpgradeSettings(MSIHANDLE hInstall)
{
    CRegRegRoot root;
    AVPUpgradeSettings::CRegKey avp6key = root.getkey(HKEY_LOCAL_MACHINE, L"Software\\KasperskyLab\\AVP6");
    AVPUpgradeSettings::CRegKey avp7key = root.getkey(HKEY_LOCAL_MACHINE, L"Software\\KasperskyLab\\protected\\AVP7");
    AVPUpgradeSettings::CRegKey avp8key = root.getkey(HKEY_LOCAL_MACHINE, L"Software\\KasperskyLab\\protected\\AVP8");
    if (avp8key)
    {
        if (avp7key)
        {
            CUpgrTracer tracer(hInstall);
            installLog(hInstall, "UpgradeAVPSettings: not implemented");
//            upgradesettings_avp7_avp8(avp7key, avp8key, &tracer);
        }
        else if (avp6key)
		{
            CUpgrTracer tracer(hInstall);
            installLog(hInstall, "UpgradeAVPSettings: not implemented");
//            upgradesettings_avp6_avp8(avp6key, avp8key, &tracer);
		}
		else
            installLog(hInstall, "UpgradeAVPSettings: no prev settings");
    }
    else
        installLog(hInstall, "UpgradeAVPSettings: unable to open AVP8 key");

    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall AVP6UpgradeQB(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	wstring sData;

	if (CAGetDataW(hInstall, sData))
	{
		CADataParser<wchar_t> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const wstring& sSrcDir = parser.value(L"Src");
			const wstring& sDstDir = parser.value(L"Dst");

            installLog(hInstall, "AVP6UpgradeQB: Copying from %ls to %ls", sSrcDir.c_str(), sDstDir.c_str());

            if (!CopyFilesW(sSrcDir.c_str(), sDstDir.c_str()))
                installLog(hInstall, "AVP6UpgradeQB: Copying failed: 0x%08X", GetLastError());
        }
        else
            installLog(hInstall, "AVP6UpgradeQB: unable to parse cdata %ls", sData.c_str());
    }
    else
        installLog(hInstall, "AVP6UpgradeQB: unable to get cdata");

    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall AVP6UpgradeAS(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	wstring sData;

	if (CAGetDataW(hInstall, sData))
	{
		CADataParser<wchar_t> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const wstring& sSrcDir = parser.value(L"Src");
			const wstring& sDstDir = parser.value(L"Dst");

            installLog(hInstall, "AVP6UpgradeAS: Copying from %ls to %ls", sSrcDir.c_str(), sDstDir.c_str());

            if (!CopyFileW(sSrcDir.c_str(), sDstDir.c_str(), FALSE))
                installLog(hInstall, "AVP6UpgradeAS: Copying failed: 0x%08X", GetLastError());
        }
        else
            installLog(hInstall, "AVP6UpgradeAS: unable to parse cdata %ls", sData.c_str());
    }
    else
        installLog(hInstall, "AVP6UpgradeAS: unable to get cdata");

    return ERROR_SUCCESS;
}