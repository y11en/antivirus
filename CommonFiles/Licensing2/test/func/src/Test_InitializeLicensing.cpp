#include "stdafx.h"
#include "Test_InitializeLicensing.h"
#include "../../api/include/TestContext.h"
#include "ProgramClientInfo_Helpers.h"

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#pragma warning( disable : 4786 )

namespace KasperskyLicensing  {
namespace Test  {

//////////////////////////////////////////////////////////////////////////
bool CompareWarningVectors(
	const std::vector<ILicensing::WarningCode>& vec1,
	const std::vector<ILicensing::WarningCode>& vec2
	)
{
	if (vec1.empty() && vec2.empty())
		return true;
	if (vec1.size() != vec2.size())
		return false;
	std::vector<ILicensing::WarningCode> vec1_sorted(vec1), vec2_sorted(vec2);
	std::sort(vec1_sorted.begin(), vec1_sorted.end());
	std::sort(vec2_sorted.begin(), vec2_sorted.end());
	return std::equal(vec1_sorted.begin(), vec1_sorted.end(), vec2_sorted.begin());
}

////////////////////////////////////////////////////////////////////////////
bool Test_InitializeLicensing_Common(
	const char_t* szBasesPath, 
	const char_t* szKeysPath,
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

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool Test_InitializeLicensing_Common_Bases_Ex(
	const char_t* szBasesPath, 
	const char_t* szKeysPath,
	ControlMode ctrlMode,
	const std::vector<ILicensing::WarningCode>& warnings_etalon,
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

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	std::vector<ILicensing::WarningCode> vecWarnings;
	test_context.GetInitWarnings(vecWarnings);
	if (CompareWarningVectors(vecWarnings, warnings_etalon))
	{
		return true;
	}
	else
	{
		std::stringstream ss;
		ss << "Invalid init status (returned:";
		if (vecWarnings.empty())
		{
			ss << " <empty>";
		}
		else
		{
			for (std::vector<ILicensing::WarningCode>::const_iterator it = vecWarnings.begin();
					it != vecWarnings.end();
					++it)
			{
				ss << " " << static_cast<int>(*it);
			}
		}
		ss << ", expected:";
		{
			for (std::vector<ILicensing::WarningCode>::const_iterator it = warnings_etalon.begin();
					it != warnings_etalon.end();
					++it)
			{
				ss << " " << static_cast<int>(*it);
			}
		}
		ss << ").";
		ss.str().swap(sErrorDesc);
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////
bool Test_InitializeLicensing_Common_Bases(
	const char_t* szBasesPath, 
	const char_t* szKeysPath,
	ILicensing::WarningCode status_etalon,
	std::string& sErrorDesc
	)
{
	std::vector<ILicensing::WarningCode> etalon_warnings;
	etalon_warnings.push_back(status_etalon);
	return Test_InitializeLicensing_Common_Bases_Ex(
									szBasesPath, 
									szKeysPath, 
									CTRL_BLIST_AND_BASES, 
									etalon_warnings,
									sErrorDesc
									);
}

////////////////////////////////////////////////////////////////////////////
bool Test_InitializeLicensing_Normal(std::string& sErrorDesc)
{
	return Test_InitializeLicensing_Common_Bases_Ex(
									_T(".\\Bases(Normal)"), 
									_T(".\\Keys(Full)"), 
									CTRL_BLIST_AND_BASES,
									std::vector<ILicensing::WarningCode>(),
									sErrorDesc
									);
}

bool Test_InitializeLicensing_BlackListIsCorrupted(std::string& sErrorDesc)
{
	return Test_InitializeLicensing_Common_Bases(
									_T(".\\Bases(BlackListIsCorrupted)"), 
									_T(".\\Keys(Full)"), 
									ILicensing::WARN_INCORRECT_BLACKLIST, 
									sErrorDesc
									);
}

bool Test_InitializeLicensing_DailyAvcIsCorrupted(std::string& sErrorDesc)
{
	return Test_InitializeLicensing_Common_Bases(
									_T(".\\Bases(DailyAvcIsCorrupted)"), 
									_T(".\\Keys(Full)"), 
									ILicensing::WARN_INCONSISTENT_AVDATABASE,
									sErrorDesc
									);
}

bool Test_InitializeLicensing_DailyAvcIsLockedForRead(std::string& sErrorDesc)
{
	return Test_InitializeLicensing_Common_Bases(
									_T(".\\Bases(DailyAvcIsLockedForRead)"), 
									_T(".\\Keys(Full)"), 
									ILicensing::WARN_INCONSISTENT_AVDATABASE,
									sErrorDesc
									);
}

//реестр базы создавался уже при отсутсвии black.lst
bool Test_InitializeLicensing_NoBlackList(std::string& sErrorDesc)
{
	return Test_InitializeLicensing_Common_Bases(
									_T(".\\Bases(NoBlackList)"), 
									_T(".\\Keys(Full)"), 
									ILicensing::WARN_INCORRECT_BLACKLIST,
									sErrorDesc
									);
}

bool Test_InitializeLicensing_WrongInfoInXmlAbouDailyAvc(std::string& sErrorDesc)
{

	return Test_InitializeLicensing_Common_Bases(
									_T(".\\Bases(WrongInfoInXmlAbouDailyAvc)"), 
									_T(".\\Keys(Full)"), 
									ILicensing::WARN_INCONSISTENT_AVDATABASE,
									sErrorDesc
									);
}

bool Test_InitializeLicensing_DailyAvcIsCorrupted_TestKeyInBlackList(std::string& sErrorDesc)
{
	return Test_InitializeLicensing_Common_Bases_Ex(
									_T(".\\Bases(DailyAvcIsCorrupted)"), 
									//_T(".\\Keys(Full)"),
									_T(".\\Keys(InBlackList)"), 
									CTRL_BLIST_ONLY,
									std::vector<ILicensing::WarningCode>(),				//ILicensing::WARN_INCORRECT_DBINDEX,  // ????
									sErrorDesc
									);
}

}  // namespace Test
}  // namespace KasperskyLicensing

