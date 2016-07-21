#ifndef WININETREADER_H_E16C2CCB_A948_4874_8B7F_C93E168D0FFB
#define WININETREADER_H_E16C2CCB_A948_4874_8B7F_C93E168D0FFB


#include "commonHeaders.h"
#include "mutex.h"


namespace ProxyDetectorNamespace {

// The WinInetReader class works with jsproxy.dll which allow to determine proxy server
//  address. The class wraps dll initialization and release functionality
class KAVUPDCORE_API WinInetReader
{
public:
	// constructor loads and initializes jsproxy library
	WinInetReader(const size_t networkTimeoutSeconds, KLUPD::DownloadProgress &, KLUPD::JournalInterface &, KLUPD::Log *);
	// unload library and free resources
	~WinInetReader();

    // return false in case failed to download WPAD Script
	bool downloadWPAD_Script();

    // unique wpad script file name in temporary folder
    KLUPD::Path m_wpad_LocalFullFileName;

private:
	// implementation class hidden into visibility scope of ProxySettingReader
	class ProxyGetMethod;

	// get the path to WPAD script, return false in case error
	static bool getWPAD_URL(KLUPD::Path &, KLUPD::Log *);


    // getting configuration script from configured URL
	// wpad_url [out] - buffer to receive the URL
	//  from which a WPAD autoproxy script can be downloaded
	static bool readScriptURLFromRegistry(KLUPD::Path &wpad_url, KLUPD::Log *);

	// generates unique file name
    static KLUPD::Path createUniqueTempFileName();


    static Mutex m_winInetLibraryMutex;

    const size_t m_networkTimeoutSeconds;

    KLUPD::DownloadProgress &m_downloadProgress;
    KLUPD::JournalInterface &m_journal;
    KLUPD::Log *pLog;
};


} // namespace ProxyDetectorNamespace



#endif // #ifndef WININETREADER_H_E16C2CCB_A948_4874_8B7F_C93E168D0FFB

