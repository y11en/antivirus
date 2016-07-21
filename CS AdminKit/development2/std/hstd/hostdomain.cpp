/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	hostdomain.cpp
 * \author	Andrew Kazachkov
 * \date	31.03.2003 13:27:20
 * \brief	
 * 
 */

#include "build/general.h"
#include "std/conv/klconv.h"
#include "std/base/klbase.h"
#include "std/err/error.h"
#include "std/err/klerrors.h"

#include "std/hstd/hostdomain.h"


#ifdef _WIN32
# define STRICT

# ifdef _WIN32_WINNT
#  undef _WIN32_WINNT
# endif

# ifdef WINVER
#  undef WINVER
# endif

# define _WIN32_WINNT	0x0500
# define WINVER			0x0500
# include <atlbase.h>
# include <ntsecapi.h>
#endif

#ifdef __unix__
#include <sys/types.h>
#include <sys/socket.h>
#endif



#define KLCS_MODULENAME L"KLSTD"

using namespace KLERR;
using namespace KLSTD;


#ifdef KLSTD_WINNT

	typedef BOOL (WINAPI *pGetComputerNameExW_t)(
												COMPUTER_NAME_FORMAT,
												LPWSTR,
												LPDWORD);

	KLCSC_DECL void KLSTD_GetComputerNameEx(
					IN COMPUTER_NAME_FORMAT NameType,
					OUT LPWSTR lpBuffer,
					IN OUT LPDWORD nSize)
	{
		HMODULE hLib=NULL;
		KLERR_TRY
			hLib=LoadLibrary(_T("kernel32.dll"));
			if(!hLib)
				KLSTD_THROW_LASTERROR();
			pGetComputerNameExW_t p=
					(pGetComputerNameExW_t)GetProcAddress(
													hLib,
													"GetComputerNameExW");
			if(!p)
				KLSTD_THROW_LASTERROR();
			if(!p(NameType, lpBuffer, nSize))
				KLSTD_THROW_LASTERROR();
		KLERR_CATCH(pError)
			KLERR_SAY_FAILURE(1, pError);
		KLERR_FINALLY
			if(hLib)
				FreeLibrary(hLib);
			KLERR_RETHROW();
		KLERR_ENDTRY
	};
#endif

