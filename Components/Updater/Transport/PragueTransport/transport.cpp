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
// AVP Prague stamp begin( Interface version,  )
#define Transport_VERSION ((tVERSION)1)
// AVP Prague stamp end
// AVP Prague stamp begin( Includes for interface,  )
#include "transport.h"
// AVP Prague stamp end
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_io.h>
// AVP Prague stamp end
#include "../../SharedCode/PragueHelpers/PragueHelpers.h"
#include "p_transportimplementation.h"
#include <Prague/pr_cpp.h>
#include <helper/updaterStaff.h>
const char *Transport::s_updaterTemporarySubfolder = "Updater/Temporary Files/";
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Transport". Static(shared) property table variables
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getUserAgent )
// Interface "Transport". Method "Get" for property(s):
//  -- TRANSPORT_USER_AGENT
tERROR Transport::getUserAgent( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = m_userAgent.size();
    if(!buffer || !size)
        return errOK;

    if(!m_userAgent.empty())
    {
        if(size < m_userAgent.size())
            return errBUFFER_TOO_SMALL;
        memcpy_s(buffer, size, &m_userAgent[0], m_userAgent.size());
    }
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setUserAgent )
// Interface "Transport". Method "Set" for property(s):
//  -- TRANSPORT_USER_AGENT
tERROR Transport::setUserAgent( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = size;
    m_userAgent.assign(buffer, buffer + size);
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getHttpHeader )
// Interface "Transport". Method "Get" for property(s):
//  -- TRANSPORT_HTTP_HEADER
tERROR Transport::getHttpHeader( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = m_httpHeader.size();
    if(!buffer || !size)
        return errOK;

    if(!m_httpHeader.empty())
    {
        if(size < m_httpHeader.size())
            return errBUFFER_TOO_SMALL;
        memcpy_s(buffer, size, &m_httpHeader[0], m_httpHeader.size());
    }
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setHttpHeader )
// Interface "Transport". Method "Set" for property(s):
//  -- TRANSPORT_HTTP_HEADER
tERROR Transport::setHttpHeader( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = size;
    m_httpHeader.assign(buffer, buffer + size);
    return errOK;
}
// AVP Prague stamp end
Transport::Transport()
 : m_log("** transport ** ", static_cast<tTRACE_LEVEL>(351)),
   pLog(&m_log),
   m_pragueDownloadProgress(&m_log),
   m_httpTransport(0),
   m_tm(0),
   m_proxyDetector(0),
   m_task(0),
   m_networkTimeout(60)
{
    m_pragueDownloadProgress.m_transport = this;
}
Transport::~Transport()
{
    delete m_httpTransport;
}
void Transport::prepareProxyAddress(cProxySettings &proxySettingsRequest,
                                    const KLUPD::Path &destinationUrl,
                                    KLUPD::Address &proxyAddress,
                                    KLUPD::AuthorizationTypeList &authorizationTypes)
{
    cStringObj destinationUrlPragueString = destinationUrl.toWideChar();
    const tERROR getProxyAddressResult = m_proxyDetector->getProxyAddress(&proxySettingsRequest, &destinationUrlPragueString);
    if(PR_FAIL(getProxyAddressResult))
    {
        TRACE_MESSAGE2("Failed to prepare server proxy address, result %s",
            PRAGUE_HELPERS::toStringPragueResult(getProxyAddressResult).c_str());
        proxySettingsRequest.m_bUseProxy = cFALSE;
        return;
    }
    // proxy server is not used
    if(!proxySettingsRequest.m_bUseProxy)
        return;


    proxyAddress.m_hostname = proxySettingsRequest.m_strProxyHost.data();

    std::wostringstream strService;
    strService << proxySettingsRequest.m_nProxyPort;
    proxyAddress.m_service = strService.str();

    proxyAddress.m_credentials.userName(proxySettingsRequest.m_strProxyLogin.data());
    std::string resultExplanation;
    proxyAddress.m_credentials.password(PRAGUE_HELPERS::cryptPassword(PRAGUE_HELPERS::decrypt,
        m_tm, proxySettingsRequest.m_strProxyPassword, resultExplanation));
    if(!resultExplanation.empty())
        TRACE_MESSAGE(resultExplanation.c_str());
    // proxy authorization
	if(proxySettingsRequest.m_bProxyAuth)
	{
		authorizationTypes.push_back(KLUPD::ntlmAuthorizationWithCredentials);
		authorizationTypes.push_back(KLUPD::basicAuthorization);
	}
	else
	{
        proxyAddress.m_credentials.clear();
		authorizationTypes.push_back(KLUPD::basicAuthorization);
		authorizationTypes.push_back(KLUPD::ntlmAuthorization);
		authorizationTypes.push_back(KLUPD::ntlmAuthorizationWithCredentials);
	}
}
tERROR Transport::httpRequest(const KLUPD::HttpProtocol::Method &method, const KLUPD::Path &server, const KLUPD::Path &request, hIO p_hDestination)
{
    if(!m_tm)
    {
        TRACE_MESSAGE("Failed to get file, because obligatory Task Manager parameter is not set");
        return errPARAMETER_INVALID;
    }
    if(!m_proxyDetector)
    {
        TRACE_MESSAGE("Failed to get file, because obligatory Proxy Detector parameter is not set");
        return errPARAMETER_INVALID;
    }
//    if(!m_task)
//    {
//        // task parameter is required to request proxy server
//         // settings from Bussiness Logic component correctly
//        TRACE_MESSAGE("Failed to get file, because obligatory Task parameter is not set");
//        return errPARAMETER_INVALID;
//    }
    // user may have called closeHttpSession(), then re-create HTTP transport object
    if(m_pragueDownloadProgress.cancelDownload())
    {
        delete m_httpTransport;
        m_httpTransport = 0;
        m_pragueDownloadProgress.cancelDownload(false);
    }

    KLUPD::Path url = server;
    KLUPD::Address proxyAddress;
    KLUPD::AuthorizationTypeList authorizationTypes;
    cProxySettings proxySettingsRequest;
    const tERROR requestProxySettingsResult = this->sysSendMsg(pmc_TRANSPORT,
        pm_REQEST_PROXY_SETTINGS, 0, &proxySettingsRequest, SER_SENDMSG_PSIZE);
    if(PR_SUCC(requestProxySettingsResult))
    {
        TRACE_MESSAGE2("Analysing proxy server configuration from product: %s",
            PRAGUE_HELPERS::toString(proxySettingsRequest).c_str());
        prepareProxyAddress(proxySettingsRequest, url, proxyAddress, authorizationTypes);
        TRACE_MESSAGE2("Analysed proxy server: %s", PRAGUE_HELPERS::toString(proxySettingsRequest).c_str());
    }
    else
    {
        proxySettingsRequest.m_bUseProxy = cFALSE;
        TRACE_MESSAGE2("Direct connection (no proxy), because failed to request proxy server settings, result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(requestProxySettingsResult).c_str());
    }


    KLUPD::Address serverAddress(url);
    serverAddress.m_fileName += request;

    TRACE_MESSAGE2("Downloading file: %S", serverAddress.toString().toWideChar());

#ifdef WIN32
	char buffer[256];
	DWORD size = 250;
	memset(buffer, 0, 256);
	if(GetUserNameA(buffer, &size))
    {
		TRACE_MESSAGE2("Current user: %s", buffer);
    }
	else
    {
        const int lastError = GetLastError();
		TRACE_MESSAGE2("Unable to determine current user, last error %d", lastError);
    }
#endif


    if(!m_httpTransport)
    {
        const size_t defaultNetworkTimeout = m_networkTimeout ? m_networkTimeout : 60;
        m_httpTransport = new HttpTransport(*this,
            authorizationTypes, proxyAddress.m_credentials,
            PRAGUE_HELPERS::networkTimeoutSeconds(m_pragueEnvironmentWrapper, defaultNetworkTimeout),
            m_pragueDownloadProgress, pLog);
    }

    m_httpTransport->m_destinationIO = p_hDestination;
    m_httpTransport->m_currentOffset = 0;

    const KLUPD::CoreError downloadResult =
        (method == KLUPD::HttpProtocol::get)
        ?
            m_httpTransport->getFile(
                serverAddress.m_fileName, KLUPD::Path(),
                serverAddress,
                m_userAgent.empty() ? "" : std::string(&m_userAgent[0], m_userAgent.size()),
                !!proxySettingsRequest.m_bUseProxy, proxyAddress,
                authorizationTypes, 0)
        :
            m_httpTransport->postFile(
                m_postData,
                serverAddress.m_fileName, KLUPD::Path(),
                serverAddress,
                m_userAgent.empty() ? "" : std::string(&m_userAgent[0], m_userAgent.size()),
                !!proxySettingsRequest.m_bUseProxy, proxyAddress,
                authorizationTypes);

    if(KLUPD::isSuccess(downloadResult))
        return errOK;

    TRACE_MESSAGE2("Failed to download file via HTTP Transport, result '%S'",
        KLUPD::toString(downloadResult).toWideChar());
    return PRAGUE_HELPERS::translateUpdaterResultCodeToPrague(downloadResult);
}
// AVP Prague stamp begin( External (user called) interface method implementation, httpGetFile )
// Parameters are:
tERROR Transport::httpGetFile( hSTRING p_strURL, hIO p_hDestination )
{
    return httpRequest(KLUPD::HttpProtocol::get, cStringObj(p_strURL).data(), KLUPD::Path(), p_hDestination);
}
// AVP Prague stamp end
// AVP Prague stamp begin( External (user called) interface method implementation, httpCloseSession )
// Parameters are:
tERROR Transport::httpCloseSession()
{
    TRACE_MESSAGE("Request to cancel download obtained");
    m_pragueDownloadProgress.cancelDownload(true);
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( External (user called) interface method implementation, httpPost )
// Parameters are:
tERROR Transport::httpPost( hSTRING p_url, hSTRING p_request, tVOID* p_postData, tDWORD p_postDataSize )
{
    m_postData.assign(static_cast<const unsigned char *>(p_postData), static_cast<const unsigned char *>(p_postData) + p_postDataSize);
    return httpRequest(KLUPD::HttpProtocol::post, cStringObj(p_url).data(), cStringObj(p_request).data(), 0);
}
// AVP Prague stamp end
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Transport". Register function
tERROR Transport::Register( hROOT root ) 
{
    tERROR error;
// AVP Prague stamp end
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Transport_PropTable)
    mpLOCAL_PROPERTY_BUF( pgTRANSPORT_TM, Transport, m_tm, cPROP_BUFFER_READ_WRITE )
    mpLOCAL_PROPERTY_BUF( pgPROXY_DETECTOR, Transport, m_proxyDetector, cPROP_BUFFER_READ_WRITE )
    mpLOCAL_PROPERTY_FN( pgTRANSPORT_USER_AGENT, FN_CUST(getUserAgent), FN_CUST(setUserAgent) )
    mpLOCAL_PROPERTY_BUF( pgTRANSPORT_TASK, Transport, m_task, cPROP_BUFFER_READ_WRITE )
    mpLOCAL_PROPERTY_BUF( pgTRANSPORT_NETWORK_TIMEOUT_SECONDS, Transport, m_networkTimeout, cPROP_BUFFER_READ_WRITE )
    mpLOCAL_PROPERTY_FN( pgTRANSPORT_HTTP_HEADER, FN_CUST(getHttpHeader), FN_CUST(setHttpHeader) )
mpPROPERTY_TABLE_END(Transport_PropTable)
// AVP Prague stamp end
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Transport)
mINTERNAL_TABLE_END(Transport)
// AVP Prague stamp end
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Transport)
    mEXTERNAL_METHOD(Transport, httpGetFile)
    mEXTERNAL_METHOD(Transport, httpCloseSession)
    mEXTERNAL_METHOD(Transport, httpPost)
