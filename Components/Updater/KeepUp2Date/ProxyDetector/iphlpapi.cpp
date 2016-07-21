#include "iphlpapi.h"

#ifdef WIN32

#include <tchar.h>

CIPHlpAPI::CIPHlpAPI()
{
	if(m_hIpHlpAPI = ::LoadLibrary(_T("iphlpapi.dll")))
	{
		m_pGetAdaptersAddresses = (pfnGetAdaptersAddresses)::GetProcAddress(m_hIpHlpAPI, "GetAdaptersAddresses");
		m_pGetIpAddrTable = (pfnGetIpAddrTable)::GetProcAddress(m_hIpHlpAPI, "GetIpAddrTable");
		if(!m_pGetAdaptersAddresses || !m_pGetIpAddrTable)
		{
			m_pGetAdaptersAddresses = NULL;
			m_pGetIpAddrTable = NULL;
			FreeLibrary(m_hIpHlpAPI);
			m_hIpHlpAPI = NULL;
		}
	}
}

CIPHlpAPI::~CIPHlpAPI()
{
	if(m_hIpHlpAPI)
		FreeLibrary(m_hIpHlpAPI);
}

bool CIPHlpAPI::IsInitialized() const
{
	return m_hIpHlpAPI && m_pGetAdaptersAddresses && m_pGetIpAddrTable;
}

DWORD CIPHlpAPI::SafeGetAdaptersAddresses(ULONG Family,
							   DWORD Flags,
							   PVOID Reserved,
							   PIP_ADAPTER_ADDRESSES pAdapterAddresses,
							   PULONG pOutBufLen)
{
	DWORD RetVal = ERROR_INVALID_FUNCTION;
	if(m_pGetAdaptersAddresses)
	{
		__try
		{
			RetVal = m_pGetAdaptersAddresses(Family, Flags, Reserved, pAdapterAddresses, pOutBufLen);
		}
		__except(1)
		{
		}
	}
	return RetVal;
}


#endif // WIN32
