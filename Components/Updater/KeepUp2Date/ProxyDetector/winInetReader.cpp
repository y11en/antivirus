#include "winInetReader.h"

#include "proxyDetector.h"
#include "windowsRegistryWrapper.h"

/////////////////////////////////////////////////////////////////
/// WinInetReader::ProxyGetMethod

// this class gets proxy settings read method from registry and provides interface
//  to determine which way should be used to obtain Proxy Settings from OS
class ProxyDetectorNamespace::WinInetReader::ProxyGetMethod
{
public:
	ProxyGetMethod(KLUPD::Log *);

	// returns true in case WinInet API should be used to obtain proxy server settings
	bool automaticProxyDetection();
	// returns true proxy server settings should be read from Windows Registry
	bool proxyDetectionURL();
	// returns true if no method is found
	bool empty();

private:
	// read from registry data about proxy server address:
	//  whether proxy address is stored in registry or should be obtained with WinInet API
	void read();

	unsigned char m_proxyGetMethod;
    KLUPD::Log *pLog;
};

ProxyDetectorNamespace::WinInetReader::ProxyGetMethod::ProxyGetMethod(KLUPD::Log *log)
 : m_proxyGetMethod(0),
   pLog(log)
{
	read();
}
bool ProxyDetectorNamespace::WinInetReader::ProxyGetMethod::automaticProxyDetection()
{
    const unsigned char magic = 1 << 3;
	return !!(m_proxyGetMethod & magic);
}
bool ProxyDetectorNamespace::WinInetReader::ProxyGetMethod::proxyDetectionURL()
{
    const unsigned char magic = 1 << 2;
	return !!(m_proxyGetMethod & magic);
}
bool ProxyDetectorNamespace::WinInetReader::ProxyGetMethod::empty()
{
    return !automaticProxyDetection() && !proxyDetectionURL();
}

void ProxyDetectorNamespace::WinInetReader::ProxyGetMethod::read()
{
    HKEY key = 0;
    const std::string registryKey("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Connections");

    if(!WindowsRegistryWrapper::openCurrentThreadUserRegistryKey(registryKey.c_str(), key, pLog))
    {
        TRACE_MESSAGE2("Failed get proxy method, unable to open registry key '%s'", registryKey.c_str());
        m_proxyGetMethod = 0;
        return;
    }

    const std::string settingsKey("DefaultConnectionSettings");
    std::vector<unsigned char> settingsValue;
    const bool error = !WindowsRegistryWrapper::readRegistryBinary(key, settingsKey, settingsValue, pLog);
    RegCloseKey(key);

    if(error || settingsValue.size() < 9)
    {
        TRACE_MESSAGE2("Failed get proxy method, unable to read registry key '%s'", settingsKey.c_str());
        m_proxyGetMethod = 0;
        return;
    }

    m_proxyGetMethod = settingsValue[8];
}



/////////////////////////////////////////////////////////////////
/// WinInetReader

ProxyDetectorNamespace::Mutex ProxyDetectorNamespace::WinInetReader::m_winInetLibraryMutex("KASPERSKY_WININET_LIBRARY_MUTEX_03F7018E_BD05_40db_B65B_1F8FD7726974", false);


ProxyDetectorNamespace::WinInetReader::WinInetReader(const size_t networkTimeoutSeconds,
    KLUPD::DownloadProgress &downloadProgress, KLUPD::JournalInterface &journal, KLUPD::Log *log)
 : m_networkTimeoutSeconds(networkTimeoutSeconds),
   m_downloadProgress(downloadProgress),
   m_journal(journal),
   pLog(log)
{
}
ProxyDetectorNamespace::WinInetReader::~WinInetReader()
{
   	// removing temporary file
    if(!m_wpad_LocalFullFileName.empty())
        KLUPD::LocalFile(m_wpad_LocalFullFileName, pLog).unlink();
}

