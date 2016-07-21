#ifndef REGISTRYREADER_H_5CB1F1CC_DB28_4b3f_B566_05A242C2B61B
#define REGISTRYREADER_H_5CB1F1CC_DB28_4b3f_B566_05A242C2B61B

// 4786 - identifier was truncated to '255' characters in the browser information
#pragma warning(disable:4786)
#include <string>

#include "../Client/include/cfg_updater2.h"

namespace KLUPD
{
    // forward declaration
    class Log;
}

namespace ProxyDetectorNamespace {

// this class contains a set of functions to read proxy settings from Windows Registry
class RegistryReader
{
public:
    static bool get(KLUPD::Address &proxyAddress, const KLUPD::Path &destinationUrl, KLUPD::Log *);

private:
    // reads proxy server string from registry
    static bool readStringFromRegistry(std::string &, KLUPD::Log *);
    // parses proxy server string which was read from Windows registry
    static void parseString(const std::string &, KLUPD::Address &proxyAddress, const KLUPD::Path &destinationUrl, KLUPD::Log *);
};


} // namespace ProxyDetectorNamespace


#endif // #ifndef REGISTRYREADER_H_5CB1F1CC_DB28_4b3f_B566_05A242C2B61B

