/*
*/

#if !defined(__MSISUPPORT_CA_LOGGING_H)
#define __MSISUPPORT_CA_LOGGING_H

//////////////////////////////////////////////////////////////////////////
#include <string>
#include "msi_record.h"

//////////////////////////////////////////////////////////////////////////
namespace MsiSupport
{
namespace Utility
{

//////////////////////////////////////////////////////////////////////////
class CALogWriter
{
public:
	CALogWriter(MSIHANDLE handle)
		: m_handle(handle)
		{ }
	bool Write(const std::basic_string<TCHAR>& msg) const
		{ return Write(m_handle, msg); }
	bool Write(const TCHAR* msg) const
		{ return Write(m_handle, msg); }
	bool Write(const Record& rec) const
		{ return Write(m_handle, rec); }
	static bool Write(MSIHANDLE hInstall, const std::basic_string<TCHAR>& msg)
		{
			Record rec(2);
			rec.SetString(0, TEXT("[1]"));
			rec.SetString(1, msg);
			return Write(hInstall, rec);
		}
	static bool Write(MSIHANDLE hInstall, const TCHAR* msg)
		{
			Record rec(2);
			rec.SetString(0, TEXT("[1]"));
			rec.SetString(1, msg);
			return Write(hInstall, rec);
		}
private:
	static bool Write(MSIHANDLE hInstall, const Record& rec)
		{
			return (::MsiProcessMessage(hInstall, INSTALLMESSAGE_INFO, rec.GetHandle()) == ERROR_SUCCESS);
		}
private:
	MSIHANDLE m_handle;
};

}  // namespace Utility
}  // namespace MsiSupport

#endif  // !defined(__MSISUPPORT_CA_LOGGING_H)
