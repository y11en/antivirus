#if !defined(__KL_LICENSING2_TEST_GETINSTALLEDLICENSEKEYS_H)
#define __KL_LICENSING2_TEST_GETINSTALLEDLICENSEKEYS_H

namespace KasperskyLicensing  {
namespace Test  {

bool Test_GetInstalledLicenseKeys_SimpleCase(std::string& sErrorDesc);
bool Test_GetInstalledLicenseKeys_OnlyKey22(std::string& sErrorDesc);
bool Test_GetInstalledLicenseKeys_NoKeys(std::string& sErrorDesc);

}  // namespace Test
}  // namespace KasperskyLicensing

#endif  // !defined(__KL_LICENSING2_TEST_GETINSTALLEDLICENSEKEYS_H)
