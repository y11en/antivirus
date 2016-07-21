#if !defined(__KL_LICENSING2_TEST_UNINSTALLLICENSEKEY_H)
#define __KL_LICENSING2_TEST_UNINSTALLLICENSEKEY_H

namespace KasperskyLicensing  {
namespace Test  {

bool Test_UninstallLicenseKey_SimpleUninstall(std::string& sErrorDesc);
bool Test_UninstallLicenseKey_InvalidSerial(std::string& sErrorDesc);
bool Test_UninstallLicenseKey_WriteAcces(std::string& sErrorDesc);

}  // namespace Test
}  // namespace KasperskyLicensing

#endif  // !defined(__KL_LICENSING2_TEST_UNINSTALLLICENSEKEY_H)
