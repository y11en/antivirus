#if !defined(__KL_LICENSING2_TEST_PARSELICENSEKEY_H)
#define __KL_LICENSING2_TEST_PARSELICENSEKEY_H

namespace KasperskyLicensing  {
namespace Test  {

bool Test_ParseLicenseKey_ValidKey(std::string& sErrorDesc);
bool Test_ParseLicenseKey_WrongSignature(std::string& sErrorDesc);
bool Test_ParseLicenseKey_WrongInfoThanSign(std::string& sErrorDesc);
bool Test_ParseLicenseKey_Non6Version(std::string& sErrorDesc);

}  // namespace Test
}  // namespace KasperskyLicensing

#endif  // !defined(__KL_LICENSING2_TEST_PARSELICENSEKEY_H)
