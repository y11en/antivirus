#ifndef __IPHLPAPI_WRAPPER_INCLUDED
#define __IPHLPAPI_WRAPPER_INCLUDED

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>

#include <Iptypes.h>
#include <Iprtrmib.h>

class CIPHlpAPI
{
	HMODULE m_hIpHlpAPI;
public:
	CIPHlpAPI();
	~CIPHlpAPI();
	bool IsInitialized() const;

	typedef DWORD (__stdcall *pfnGetAdaptersAddresses)( ULONG                           Family,
		DWORD                           Flags,
		PVOID                           Reserved,
		PIP_ADAPTER_ADDRESSES pAdapterAddresses, 
		PULONG                          pOutBufLen );

	typedef DWORD (__stdcall *pfnGetIpAddrTable)(   PMIB_IPADDRTABLE  pIpAddrTable,
		PULONG                      pdwSize,
		BOOL                        bOrder	);

	pfnGetAdaptersAddresses  m_pGetAdaptersAddresses;
	pfnGetIpAddrTable        m_pGetIpAddrTable;

	DWORD SafeGetAdaptersAddresses(ULONG Family,
									DWORD Flags,
									PVOID Reserved,
									PIP_ADAPTER_ADDRESSES pAdapterAddresses,
									PULONG pOutBufLen);
};

#endif // WIN32

#endif //__IPHLPAPI_WRAPPER_INCLUDED