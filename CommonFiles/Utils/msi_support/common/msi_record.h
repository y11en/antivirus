/*
*/

#if !defined(__MSISUPPORT_RECORD_H)
#define __MSISUPPORT_RECORD_H

#include <vector>
#include <string>
#include <msi.h>
#include <msiquery.h>

namespace MsiSupport  {
namespace Utility  {

//////////////////////////////////////////////////////////////////////////
class Record
{
public:
	Record(size_t size)
		: m_handle(::MsiCreateRecord(size))
		{ }
	~Record()
		{
			if (m_handle)
				::MsiCloseHandle(m_handle);
		}
	MSIHANDLE GetHandle() const  
		{ return m_handle; }
	MSIHANDLE DetachHandle()
		{
			MSIHANDLE tmp = m_handle;
			m_handle = NULL;
			return tmp;
		}
	bool GetInteger(size_t index, int& value)
		{ 
			if (m_handle == NULL)
				return false;
			int tmp = ::MsiRecordGetInteger(m_handle, index);
			if (tmp == MSI_NULL_INTEGER)
				return false;
			value = tmp;
			return true;
		}
	bool SetInteger(size_t index, int value)
		{ 
			return (m_handle != NULL &&
					::MsiRecordSetInteger(m_handle, index, value) == ERROR_SUCCESS);
		}
	bool GetString(size_t index, std::basic_string<TCHAR>& value)
		{ 
			if (m_handle == NULL)
				return false;
			DWORD buf_size = 64;
			std::vector<TCHAR> buf(buf_size, TEXT('\0'));
			switch (::MsiRecordGetString(m_handle, index, &buf[0], &buf_size))
			{
				case ERROR_SUCCESS:
					break;
				case ERROR_MORE_DATA:
					std::vector<TCHAR>(buf_size, TEXT('\0')).swap(buf);
					if (::MsiRecordGetString(m_handle, index, &buf[0], &buf_size) != ERROR_SUCCESS)
						return false;
					break;
				default:
					return false;
			}
			value.assign(&buf[0], buf_size);
			return true;
		}
	bool SetString(size_t index, const std::basic_string<TCHAR>& value)
		{ 
			return (m_handle != NULL &&
					::MsiRecordSetString(
									m_handle, 
									index, 
									(value.empty() ? TEXT("") : value.c_str())
									) == ERROR_SUCCESS);
		}
	bool SetString(size_t index, const TCHAR* value)
		{ 
			return (m_handle != NULL &&
					::MsiRecordSetString(m_handle, index, value) == ERROR_SUCCESS);
		}
private:
	MSIHANDLE m_handle;
};

}  // namespace Utility
}  // namespace MsiSupport

#endif  // !defined(__MSISUPPORT_RECORD_H)
