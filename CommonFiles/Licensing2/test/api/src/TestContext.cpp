#include "stdafx.h"
#include "../include/TestContext.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iterator>

namespace KasperskyLicensing  {
namespace Test  {

////////////////////////////////////////////////////////////////////////////
bool TestContext::InitializeKeyStorage(const char_t* path, const char_t* mask)
{
	try
	{
		m_pKeyStorage = std::auto_ptr<WinFileStorageRW>(new WinFileStorageRW(path, mask));
	}
	catch (ILicensingError& err)
	{
		m_error = LicensingError(err.GetErrorCode(), err.GetSpecificCode());
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool TestContext::InitializeBasesStorage(const char_t* path, const char_t* mask)
{
	try
	{
		m_pBasesStorage = std::auto_ptr<WinFileStorageRW>(new WinFileStorageRW(path, mask));
	}
	catch (ILicensingError& err)
	{
		m_error = LicensingError(err.GetErrorCode(), err.GetSpecificCode());
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool TestContext::InitializeLicensing(
	const char_t* dskm_path, 
	ControlMode ctrlMode, 
	const ProgramClientInfo& clientInfo
	)
{
	try
	{
		m_pLicensing = MakeLicensingEx(
								*(m_pKeyStorage.get()), 
								*(m_pBasesStorage.get()), 
								ctrlMode, 
								clientInfo, 
								dskm_path
								);
	}
	catch (ILicensingError& err)
	{
		m_error = LicensingError(err.GetErrorCode(), err.GetSpecificCode());;
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
void TestContext::GetInitWarnings(
	std::vector<ILicensing::WarningCode>& vecWarnings
	) const
{
	ILicensing::WarningEnumerator enumerator(m_pLicensing->GetInitWarnings());
	vecWarnings.clear();
	while (!enumerator.IsDone())
	{
		vecWarnings.push_back(enumerator.Item());
		enumerator.Next();
	}
}

////////////////////////////////////////////////////////////////////////////
ILicenseContextPtr TestContext::MakeEmptyLicenseContext() const
{
	return m_pLicensing->MakeLicenseContext();
}

////////////////////////////////////////////////////////////////////////////
bool TestContext::SetLicenseContext(ILicenseContextPtr pContext)
{
	if (!pContext.GetPointer())
		return false;
	m_pCurrentContext = pContext;
	try
	{
		m_pCurrentUtility = m_pLicensing->MakeLicenseUtility(*m_pCurrentContext.GetPointer());
	}
	catch (ILicensingError& err)
	{
		m_error = LicensingError(err.GetErrorCode(), err.GetSpecificCode());;
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool TestContext::ReadKeyData(const char_t* path, std::vector<char>& key_data)
{
	std::ifstream file;
	file.open(path, std::ios_base::in | std::ios_base::binary);
	if (!file.is_open())	
		return false;
	std::vector<char> tmp_key_data;
	std::vector<char> buf(1024, 0);
	while (!file.eof())
	{
		file.read(&buf[0], buf.size());
		std::copy(buf.begin(), buf.begin() + file.gcount(), std::back_inserter(tmp_key_data));
	}
	tmp_key_data.swap(key_data);
	return true;
}

////////////////////////////////////////////////////////////////////////////
ILicenseKeyPtr TestContext::ParseLicenseKey(const char_t* path)
{
	AutoPointer<ILicenseKey> pKey;
	try
	{
		std::vector<char> key_data;
		if (!ReadKeyData(path, key_data))
			throw LicensingError(ILicensingError::LICERR_NOT_FOUND);
		pKey = m_pLicensing->ParseLicenseKey(&key_data[0], key_data.size());
	}
	catch (ILicensingError& err)
	{
		m_error = LicensingError(err.GetErrorCode(), err.GetSpecificCode());;
	}
	return pKey;
}

////////////////////////////////////////////////////////////////////////////
LicenseKeyInfo::SerialNumber 
TestContext::MakeSerialNumber(
	unsigned int customer_id, 
	unsigned int application_id, 
	unsigned int sequence_number
	)
{
	LicenseKeyInfo::SerialNumber result = { customer_id, application_id, sequence_number };
	return result;
}

////////////////////////////////////////////////////////////////////////////
bool TestContext::InstallLicenseKey(const char_t* name, const char_t* file_path)
{
	try
	{
		std::vector<char> key_data;
		if (!ReadKeyData(file_path, key_data))
			throw LicensingError(ILicensingError::LICERR_NOT_FOUND);
		m_pCurrentUtility->InstallLicenseKey(name, &key_data[0], key_data.size());
	}
	catch (ILicensingError& err)
	{
		m_error = LicensingError(err.GetErrorCode(), err.GetSpecificCode());;
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
bool TestContext::UninstallLicenseKey(const LicenseKeyInfo::SerialNumber& sn)
{
	try
	{
		m_pCurrentUtility->UninstallLicenseKey(sn);
	}
	catch (ILicensingError& err)
	{
		m_error = LicensingError(err.GetErrorCode(), err.GetSpecificCode());;
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool TestContext::GetInstalledLicenseKeys(
	InstalledLicenseKeysEnumerator& enumerator
	)
{
	try
	{
		enumerator = m_pCurrentUtility->GetInstalledKeys();
	}
	catch (ILicensingError& err)
	{
		m_error = LicensingError(err.GetErrorCode(), err.GetSpecificCode());;
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool TestContext::CheckLicenseRestriction(
	unsigned int restr_id,
	unsigned int restr_value,
	bool& bPositive,
	std::vector<unsigned int>& vecNotifications
	)
{
	try
	{
		const ILicenseRestriction& restr = m_pCurrentUtility->GetLicenseObject().GetRestriction(restr_id);
		AutoPointer<IVerdict> pVerdict = restr.Examine(restr_value);
		pVerdict->IsPositive();
		std::vector<unsigned int> vecTemp;
		IVerdict::NotificationEnumerator en = pVerdict->GetNotifications();
		for (; !en.IsDone(); en.Next())
		{
			vecTemp.push_back(en.Item());
		}
		bPositive = pVerdict->IsPositive();
		vecNotifications.swap(vecTemp);
	}
	catch (ILicensingError& err)
	{
		m_error = LicensingError(err.GetErrorCode(), err.GetSpecificCode());;
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
std::string TestContext::GetErrorDescription(const ILicensingError& err)
{
	std::string result;
	switch(err.GetErrorCode())
	{
		case ILicensingError::LICERR_NOT_FOUND:
			result = "Entity not found error";
			break;
		case ILicensingError::LICERR_PARSING_ERROR:
			result = "Data parsing error";
			break;
		case ILicensingError::LICERR_INVALID_SIGNATURE:
			result = "Invalid signature error";
			break;
		case ILicensingError::LICERR_KEY_INCOMPATIBLE:
			result = "Incompatible key error";
			break;
		case ILicensingError::LICERR_KEY_MISMATCH:
			result = "Key mismatch error";
			break;
		case ILicensingError::LICERR_DUPLICATE_KEY:
			result = "Duplicate key error";
			break;
		case ILicensingError::LICERR_STORAGE_READ:
			{
				std::stringstream ss;
				ss << "Read from storage error, specific code = " << err.GetSpecificCode();
				result = ss.str();
			}
			break;
		case ILicensingError::LICERR_STORAGE_WRITE:
			{
				std::stringstream ss;
				ss << "Write from storage error, specific code = " << err.GetSpecificCode();
				result = ss.str();
			}
			break;
		case ILicensingError::LICERR_OS_SPECIFIC:
			{
				std::stringstream ss;
				ss << "Operation system error, specific code = " << err.GetSpecificCode();
				result = ss.str();
			}
			break;
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
ExternalProgramRunner::ExternalProgramRunner(const char* szCommandLine)
	: m_sCommandLine(szCommandLine)
{
    ZeroMemory(&m_pi, sizeof(m_pi));
}

//////////////////////////////////////////////////////////////////////////
bool ExternalProgramRunner::Start()
{
	Stop();

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    // Start the child process. 
    if (!::CreateProcessA( 
					NULL,   // No module name (use command line). 
					const_cast<char*>(m_sCommandLine.c_str()), // Command line. 
					NULL,             // Process handle not inheritable. 
					NULL,             // Thread handle not inheritable. 
					FALSE,            // Set handle inheritance to FALSE. 
					0,                // No creation flags. 
					NULL,             // Use parent's environment block. 
					NULL,             // Use parent's starting directory. 
					&si,              // Pointer to STARTUPINFO structure.
					&m_pi))             // Pointer to PROCESS_INFORMATION structure.
    {
        return false;
    }

	return true;
}

//////////////////////////////////////////////////////////////////////////
void ExternalProgramRunner::Stop()
{
	if (m_pi.hProcess)
	{
		if (::WaitForSingleObject(m_pi.hProcess, 0) != WAIT_OBJECT_0)
		{
			::TerminateProcess(m_pi.hProcess, 0);
			::WaitForSingleObject(m_pi.hProcess, INFINITE);
		}
		::CloseHandle(m_pi.hProcess);
		::CloseHandle(m_pi.hThread);
	    ZeroMemory(&m_pi, sizeof(m_pi));
	}
}

//////////////////////////////////////////////////////////////////////////
void ExternalProgramRunner::WaitForExit()
{
	if (m_pi.hProcess)
		::WaitForSingleObject(m_pi.hProcess, INFINITE);
}

//////////////////////////////////////////////////////////////////////////
bool StringsEqual(const char* s1, const char* s2)
{
	return (s1 && s2 && (strcmp(s1, s2) == 0));
}

//////////////////////////////////////////////////////////////////////////
bool StringsEqual(const wchar_t* s1, const wchar_t* s2)
{
	return (s1 && s2 && (wcscmp(s1, s2) == 0));
}

//////////////////////////////////////////////////////////////////////////
bool operator == (
	const LicenseKeyInfo::SerialNumber& sn1, 
	const LicenseKeyInfo::SerialNumber& sn2
	)
{
	return (sn1.customer_id == sn2.customer_id && 
			sn1.application_id == sn2.application_id &&
			sn1.sequence_number == sn2.sequence_number);
}

}  // namespace Test
}  // namespace KasperskyLicensing
