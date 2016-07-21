#include "JSProxyWrapper.h"

#include "proxyDetector.h"

#include "windowsRegistryWrapper.h"

namespace ProxyDetectorNamespace {

class JSProxyWrapper::JSProxyWrapperImplementation
{
public:
    JSProxyWrapperImplementation();
    ~JSProxyWrapperImplementation();

    KLUPD::ProxyServerMode internetGetProxyInfo(const KLUPD::Path &wpad_LocalFullFileName,
        KLUPD::Address &proxyAddress, const KLUPD::Path &targetHTTPResource, KLUPD::NoCaseString &resultReadableDescription);

    // saves initialization error to output it later on automatic proxy server address detection
    KLUPD::NoCaseString m_readableState;

private:
    // perform initialization
    bool initialize(const KLUPD::Path &wpad_LocalFullFileName);

	// this type structure is needed to get function addresses from jsproxy.dll
	struct AutoProxyHelperFunctions
	{
		const struct AutoProxyHelperVtbl *lpVtbl;
	};
	// this type structure is needed to get function addresses from jsproxy.dll
	struct AutoProxyScriptBuffer
	{
		DWORD m_structSize;
		LPSTR m_buffer;
		DWORD m_bufferSize;
	};

	// defined type of function for initialize library API
	typedef BOOL(CALLBACK *Function_InitializeDll)(DWORD dwVersion, LPSTR lpszDownloadedTempFile,
		LPSTR lpszMime, AutoProxyHelperFunctions *lpAutoProxyCallbacks, AutoProxyScriptBuffer *lpAutoProxyScriptBuffer);
	// defined type of function for de-initialize library API
	typedef BOOL (CALLBACK *Function_DeInitializeDll)(LPSTR lpszMime, DWORD dwReserved);
	
	// defined type of function to get proxy settings
	typedef BOOL(CALLBACK *Function_GetProxyInfo)(LPCSTR lpszUrl,
		DWORD dwUrlLength, LPSTR lpszUrlHostName, DWORD dwUrlHostNameLength,
		LPSTR *lplpszProxyHostName, LPDWORD lpdwProxyHostNameLength);


    // wrapper for InternetInitializeAutoProxyDll() function call from jsproxy.dll
	bool InternetInitializeAutoProxyDll(const KLUPD::Path &wpad_LocalFullFileName);

	/// helper function which parse address obtained with WinInet InternetGetProxyInfo() function
	static KLUPD::ProxyServerMode getFirstProxyFromString(const std::string &input,
        KLUPD::Address &proxyAddress, KLUPD::NoCaseString &resultReadableDescription);


    Function_InitializeDll m_functionInitializeDll;
    Function_DeInitializeDll m_functionDeInitializeDll;
    Function_GetProxyInfo m_functionGetProxyInfo;

    HMODULE m_module;

};

}   // namespace ProxyDetectorNamespace

ProxyDetectorNamespace::JSProxyWrapper::JSProxyWrapperImplementation::JSProxyWrapperImplementation()
 : m_functionDeInitializeDll(0),
   m_module(0)
{
}

ProxyDetectorNamespace::JSProxyWrapper::JSProxyWrapperImplementation::~JSProxyWrapperImplementation()
{
	if(m_functionDeInitializeDll)
		m_functionDeInitializeDll(0, 0);
	if(m_module)
		::FreeLibrary(m_module);
}

