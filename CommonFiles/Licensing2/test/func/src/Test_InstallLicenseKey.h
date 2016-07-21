#if !defined(__KL_LICENSING2_TEST_INSTALLLICENSEKEY_H)
#define __KL_LICENSING2_TEST_INSTALLLICENSEKEY_H

namespace KasperskyLicensing  {
namespace Test  {

	bool Test_InstallLicenseKey_WrongProgramVersion(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_ExpiredKey(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_InBlackList(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_WrongOL_product(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_WrongOL_Application(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_WrongOL_Component(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_LocalizationId(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_SalesChannel(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_MarketSector(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_Package(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_WrongPlatform(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_WriteAccess(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_AlreadyExists(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_IdenticalSerials(std::string& sErrorDesc);

// Вспомогательные функции для последующих тестов
	
	bool Test_InstallLicenseKey_TmpKey1(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_TmpKey2(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_TmpKey3(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_TmpKey4(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_TmpKey5(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_TmpKey6(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_TmpKey7(std::string& sErrorDesc);
	bool Test_InstallLicenseKey_TmpKey8(std::string& sErrorDesc);
}  // namespace Test
}  // namespace KasperskyLicensing

#endif  // !defined(__KL_LICENSING2_TEST_INSTALLLICENSEKEY_H)
