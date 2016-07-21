/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file WNF/ComputerInfo.cpp
 * \author Андрей Брыксин
 * \date 2006
 * \brief Интерфейс для получения информации о компьютере.
 *
 */

#include <std/wnf/WindowsNetInfo.h>

#define KLCS_MODULENAME L"KLСF"

#ifdef N_PLAT_NLM

void KLWNF_GetComputerInfo(const std::wstring& name, KLWNF::ComputerInfo& info) {

	info.domain.clear();// = L"";
	info.name = name;
	info.versionMajor = 5;
	info.versionMinor = 60;
	info.ptype = KLWNF::P_NOVELL;
	info.ctype = (KLWNF::ComputerType)(KLWNF::CT_NOVELL);

}

#endif // N_PLAT_NLM

#ifdef _WIN32
#include <std/win32/klos_win32v50.h>
#include <atlbase.h>
#include <std/base/klstd.h>
#include <std/wnf/WindowsNetInfo.h>

#ifndef KLSTD_WIN9X			// Win 9x
	#include "lm.h"
	#include <Winnetwk.h>
	#pragma comment(lib, "netapi32.lib")
#endif
#endif // _WIN32

#if __unix
#include <sys/utsname.h>
#include <std/conv/klconv.h>
#endif

static AVP_dword  makeProductVersion(const std::wstring& wstrVersion)
{
    std::vector<std::wstring> vecComponents;
    vecComponents.reserve(4);
    KLSTD_SplitString(wstrVersion, L".", vecComponents);
    size_t nComponents = std::min(vecComponents.size(), size_t(4));
    if(3 == nComponents)
    {
        vecComponents.push_back(L"0");
        ++nComponents;
    };
    AVP_dword lValue = 0;
    for(size_t i = 0; i < nComponents; ++i)
    {
        wchar_t* pEndPtr = NULL;
        const size_t nOffset = nComponents - i - 1;        
        AVP_dword lComponent = wcstoul(vecComponents[nOffset].c_str(), &pEndPtr, 10);        
        lValue |= AVP_dword(ULONG_MAX == lComponent ? 0 : AVP_byte(lComponent)) << (i*8);
        /*
        fprintf(stdout, "%s -> %lu (%lf( \n", 
                (const char*)(KLSTD_W2CA2)vecComponents[nOffset].c_str(), 
                (AVP_dword )lComponent,
                double(lValue)); 
        */
    };
    return lValue;
}

using namespace std;
static wstring ReplaceToken( const wstring & str_, const wstring & strToken, const wstring & strValue )
{
    wstring str = str_;
    wstring::size_type pos = str.find( strToken);
    
    while( pos != wstring::npos )
    {
        str.replace( pos, strToken.size(), strValue);
        pos = str.find( strToken, pos + strValue.size());
    }
    return str;
}



void KLWNF_GetComputerInfo(const std::wstring& name, KLWNF::ComputerInfo& info)
{
#ifdef _WIN32	
#ifdef KLSTD_WIN9X			// Win 9x

	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	info.domain = L"";
	info.name = name;
	info.versionMajor = osvi.dwMajorVersion;
	info.versionMinor = osvi.dwMinorVersion;
	info.ptype = KLWNF::P_DOS;
	info.ctype = (KLWNF::ComputerType)(KLWNF::CT_WORKSTATION|KLWNF::CT_WINDOWS);

#else						// WinNT

	LPSERVER_INFO_101 pBuf = NULL;
	NET_API_STATUS nStatus = NetServerGetInfo(name.empty()?NULL:const_cast<wchar_t*>(name.c_str()), 
											  101, 
											  (LPBYTE*)&pBuf);
	if (nStatus == NERR_Success)
	{
		info.domain = L"";
		info.name = pBuf->sv101_name;
		info.versionMajor = pBuf->sv101_version_major;
		info.versionMinor = pBuf->sv101_version_minor;
		
		switch (pBuf->sv101_platform_id) {
			case PLATFORM_ID_OS2:
				info.ptype = KLWNF::P_OS2;
			break;
			case PLATFORM_ID_NT:
				info.ptype = KLWNF::P_NT;
			break;
			case PLATFORM_ID_OSF:
				info.ptype = KLWNF::P_OSF;
			break;
			case PLATFORM_ID_VMS:
				info.ptype = KLWNF::P_VMS;
			break;
			case PLATFORM_ID_DOS:
			default:
				info.ptype = KLWNF::P_DOS;
		}

		info.ctype = (KLWNF::ComputerType)pBuf->sv101_type;

		NetApiBufferFree(pBuf);
	}
	else
		KLSTD_THROW_LASTERROR_CODE(nStatus);

#endif
#elif __unix
	struct utsname osname;
	int res = uname( &osname );
	if ( 0 == res )
    {
    //KLERR_BEGIN
        const std::wstring wstrVersion = (const wchar_t*)KLSTD_A2CW2( osname.release );
        AVP_dword qwVersion = makeProductVersion(ReplaceToken(wstrVersion, L"-", L"." ));
	    info.domain = L"";
	    info.name = (const wchar_t*)KLSTD_A2CW2(osname.sysname);
	    info.versionMajor = (int)(unsigned)(qwVersion >> 16);
	    info.versionMinor = (int)(unsigned)(qwVersion & 0xFFFF);
	    info.ptype = KLWNF::P_VMS;
	    info.ctype = (KLWNF::ComputerType)(KLWNF::CT_SERVER_LINUX);
    //KLERR_ENDT(1)
    };
#endif
}

