// Test.h: interface for the CTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEST_H__F289A533_E317_4001_84BE_B9C7B0DED006__INCLUDED_)
#define AFX_TEST_H__F289A533_E317_4001_84BE_B9C7B0DED006__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tst/kltester/testmodule.h>
#include <std/wnf/windowsnetinfo.h>
#include <exception>
#include <list>
#include <vector>

using namespace KLPAR;
using namespace KLERR;
using namespace KLSTD;
using namespace KLWNF;

//////////////////////////////////////////////////////////////////////

#define KLCS_MODULENAME				L"WNF_TEST"

//////////////////////////////////////////////////////////////////////
// CTest

class CTest
{
public:
	CTest();

	bool Run();

private:
	void GetADsStructure(ADObjectInfo parentAdObject, WindowsNetInfo* pWindowsNetInfo, std::wstring sTraceIndent = L"");
	void GetDomains(WindowsNetInfo* pWindowsNetInfo, OBJ_LIST& arDomains);
	void GetWorkgroups(WindowsNetInfo* pWindowsNetInfo, OBJ_LIST& arWorkgroups);
	void GetADDomainWorkstations(std::wstring sDomain, WindowsNetInfo* pWindowsNetInfo, OBJ_LIST2000& arWorkstations);
	void GetDomainWorkstations(std::wstring sDomain, WindowsNetInfo* pWindowsNetInfo, OBJ_LIST2000& arWorkstations);
	void GetWorkgroupComputers(std::wstring sDomain, WindowsNetInfo* pWindowsNetInfo, OBJ_LIST& arWorkstations);
	void GetDomainUsers(std::wstring sDomain, WindowsNetInfo* pWindowsNetInfo, OBJ_LIST& arUsers);
	void GetComputerInfo(std::wstring sComputer, WindowsNetInfo* pWindowsNetInfo);
	void GetUserInfo(std::wstring sDomain, std::wstring sUserName, WindowsNetInfo* pWindowsNetInfo);
	void GetLoggedInUsers(std::wstring sComputer, WindowsNetInfo* pWindowsNetInfo);
	void GetUserGroups(std::wstring sDomain, std::wstring sUser, WindowsNetInfo* pWindowsNetInfo);
};

#endif // !defined(AFX_TEST_H__F289A533_E317_4001_84BE_B9C7B0DED006__INCLUDED_)