#ifdef KLSTD_WINNT

    KLCSC_DECL KLSTD_NOTHROW void KLSTD_FreeDomainInfo(
            PPOLICY_PRIMARY_DOMAIN_INFO pPolicyPrimaryDomainInfo) throw()
    {
        if(pPolicyPrimaryDomainInfo)
            LsaFreeMemory(pPolicyPrimaryDomainInfo);
    };
    
    KLCSC_DECL void KLSTD_GetDomainNameAndSID(
                        PPOLICY_PRIMARY_DOMAIN_INFO&    pPolicyPrimaryDomainInfo,
                        KLSTD::KlDomainType&	        nDomainType)
    {
        KLSTD_CHK(pPolicyPrimaryDomainInfo, pPolicyPrimaryDomainInfo == NULL);
        ;
		LSA_HANDLE					hPolicyHandle = NULL;
		KLERR_TRY
			NTSTATUS				nResult=0;
			LSA_OBJECT_ATTRIBUTES	lsaObjectAttributes;
			KLSTD_ZEROSTRUCT(lsaObjectAttributes);		
			nResult = LsaOpenPolicy(
							NULL,
							&lsaObjectAttributes,
							POLICY_VIEW_LOCAL_INFORMATION,
							&hPolicyHandle);
			if(nResult != ERROR_SUCCESS)
				KLSTD_THROW_LASTERROR_CODE(LsaNtStatusToWinError(nResult));
			nResult = LsaQueryInformationPolicy(
							hPolicyHandle,
							PolicyPrimaryDomainInformation,
							(void**)&pPolicyPrimaryDomainInfo);
			if(nResult != ERROR_SUCCESS)
				KLSTD_THROW_LASTERROR_CODE(LsaNtStatusToWinError(nResult));
			KLSTD_ASSERT_THROW(
							pPolicyPrimaryDomainInfo &&
							pPolicyPrimaryDomainInfo->Name.Buffer &&
							pPolicyPrimaryDomainInfo->Name.Length > 0);
			nDomainType=(pPolicyPrimaryDomainInfo->Sid!=NULL) ?
							KLSTD::KLDT_WIN_DOMAIN : KLSTD::KLDT_WIN_WORKGROUP;
		KLERR_CATCH(pError)
			KLERR_SAY_FAILURE(1, pError);//Fatal: we unable to get domain name
		KLERR_FINALLY
			if(hPolicyHandle)
				LsaClose(hPolicyHandle);
			KLERR_RETHROW();
		KLERR_ENDTRY
    }

	static void GetDomainNameAndType(
							std::wstring&			wstrDomain,
							KLSTD::KlDomainType&	nDomainType)
	{
		PPOLICY_PRIMARY_DOMAIN_INFO	pPolicyPrimaryDomainInfo = NULL;
		KLERR_TRY
            KLSTD_GetDomainNameAndSID(pPolicyPrimaryDomainInfo, nDomainType);
			wstrDomain=pPolicyPrimaryDomainInfo->Name.Buffer;
		KLERR_CATCH(pError)
			KLERR_SAY_FAILURE(1, pError);//Fatal: we unable to get domain name
		KLERR_FINALLY
            KLSTD_FreeDomainInfo(pPolicyPrimaryDomainInfo);
			KLERR_RETHROW();
		KLERR_ENDTRY
	};

#elif defined(KLSTD_WIN9X)

	static bool ReadStringFromReg(
								HKEY			hParent,
								LPCTSTR			szKey,
								LPCTSTR			szName,
								std::wstring&	wstrValue)
	{
		bool bResult=true;
		USES_CONVERSION;	
		KLERR_TRY
			CRegKey	Key;
			long lResult=Key.Open(hParent, szKey, KEY_READ);
			if(lResult)KLSTD_THROW_LASTERROR_CODE(lResult);
			DWORD dwCount=0, dwType=0;
			lResult=RegQueryValueEx(
								Key.m_hKey,
								szName,
								NULL,
								NULL,
								NULL,
								&dwCount);
			if(lResult && !dwCount)KLSTD_THROW_LASTERROR_CODE(lResult);
			LPTSTR szValue=(LPTSTR)alloca(dwCount);
			lResult=RegQueryValueEx(
								Key.m_hKey,
								szName,
								NULL,
								&dwType,
								(PBYTE)szValue,
								&dwCount);
			if(lResult)KLSTD_THROW_LASTERROR_CODE(lResult);
			KLSTD_ASSERT_THROW(dwType==REG_SZ || dwType==REG_EXPAND_SZ);	
			wstrValue=T2W(szValue);
		KLERR_CATCH(pError)
			KLERR_SAY_FAILURE(3, pError);
			bResult=false;
		KLERR_FINALLY
			;
		KLERR_ENDTRY
		return bResult;
	};

	static void GetDomainNameAndType(
							std::wstring&			wstrDomain,
							KLSTD::KlDomainType&	nDomainType)
	{
		const TCHAR c_szKeyLogDom[]=
			_T("SYSTEM\\CurrentControlSet\\Services\\MSNP32\\NetworkProvider");

		const TCHAR c_szWorkgroupKey[]=
			_T("SYSTEM\\CurrentControlSet\\Services\\VxD\\VNETSUP");

		if(ReadStringFromReg(
							HKEY_LOCAL_MACHINE,
							c_szWorkgroupKey,
							_T("Workgroup"),
							wstrDomain))
			nDomainType=KLDT_WIN_WORKGROUP;
		else
		if(ReadStringFromReg(
							HKEY_LOCAL_MACHINE,
							c_szKeyLogDom,
							_T("AuthenticatingAgent"),
							wstrDomain))
			nDomainType=KLDT_WIN_DOMAIN;
		else
			KLSTD_NOINIT(L"Network");
	};
