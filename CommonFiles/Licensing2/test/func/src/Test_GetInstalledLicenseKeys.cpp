#include "stdafx.h"
#include "Test_GetInstalledLicenseKeys.h"
#include "../../api/include/TestContext.h"
#include "ProgramClientInfo_Helpers.h"

#include <string>
#include <sstream>

namespace KasperskyLicensing  {
namespace Test  {

////////////////////////////////////////////////////////////////////////////
bool Test_GetInstalledLicenseKeys_SimpleCase(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
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

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1000 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
				{1100 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1110 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// устанавливаем один или несколько ключей в данный контекст
	if (!test_context.InstallLicenseKey(_T("key11.key"), _T(".\\Keys(GetInstalledLicenseKeys)\\key11.key")))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

		if (!test_context.InstallLicenseKey(_T("key12.key"), _T(".\\Keys(GetInstalledLicenseKeys)\\key12.key")))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	// получаем список установленных ключей для данного контекста
	InstalledLicenseKeysEnumerator enumerator(0);
	if (!test_context.GetInstalledLicenseKeys(enumerator))
	{
		sErrorDesc = "Get installed license keys: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем список установленных ключей для данного контекста
	bool key1_ok = false, key2_ok = false, invalid_keys = false;
	while (!enumerator.IsDone())
	{
		const ILicenseKey& key = enumerator.Item();
		if (
			!key1_ok &&
			StringsEqual(key.GetName(), "key11.key") &&
				key.GetKeyInfo().serial_number == TestContext::MakeSerialNumber(11, 11, 11) &&
//				StringsEqual(key.GetKeyInfo().distributor_info.address, "afdgsfgdfgd")
//				StringsEqual(key.GetKeyInfo().distributor_info.name, "Tester-distributor")
				key.GetKeyInfo().life_span == 365
		   )
		{
			key1_ok = true;
		}
		else if (
			!key2_ok &&
			StringsEqual(key.GetName(), "key12.key") && 
				key.GetKeyInfo().serial_number == TestContext::MakeSerialNumber(11, 11, 12) &&
//				StringsEqual(key.GetKeyInfo().distributor_info.address, "afdgsfgdfgd")
//				StringsEqual(key.GetKeyInfo().distributor_info.name, "Tester-distributor")
				key.GetKeyInfo().life_span == 730
				)
		{
			key2_ok = true;
		}
		else
		{
			invalid_keys = true;
		}
		enumerator.Next();
	}

	return (key1_ok && key2_ok && !invalid_keys);
}


bool Test_GetInstalledLicenseKeys_OnlyKey22(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
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

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
				{1100 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//				{1100 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//				{1110 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// устанавливаем один или несколько ключей в данный контекст
	if (!test_context.InstallLicenseKey(_T("key21.key"), _T(".\\Keys(GetInstalledLicenseKeys)\\key21.key")))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}


	// создаем новый контекст
	pContext = test_context.MakeEmptyLicenseContext();

	//задаем второй контекст
	{
		LicenseObjectId lic_objects[] = 
			{
//				{1100 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
				{1300 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//				{1110 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// устанавливаем один или несколько ключей в данный контекст
	if (!test_context.InstallLicenseKey(_T("key22.key"), _T(".\\Keys(GetInstalledLicenseKeys)\\key22.key")))

	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	// получаем список установленных ключей для данного контекста
	InstalledLicenseKeysEnumerator enumerator(0);
	if (!test_context.GetInstalledLicenseKeys(enumerator))
	{
		sErrorDesc = "Get installed license keys: " + test_context.GetLastErrorDesc();
		return false;
	}


	// проверяем список установленных ключей для данного контекста
	bool key1_ok = false, invalid_keys = false;
	while (!enumerator.IsDone())
	{
		const ILicenseKey& key = enumerator.Item();
		if (
			!key1_ok &&
			StringsEqual(key.GetName(), "key22.key") &&
				key.GetKeyInfo().serial_number == TestContext::MakeSerialNumber(11, 11, 22) &&
				key.GetKeyInfo().life_span == 365
		   )
			key1_ok = true;
		else
		{
			invalid_keys = true;
		}
		enumerator.Next();
	}

	return (key1_ok && !invalid_keys);
}

bool Test_GetInstalledLicenseKeys_NoKeys(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
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

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1100 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
				{1100 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//				{1110 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// устанавливаем один или несколько ключей в данный контекст
	if (!test_context.InstallLicenseKey(_T("key30.key"), _T(".\\Keys(GetInstalledLicenseKeys)\\key30.key")))
	{
		sErrorDesc = "Install license key: " + test_context.GetLastErrorDesc();
		return false;
	}


	// создаем новый контекст
	pContext = test_context.MakeEmptyLicenseContext();

	//задаем второй контекст
	{
		LicenseObjectId lic_objects[] = 
			{
//				{1100 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
				{9900 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//				{1110 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// получаем список установленных ключей для данного контекста
	InstalledLicenseKeysEnumerator enumerator(0);
	if (!test_context.GetInstalledLicenseKeys(enumerator))
	{
		sErrorDesc = "Get installed license keys: ";
		return false;
	}


	if (!enumerator.IsDone())
	{
		sErrorDesc = "Installed keys list is not empty";
		return false;
	}
	
	return true;
}

}  // namespace Test
}  // namespace KasperskyLicensing

