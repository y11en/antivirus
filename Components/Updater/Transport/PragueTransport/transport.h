// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  17 August 2007,  20:06 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Voronkov
// File Name   -- transport.cpp
// -------------------------------------------
// AVP Prague stamp end
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __transport_cpp__a16bb087_729f_4f9a_a288_76a9ee4a567d )
#define __transport_cpp__a16bb087_729f_4f9a_a288_76a9ee4a567d
// AVP Prague stamp end
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end
#include <ProductCore/iface/i_taskmanager.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_io.h>
#include <Updater/transport.h>
#include "p_transportimplementation.h"
#include "PragueDownloadProgress.h"
#include "../../SharedCode/PragueHelpers/PragueLog.h"
#include "../../SharedCode/PragueHelpers/PragueEnvironmentWrapper.h"
#include "../ProxyServerAddressDetector/i_proxyserveraddressdetector.h"
#include "HttpTransport.h"
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable Transport : public cTransport 
{
private:
// Internal function declarations

// Property function declarations
	tERROR pr_call getUserAgent( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setUserAgent( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getHttpHeader( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setHttpHeader( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call httpGetFile( hSTRING p_url, hIO p_destinationIO );
	tERROR pr_call httpCloseSession();
	tERROR pr_call httpPost( hSTRING p_url, hSTRING p_request, tVOID* p_postData, tDWORD p_postDataSize );

// Data declaration
    hTASKMANAGER                m_tm;             // --
    hPROXYSERVERADDRESSDETECTOR m_proxyDetector;  // --
    cTask *                     m_task;           // --
    tDWORD                      m_networkTimeout; // --
// AVP Prague stamp end
public:
    Transport();
    ~Transport();
private:
    // helper to get string data from hSTRING structure
    void prepareProxyAddress(cProxySettings &input, const KLUPD::Path &destinationUrl,
        KLUPD::Address &result, KLUPD::AuthorizationTypeList &);
    tERROR httpRequest(const KLUPD::HttpProtocol::Method &, const KLUPD::Path &server, const KLUPD::Path &request, hIO p_destinationIO);
    PRAGUE_HELPERS::PragueEnvironmentWrapper m_pragueEnvironmentWrapper;
    PRAGUE_HELPERS::PragueLog m_log;
    KLUPD::Log *pLog;
    PragueDownloadProgress m_pragueDownloadProgress;
    HttpTransport *m_httpTransport;
    // User-Agent protepry storage
    std::vector<char> m_userAgent;
    // HTTP header of received message
    std::vector<char> m_httpHeader;
    std::vector<unsigned char> m_postData;
    // folder where data files for update operation are located
    static const char *s_updaterTemporarySubfolder;
// AVP Prague stamp begin( C++ class declaration end,  )
public:
    mDECLARE_INITIALIZATION(Transport)
};
// AVP Prague stamp end
// AVP Prague stamp begin( Header endif,  )
#endif // transport_cpp
// AVP Prague stamp end
