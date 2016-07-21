#if !defined(__KL_LICENSING2_TEST_INITIALIZELICENSING_H)
#define __KL_LICENSING2_TEST_INITIALIZELICENSING_H

namespace KasperskyLicensing  {
namespace Test  {

bool Test_InitializeLicensing_Normal(std::string& sErrorDesc);
bool Test_InitializeLicensing_BlackListIsCorrupted(std::string& sErrorDesc);
bool Test_InitializeLicensing_DailyAvcIsCorrupted(std::string& sErrorDesc);
bool Test_InitializeLicensing_DailyAvcIsLockedForRead(std::string& sErrorDesc);
bool Test_InitializeLicensing_NoBlackList(std::string& sErrorDesc);
bool Test_InitializeLicensing_WrongInfoInXmlAbouDailyAvc(std::string& sErrorDesc);
bool Test_InitializeLicensing_DailyAvcIsCorrupted_TestKeyInBlackList(std::string& sErrorDesc);

}  // namespace Test
}  // namespace KasperskyLicensing

#endif  // !defined(__KL_LICENSING2_TEST_INITIALIZELICENSING_H)
