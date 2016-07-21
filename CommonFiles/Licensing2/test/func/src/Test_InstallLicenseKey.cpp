#include "stdafx.h"
#include "Test_InstallLicenseKey.h"
#include "../../api/include/TestContext.h"
#include "ProgramClientInfo_Helpers.h"

#include <string>
#include <sstream>

namespace KasperskyLicensing  {
namespace Test  {

////////////////////////////////////////////////////////////////////////////
bool Test_InstallLicenseKey_Common_WksBeta(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_WksBeta(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}


bool Test_InstallLicenseKey_Common_WksRelease(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}

bool Test_InstallLicenseKey_Common_PerBeta(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_PerBeta(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}


bool Test_InstallLicenseKey_Common_PerRelease(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_PerRelease(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}

bool Test_InstallLicenseKey_Common_Unix(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_Unix(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}


bool Test_InstallLicenseKey_Common_WrongOL_product(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_WrongOL_product(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}


bool Test_InstallLicenseKey_Common_WrongOL_c_a(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_WrongOL_c_a(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}


bool Test_InstallLicenseKey_Common_LocalizationId(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_LocalizationId(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}


bool Test_InstallLicenseKey_Common_SalesChannel(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_SalesChannel(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}

bool Test_InstallLicenseKey_Common_MarketSector(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_MarketSector(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}

bool Test_InstallLicenseKey_Common_Package(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_Package(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}


bool Test_InstallLicenseKey_Common_IdenticalSerials_step1(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_IdenticalSerials_step1(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}


bool Test_InstallLicenseKey_Common_IdenticalSerials_step2(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szKeyName,
	const char_t* szKeyPath,
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
	Fill_ProgramClientInfo_IdenticalSerials_step2(clientInfo);

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

	if (!test_context.InstallLicenseKey(szKeyName, szKeyPath))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool Test_InstallLicenseKey_WrongProgramVersion(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_WksBeta(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("WrongProgramVersion(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}

bool Test_InstallLicenseKey_ExpiredKey(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_PerRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("expired(add).key"), 
									_T(".\\Keys(Expired)\\expired.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


bool Test_InstallLicenseKey_InBlackList(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_PerRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("inblacklist(add).key"), 
									_T(".\\Keys(InBlackList)\\inblacklist.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


bool Test_InstallLicenseKey_WrongOL_product(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_WrongOL_product(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("WrongOL_product(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


bool Test_InstallLicenseKey_WrongOL_Application(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{12000 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },//in key 1200
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_WrongOL_c_a(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("WrongOL_Application(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


bool Test_InstallLicenseKey_WrongOL_Component(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },//in key 1200
			{11000 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },//in key 1100
		};
	return Test_InstallLicenseKey_Common_WrongOL_c_a(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("WrongOL_Component(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


bool Test_InstallLicenseKey_LocalizationId(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_LocalizationId(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("localizationid(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}

bool Test_InstallLicenseKey_SalesChannel(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_SalesChannel(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("SalesChannel(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}

bool Test_InstallLicenseKey_MarketSector(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_MarketSector(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("MarketSector(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}

bool Test_InstallLicenseKey_Package(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_Package(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("Package(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


bool Test_InstallLicenseKey_WrongPlatform(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_Unix(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("WrongPlatform(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


bool Test_InstallLicenseKey_WriteAccess(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_WksRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\WriteAccess"), 
									_T("WriteAccess(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}

bool Test_InstallLicenseKey_AlreadyExists(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	bool res1 = Test_InstallLicenseKey_Common_WksRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("AlreadyExists(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);

	LicenseObjectId lic_objects2[] = 
		{
			//{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	bool res2 = Test_InstallLicenseKey_Common_WksRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("AlreadyExists(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects2[0], 
									sizeof(lic_objects2) / sizeof(lic_objects2[0]), 
									sErrorDesc
									);
		return (res1 && !res2);
}

bool Test_InstallLicenseKey_IdenticalSerials(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
			{6660 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			//{1 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	bool res1 = Test_InstallLicenseKey_Common_IdenticalSerials_step1(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("IdenticalSerials_step1(add).key"), 
									_T(".\\Keys(IdentialSerials)\\IdenticalSerials_step1.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
	LicenseObjectId lic_objects2[] = 
		{
			//{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	bool res2 = Test_InstallLicenseKey_Common_IdenticalSerials_step2(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("IdenticalSerials_step2(add).key"), 
									_T(".\\Keys(IdentialSerials)\\IdenticalSerials_step2.key"), 
									&lic_objects2[0], 
									sizeof(lic_objects2) / sizeof(lic_objects2[0]), 
									sErrorDesc
									);
	return (res1 && !res2);
}

bool Test_InstallLicenseKey_TmpKey1(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ },
		};
	return Test_InstallLicenseKey_Common_WksRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("TmpKey1(add).key"), 
									_T(".\\Keys(Full)\\full.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}

bool Test_InstallLicenseKey_TmpKey2(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1210 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
		};
	return Test_InstallLicenseKey_Common_WksRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("TmpKey2(add).key"), 
									_T(".\\Keys(CheckLicenseRestriction)\\SimpleLO_true.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}

bool Test_InstallLicenseKey_TmpKey3(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1300 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
		};
	return Test_InstallLicenseKey_Common_WksRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("TmpKey3(add).key"), 
									_T(".\\Keys(CheckLicenseRestriction)\\SimpleLO_1st.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}

//bool Test_InstallLicenseKey_TmpKey4(std::string& sErrorDesc)
//{
//	LicenseObjectId lic_objects[] = 
//		{
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//			{1300 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//			{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
//		};
//	return Test_InstallLicenseKey_Common_WksRelease(
//									_T(".\\Bases(Normal)"), 
//									_T(".\\KeysStorage"), 
//									_T("TmpKey4(add).key"), 
//									_T(".\\Keys(CheckLicenseRestriction)\\SimpleLO_1st.key"), 
//									&lic_objects[0], 
//									sizeof(lic_objects) / sizeof(lic_objects[0]), 
//									sErrorDesc
//									);
//}



bool Test_InstallLicenseKey_TmpKey5(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
		};
	return Test_InstallLicenseKey_Common_WksRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("TmpKey5(add).key"), 
									_T(".\\Keys(CheckLicenseRestriction)\\SimpleLO_2d.key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}

//bool Test_InstallLicenseKey_TmpKey6(std::string& sErrorDesc)
//{
//	LicenseObjectId lic_objects[] = 
//		{
////			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//			{1220 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
//		};
//	return Test_InstallLicenseKey_Common_WksRelease(
//									_T(".\\Bases(Normal)"), 
//									_T(".\\KeysStorage"), 
//									_T("TmpKey6(add).key"), 
//									_T(".\\Keys(CheckLicenseRestriction)\\SimpleLO_1st.key"), 
//									&lic_objects[0], 
//									sizeof(lic_objects) / sizeof(lic_objects[0]), 
//									sErrorDesc
//									);
//}
//
//bool Test_InstallLicenseKey_TmpKey7(std::string& sErrorDesc)
//{
//	LicenseObjectId lic_objects[] = 
//		{
////			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//			{1220 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
//		};
//	return Test_InstallLicenseKey_Common_WksRelease(
//									_T(".\\Bases(Normal)"), 
//									_T(".\\KeysStorage"), 
//									_T("TmpKey7(add).key"), 
//									_T(".\\Keys(CheckLicenseRestriction)\\SimpleLO_2d.key"), 
//									&lic_objects[0], 
//									sizeof(lic_objects) / sizeof(lic_objects[0]), 
//									sErrorDesc
//									);
//}

bool Test_InstallLicenseKey_TmpKey8(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
			{6600 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
		};
	return Test_InstallLicenseKey_Common_WksRelease(
									_T(".\\Bases(Normal)"), 
									_T(".\\KeysStorage"), 
									_T("TmpKey8(add).key"), 
									_T(".\\Keys(InBlackList)\\simple(InBlackList).key"), 
									&lic_objects[0], 
									sizeof(lic_objects) / sizeof(lic_objects[0]), 
									sErrorDesc
									);
}


}  // namespace Test
}  // namespace KasperskyLicensing

