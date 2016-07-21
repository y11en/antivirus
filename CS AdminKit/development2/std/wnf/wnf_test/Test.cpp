// Test.cpp: implementation of the CTest class.
//
//////////////////////////////////////////////////////////////////////

#include <std/base/klstd.h>

#ifdef _WIN32
	#include <crtdbg.h>
#endif

#include "Test.h"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLWNF;

//////////////////////////////////////////////////////////////////////
// CTest
//////////////////////////////////////////////////////////////////////

CTest::CTest()
{
}

void CTest::GetADsStructure(ADObjectInfo parentAdObject, WindowsNetInfo* pWindowsNetInfo, std::wstring sTraceIndent)
{
	AD_OBJ_LIST ous;
	pWindowsNetInfo->GetADsOrganizationalUnits(parentAdObject, ous);
	for(AD_OBJ_LIST::iterator i = ous.begin();i != ous.end();i++)
	{
		KLSTD_TRACE1(1, std::wstring(sTraceIndent + L"OU: %ls\n").c_str(), i->name.c_str());
		// Enum children
		GetADsStructure(*i, pWindowsNetInfo, sTraceIndent + L"\t");
	}
	AD_OBJ_LIST wks;
	pWindowsNetInfo->GetADsOrganizationalUnitComputers(parentAdObject, wks);
	for(AD_OBJ_LIST::iterator j = wks.begin();j != wks.end();j++)
	{
		KLSTD_TRACE1(1, std::wstring(sTraceIndent + L"WKS: %ls\n").c_str(), j->name.c_str());
	}
}

void CTest::GetDomains(WindowsNetInfo* pWindowsNetInfo, OBJ_LIST& arDomains)
{
	pWindowsNetInfo->GetDomains(arDomains);
	if (arDomains.size() == 0) KLSTD_THROW(STDE_BADFORMAT);

	for(OBJ_LIST::iterator i = arDomains.begin();i != arDomains.end();i++)
	{
		std::wstring s = *i;
	}
}

void CTest::GetWorkgroups(WindowsNetInfo* pWindowsNetInfo, OBJ_LIST& arWorkgroups)
{
	pWindowsNetInfo->GetWorkgroups(arWorkgroups);
//	if (arWorkgroups.size() == 0) KLSTD_THROW(STDE_BADFORMAT);

	for(OBJ_LIST::iterator i = arWorkgroups.begin();i != arWorkgroups.end();i++)
	{
		std::wstring s = *i;
	}
}

void CTest::GetDomainWorkstations(std::wstring sDomain, WindowsNetInfo* pWindowsNetInfo, OBJ_LIST2000& arWorkstations)
{
	pWindowsNetInfo->GetDomainWorkstations2(sDomain, arWorkstations);

	for(OBJ_LIST2000::iterator i = arWorkstations.begin();i != arWorkstations.end();i++)
	{
		std::wstring s = (*i).name;
		std::wstring sDNSName = (*i).fullDnsName;
	}
}

void CTest::GetADDomainWorkstations(std::wstring sDomain, WindowsNetInfo* pWindowsNetInfo, OBJ_LIST2000& arWorkstations)
{
	pWindowsNetInfo->GetADDomainWorkstations(sDomain, arWorkstations);

	for(OBJ_LIST2000::iterator i = arWorkstations.begin();i != arWorkstations.end();i++)
	{
		std::wstring s = (*i).name;
		std::wstring sDNSName = (*i).fullDnsName;
	}
}

void CTest::GetWorkgroupComputers(std::wstring sDomain, WindowsNetInfo* pWindowsNetInfo, OBJ_LIST& arWorkstations)
{
	pWindowsNetInfo->GetWorkgroupComputers(sDomain, arWorkstations);
//	if (arWorkstations.size() < 10) KLSTD_THROW(STDE_BADFORMAT);

	for(OBJ_LIST::iterator i = arWorkstations.begin();i != arWorkstations.end();i++)
	{
		std::wstring s = *i;
	}
}

void CTest::GetDomainUsers(std::wstring sDomain, WindowsNetInfo* pWindowsNetInfo, OBJ_LIST& arUsers)
{
	pWindowsNetInfo->GetDomainUsers(sDomain, arUsers, false);
	if (arUsers.size() < 10) KLSTD_THROW(STDE_BADFORMAT);

	for(OBJ_LIST::iterator i = arUsers.begin();i != arUsers.end();i++)
	{
		std::wstring s = *i;
	}
}

void CTest::GetComputerInfo(std::wstring sComputer, WindowsNetInfo* pWindowsNetInfo)
{
	ComputerInfo info;

	pWindowsNetInfo->GetComputerInfo(std::wstring(L""), sComputer, info);
}

void CTest::GetUserInfo(std::wstring sDomain, std::wstring sUserName, WindowsNetInfo* pWindowsNetInfo)
{
	UserInfo ui;
	pWindowsNetInfo->GetUserInfo(sDomain, sUserName, ui);
}