KLUPD::ProxyServerMode ProxyDetectorNamespace::JSProxyWrapper::JSProxyWrapperImplementation::internetGetProxyInfo(
    const KLUPD::Path &wpad_LocalFullFileName,
    KLUPD::Address &proxyAddress,
    const KLUPD::Path &targetHTTPResource,
    KLUPD::NoCaseString &resultReadableDescription)
{
    if(!initialize(wpad_LocalFullFileName))
    {
        resultReadableDescription = m_readableState + L"Proxy server is not going to be used, because jsproxy error state for '"
            + targetHTTPResource.toWideChar() + L"'";
        return KLUPD::directConnection;
    }

	// get hostname from address
    KLUPD::Address parsedSource(targetHTTPResource);
	if(parsedSource.m_hostname.empty())
    {
        resultReadableDescription = m_readableState + L"Failed to detect proxy server address, because host name is empty for '"
            + targetHTTPResource.toWideChar() + L"'";
        return KLUPD::directConnection;
    }


    char *proxyString = 0;
    DWORD proxyHostNameLength = 0;
    if(!m_functionGetProxyInfo(targetHTTPResource.toAscii().c_str(), targetHTTPResource.size(),
        const_cast<LPSTR>(parsedSource.m_hostname.toAscii().c_str()), parsedSource.m_hostname.size(),
            &proxyString, &proxyHostNameLength))
	{
        const int lastError = GetLastError();
        resultReadableDescription = m_readableState + L"Failed to get proxy information for '"
            + targetHTTPResource.toWideChar()
            + L"', target host '" + parsedSource.m_hostname.toWideChar()
            + L"', last error " + KLUPD::errnoToString(lastError, KLUPD::windowsStyle) + L" |";

        return KLUPD::directConnection;
	}

	// library was not initialized
	if(!proxyString)
    {
        const int lastError = GetLastError();
        resultReadableDescription = m_readableState + L"Automatic proxy detection library is not initialized for '"
            + targetHTTPResource.toWideChar() + L"', last error " + KLUPD::errnoToString(lastError, KLUPD::windowsStyle) + L" |";

        return KLUPD::directConnection;
    }

	const KLUPD::ProxyServerMode result = getFirstProxyFromString(proxyString, proxyAddress, resultReadableDescription);

    // ignore return code
    GlobalFree(proxyString);
    return result;
}

bool ProxyDetectorNamespace::JSProxyWrapper::JSProxyWrapperImplementation::initialize(const KLUPD::Path &wpad_LocalFullFileName)
{
	// load library failed
    m_module = ::LoadLibrary(_T("jsproxy.dll"));
	if(!m_module)
    {
        const int lastError = GetLastError();
        m_readableState += KLUPD::NoCaseString(L"Failed to load 'jsproxy.dll', last error ")
            + KLUPD::errnoToString(lastError, KLUPD::windowsStyle) + L" |";
        return false;
    }

	// resolving function addresses
	m_functionInitializeDll = reinterpret_cast<Function_InitializeDll>(GetProcAddress(m_module, "InternetInitializeAutoProxyDll"));
	m_functionGetProxyInfo = reinterpret_cast<Function_GetProxyInfo>(GetProcAddress(m_module, "InternetGetProxyInfo"));

	// function were not found in module
	if(!m_functionInitializeDll || !m_functionGetProxyInfo)
    {
        const int lastError = GetLastError();
        m_readableState += KLUPD::NoCaseString(L"Failed get procedure address 'InternetInitializeAutoProxyDll', 'InternetGetProxyInfo', last error ")
            + KLUPD::errnoToString(lastError, KLUPD::windowsStyle) + L" |";
        return false;
    }

	// jsproxy.dll initialization
	if(!InternetInitializeAutoProxyDll(wpad_LocalFullFileName))
        return false;

	// initialization successful,
	//  now saving API address to perform de-initialization jsproxy.dll library
	m_functionDeInitializeDll = reinterpret_cast<Function_DeInitializeDll>(GetProcAddress(m_module, "InternetDeInitializeAutoProxyDll"));

    return true;
}


bool ProxyDetectorNamespace::JSProxyWrapper::JSProxyWrapperImplementation::InternetInitializeAutoProxyDll(const KLUPD::Path &wpad_LocalFullFileNameIn)
{
    KLUPD::Path wpad_LocalFullFileName = wpad_LocalFullFileNameIn;
    wpad_LocalFullFileName.convertPathToWindowsPlatformFormat();

    // coping filename to non-const memory, because the InternetInitializeAutoProxyDll()
    //  requires pointer to non-const memory

    // WARNING: WPAD parse can work only with ASCII paths
    const std::string asciiPath = wpad_LocalFullFileName.toAscii();
    std::vector<char> wpad_LocalFullFileNameCopy(asciiPath.begin(), asciiPath.end());

    if(wpad_LocalFullFileNameCopy.empty())
    {
        m_readableState += L"Failed to initialize JSProxy library, because empty WPAD file name |";
        return false;
    }
    wpad_LocalFullFileNameCopy.push_back(0);

    // initialization jsproxy.dll with just downloaded WPAD script file
    if(!!m_functionInitializeDll(0, &wpad_LocalFullFileNameCopy[0], 0, 0, 0))
        return true;

    const int lastError = GetLastError();
    m_readableState += KLUPD::NoCaseString(L"Failed initialize internet automatic get proxy settings library, WPAD script '")
        + wpad_LocalFullFileNameIn.toWideChar()
        + L"', last error " + KLUPD::errnoToString(lastError, KLUPD::windowsStyle) + L" |";

    return false;
}


