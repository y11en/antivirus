#if !defined(AFX_WINDOWSREGISTRYWRAPPER_H__0B377A4F_648F_4B38_85CC_404A3D31C98F__INCLUDED_)
#define AFX_WINDOWSREGISTRYWRAPPER_H__0B377A4F_648F_4B38_85CC_404A3D31C98F__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "commonHeaders.h"

namespace ProxyDetectorNamespace {

/// here are wrapper functions to work with Windows Registry are placed
class WindowsRegistryWrapper
{
public:
    // helper class implements scope lock idiom for HANDLE type
    class AutoCloseHandle
    {
    public:
        AutoCloseHandle(HANDLE = 0);
        ~AutoCloseHandle();

        HANDLE m_handle;

    private:
        // disable copy operations
        AutoCloseHandle &operator=(const AutoCloseHandle &);
        AutoCloseHandle(const AutoCloseHandle &);
    };


	/// these are Wrappers for RegQueryValueEx() function which incapsulates
	///  work with size (you do not need to pass buffer size)
	/// @param const HKEY [in] - handle to an open registry key
	/// @param valueName [in] - pointer to a 0-terminated string containing the name of the value to query
	/// @param value [out] - pointer to a buffer that receives the value's data
	static bool readRegistryBinary(const HKEY, const std::string &valueName, std::vector<unsigned char> &value, KLUPD::Log *);
	static bool readRegistryString(const HKEY, const std::string &valueName, std::string &value, KLUPD::Log *);
	static bool readRegistryDword(const HKEY, const std::string &valueName, DWORD &value, KLUPD::Log *);

    // applicaiton may impersonate only single thread to run
      // under user, but run itself under other user or system account
     // This helper API is envelop to open registry correctly
    // In case failed to open key using thread token, then opens registry key using process impersonation
    static bool openCurrentThreadUserRegistryKey(const char *subKey, HKEY &phkkResult, KLUPD::Log *);

    // helper API, to get SID string for current thread
    static bool getSidStringForCurrentThread(std::string &sidString, KLUPD::NoCaseString &resultDescription);
};


}	// namespace ProxyDetectorNamespace

#endif // !defined(AFX_WINDOWSREGISTRYWRAPPER_H__0B377A4F_648F_4B38_85CC_404A3D31C98F__INCLUDED_)