#endif

#ifdef _WIN32
	KLCSC_DECL void KLSTD_GetHostAndDomainName(
							std::wstring&			wstrHost,
							std::wstring*			pwstrDomain,
							KLSTD::KlDomainType*	pnDomainType)
	{
		KLSTD_USES_CONVERSION;
#ifdef KLSTD_WINNT
		DWORD dwVersion = GetVersion();
		bool bIsNT=((dwVersion & 0x80000000) == 0);
		bool bVersion4= ((DWORD)(LOBYTE(LOWORD(dwVersion))) <= 4);
		if(bVersion4)
#endif
		{//nt4 or 9x
			TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1]=_T("");	
			DWORD dwComputerName=KLSTD_COUNTOF(szComputerName);
			if(!GetComputerName(szComputerName, &dwComputerName))
				KLSTD_THROW_LASTERROR();
			wstrHost=KLSTD_T2W(szComputerName);
		}
#ifdef KLSTD_WINNT
		else
		{
			TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1]=_T("");	
			DWORD dwComputerName=KLSTD_COUNTOF(szComputerName);
			KLSTD_GetComputerNameEx(
							ComputerNamePhysicalNetBIOS,
							szComputerName,
							&dwComputerName);
			wstrHost=KLSTD_T2W(szComputerName);
		};
#endif
		if(pwstrDomain || pwstrDomain)
		{
			KLSTD::KlDomainType nDomainType;
			std::wstring wstrDomain;

			GetDomainNameAndType(wstrDomain, nDomainType);
			if(pnDomainType)
				*pnDomainType=nDomainType;
			if(pwstrDomain)
				*pwstrDomain=wstrDomain;
		};
	};

#else
	KLCSC_DECL void KLSTD_GetHostAndDomainName(
						std::wstring&			wstrHost,
						std::wstring*			pwstrDomain,
						KLSTD::KlDomainType*	pnDomainType)
	{
//	#error "Not implemented";
#ifdef _WIN32
      int s = socket(AF_INET, SOCK_DGRAM, 0);

      if ( s < 0 ) {
        return; // fail
      }

      struct ifconf ifc;
      struct ifreq ifrs[100];

      ifc.ifc_len = sizeof( ifrs );
      ifc.ifc_buf = &ifrs[0];

      if ( ioctl( s, SIOCGIFCONF, &ifc ) != 0 ) {
        close(s);
        return; // fail
      }

      ifr = ifc.ifc_req;

      size_t n = ifc.ifc_len / sizeof(struct ifreq);

      /* Loop through interfaces, looking for given IP address */
      for (i=n-1;i>=0 && total < max_interfaces;i--) {
        if (ioctl(fd, SIOCGIFADDR, &ifr[i]) != 0) {
          continue;
        }

        iname = ifr[i].ifr_name;
        ipaddr = (*(struct sockaddr_in *)&ifr[i].ifr_addr).sin_addr;

        if (ioctl(fd, SIOCGIFFLAGS, &ifr[i]) != 0) {
          continue;
        }

        if (!(ifr[i].ifr_flags & IFF_UP)) {
          continue;
        }

        if (ioctl(fd, SIOCGIFNETMASK, &ifr[i]) != 0) {
          continue;
        }

        nmask = ((struct sockaddr_in *)&ifr[i].ifr_addr)->sin_addr;

        strncpy(ifaces[total].name, iname, sizeof(ifaces[total].name)-1);
        ifaces[total].name[sizeof(ifaces[total].name)-1] = 0;
        ifaces[total].ip = ipaddr;
        ifaces[total].netmask = nmask;
        total++;
      }
      
      close(s);
#endif
      
	};
#endif
