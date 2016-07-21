#include "stdafx.h"
#include "Test_UninstallLicenseKey.h"
#include "../../api/include/TestContext.h"
#include "ProgramClientInfo_Helpers.h"

#include <string>
#include <sstream>

namespace KasperskyLicensing  {
namespace Test  {

////////////////////////////////////////////////////////////////////////////
bool Test_UninstallLicenseKey_Common(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const LicenseKeyInfo::SerialNumber& sn,
	LicenseObjectId* pLicObj,
	int nLicObjCount,
	std::string& sErrorDesc
	)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(szBasesPath))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(szKeysPath))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	for (int i = 0; i < nLicObjCount; ++i)
	{
		pContext->AddItem(pLicObj[i]);
	}

	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.UninstallLicenseKey(sn))
	{
		sErrorDesc = "Uninstall license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool Test_UninstallLicenseKey_SimpleUninstall(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1000 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
		};
	return Test_UninstallLicenseKey_Common(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									TestContext::MakeSerialNumber(1, 2, 3),
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


bool Test_UninstallLicenseKey_InvalidSerial(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1000 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
		};
	return Test_UninstallLicenseKey_Common(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									TestContext::MakeSerialNumber(1, 2, 999),
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


bool Test_UninstallLicenseKey_WriteAcces(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1000 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
		};
	ExternalProgramRunner runner(".\\lock_w.exe .\\KeysStorage\\TmpKey1(add).key");
	runner.Start();
	bool result = Test_UninstallLicenseKey_Common(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									TestContext::MakeSerialNumber(1, 2, 3),
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
	runner.Stop();
	return result;
}

}  // namespace Test
}  // namespace KasperskyLicensing

