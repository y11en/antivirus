#ifndef __KL_IPHLP_WRAPPER_H__
#define __KL_IPHLP_WRAPPER_H__

#ifdef _WIN32

#include <std/base/klstd.h>

#include <winsock2.h>
#include <std/win32/klos_win32v50.h>
#include <iptypes.h>
#include <iphlpapi.h>

#include <common/kldelayload.h>
#include <std/win32/klos_win32_errors.h>

const wchar_t c_szwKLIPHLPWRAPPER_ModuleName[] = L"KLIPHLPWRAPPER";

DWORD WINAPI KLIPHLPWRAPPER_GetAdaptersAddresses(
                        ULONG Family,
                        DWORD Flags,
                        PVOID Reserved,
                        PIP_ADAPTER_ADDRESSES pAdapterAddresses, 
                        PULONG pOutBufLen)
{
	DWORD dwResult = 0;
    __try
    {
        dwResult = GetAdaptersAddresses(
                            Family,
                            Flags,
                            Reserved,
                            pAdapterAddresses,
                            pOutBufLen);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLIPHLPWRAPPER_ModuleName, L"GetAdaptersAddresses not supported !!!\n");
        dwResult = ERROR_FUNCTION_FAILED;
    };
    return dwResult;
};


DWORD KLIPHLPWRAPPER_GetIpAddrTable(
    OUT    PMIB_IPADDRTABLE pIpAddrTable,
    IN OUT PULONG           pdwSize,
    IN     BOOL             bOrder)
{
    DWORD dwResut = 0;
    __try
    {
        dwResut = GetIpAddrTable(
                        pIpAddrTable,
                        pdwSize,
                        bOrder);
    }
    __except(KLDELAYLOAD_DL_CHECK())
    {
        dwResut = ERROR_INVALID_FUNCTION;
    };
    return dwResut;
};

DWORD WINAPI KLIPHLPWRAPPER_GetAdaptersInfo(
            PIP_ADAPTER_INFO    pAdapterInfo, 
            PULONG              pOutBufLen)
{
    DWORD dwResut = 0;
    __try
    {
        dwResut = GetAdaptersInfo(
                        pAdapterInfo,
                        pOutBufLen);
    }
    __except(KLDELAYLOAD_DL_CHECK())
    {
        dwResut = ERROR_INVALID_FUNCTION;
    };
    return dwResut;
};

DWORD WINAPI KLIPHLPWRAPPER_GetNetworkParams(
                    PFIXED_INFO pFixedInfo, 
                    PULONG      pOutBufLen)
{
    DWORD dwResut = 0;
    __try
    {
        dwResut = GetNetworkParams(
                        pFixedInfo,
                        pOutBufLen);
    }
    __except(KLDELAYLOAD_DL_CHECK())
    {
        dwResut = ERROR_INVALID_FUNCTION;
    };
    return dwResut;
};

#endif //_WIN32


#endif //__KL_IPHLP_WRAPPER_H__
