/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file WNF/WindowsNetInfo.cpp
 * \author Андрей Брыксин
 * \date 2002
 * \brief Интерфейс для получения информации о Windows - сети.
 *
 */

#include <std/win32/klos_win32v50.h>
#include <build/general.h>
#include <std/base/klstd.h>
#include <std/base/klbase.h>
#include <std/par/errors.h>
#include <std/wnf/windowsnetinfo.h>
#include <Winnetwk.h>
#include "netenum.h"
#include "klnetinfo.h"
#include <atlbase.h>
#include <Iads.h>
#include <Adshlp.h>
#include <Dsgetdc.h>

#pragma comment(lib, "netapi32.lib")

namespace KLWNF {

#define KLCS_MODULENAME L"KLWNF"

///////////////////////////////////////////////////////////////////////////

	void _GetADsObject(LPCWSTR pszObjectName, REFIID riid, void** ppIObject)
	{
		HRESULT hr = E_FAIL;

		__try
		{
			hr = ADsGetObject(pszObjectName, riid, ppIObject); 
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			KLSTD_TRACE0(3, L"_GetADsObject - Error loadind 'ActiveDS.dll'\n");
			KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_UNAVAIL);
		}
		if (FAILED(hr))
		{	
			KLSTD_TRACE2(3, L"_GetADsObject - Error retrieving '%ls' object, HRESULT: %u\n", pszObjectName, hr);
			KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_NOTFOUND);
		}
	}

///////////////////////////////////////////////////////////////////////////

class CEnumDomainParams
{
public:
	OBJ_LIST* m_pDomains;
	bool m_bEnumDomains;					// true - domains, false - workgroups (no PDC)

	CEnumDomainParams(OBJ_LIST* pDomains, bool bEnumDomains)
	{
		m_pDomains = pDomains;
		m_bEnumDomains = bEnumDomains;
	}
};

class CWindowsNetInfo : public KLSTD::KLBaseImpl<WindowsNetInfo>
{
public:
	CWindowsNetInfo(bool volatile* pbCancelEnumeration = NULL):
		m_hNetApiDll(LoadLibrary(TEXT("netapi32.dll"))),
		m_fnDsGetDcName(NULL),
		m_pbCancelEnumeration(pbCancelEnumeration)
	{
		KLSTD_ASSERT(m_hNetApiDll);
		if (m_hNetApiDll)
		{
#ifdef UNICODE
			m_fnDsGetDcName = (fnDsGetDcName)GetProcAddress(m_hNetApiDll, "DsGetDcNameW");
#else
			m_fnDsGetDcName = (fnDsGetDcName)GetProcAddress(m_hNetApiDll, "DsGetDcNameA");
#endif // UNICODE
		}
	}
	~CWindowsNetInfo()
	{
		if (m_hNetApiDll) FreeLibrary(m_hNetApiDll);
	}
    virtual void GetWorkgroups (OBJ_LIST& workgroups)
	{
		CEnumDomainParams NEP(&workgroups, false);
		CNetEnum<CWindowsNetInfo> neNetworkEnum(false);

		neNetworkEnum.Create(this, OnEnumDomains, (DWORD)&NEP);
		neNetworkEnum.Enumerate();
	}

    virtual void GetDomains (OBJ_LIST& domains)
	{
		CEnumDomainParams NEP(&domains, true);
		CNetEnum<CWindowsNetInfo> neNetworkEnum(false);

		neNetworkEnum.Create(this, OnEnumDomains, (DWORD)&NEP);
		neNetworkEnum.Enumerate();
	}

	virtual void GetDomainWorkstations(const std::wstring& domain, OBJ_LIST& workstations)
	{
		CNetDomain Domain(domain.c_str());
		Domain.GetComputers(workstations);
	}

	virtual void GetDomainWorkstations2(const std::wstring& domain, OBJ_LIST2000& workstations)
	{
		CNetDomain Domain(domain.c_str());
		Domain.GetComputers(workstations);
	}

