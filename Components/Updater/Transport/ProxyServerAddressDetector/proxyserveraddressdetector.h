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
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __proxyserveraddressdetector_cpp__62949850_18a9_4bc4_80f0_624312a7d546 )
#define __proxyserveraddressdetector_cpp__62949850_18a9_4bc4_80f0_624312a7d546
// AVP Prague stamp end
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_csect.h>
// AVP Prague stamp end
#include "p_proxyserveraddressdetectorimplementation.h"
#include "../../SharedCode/PragueHelpers/PragueHelpers.h"
#include "../../SharedCode/PragueHelpers/PragueLog.h"
#include "../../KeepUp2Date/ProxyDetector/proxydetector.h"
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable ProxyServerAddressDetectorImplementation : public cProxyServerAddressDetector 
{
private:
// Internal function declarations

// Property function declarations

public:
// External function declarations
	tERROR pr_call getProxyAddress( cProxySettings* p_proxyAddress, const cStringObj* p_destinationUrl );

// Data declaration
// AVP Prague stamp end
public:
    ProxyServerAddressDetectorImplementation();
    ~ProxyServerAddressDetectorImplementation();
private:
    PRAGUE_HELPERS::PragueEnvironmentWrapper m_pragueEnvironmentWrapper;
    PRAGUE_HELPERS::PragueLog m_log;
    KLUPD::Log *pLog;
// TODO: there is no ability to stop immediatelly download WPAD script,
 // until checkCancel() is implemented in m_emptyDownloadProgress
    KLUPD::EmptyDownloadProgress m_emptyDownloadProgress;
    KLUPD::EmptyJournal m_emptyJournal;
    ProxyDetectorNamespace::ProxyDetector m_proxyDetector;

    hCRITICAL_SECTION m_mutex;
// AVP Prague stamp begin( C++ class declaration end,  )
public:
    mDECLARE_INITIALIZATION(ProxyServerAddressDetectorImplementation)
};
// AVP Prague stamp end
// AVP Prague stamp begin( Header endif,  )
#endif // proxyserveraddressdetector_cpp
// AVP Prague stamp end
