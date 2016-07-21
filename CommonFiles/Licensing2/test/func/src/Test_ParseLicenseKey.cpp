#include "stdafx.h"
#include "Test_ParseLicenseKey.h"
#include "../../api/include/TestContext.h"
#include "ProgramClientInfo_Helpers.h"

#include <string>
#include <sstream>

namespace KasperskyLicensing  {
namespace Test  {

////////////////////////////////////////////////////////////////////////////
bool Test_ParseLicenseKey_Common(
	const char_t* szBasesPath, 
	const char_t* szKeysPath, 
	const char_t* szParseKeyPath,
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

	ILicenseKeyPtr pKey = test_context.ParseLicenseKey(szParseKeyPath);
	if (pKey.GetPointer())
	{
		time_t tCreated = pKey->GetKeyInfo().creation_date;
	}
	else
	{
		sErrorDesc = "Parse license key: " + test_context.GetLastErrorDesc();
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool Test_ParseLicenseKey_ValidKey(std::string& sErrorDesc)
{
	return Test_ParseLicenseKey_Common(
							_T(".\\Bases(Normal)"), 
							_T(".\\Keys(NormalSimple)"), 
							_T(".\\Keys(NormalSimple)\\key.key"), 
							sErrorDesc
							);
}

////////////////////////////////////////////////////////////////////////////
bool Test_ParseLicenseKey_WrongSignature(std::string& sErrorDesc)
{
	return Test_ParseLicenseKey_Common(
							_T(".\\Bases(Normal)"), 
							_T(".\\Keys(WrongSignature)"), 
							_T(".\\Keys(WrongSignature)\\key.key"), 
							sErrorDesc
							);
}

////////////////////////////////////////////////////////////////////////////
bool Test_ParseLicenseKey_WrongInfoThanSign(std::string& sErrorDesc)
{
	return Test_ParseLicenseKey_Common(
							_T(".\\Bases(Normal)"), 
							_T(".\\Keys(WrongInfoThanSign)"), 
							_T(".\\Keys(WrongInfoThanSign)\\key.key"), 
							sErrorDesc
							);
}

////////////////////////////////////////////////////////////////////////////
bool Test_ParseLicenseKey_Non6Version(std::string& sErrorDesc)
{
	return Test_ParseLicenseKey_Common(
							_T(".\\Bases(Normal)"), 
							_T(".\\Keys(Non6Version)"), 
							_T(".\\Keys(Non6Version)\\key.key"), 
							sErrorDesc
							);
}

////////////////////////////////////////////////////////////////////////////

}  // namespace Test
}  // namespace KasperskyLicensing