	virtual void GetADDomainWorkstations(const std::wstring& domain, OBJ_LIST2000& workstations)
	{
		PDOMAIN_CONTROLLER_INFO pDomainControllerInfo = NULL;
		if (_GetDsDCInfo(domain.c_str(), &pDomainControllerInfo) == NO_ERROR)
		{
			std::wstring wstrServerName = L"LDAP://";
			wstrServerName += pDomainControllerInfo->DomainName;
			NetApiBufferFree(pDomainControllerInfo);

			CComPtr<IDirectorySearch> pISearch;
			_GetADsObject(wstrServerName.c_str(), __uuidof(IDirectorySearch), (void**)&pISearch);

			ADS_SEARCHPREF_INFO prefInfo[2];
			prefInfo[0].dwSearchPref = ADS_SEARCHPREF_SEARCH_SCOPE;
			prefInfo[0].vValue.dwType = ADSTYPE_INTEGER;
			prefInfo[0].vValue.Integer = ADS_SCOPE_SUBTREE;
			prefInfo[1].dwSearchPref = ADS_SEARCHPREF_PAGESIZE;
			prefInfo[1].vValue.dwType = ADSTYPE_INTEGER;
			prefInfo[1].vValue.Integer = 99;
			pISearch->SetSearchPreference(prefInfo, KLSTD_COUNTOF(prefInfo));

			HRESULT hr;
			ADS_SEARCH_HANDLE hSearch = NULL;
			LPCWSTR pszAttr[]={
				L"name",
				L"dNSHostName"
			};
			int nColumns = KLSTD_COUNTOF(pszAttr);
            if (SUCCEEDED(hr = pISearch->ExecuteSearch(L"(&(objectClass=computer)(userAccountControl:1.2.840.113556.1.4.803:=0))", (LPWSTR*)&pszAttr[0], nColumns, &hSearch)))
			{
				int nItem = 0;
				ComputerInfo2000 ci2000;
				while( pISearch->GetNextRow(hSearch) == S_OK ) //!= 0x00005012L)
				{
					ADS_SEARCH_COLUMN col = {0};
					if (SUCCEEDED(hr = pISearch->GetColumn(hSearch, (LPWSTR)pszAttr[0], &col)) &&
						(col.pADsValues->dwType == ADSTYPE_CASE_EXACT_STRING ||
						col.pADsValues->dwType == ADSTYPE_CASE_IGNORE_STRING))
					{
						ci2000.name = (LPWSTR)col.pADsValues->CaseIgnoreString;
						pISearch->FreeColumn(&col);

						if (SUCCEEDED(pISearch->GetColumn(hSearch, (LPWSTR)pszAttr[1], &col)) &&
							(col.pADsValues->dwType == ADSTYPE_CASE_EXACT_STRING ||
							col.pADsValues->dwType == ADSTYPE_CASE_IGNORE_STRING))
						{
							ci2000.fullDnsName = (LPWSTR)col.pADsValues->CaseIgnoreString;
							pISearch->FreeColumn(&col);

							workstations.push_back(ci2000);
						}
						else
							KLSTD_TRACE1(3, L"EnumDomainHosts::pISearch->GetColumn(2) - Failed, HRESULT: %u\n", hr);
					}
					else
						KLSTD_TRACE1(3, L"EnumDomainHosts::pISearch->GetColumn(1) - Failed, HRESULT: %u\n", hr);
				}
				pISearch->CloseSearchHandle(hSearch);
			}
			else
			{
				KLSTD_TRACE1(3, L"EnumDomainHosts::pISearch->ExecuteSearch - Failed, HRESULT: %u\n", hr);
				KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_FAULT);
			}
		}
		else
			KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_FAULT);
	}

    virtual void GetWorkgroupComputers(const std::wstring& workgroup, OBJ_LIST& computers)
	{
		CNetEnum<CWindowsNetInfo> neNetworkEnum(false);

		neNetworkEnum.Create(this, OnEnumComputers, (DWORD)&computers);

		NETRESOURCE nr;
		nr.lpRemoteName = (wchar_t*)workgroup.c_str();
		nr.dwDisplayType = RESOURCEDISPLAYTYPE_DOMAIN;
		nr.dwUsage = RESOURCEUSAGE_CONTAINER;
		nr.dwScope = RESOURCE_GLOBALNET;
		DWORD dwSize = MAX_PATH;
		TCHAR szProviderName[MAX_PATH];
		DWORD dwErrorCode = WNetGetProviderName(WNNC_NET_LANMAN, szProviderName, &dwSize);
		if (dwErrorCode == NO_ERROR)
			nr.lpProvider = szProviderName;
		else
			nr.lpProvider = NULL;

		neNetworkEnum.Enumerate(&nr, CNetEnumBase::SEARCHDEFAULT, RESOURCEUSAGE_CONTAINER);
	}

    virtual void GetWorkgroupComputers2(const std::wstring& workgroup, OBJ_LIST2000& computers)
	{
		CNetEnum<CWindowsNetInfo> neNetworkEnum(false);

		neNetworkEnum.Create(this, OnEnumComputers2, (DWORD)&computers);

		NETRESOURCE nr;
		nr.lpRemoteName = (wchar_t*)workgroup.c_str();
		nr.dwDisplayType = RESOURCEDISPLAYTYPE_DOMAIN;
		nr.dwUsage = RESOURCEUSAGE_CONTAINER;
		nr.dwScope = RESOURCE_GLOBALNET;
		DWORD dwSize = MAX_PATH;
		TCHAR szProviderName[MAX_PATH];
		DWORD dwErrorCode = WNetGetProviderName(WNNC_NET_LANMAN, szProviderName, &dwSize);
		if (dwErrorCode == NO_ERROR)
			nr.lpProvider = szProviderName;
		else
			nr.lpProvider = NULL;

		neNetworkEnum.Enumerate(&nr, CNetEnumBase::SEARCHDEFAULT, RESOURCEUSAGE_CONTAINER);
	}
	
    virtual void GetDomainUsers(const std::wstring& sDomainName, OBJ_LIST& users, bool bIncludeDisabledAccounts)
	{
		std::wstring sUserName;
		bool bAccountDisabled;

		CNetDomain Domain(sDomainName.c_str());
		CNetUsers NetUsers(Domain);

		NetUsers.StartQueryNextUser();
		while(NetUsers.QueryNextUser(sUserName, bAccountDisabled))
		{
			if (bIncludeDisabledAccounts || !bAccountDisabled) users.push_back(sUserName);
		}
	}

	virtual void GetComputerInfo(const std::wstring& workgroup, const std::wstring& name, ComputerInfo& info)
	{
		DWORD dwLevel = 101;
		LPSERVER_INFO_101 pBuf = NULL;

		NET_API_STATUS nStatus = NetServerGetInfo(const_cast<wchar_t*>(name.c_str()), dwLevel, (LPBYTE*)&pBuf);
		if (nStatus == NERR_Success)
		{
			info.domain = workgroup;
			info.name = pBuf->sv101_name;
			info.versionMajor = pBuf->sv101_version_major;
			info.versionMinor = pBuf->sv101_version_minor;
			
			switch (pBuf->sv101_platform_id) {
				case PLATFORM_ID_DOS:
					info.ptype = P_DOS;
				break;
				case PLATFORM_ID_OS2:
					info.ptype = P_OS2;
				break;
				case PLATFORM_ID_NT:
					info.ptype = P_NT;
				break;
				case PLATFORM_ID_OSF:
					info.ptype = P_OSF;
				break;
				case PLATFORM_ID_VMS:
					info.ptype = P_VMS;
				break;
				default:
					info.ptype = P_DOS;
			}

			info.ctype = (ComputerType)pBuf->sv101_type;

			NetApiBufferFree(pBuf);
		}
		else
			KLSTD_THROW_LASTERROR_CODE(nStatus);
	}
	
    virtual void GetUserInfo(const std::wstring& sDomainName, const std::wstring& name, UserInfo& info)
	{
		CNetDomain Domain(sDomainName.c_str());
		CNetUsers Users(Domain);

		CNetUserInfo ui;
		Users.GetUserInfo(name.c_str(), &ui);

		info.domain = sDomainName;
		info.name = name;
		
		switch(ui.priv) {
			case USER_PRIV_GUEST:
				info.pLevel = PL_GUEST;
			break;
			case USER_PRIV_ADMIN:
				info.pLevel = PL_ADMINISTRATOR;
			break;
			case USER_PRIV_USER:
				info.pLevel = PL_USER;
			break;
			default:
				info.pLevel = PL_GUEST;
		}

		info.lastLogon = ui.last_logon;
		info.lastLogoff = ui.last_logoff;
	}

    virtual void GetLoggedInUsers(const std::wstring& ComputerName, OBJ_LIST& loggedInUsers, OBJ_LIST& domains)
	{
		LPWKSTA_USER_INFO_1 pBuf = NULL;
		LPWKSTA_USER_INFO_1 pTmpBuf;
		DWORD dwLevel = 1;
		DWORD dwPrefMaxLen = -1;
		DWORD dwEntriesRead = 0;
		DWORD dwTotalEntries = 0;
		DWORD dwResumeHandle = 0;
		NET_API_STATUS nStatus;

		do
		{
			nStatus = NetWkstaUserEnum((wchar_t*)ComputerName.c_str(), dwLevel, (LPBYTE*)&pBuf, dwPrefMaxLen, &dwEntriesRead, &dwTotalEntries, &dwResumeHandle);
			if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
			{
				pTmpBuf = pBuf;
				
				// Loop through the entries.
				for (DWORD i = 0; (i < dwEntriesRead); i++)
				{
					loggedInUsers.push_back(pTmpBuf->wkui1_username);
					domains.push_back(pTmpBuf->wkui1_logon_domain);

					pTmpBuf++;
				}

				NetApiBufferFree(pBuf);
			}
			else
				KLSTD_THROW_LASTERROR_CODE(nStatus);
		}
		while (nStatus == ERROR_MORE_DATA);
	}

    virtual void GetUserGroups(const std::wstring& sDomainName, const std::wstring& name, OBJ_LIST& groups)
	{
		CNetDomain Domain(sDomainName.c_str());
		CNetUsers Users(Domain);

		Users.GetUserGroups(name.c_str(), groups);
	}

    virtual void GetADsOrganizationalUnits(const ADObjectInfo& parentAdObject, AD_OBJ_LIST & OUs, bool bIncludeContainers = false)
	{
		EnumADSObjects((ADObjectHandle*)(KLSTD::KLBase*)parentAdObject.handle, OUs, false, bIncludeContainers);
	}

    virtual void GetADsOrganizationalUnitComputers(const ADObjectInfo& parentAdObject, AD_OBJ_LIST & OUComps)
	{
		EnumADSObjects((ADObjectHandle*)(KLSTD::KLBase*)parentAdObject.handle, OUComps, true);
	}

	virtual std::wstring DistDomainNameToNT4Name(const std::wstring& domain)
	{
		KLSTD_CHKINPTR(m_fnDsGetDcName);

		std::wstring wstrDNSDomain(domain.begin() + 3, domain.end());

		size_t nPos = wstrDNSDomain.find(L",DC=");
		while(nPos != std::wstring::npos)
		{
			wstrDNSDomain.replace(nPos, 4, L".");
			nPos = wstrDNSDomain.find(L",DC=", nPos + 1);
		}

		PDOMAIN_CONTROLLER_INFO pDomainControllerInfo = NULL;
		DWORD dwResult = m_fnDsGetDcName(NULL, KLSTD_W2T2(wstrDNSDomain.c_str()), NULL, NULL, 
										 DS_DIRECTORY_SERVICE_REQUIRED | DS_IS_DNS_NAME | DS_RETURN_FLAT_NAME,
										 &pDomainControllerInfo);
		if (dwResult != NO_ERROR)
		{
			KLSTD_TRACE3(1, L"DistDomainNameToNT4Name(DN:'%ls', DNS:'%ls')::DsGetDcName - Error '%u'\n", domain.c_str(), wstrDNSDomain.c_str(), dwResult);
			KLSTD_THROW(KLSTD::STDE_FAULT);
		}
		
		std::wstring rc = KLSTD_T2W2(pDomainControllerInfo->DomainName);
		NetApiBufferFree(pDomainControllerInfo);
		return rc;
	}
	
