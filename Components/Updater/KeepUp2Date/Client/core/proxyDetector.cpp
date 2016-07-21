//
// Purpose: this file is to enable Proxy Detection
//  functionality in Updater library.

// This includes implementation of ProxyDetection library
//  and thus code is generated and included it into library

// library works only on WIN32 platform
#ifdef WIN32
	#define PROXYDETECTOR_EXPORTS

	// 4251 - needs to have dll-interface to be used by clients
	// 4786 - identifier was truncated to '255' characters in the browser information
	#pragma warning(disable: 4251 4786)

#endif

// required for all OSs
#include "../../proxyDetector/proxyDetector.cpp"

#ifdef WIN32
	#include "../../proxyDetector/registryReader.cpp"
	#include "../../proxyDetector/windowsRegistryWrapper.cpp"
    #include "../../proxyDetector/mutex.cpp"
	#include "../../proxyDetector/JSProxyWrapper.cpp"
	#include "../../proxyDetector/winInetReader.cpp"
#endif
