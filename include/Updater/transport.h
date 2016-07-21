#ifndef S_TRANSPORT_H_INCLUDED_8DB8225E_2483_4451_8BED_5BCD580EA977
#define S_TRANSPORT_H_INCLUDED_8DB8225E_2483_4451_8BED_5BCD580EA977

#include <Prague/prague.h>
#include <Prague/pr_serializable.h>

#include <ProductCore/structs/s_taskmanager.h>

// updater sends a message class
#define PID_TRANSPORT  ((tPID)(24006))

 // transport message class
#define pmc_TRANSPORT 0x8ee7a4af
    // request proxy address settings
    #define pm_REQEST_PROXY_SETTINGS 0x00001200


enum
{
    // cProxySettings structure identifier
    transportStruture_ProxySettings = 1,
    // cProxyCredentialsRequest structure identifier
    transportStruture_ProxyCredentials = 2,
};

enum
{
    // request proxy authorization credentials message
    transportRequest_ProxyCredentials = pmc_TRANSPORT,
};


// proxy server configuration structure
struct cProxySettings : public cSerializable
{
	DECLARE_IID(cProxySettings, cSerializable, PID_TRANSPORT, transportStruture_ProxySettings);

    cProxySettings(
                const tBOOL useProxy = cTRUE,
                const tBOOL automaticallyDetectProxyAddress = cTRUE,
                const tBOOL bypassProxyServerForLocalAddresses = cTRUE,
                const cStringObj &proxyHost = cStringObj(),
                const tWORD proxyPort = 80,
                const cStringObj &proxyLogin = cStringObj(),
                const cStringObj &encryptedPassword = cStringObj(),
                const tBOOL proxyAuthorization = cFALSE)
        : m_bUseProxy(useProxy),
          m_bUseIEProxySettings(automaticallyDetectProxyAddress),
          m_bypassProxyServerForLocalAddresses(bypassProxyServerForLocalAddresses),
          m_strProxyHost(proxyHost),
          m_nProxyPort(proxyPort),
          m_strProxyLogin(proxyLogin),
          m_strProxyPassword(encryptedPassword),
          m_bProxyAuth(proxyAuthorization)
	{
	}

	tBOOL       m_bUseProxy;
    // automatically detect proxy server address
	tBOOL       m_bUseIEProxySettings;
	tBOOL       m_bypassProxyServerForLocalAddresses;
	cStringObj  m_strProxyHost;
	tWORD       m_nProxyPort;
	cStringObj  m_strProxyLogin;
    // encrypted password for authorization on proxy server
	cStringObj  m_strProxyPassword;
	tBOOL       m_bProxyAuth;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProxySettings, "ProxySettings" )
	SER_VALUE ( m_bUseProxy,                   tid_BOOL,             "UseProxy" )
	SER_VALUE ( m_bUseIEProxySettings,         tid_BOOL,             "UseIEProxySettings" )
	SER_VALUE ( m_bypassProxyServerForLocalAddresses, tid_BOOL,      "BypassProxyServerForLocalAddresses" )
	SER_VALUE ( m_strProxyHost,                tid_STRING_OBJ,       "ProxyHost" )
	SER_VALUE ( m_nProxyPort,                  tid_WORD,             "ProxyPort" )
	SER_VALUE ( m_strProxyLogin,               tid_STRING_OBJ,       "ProxyLogin" )
	SER_PASSWORD ( m_strProxyPassword,                               "ProxyPassword" )
	SER_VALUE ( m_bProxyAuth,                  tid_BOOL,             "ProxyAuth" )
IMPLEMENT_SERIALIZABLE_END();





//-------------------------------------------------------------------
// cProxyCredentialsRequest structure
struct cProxyCredentialsRequest : public cTaskHeader
{
	cProxyCredentialsRequest()
        : m_bSave(cTRUE)
	{
	}
	
	DECLARE_IID(cProxyCredentialsRequest, cTaskHeader, PID_TRANSPORT, transportStruture_ProxyCredentials);

	cProxySettings *m_settings;
	tBOOL m_bSave;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cProxyCredentialsRequest, "ProxyCredentialsRequest")
	SER_BASE( cTaskHeader,  0 )
	SER_VALUE_PTR_NO_NAMESPACE(m_settings, cProxySettings::eIID, "settings")
	SER_VALUE(m_bSave, tid_BOOL, "Save")
IMPLEMENT_SERIALIZABLE_END();


#endif  // S_TRANSPORT_H_INCLUDED_8DB8225E_2483_4451_8BED_5BCD580EA977