KLUPD::ProxyServerMode ProxyDetectorNamespace::JSProxyWrapper::JSProxyWrapperImplementation::getFirstProxyFromString(
    const std::string &input,
    KLUPD::Address &proxyAddress,
    KLUPD::NoCaseString &resultReadableDescription)
{
    // proxy type symbolic constants according to
     // http://wp.netscape.com/eng/mozilla/2.0/relnotes/demo/proxy-live.html
    const std::string DIRECT = "DIRECT";
    const std::string PROXY = "PROXY";

    std::string::size_type offset = input.find(PROXY);
	if(offset != input.npos)
	{
		// skip keyword and all spaces following it
		offset += PROXY.size();
		while(' ' == input[offset] || '\t' == input[offset])
			++offset;

    // extract proxy path
		std::string::size_type posSemicolon = input.find(';', offset);
        if(posSemicolon == std::string::npos)
            posSemicolon = input.size();

        std::string strAddr(input, offset, posSemicolon - offset);
        if(strAddr.find("http://") == std::string::npos)
            strAddr.insert(0, "http://");

        KLUPD::Path pathProxy;
        pathProxy.fromAscii(strAddr);
        pathProxy.correctPathDelimiters();

        if( proxyAddress.parse(pathProxy) )
            return KLUPD::withProxy;
	}

    // direct connection
	if(input.find(DIRECT) != input.npos)
    {
        resultReadableDescription += KLUPD::NoCaseString(L"Direct connection configuration is determined from WPAD script '")
            + KLUPD::asciiToWideChar(input) + L"'";
        return KLUPD::directConnection;
    }
    
    resultReadableDescription += KLUPD::NoCaseString(L"Failed to get proxy address from string automatically '")
        + KLUPD::asciiToWideChar(input) + L"'";
	return KLUPD::directConnection;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

ProxyDetectorNamespace::JSProxyWrapper::CacheItem::CacheItem(const KLUPD::ProxyServerMode &result, const KLUPD::Address &address)
 : m_result(result),
   m_address(address)
{
}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

ProxyDetectorNamespace::Mutex ProxyDetectorNamespace::JSProxyWrapper::m_JSProxyLibraryMutex("KASPERSKY_JSPROXY_MUTEX_84C4E19A_4F1F_4386_A33E_52E1362BF586", false);


ProxyDetectorNamespace::JSProxyWrapper::JSProxyWrapper()
{
}

KLUPD::ProxyServerMode ProxyDetectorNamespace::JSProxyWrapper::internetGetProxyInfo(const KLUPD::Path &wpad_LocalFullFileName,
    KLUPD::Address &proxyAddress, const KLUPD::Path &targetHTTPResource, KLUPD::NoCaseString &resultReadableDescription)
{
    // jsproxy.dll may be accesses only from single thread
    Mutex::Lock lock(m_JSProxyLibraryMutex);

    // check cache first
    Cache::const_iterator cachedItem = m_cache.find(targetHTTPResource);
    if(cachedItem != m_cache.end())
    {
        proxyAddress = cachedItem->second.m_address;
        return cachedItem->second.m_result;
    }

    const KLUPD::ProxyServerMode result = JSProxyWrapperImplementation().internetGetProxyInfo(wpad_LocalFullFileName,
        proxyAddress, targetHTTPResource, resultReadableDescription);

    // cache resolved address
    m_cache[targetHTTPResource] = CacheItem(result, proxyAddress);

    return result;
}