bool ProxyDetectorNamespace::WinInetReader::getWPAD_URL(KLUPD::Path &wpad_url, KLUPD::Log *pLog)
{
    ProxyGetMethod proxyGetMethod(pLog);

    if(proxyGetMethod.automaticProxyDetection() || proxyGetMethod.empty())
	{
	    if(proxyGetMethod.empty())
        {
            TRACE_MESSAGE("Warning: no proxy get methods detected, the DHCP and DNS request will be tried for proxy server address detection");
        }
        else
            TRACE_MESSAGE("Trying to detect proxy server address with DHCP and DNS requests");

		std::vector<char> wpad_urlTmp(1024, 0);

        // the DetectAutoProxyUrl() from Wininet.dll is not thread-safe
        {
            Mutex::Lock lock(m_winInetLibraryMutex);
            
		    // failed to determine the location of a WPAD autoproxy script
		    if(!DetectAutoProxyUrl(&wpad_urlTmp[0], wpad_urlTmp.size() - 1, 0xFFFFFFFF))
            {
                const int lastError = GetLastError();
                TRACE_MESSAGE2("Failed to detect automatic proxy address, last error %S",
                    KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
                wpad_url.erase();
			    return false;
            }
        }

        wpad_url.fromAscii(&wpad_urlTmp[0]);

    }
	else if(proxyGetMethod.proxyDetectionURL()
		&& !readScriptURLFromRegistry(wpad_url, pLog))
	{
        TRACE_MESSAGE("Failed to read the location of a WPAD autoproxy script from Windows Registry");
        wpad_url.erase();
        return false;
	}

    return !wpad_url.empty();
}

bool ProxyDetectorNamespace::WinInetReader::downloadWPAD_Script()
{
    // buffer to receive the URL from which a WPAD autoproxy script can be downloaded
    KLUPD::Path wpad_url;
    if(!getWPAD_URL(wpad_url, pLog))
        return false;

    /////////////////////////////////////////////
    /// preparing parameters
    KLUPD::Address parsedSource(wpad_url);
    if(parsedSource.m_hostname.empty())
    {
        TRACE_MESSAGE2("Failed to get WPAD script file name from URL '%S'", wpad_url.toWideChar());
        return false;
    }
    if(parsedSource.m_fileName.empty())
    {
        TRACE_MESSAGE2("Failed to get file name from WPAD full path '%S'", wpad_url.toWideChar());
        return false;
    }
	
    // filling source
    KLUPD::Source source(KLUPD::toProtocolPrefix(parsedSource.m_protocol) + parsedSource.m_hostname, UST_UserURL, false);
	
    // information about downloading file
    {
        m_wpad_LocalFullFileName = createUniqueTempFileName();
        TRACE_MESSAGE2("WPAD script file name '%S'", m_wpad_LocalFullFileName.toWideChar());

        const KLUPD::CoreError unlinkResult = KLUPD::LocalFile(m_wpad_LocalFullFileName, pLog).unlink();
        if(!KLUPD::isSuccess(unlinkResult))
        {
            TRACE_MESSAGE3("Failed to unlink temporary file to store WPAD script '%S', result '%S'",
                m_wpad_LocalFullFileName.toWideChar(), KLUPD::toString(unlinkResult).toWideChar());
            return false;
        }

        // file name in temporary folder to download WPAD script into
        const KLUPD::Path pathToGetFolder(m_wpad_LocalFullFileName);
        if(!KLUPD::createFolder(pathToGetFolder.getFolderFromPath(), pLog))
        {
            TRACE_MESSAGE2("Failed to create temporary folder for proxy detector '%S'",
                m_wpad_LocalFullFileName.toWideChar());
            return false;
        }
    }

    /////////////////////////////////////////////
    /// initialize transport functionality
    KLUPD::HttpProtocolMemoryImplementation http(KLUPD::AuthorizationTypeList(),
        KLUPD::Credentials(), m_networkTimeoutSeconds, m_downloadProgress, m_journal, pLog);

    /////////////////////////////////////////////
    /// downloading WPAD-script
    KLUPD::Address serverAddress(source.m_url);
    const KLUPD::CoreError getFileResult = http.getFile(
        parsedSource.m_fileName, parsedSource.m_path,
        serverAddress,
            "Kaspersky Proxy-Server detection agent", // identity information
            false, KLUPD::Address(), // no proxy
            KLUPD::AuthorizationTypeList()); // no authorization

    if(getFileResult == KLUPD::CORE_NO_ERROR)
    {
        m_journal.publishMessage(KLUPD::EVENT_FILE_DOWNLOADED, wpad_url.toWideChar());

        // storing file to disk
        const KLUPD::CoreError wpadSaveResult = KLUPD::LocalFile(m_wpad_LocalFullFileName, pLog).write(http.m_fileInMemory);
        if(!KLUPD::isSuccess(wpadSaveResult))
        {
            TRACE_MESSAGE2("Failed to save WPAD script file '%S'",
                m_wpad_LocalFullFileName.toWideChar());
            m_journal.publishMessage(wpadSaveResult, m_wpad_LocalFullFileName.toWideChar());
            return false;
        }
    }
    else
        m_journal.publishMessage(getFileResult, wpad_url.toWideChar());

    return getFileResult == KLUPD::CORE_NO_ERROR;
}

bool ProxyDetectorNamespace::WinInetReader::readScriptURLFromRegistry(KLUPD::Path &wpad_url, KLUPD::Log *pLog)
{
    HKEY key = 0;
    const std::string internetSettingsRegistryKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
    if(!WindowsRegistryWrapper::openCurrentThreadUserRegistryKey(internetSettingsRegistryKey.c_str(), key, pLog))
    {
        TRACE_MESSAGE2("Failed get WPAD script URL location, unable to open registry key '%s'", internetSettingsRegistryKey.c_str());
        return false;
    }

    const char *autoConfigUrlLiteral = "AutoConfigURL";
    std::string url;
    const bool ret = WindowsRegistryWrapper::readRegistryString(key, autoConfigUrlLiteral, url, pLog);
    wpad_url.fromAscii(url);

    RegCloseKey(key);

    if(!ret)
    {
        TRACE_MESSAGE2("Failed get WPAD script URL location, unable to read registry key '%s'", autoConfigUrlLiteral);
        return false;
    }
    return true;
}

KLUPD::Path ProxyDetectorNamespace::WinInetReader::createUniqueTempFileName()
{
	const DWORD bufferPathLength = 2048;
	TCHAR tempFilePath[bufferPathLength];

	if(!::GetTempPath(bufferPathLength, tempFilePath))
	{
		// using current folder in case unable to determine temporary folder
		_tcscpy(tempFilePath, _T(".\\"));
	}

	TCHAR tempFileName[bufferPathLength];
	if(::GetTempFileName(tempFilePath, _T("wpad_script"), 0, tempFileName))
        return KLUPD::tStringToWideChar(tempFileName);

	// failed to generate unique file name
    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << time(0) << ".wpad_script";

    KLUPD::Path path = KLUPD::tStringToWideChar(tempFilePath);
    path.correctPathDelimiters();
    return path.m_value + KLUPD::asciiToWideChar(stream.str());
}
