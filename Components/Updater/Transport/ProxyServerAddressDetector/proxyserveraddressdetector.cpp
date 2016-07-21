// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Saturday,  28 April 2007,  13:24 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Voronkov
// File Name   -- proxyserveraddressdetector.cpp
// -------------------------------------------
// AVP Prague stamp end
// AVP Prague stamp begin( Interface version,  )
#define ProxyServerAddressDetector_VERSION ((tVERSION)1)
// AVP Prague stamp end
// AVP Prague stamp begin( Includes for interface,  )
#include "proxyserveraddressdetector.h"
// AVP Prague stamp end
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ProxyServerAddressDetector". Static(shared) property table variables
// AVP Prague stamp end
// AVP Prague stamp begin( External (user called) interface method implementation, getProxyAddress )
// Parameters are:
tERROR ProxyServerAddressDetectorImplementation::getProxyAddress( cProxySettings* p_proxyAddress, const cStringObj* p_destinationUrl )
{
    if(!m_mutex)
    {
        TRACE_MESSAGE("Proxy server detector mutex is not available");
        return errPARAMETER_INVALID;
    }

    cAutoCS autoCSState(m_mutex, false);

    const KLUPD::NoCaseString destinationUrl = p_destinationUrl->data();

    // bypass proxy server for local addresses
    if(!!p_proxyAddress->m_bUseProxy && !!p_proxyAddress->m_bypassProxyServerForLocalAddresses)
    {
        KLUPD::NoCaseString localAddressAnalysisResult;
        const bool localAddress = m_proxyDetector.IsLocalAddressEx(KLUPD::Address(destinationUrl).m_hostname.m_value, localAddressAnalysisResult);
        TRACE_MESSAGE3("Address locality check result %S: %S", localAddress ? L"positive" : L"negative", localAddressAnalysisResult.toWideChar());
        if(localAddress)
        {
            p_proxyAddress->m_bUseProxy = cFALSE;
            return errOK;
        }
    }

    // automatic proxy server address detection
    KLUPD::Address proxyAddress;
    if(!!p_proxyAddress->m_bUseProxy && !!p_proxyAddress->m_bUseIEProxySettings)
    {
        if(m_proxyDetector.getProxyAddress(proxyAddress, destinationUrl) == KLUPD::directConnection)
        {
            p_proxyAddress->m_bUseProxy = cFALSE;
            return errOK;
        }

        p_proxyAddress->m_strProxyHost = proxyAddress.m_hostname.toWideChar();
        p_proxyAddress->m_nProxyPort = atoi(proxyAddress.m_service.toAscii().c_str());
    }

    return errOK;
}
// AVP Prague stamp end
ProxyServerAddressDetectorImplementation::ProxyServerAddressDetectorImplementation()
 : m_log("** transport ** ", static_cast<tTRACE_LEVEL>(351)),
   pLog(&m_log),
   m_proxyDetector(PRAGUE_HELPERS::networkTimeoutSeconds(m_pragueEnvironmentWrapper),
        m_emptyDownloadProgress, m_emptyJournal, &m_log),
   m_mutex(0)
{
    const tERROR mutexCreateResult = g_root->sysCreateObjectQuick((hOBJECT *)&m_mutex, IID_CRITICAL_SECTION);
    if(PR_FAIL(mutexCreateResult))
    {
        TRACE_MESSAGE2("Failed to create proxy server object critical section, result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(mutexCreateResult).c_str());
    }
}
ProxyServerAddressDetectorImplementation::~ProxyServerAddressDetectorImplementation()
{
    if(m_mutex)
        m_mutex->sysCloseObject();
}

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "ProxyServerAddressDetector". Register function
tERROR ProxyServerAddressDetectorImplementation::Register( hROOT root ) 
{
    tERROR error;
// AVP Prague stamp end
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(ProxyServerAddressDetector)
mINTERNAL_TABLE_END(ProxyServerAddressDetector)
// AVP Prague stamp end
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(ProxyServerAddressDetector)
    mEXTERNAL_METHOD(ProxyServerAddressDetector, getProxyAddress)
mEXTERNAL_TABLE_END(ProxyServerAddressDetector)
// AVP Prague stamp end
// AVP Prague stamp begin( Registration call,  )

    PR_TRACE_FUNC_FRAME_( *root, "ProxyServerAddressDetector::Register method", &error );

    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_PROXYSERVERADDRESSDETECTOR,         // interface identifier
        PID_PROXYSERVERADDRESSDETECTORIMPLEMENTATION,// plugin identifier
        0x00000000,                             // subtype identifier
        ProxyServerAddressDetector_VERSION,     // interface version
        VID_VORONKOV,                           // interface developer
        &i_ProxyServerAddressDetector_vtbl,                         // internal(kernel called) function table
        (sizeof(i_ProxyServerAddressDetector_vtbl)/sizeof(tPTR)),   // internal function table size
        &e_ProxyServerAddressDetector_vtbl,     // external function table
        (sizeof(e_ProxyServerAddressDetector_vtbl)/sizeof(tPTR)),// external function table size
        NULL,                                   // property table
        0,                                      // property table size
        sizeof(ProxyServerAddressDetectorImplementation)-sizeof(cObjImpl),// memory size
        0                                       // interface flags
    );

    #ifdef _PRAGUE_TRACE_
        if ( PR_FAIL(error) )
            PR_TRACE( (root,prtDANGER,"ProxyServerAddressDetector(IID_PROXYSERVERADDRESSDETECTOR) registered [%terr]",error) );
    #endif // _PRAGUE_TRACE_
// AVP Prague stamp end
// AVP Prague stamp begin( C++ class regitration end,  )
    return error;
}

tERROR pr_call ProxyServerAddressDetector_Register( hROOT root ) { return ProxyServerAddressDetectorImplementation::Register(root); }
// AVP Prague stamp end
// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end