void CTest::GetLoggedInUsers(std::wstring sComputer, WindowsNetInfo* pWindowsNetInfo)
{
	OBJ_LIST arUsers, arDomains;

	pWindowsNetInfo->GetLoggedInUsers(sComputer, arUsers, arDomains);
	if (arUsers.size() < 1) KLSTD_THROW(STDE_BADFORMAT);

	for(OBJ_LIST::iterator i = arDomains.begin();i != arDomains.end();i++)
	{
		std::wstring s = *i;
	}
	for(OBJ_LIST::iterator j = arUsers.begin();j != arUsers.end();j++)
	{
		std::wstring s = *j;
	}
}

void CTest::GetUserGroups(std::wstring sDomain, std::wstring sUser, WindowsNetInfo* pWindowsNetInfo)
{
	OBJ_LIST arGroups;

	pWindowsNetInfo->GetUserGroups(sDomain, sUser, arGroups);
	if (arGroups.size() == 0) KLSTD_THROW(STDE_BADFORMAT);

	for(OBJ_LIST::iterator i = arGroups.begin();i != arGroups.end();i++)
	{
		std::wstring s = *i;
	}
}

bool CTest::Run()
{
	bool bResult = true;

	// Perform test
	OBJ_LIST arDomains;
	OBJ_LIST arWorkgroups;
	OBJ_LIST arWorkstations;
	OBJ_LIST2000 arWorkstations2;
	OBJ_LIST arBrowsedWorkstations;
	OBJ_LIST arUsers;

//	ComputerInfo ci;
//	KLWNF_GetComputerInfo(L"", ci);

	bool bCancel(false);
	CAutoPtr<WindowsNetInfo> pWindowsNetInfo;
	KLWNF_CreateWindowsNetInfo(&pWindowsNetInfo, &bCancel);

KLSTD_TRACE0(1, L"GetADDomainWorkstations\r\n");
	pWindowsNetInfo->GetADDomainWorkstations(L"kl", arWorkstations2);
KLSTD_TRACE0(1, L"GetADDomainWorkstations::Done\r\n");

KLSTD_TRACE0(1, L"CTest::GetADsStructure\r\n");
	ComputerInfo info;
	std::wstring s(L"vintik");
	pWindowsNetInfo->GetComputerInfo(s, s, info);
KLSTD_TRACE0(1, L"CTest::GetADsStructure::Done\r\n");
	
KLSTD_TRACE0(1, L"CTest::GetADsStructure\r\n");
	ADObjectInfo Root;
	GetADsStructure(Root, pWindowsNetInfo);
KLSTD_TRACE0(1, L"CTest::GetADsStructure::Done\r\n");

// Next 2 tests may work VERY long
// Get domains
KLSTD_TRACE0(1, L"CTest::GetDomains\r\n");
//	GetDomains(pWindowsNetInfo, arDomains);
KLSTD_TRACE0(1, L"CTest::GetDomains::Done\r\n");

// Get workgroups
KLSTD_TRACE0(1, L"CTest::GetWorkgroups\r\n");
//	GetWorkgroups(pWindowsNetInfo, arWorkgroups);
KLSTD_TRACE0(1, L"CTest::GetWorkgroups::Done\r\n");

// Get Domain Workstations
KLSTD_TRACE0(1, L"CTest::GetDomainWorkstations\r\n");
	arWorkstations2.clear();
	GetDomainWorkstations(L"kl", pWindowsNetInfo, arWorkstations2);
KLSTD_TRACE0(1, L"CTest::GetDomainWorkstations::Done\r\n");

KLSTD_TRACE0(1, L"CTest::GetADDomainWorkstations\r\n");
	arWorkstations2.clear();
	GetADDomainWorkstations(L"kl", pWindowsNetInfo, arWorkstations2);
KLSTD_TRACE0(1, L"CTest::GetADDomainWorkstations::Done\r\n");

// Get workgroup Workstations
KLSTD_TRACE0(1, L"CTest::GetWorkgroupComputers\r\n");
	GetWorkgroupComputers(L"KL", pWindowsNetInfo, arBrowsedWorkstations);
KLSTD_TRACE0(1, L"CTest::GetWorkgroupComputers::Done\r\n");

// Get Domain Users
KLSTD_TRACE0(1, L"CTest::GetDomainUsers\r\n");
	GetDomainUsers(L"KL", pWindowsNetInfo, arUsers);
KLSTD_TRACE0(1, L"CTest::GetDomainUsers::Done\r\n");

// Get Computer Info
//	GetComputerInfo(L"csadmin", pWindowsNetInfo);

// Get User Info
	GetUserInfo(L"KL", L"andrey", pWindowsNetInfo);

// Get LoggedIn Users
//	GetLoggedInUsers(L"avp_server", pWindowsNetInfo);

// Get User Groups
KLSTD_TRACE0(1, L"CTest::GetUserGroups");
	GetUserGroups(L"KL", L"andrey", pWindowsNetInfo);
KLSTD_TRACE0(1, L"CTest::GetUserGroups::Done");

	return bResult;
};


