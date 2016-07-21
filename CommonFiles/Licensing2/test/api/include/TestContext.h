/*
*/

#if !defined(__KL_LICENSING2_TESTCONTEXT_H)
#define __KL_LICENSING2_TESTCONTEXT_H

#include <windows.h>
#include <vector>
#include <string>
#include <Licensing2/include/LicensingInterface.h>
#include <Licensing2/include/utility/WinFileStorageRW.h>

namespace KasperskyLicensing  {
namespace Test  {

typedef AutoPointer<ILicensing> ILicensingPtr;
typedef AutoPointer<ILicenseKey> ILicenseKeyPtr;
typedef AutoPointer<ILicenseContext> ILicenseContextPtr;
typedef AutoPointer<ILicenseUtility> ILicenseUtilityPtr;
typedef Enumerator<const ILicenseKey&> InstalledLicenseKeysEnumerator;
typedef std::basic_string<char_t> string_t;

////////////////////////////////////////////////////////////////////////////
class TestContext
{
public:
	TestContext() : m_error(ILicensingError::LICERR_OK)  {}
	const ILicensingError& GetLastError() const  { return m_error; }
	ILicensingError::ErrorCodeType GetLastErrorCode() const  { return GetLastError().GetErrorCode(); }
	std::string GetLastErrorDesc() const  { return GetErrorDescription(GetLastError()); }
	bool InitializeKeyStorage(const char_t* path, const char_t* mask = _T("*.key"));
	bool InitializeBasesStorage(const char_t* path, const char_t* mask = _T("*.*"));
	bool InitializeLicensing(const char_t* dskm_path, ControlMode ctrlMode, const ProgramClientInfo& clientInfo);
	void GetInitWarnings(std::vector<ILicensing::WarningCode>& vecWarnings) const;
	ILicenseKeyPtr ParseLicenseKey(const char_t* path);
	ILicenseContextPtr MakeEmptyLicenseContext() const;
	bool SetLicenseContext(ILicenseContextPtr pContext);
	static LicenseKeyInfo::SerialNumber MakeSerialNumber(
												unsigned int customer_id, 
												unsigned int application_id, 
												unsigned int sequence_number
												);
	bool InstallLicenseKey(const char_t* name, const char_t* file_path);
	bool UninstallLicenseKey(const LicenseKeyInfo::SerialNumber& sn);
	bool GetInstalledLicenseKeys(InstalledLicenseKeysEnumerator& enumerator);
	bool CheckLicenseRestriction(
						unsigned int restr_id,
						unsigned int restr_value,
						bool& bPositive,
						std::vector<unsigned int>& vecNotifications
						);
private:
	static bool ReadKeyData(const char_t* path, std::vector<char>& key_data);
	static std::string GetErrorDescription(const ILicensingError& err);
private:
	LicensingError m_error;
	std::auto_ptr<WinFileStorageRW> m_pKeyStorage;
	std::auto_ptr<WinFileStorageRW> m_pBasesStorage;
	ILicensingPtr		m_pLicensing;
	ILicenseContextPtr	m_pCurrentContext;
	ILicenseUtilityPtr	m_pCurrentUtility;
};

//////////////////////////////////////////////////////////////////////////
class ExternalProgramRunner
{
public:
	ExternalProgramRunner(const char* szCommandLine);
	~ExternalProgramRunner()  { Stop(); }
	bool Start();
	void Stop();
	void WaitForExit();
private:
	std::string m_sCommandLine;
	PROCESS_INFORMATION m_pi;
};

bool StringsEqual(const char* s1, const char* s2);
bool StringsEqual(const wchar_t* s1, const wchar_t* s2);

bool operator == (const LicenseKeyInfo::SerialNumber& sn1, const LicenseKeyInfo::SerialNumber& sn2);

}  // namespace Test
}  // namespace KasperskyLicensing

#endif