// Implementation
protected:
	typedef DWORD (WINAPI *fnDsGetDcName)(LPCTSTR ComputerName, 
										  LPCTSTR DomainName, 
										  GUID* DomainGuid, 
										  LPCTSTR SiteName, 
										  ULONG Flags, 
										  PDOMAIN_CONTROLLER_INFO* DomainControllerInfo);
	fnDsGetDcName m_fnDsGetDcName;
	HMODULE m_hNetApiDll;
	bool volatile* m_pbCancelEnumeration;

	class ADObjectHandle : public KLSTD::KLBaseImpl<KLSTD::KLBase>
	{
	public:
		CComPtr<IUnknown> m_pIAD;

		ADObjectHandle(IUnknown* pIAD){
			KLSTD_CHKINPTR(pIAD);
			m_pIAD = pIAD;
		}
	};

	void _GetADsRootObject(IADsContainer** ppIContainer)
	{
		KLSTD_CHKOUTPTR(ppIContainer);

		CComPtr<IADs> pIRootDSE;
		_GetADsObject(L"LDAP://RootDSE", __uuidof(IADs), (void**)&pIRootDSE);
		
		_GetADsObject(L"LDAP:", __uuidof(IADsContainer), (void**)ppIContainer);
	}

	DWORD _GetDsDCInfo(LPCTSTR pszDomain, PDOMAIN_CONTROLLER_INFO* ppDomainControllerInfo)
	{
		KLSTD_CHKINPTR(m_fnDsGetDcName);
		return m_fnDsGetDcName(NULL, pszDomain, NULL, NULL, 
							   DS_DIRECTORY_SERVICE_REQUIRED | DS_IS_FLAT_NAME | DS_RETURN_DNS_NAME,
							   ppDomainControllerInfo);
	}

	bool _GetADObjectInfo(IADs* pIADSItem, ADObjectInfo& ADObject, bool bComputers)
	{
		KLSTD_CHKMEM(pIADSItem);

		HRESULT hr;

		CComVariant VarPropertiesToCache;
		LPWSTR apszPropertiesToCache[] = { L"showInAdvancedViewOnly", L"name", L"sAMAccountName", L"dNSHostName", L"distinguishedName" };
		if (FAILED(hr = ADsBuildVarArrayStr(apszPropertiesToCache, KLSTD_COUNTOF(apszPropertiesToCache), &VarPropertiesToCache)))
		{
			KLSTD_TRACE1(3, L"_GetADObjectInfo - ADsBuildVarArrayStr(apszPropertiesToCache) FAILED, HRESULT: %u", hr);
			KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_FAULT);
		}

		// Cache required properties to avoid loading of entire list
		if (FAILED(hr = pIADSItem->GetInfoEx(VarPropertiesToCache, 0)))
		{
			KLSTD_TRACE1(3, L"EnumADSObjects - pADs->GetInfoEx() : Cannot cache properties, HRESULT: %u", hr);
			KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_FAULT);
		}

		CComBSTR bstrGUID;
		CComVariant var;
		if (SUCCEEDED(pIADSItem->Get(L"name", &var)) &&
			SUCCEEDED(pIADSItem->get_GUID(&bstrGUID)) &&
			var.vt == VT_BSTR &&
			bstrGUID.Length() > 0)
		{
			CComVariant varAdvanced;
			if (SUCCEEDED(pIADSItem->Get(L"showInAdvancedViewOnly", &varAdvanced)) &&
				varAdvanced.vt == VT_BOOL &&
				varAdvanced.boolVal)
			{
				return false;	// Exclude advanced view objects
			}

			ADObject.name = var.bstrVal;
			ADObject.guid = bstrGUID;

			if (bComputers) 
			{
				var.Clear();
				if (SUCCEEDED(pIADSItem->Get(L"sAMAccountName", &var)) && var.vt == VT_BSTR)
					ADObject.accountName = var.bstrVal;
				 
				var.Clear();
				if (SUCCEEDED(pIADSItem->Get(L"dNSHostName", &var)) && var.vt == VT_BSTR)
					ADObject.dNSHostName = var.bstrVal;

				var.Clear();
				if (SUCCEEDED(pIADSItem->Get(L"distinguishedName", &var)) && var.vt == VT_BSTR)
				{
					std::wstring wstr;
					wstr = var.bstrVal;

					size_t nPos = wstr.find(L"DC=");
					if (nPos != std::wstring::npos)
						ADObject.domainDistName.assign(wstr.begin() + nPos, wstr.end());
					else
						KLSTD_TRACE1(3, L"EnumADSObjects - distinguishedName (No DC found for '%ls')", ADObject.dNSHostName);
				}
			}
			else
			{
				var.Clear();
				if (SUCCEEDED(pIADSItem->Get(L"distinguishedName", &var)) && var.vt == VT_BSTR)
				{
                    ADObject.distinguishedName = KLSTD::FixNullString(var.bstrVal);
				}

				ADObject.handle.Attach(new ADObjectHandle(pIADSItem));
				KLSTD_CHKMEM(ADObject.handle);
			}
		}
		else
			KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_UNAVAIL);

		return true;
	}

	void EnumADSObjects(ADObjectHandle* pParentADObject, 
						AD_OBJ_LIST & Objects, 
						const bool bComputers, 
						bool bIncludeContainers = false)
	{
		const ULONG CHUNK_SIZE = 500;

		CComPtr<IADsContainer> pIContainer;
		if (!pParentADObject || !pParentADObject->m_pIAD)
			_GetADsRootObject(&pIContainer);
		else
			pIContainer = CComQIPtr<IADsContainer>(pParentADObject->m_pIAD);
 
		KLSTD_ASSERT(pIContainer);
		if (!pIContainer) 
		{
			KLSTD_TRACE0(3, L"EnumADSObjects - pIContainer == NULL");
			KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_NOTFOUND);
		}

		bool _bIncludeContainers = bIncludeContainers && !bComputers;
		LPWSTR apszTypes[] = {L"organizationalUnit", L"computer", L"Container"};
		LPWSTR apszFilterTypes[] = {NULL, NULL, NULL};
		apszFilterTypes[0] = apszTypes[bComputers?1:0];
		if (_bIncludeContainers)
			apszFilterTypes[1] = apszTypes[2];

		HRESULT hr;
		CComVariant VarFilter;
		if (FAILED(hr = ADsBuildVarArrayStr(apszFilterTypes, _bIncludeContainers?2:1, &VarFilter)) ||
			FAILED(hr = pIContainer->put_Filter(VarFilter)))
		{
			KLSTD_TRACE1(3, L"EnumADSObjects - Cannot apply filter, HRESULT: %u", hr);
			KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_FAULT);
		}

		//Create an enumerator object in the container.
		IEnumVARIANT* pIEnumVar = NULL;
		if (FAILED(hr = ADsBuildEnumerator(pIContainer, &pIEnumVar)))
		{
			KLSTD_TRACE1(3, L"EnumADSObjects - ADsBuildEnumerator failed, HRESULT: %u", hr);
			KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_FAULT);
		}
		KLERR_TRY
		{
			while(SUCCEEDED(hr)) 
			{
				//Get the next contained object.
				ULONG ulFetch = 0L;
				CComVariant var[CHUNK_SIZE];
				if (FAILED(hr = ADsEnumerateNext(pIEnumVar, CHUNK_SIZE, var, &ulFetch)))
				{
					KLSTD_TRACE1(3, L"EnumADSObjects - ADsEnumerateNext() failed, HRESULT: %u", hr);
					KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_FAULT);
				}
				if (ulFetch == 0) break;

				for (ULONG i = 0;i < ulFetch;++i)
				{
					//Check if the retrieved object is another container.
					CComPtr<IUnknown> pIChildUnk;
					hr = V_DISPATCH(var + i)->QueryInterface(IID_IUnknown, (void**)&pIChildUnk);
					if (pIChildUnk)
					{
						ADObjectInfo ADObject;
						if (_GetADObjectInfo(CComQIPtr<IADs>(pIChildUnk), ADObject, bComputers))
							Objects.push_back(ADObject);

					}
					else
					{
						KLSTD_TRACE1(3, L"EnumADSObjects - V_DISPATCH(&var)->QueryInterface() failed, HRESULT: %u", hr);
						KLERR_THROW0(KLCS_MODULENAME, KLSTD::STDE_FAULT);
					}
				}
			}

			if (pIEnumVar) ADsFreeEnumerator(pIEnumVar);	//Release the enumerator.
		}
		KLERR_CATCH(pError)
		{
			if (pIEnumVar) ADsFreeEnumerator(pIEnumVar);	//Release the enumerator.
			KLERR_RETHROW();
		}
		KLERR_ENDTRY
	}
	
	bool OnEnumDomains(CNetEnum<CWindowsNetInfo>* pNetworkEnum, NETRESOURCE &rc, DWORD dwParam)
	{
		KLSTD_CHKINPTR(pNetworkEnum);
		
		CEnumDomainParams* pNEP = (CEnumDomainParams*)dwParam;
		KLSTD_CHKINPTR(pNEP);
		KLSTD_CHKINPTR(pNEP->m_pDomains);

		if (m_pbCancelEnumeration && *m_pbCancelEnumeration) return false;

		if (pNetworkEnum->IsDomain(rc))
		{
			CNetDomain DomainInfo(rc.lpRemoteName);
			if (pNEP->m_bEnumDomains)
			{
				if (DomainInfo.IsDomain()) 
					pNEP->m_pDomains->push_back(rc.lpRemoteName);
			}
			else
			{
				if (!DomainInfo.IsDomain()) 
					pNEP->m_pDomains->push_back(rc.lpRemoteName);
			}
		}

		return true;
	}

	bool OnEnumComputers(CNetEnum<CWindowsNetInfo>* pNetworkEnum, NETRESOURCE &rc, DWORD dwParam)
	{
		KLSTD_CHKINPTR(pNetworkEnum);
		OBJ_LIST* pComputers = (OBJ_LIST*)dwParam;
		KLSTD_CHKINPTR(pComputers);

		if (m_pbCancelEnumeration && *m_pbCancelEnumeration) return false;

		if (pNetworkEnum->IsServer(rc) && rc.lpRemoteName)
        {
            LPCWSTR szHost=rc.lpRemoteName;
            if(szHost[0] == L'\\' && szHost[1] == L'\\') szHost = &szHost[2];
            pComputers->push_back(szHost);
        }

		return true;
	}
	bool OnEnumComputers2(CNetEnum<CWindowsNetInfo>* pNetworkEnum, NETRESOURCE &rc, DWORD dwParam)
	{
		KLSTD_CHKINPTR(pNetworkEnum);
		OBJ_LIST2000* pComputers = (OBJ_LIST2000*)dwParam;
		KLSTD_CHKINPTR(pComputers);

		if (m_pbCancelEnumeration && *m_pbCancelEnumeration) return false;

		if (pNetworkEnum->IsServer(rc) && rc.lpRemoteName)
        {
            LPCWSTR szHost=rc.lpRemoteName;
            if(szHost[0] == L'\\' && szHost[1] == L'\\') szHost = &szHost[2];
            pComputers->push_back(ComputerInfo2000(szHost));
        }

		return true;
	}
};
    
}

/*!
  \brief Создает объект класса WindowsNetInfo

    \param ppWindowsNetInfo      [out] Указатель на объект класса WindowsNetInfo
*/
void KLWNF_CreateWindowsNetInfo(KLWNF::WindowsNetInfo ** ppWindowsNetInfo, bool volatile* pbCancelEnumeration)
{
	KLSTD_CHKOUTPTR(ppWindowsNetInfo);

	*ppWindowsNetInfo = new KLWNF::CWindowsNetInfo(pbCancelEnumeration);
	KLSTD_CHKMEM(*ppWindowsNetInfo);
}