mEXTERNAL_TABLE_END(Transport)
// AVP Prague stamp end
// AVP Prague stamp begin( Registration call,  )
    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_TRANSPORT,                          // interface identifier
        PID_TRANSPORTIMPLEMENTATION,            // plugin identifier
        0x00000000,                             // subtype identifier
        Transport_VERSION,                      // interface version
        VID_VORONKOV,                           // interface developer
        &i_Transport_vtbl,                          // internal(kernel called) function table
        (sizeof(i_Transport_vtbl)/sizeof(tPTR)),    // internal function table size
        &e_Transport_vtbl,                      // external function table
        (sizeof(e_Transport_vtbl)/sizeof(tPTR)),// external function table size
        Transport_PropTable,                    // property table
        mPROPERTY_TABLE_SIZE(Transport_PropTable),// property table size
        sizeof(Transport)-sizeof(cObjImpl),     // memory size
        0                                       // interface flags
    );

	#ifdef _PRAGUE_TRACE_
		if(PR_FAIL(error))
			PR_TRACE((root, prtDANGER, "Prague Transport Wrapper (IID_TRANSPORT) registered [%terr]", error));
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end
// AVP Prague stamp begin( C++ class regitration end,  )
    return error;
}

tERROR pr_call Transport_Register( hROOT root ) { return Transport::Register(root); }
// AVP Prague stamp end
// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end
