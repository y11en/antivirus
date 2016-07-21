// LicensingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../api/include/RunTest.h"
#include "Test_InitializeLicensing.h"
#include "Test_SetContext.h"
#include "Test_ParseLicenseKey.h"
#include "Test_InstallLicenseKey.h"
#include "Test_UninstallLicenseKey.h"
#include "Test_GetInstalledLicenseKeys.h"
#include "Test_CheckLicenseRestrictions.h"

using namespace std;

int main(int argc, char* argv[])
{
	using namespace KasperskyLicensing::Test;

	TestRunner tr;

//	tr.RunTest("InitializeLicensing::Normal",						Test_InitializeLicensing_Normal);
//	tr.RunTest("InitializeLicensing::BlackListIsCorrupted",			Test_InitializeLicensing_BlackListIsCorrupted);
//	tr.RunTest("InitializeLicensing::DailyAvcIsCorrupted",			Test_InitializeLicensing_DailyAvcIsCorrupted);
//	tr.RunTest("InitializeLicensing::DailyAvcIsLockedForRead",		Test_InitializeLicensing_DailyAvcIsLockedForRead);
//	tr.RunTest("InitializeLicensing::NoBlackList",					Test_InitializeLicensing_NoBlackList); //реестр базы создавался уже при отсутсвии black.lst.
//	tr.RunTest("InitializeLicensing::WrongInfoInXmlAbouDailyAvc",	Test_InitializeLicensing_WrongInfoInXmlAbouDailyAvc);
//	tr.RunTest("InitializeLicensing::DailyAvcIsCorrupted_TestKeyInBlackList",	Test_InitializeLicensing_DailyAvcIsCorrupted_TestKeyInBlackList);
//	tr.RunTest("SetContext::Normal",								Test_SetContext_Normal);
//	tr.RunTest("ParseLicenseKey::ValidKey",							Test_ParseLicenseKey_ValidKey);
//	tr.RunTest("ParseLicenseKey::WrongSignature",					Test_ParseLicenseKey_WrongSignature);
//	tr.RunTest("ParseLicenseKey::Non6Version",						Test_ParseLicenseKey_Non6Version);
//	tr.RunTest("ParseLicenseKey::WrongInfoThanSign",				Test_ParseLicenseKey_WrongInfoThanSign);
//	tr.RunTest("InstallLicenseKey::WrongProgramVersion",			Test_InstallLicenseKey_WrongProgramVersion);
//	tr.RunTest("InstallLicenseKey::ExpiredKey",						Test_InstallLicenseKey_ExpiredKey);
//	tr.RunTest("InstallLicenseKey::InBlackList",					Test_InstallLicenseKey_InBlackList);
//	tr.RunTest("InstallLicenseKey::WrongOL_product",				Test_InstallLicenseKey_WrongOL_product);
//	tr.RunTest("InstallLicenseKey::WrongOL_Application",			Test_InstallLicenseKey_WrongOL_Application);
//	tr.RunTest("InstallLicenseKey::WrongOL_Component",				Test_InstallLicenseKey_WrongOL_Component);
//	tr.RunTest("InstallLicenseKey::LocalizationId",					Test_InstallLicenseKey_LocalizationId);
//	tr.RunTest("InstallLicenseKey::SalesChannel",					Test_InstallLicenseKey_SalesChannel);
//	tr.RunTest("InstallLicenseKey::MarketSector",					Test_InstallLicenseKey_MarketSector);
//	tr.RunTest("InstallLicenseKey::Package",						Test_InstallLicenseKey_Package);
// 	tr.RunTest("InstallLicenseKey::WrongPlatform",					Test_InstallLicenseKey_WrongPlatform);
//	tr.RunTest("InstallLicenseKey::WriteAccess",					Test_InstallLicenseKey_WriteAccess);
//	tr.RunTest("InstallLicenseKey::AlreadyExists",					Test_InstallLicenseKey_AlreadyExists);
//	tr.RunTest("InstallLicenseKey::IdenticalSerials",				Test_InstallLicenseKey_IdenticalSerials);
//	tr.RunTest("Prepare for group of cases 'Uninstall Key'",		Test_InstallLicenseKey_TmpKey1);
//	tr.RunTest("UninstallLicenseKey::WriteAcces",					Test_UninstallLicenseKey_WriteAcces); //uninstal the key from Test_InstallLicenseKey_TmpKey1
//	tr.RunTest("UninstallLicenseKey::InvalidSerial",				Test_UninstallLicenseKey_InvalidSerial);
//	tr.RunTest("UninstallLicenseKey::SimpleUninstall",				Test_UninstallLicenseKey_SimpleUninstall); //uninstal the key from Test_InstallLicenseKey_TmpKey1
//	tr.RunTest("GetInstalledLicenseKeys::SimpleCase",				Test_GetInstalledLicenseKeys_SimpleCase);
//	tr.RunTest("GetInstalledLicenseKeys::OnlyKey22",				Test_GetInstalledLicenseKeys_OnlyKey22);
//	tr.RunTest("GetInstalledLicenseKeys::NoKeys",					Test_GetInstalledLicenseKeys_NoKeys);
//	tr.RunTest("Prepare for SimpleLO_true/false",					Test_InstallLicenseKey_TmpKey2);
//	tr.RunTest("CheckLicenseRestrictions::SimpleLO_true",			Test_CheckLicenseRestrictions_SimpleLO_true);
//	tr.RunTest("CheckLicenseRestrictions::SimpleLO_false",			Test_CheckLicenseRestrictions_SimpleLO_false);
	tr.RunTest("Prepare for IdNotif_ManyRestr (install 1st)",		Test_InstallLicenseKey_TmpKey3);
//	tr.RunTest("CheckLicenseRestrictions::IdNotif_ManyRestr",		Test_CheckLicenseRestrictions_IdNotif_ManyRestr);
//	tr.RunTest("CheckLicenseRestrictions::IdNotif_1Key",			Test_CheckLicenseRestrictions_IdNotif_1Key);
	tr.RunTest("Prepare for IdNotif_2KeyDifferentID (install 2d)",	Test_InstallLicenseKey_TmpKey5);
//	tr.RunTest("CheckLicenseRestrictions::IdNotif_2KeyDifferentID",	Test_CheckLicenseRestrictions_IdNotif_2KeyDifferentID);
//	tr.RunTest("CheckLicenseRestrictions::IdNotif_2KeySameID",		Test_CheckLicenseRestrictions_IdNotif_2KeySameID);
	tr.RunTest("CheckLicenseRestrictions::CheckAdditivity",			Test_CheckLicenseRestrictions_CheckAdditivity);
//	tr.RunTest("CheckLicenseRestrictions::CheckVariables_NoBlackList",							Test_CheckLicenseRestrictions_CheckVariables_NoBlackList);
//	tr.RunTest("CheckLicenseRestrictions::CheckVariables_CorruptedBases",						Test_CheckLicenseRestrictions_CheckVariables_CorruptedBases);
//	tr.RunTest("Prepare for CheckVariables_SerialInBlackList (install simple(InBlackList))",	Test_InstallLicenseKey_TmpKey8);
//	tr.RunTest("CheckLicenseRestrictions::CheckVariables_SerialInBlackList",					Test_CheckLicenseRestrictions_CheckVariables_SerialInBlackList);
//	tr.RunTest("CheckLicenseRestrictions::CheckVariables_Other",	Test_CheckLicenseRestrictions_CheckVariables_Other);
//	tr.RunTest("CheckLicenseRestrictions::CheckCondition",			Test_CheckLicenseRestrictions_CheckCondition);
//	tr.RunTest("CheckLicenseRestrictions::CheсkFormula",			Test_CheckLicenseRestrictions_CheckFormula);


	cout << endl;
	
	tr.PrintStatistics(cout);

	return 0;
}

