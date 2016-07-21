#if !defined(__KL_LICENSING2_TEST_CHECKLICENSERESTRICTIONS_H)
#define __KL_LICENSING2_TEST_CHECKLICENSERESTRICTIONS_H

namespace KasperskyLicensing  {
namespace Test  {

bool Test_CheckLicenseRestrictions_SimpleLO_true(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_SimpleLO_false(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_IdNotif_ManyRestr(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_IdNotif_1Key(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_IdNotif_2KeyDifferentID(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_IdNotif_2KeySameID(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_CheckAdditivity(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_CheckVariables_NoBlackList(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_CheckVariables_CorruptedBases(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_CheckVariables_SerialInBlackList(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_CheckVariables_Other(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_CheckCondition(std::string& sErrorDesc);
bool Test_CheckLicenseRestrictions_CheckFormula(std::string& sErrorDesc);

}  // namespace Test
}  // namespace KasperskyLicensing

#endif  // !defined(__KL_LICENSING2_TEST_CHECKLICENSERESTRICTIONS_H)
