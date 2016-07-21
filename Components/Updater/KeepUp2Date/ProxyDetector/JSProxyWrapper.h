#ifndef JSPROXYWRAPPER_H_2A2AD578_931D_4713_AD24_96744ADD2346
#define JSPROXYWRAPPER_H_2A2AD578_931D_4713_AD24_96744ADD2346

#include "commonHeaders.h"

namespace ProxyDetectorNamespace {

class KAVUPDCORE_API JSProxyWrapper
{
public:
    JSProxyWrapper();

    KLUPD::ProxyServerMode internetGetProxyInfo(const KLUPD::Path &wpad_LocalFullFileName,
        KLUPD::Address &proxyAddress, const KLUPD::Path &targetHTTPResource, KLUPD::NoCaseString &resultReadableDescription);

private:
    // disable copy operations
    JSProxyWrapper &operator=(const JSProxyWrapper &);
    JSProxyWrapper(const JSProxyWrapper &);

    // hide implementation
    class JSProxyWrapperImplementation;

    struct CacheItem
    {
        CacheItem(const KLUPD::ProxyServerMode & = KLUPD::directConnection, const KLUPD::Address & = KLUPD::Address());

        KLUPD::ProxyServerMode m_result;
        KLUPD::Address m_address;
    };

    typedef std::map<KLUPD::Path, CacheItem> Cache;
    // cache for all already resolved addresses
    Cache m_cache;


    // user can not create instance of class, because there is a possibility of misuse jsporxy library
     // Warning: after tests it was found that
      // 1. jsproxy can not be initialized on static member initialization,
      // 2. jsproxy can not be used from different threads simultaneously
      // 3. the thread calling jsproxy must be the same that initialized and de-initialized the jsproxy library
    // All restrictions above prevents from keeping jsporxy libarary initialized through whole update session
    static Mutex m_JSProxyLibraryMutex;
};



} // namespace ProxyDetectorNamespace


#endif // #ifndef JSPROXYWRAPPER_H_2A2AD578_931D_4713_AD24_96744ADD2346
