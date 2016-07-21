#include "stdafx.h"
#include "Test_ParseLicenseKey.h"
#include "../../api/include/TestContext.h"

#include <string>
#include <sstream>

namespace KasperskyLicensing  {
namespace Test  {

////////////////////////////////////////////////////////////////////////////
bool Test_SetContext_Common(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
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
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;  // ProgramClientInfo::VER_TYPE_RELEASE
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
//	clientInfo.product_info.product_id.id				= 1;  // ?
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
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

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool Test_SetContext_Normal(std::string& sErrorDesc)
{
	LicenseObjectId lic_objects[] = 
		{
//			{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
			{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
//			{3 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
		};
	return Test_SetContext_Common(_T(".\\Bases(Normal)"), _T(".\\Keys(Full)"), &lic_objects[0], sizeof(lic_objects) / sizeof(lic_objects[0]), sErrorDesc);
}

}  // namespace Test
}  // namespace KasperskyLicensing

