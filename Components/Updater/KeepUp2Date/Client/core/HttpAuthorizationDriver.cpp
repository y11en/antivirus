#include "HttpAuthorizationDriver.h"

namespace {
    const char *g_authorizationPrefixBasic = "Basic";
    const char *g_authorizationPrefixNTLM  = "NTLM";
}

KLUPD::HttpAuthorizationDriver::HttpAuthorizationDriver(
    const AuthorizationTypeList &authorizationTypeSupportedByClient,
    const Credentials &proxyAuthorizationCredentials,
    Log *log)
 : m_authorized(false),
   m_currentAuthorizationType(noAuthorization),
   m_ntlmState(challenge),
   m_NtlmImplementation(log),
   m_credentials(proxyAuthorizationCredentials),
   m_supportedTypesByClient(authorizationTypeSupportedByClient),
   pLog(log)
{
}

void KLUPD::HttpAuthorizationDriver::resetAuthorizatoinState(const AuthorizationTypeList &authorizationTypeSupportedByClient)
{
    resetNtlmState();
    m_currentAuthorizationType = noAuthorization;
    m_serverAuthorizationToken.erase();
    m_supportedTypesByServer.clear();

    m_supportedTypesByClient = authorizationTypeSupportedByClient;
}

void KLUPD::HttpAuthorizationDriver::resetNtlmState()
{
    m_authorized = false;
    m_ntlmState = challenge;
    m_serverAuthorizationToken.erase();
}

void KLUPD::HttpAuthorizationDriver::credentials(const Credentials &credentials)
{
    resetNtlmState();
    m_credentials = credentials;
}
KLUPD::Credentials KLUPD::HttpAuthorizationDriver::credentials()const
{
    return m_credentials;
}

void KLUPD::HttpAuthorizationDriver::setNtlmAuthorizationToken(const char *token)
{
    if(strlen(token) < strlen(g_authorizationPrefixNTLM))
    {
        TRACE_MESSAGE2("Failed to construct NTLM challenge, because server token is not NTLM ('%s')", token);
        return;
    }

    token += strlen(g_authorizationPrefixNTLM);
    while(*token == ' ' || *token == '\t')
        ++token;

    m_serverAuthorizationToken = token;
}

KLUPD::AuthorizationType KLUPD::HttpAuthorizationDriver::currentAuthorizationType()const
{
    return m_currentAuthorizationType;
}

void KLUPD::HttpAuthorizationDriver::currentAuthorizationType(const AuthorizationType &authorizationType)
{
    m_currentAuthorizationType = authorizationType;
}

void KLUPD::HttpAuthorizationDriver::addAuthorizationTypeSupportedByServer(const AuthorizationType &type)
{
	if 
		( 
			std::find( m_supportedTypesByServer.begin(), m_supportedTypesByServer.end(), type ) 
			!= 
			m_supportedTypesByServer.end() 
		)
		return;

    m_supportedTypesByServer.push_back(type);

    // in case server supports Ntlm Authorization, then it supports all kind of Ntlm authorization
    if(type == ntlmAuthorization)
        m_supportedTypesByServer.push_back(ntlmAuthorizationWithCredentials);
    else if(type == ntlmAuthorizationWithCredentials)
        m_supportedTypesByServer.push_back(ntlmAuthorization);
}

KLUPD::AuthorizationTypeList KLUPD::HttpAuthorizationDriver::supportedAuthorizationTypesByServer()const
{
    return m_supportedTypesByServer;
}

KLUPD::AuthorizationTypeList KLUPD::HttpAuthorizationDriver::supportedAuthorizationTypesByClient()const
{
    return m_supportedTypesByClient;
}

bool KLUPD::HttpAuthorizationDriver::switchToNextAuthorization(bool &needUpdateCredentials, bool &ntlmAuthorizationTriedAlready)
{
    needUpdateCredentials = false;

    if(switchToNextAuthorizationState() || switchToNextAuthorizationType(false, ntlmAuthorizationTriedAlready))
        return true;

    needUpdateCredentials = true;
	if ( m_credentials.cancelled() )
	{
		m_credentials.cancel( false );
	}

    return switchToNextCredentialsNeeded(ntlmAuthorizationTriedAlready);
}
bool KLUPD::HttpAuthorizationDriver::switchToNextAuthorizationState()
{
    // Ntlm authorization
    if((m_currentAuthorizationType == ntlmAuthorization)
        || (m_currentAuthorizationType == ntlmAuthorizationWithCredentials))
    {
        // switch to 'response' state
        if((m_ntlmState == challenge) && (!m_serverAuthorizationToken.empty()))
        {
            m_ntlmState = response;
            return true;
        }
    }

    // no more state for current proxy authorization type
    return false;
}
bool KLUPD::HttpAuthorizationDriver::switchToNextAuthorizationType(const bool searchMethodWithCredentials,
                                                                   bool &ntlmAuthorizationTriedAlready)
{
    m_ntlmState = challenge;

    while(true)
    {
		bool authFound = false;

        switch ( m_currentAuthorizationType )
        {
        case noAuthorization:
            if(ntlmAuthorizationTriedAlready)
            {
                // if Ntlm without credentials already tried, then skip this method, because it failed already,
                 // credentials are not going to be re-requested thus this authorization method is not going to succeed
                m_currentAuthorizationType = ntlmAuthorizationWithCredentials;
            }
            else
            {
                m_currentAuthorizationType = ntlmAuthorization;
                ntlmAuthorizationTriedAlready = true;
            }
			authFound = true;
            break;

        case ntlmAuthorization:
            m_currentAuthorizationType = ntlmAuthorizationWithCredentials;
			authFound = true;
            break;

        case ntlmAuthorizationWithCredentials:
			// Try only if current type (i.e. NTLM) is not supported
			if ( !authorizationMethodSupported( m_currentAuthorizationType ) )
			{
				m_currentAuthorizationType = basicAuthorization;
				authFound = true;
			}
            break;
        }

		if ( !authFound )
		{
			TRACE_MESSAGE("No more authorization type is supported both by server and client, authorization state reset");
			// authorization will be tried from the beginning
			m_currentAuthorizationType = noAuthorization;
			return false;
		}

		// We can't use authorization types that require credentials
		// if credentials are marked as cancelled
		if ( m_credentials.cancelled() )
		{
			switch ( m_currentAuthorizationType )
			{
			case basicAuthorization:
			case ntlmAuthorizationWithCredentials:
				{
					TRACE_MESSAGE( "Credentials are cancelled, authorization state reset" );
					m_currentAuthorizationType = noAuthorization;
					return false;
				}
			}
		}

        if(authorizationMethodSupported(m_currentAuthorizationType))
        {
            if(searchMethodWithCredentials)
            {
                if(credentialsRequired(m_currentAuthorizationType))
                {
                    TRACE_MESSAGE2("Switch to '%S' is performed, user name is going to be asked from product",
                        toString(m_currentAuthorizationType).toWideChar());
                    return true;
                }
            }
            else
            {
                // credentials are not required
                if(!credentialsRequired(m_currentAuthorizationType))
                {
                    TRACE_MESSAGE2("Switch to '%S' is performed", toString(m_currentAuthorizationType).toWideChar());
                    return true;
                }

                // credentials are required and we have it
                if(!m_credentials.userName().empty())
                {
                    TRACE_MESSAGE3("Switch to '%S' is performed, user name '%S'",
                        toString(m_currentAuthorizationType).toWideChar(), m_credentials.userName().toWideChar());
                    return true;
                }
            }
        }
    }
}
bool KLUPD::HttpAuthorizationDriver::switchToNextCredentialsNeeded(bool &ntlmAuthorizationTriedAlready)
{
    m_currentAuthorizationType = noAuthorization;

    // switch to next authorization supported both by client and server
    if(!switchToNextAuthorizationType(true, ntlmAuthorizationTriedAlready))
    {
        TRACE_MESSAGE("Credentials for authorization are not requested because there is no authorization method supported both by client and server");
        return false;
    }

    return true;
}

void KLUPD::HttpAuthorizationDriver::authorized(const bool fileReceivedSuccessfully)
{
    m_authorized = fileReceivedSuccessfully;
}


bool KLUPD::HttpAuthorizationDriver::makeProxyAuthorizationHeader(std::string &result)
{
    result.erase();

    switch(m_currentAuthorizationType)
    {
    case noAuthorization:
        return true;

    case basicAuthorization:
		if ( m_credentials.empty() )
		{
			return false;
		}

		return makeBasicAuthorizationString(result);

    case ntlmAuthorization:
    case ntlmAuthorizationWithCredentials:
        if(m_authorized)
            return true;

		if 
			( 
				m_credentials.empty() 
				&& 
				( m_currentAuthorizationType == ntlmAuthorizationWithCredentials ) 
			)
		{
			return false;
		}

        if(m_ntlmState == challenge)
            return makeNtlmChallenge(result);
        else if(m_ntlmState == response)
            return makeNtlmResponse(result);

        break;
    }

    TRACE_MESSAGE2("Failed to prepare authorization header for '%S' authorization",
        toString(m_currentAuthorizationType).toWideChar());
    return false;
}

bool KLUPD::HttpAuthorizationDriver::authorizationMethodSupported(const AuthorizationType &type)const
{
    // check authorization method supported by client
    if(std::find(m_supportedTypesByClient.begin(), m_supportedTypesByClient.end(), type) == m_supportedTypesByClient.end())
        return false;

    // check authorization method supported by server
    if(std::find(m_supportedTypesByServer.begin(), m_supportedTypesByServer.end(), type) != m_supportedTypesByServer.end())
        return true;

    // in case server supports Ntlm authorization then it supports Ntlm with Credentials either
    bool supportedByServer = false;
    if(type == ntlmAuthorizationWithCredentials)
    {
        supportedByServer = std::find(m_supportedTypesByServer.begin(), m_supportedTypesByServer.end(),
            ntlmAuthorization) != m_supportedTypesByServer.end();
    }
    else if(type == ntlmAuthorization)
    {
        supportedByServer = std::find(m_supportedTypesByServer.begin(), m_supportedTypesByServer.end(),
            ntlmAuthorizationWithCredentials) != m_supportedTypesByServer.end();
    }
    return supportedByServer;
}

bool KLUPD::HttpAuthorizationDriver::makeBasicAuthorizationString(std::string &result)const
{
    result.erase();

    const std::string basicAuthorizationString = m_credentials.userName().toAscii()
        + ":" + m_credentials.password().toAscii();
    const std::vector<char> resultBuffer = CBase64::encodeBuffer(reinterpret_cast<const unsigned char *>
        (basicAuthorizationString.c_str()), basicAuthorizationString.length());

    if(!resultBuffer.empty())
        result = std::string(g_authorizationPrefixBasic) + " " + &resultBuffer[0];
    return true;
}

bool KLUPD::HttpAuthorizationDriver::makeNtlmChallenge(std::string &result)
{
    result.erase();

    // user name and domain stays empty in case AUT_Ntlm
    NoCaseString user;
    NoCaseString domain;
    if(m_currentAuthorizationType == ntlmAuthorizationWithCredentials)
    {
        size_t delimeterOffset = m_credentials.userName().find(L"\\");
        if(delimeterOffset == NoCaseStringImplementation::npos)
            user = m_credentials.userName();
        else
        {
            domain.assign(m_credentials.userName().toWideChar(), delimeterOffset);
            user.assign(m_credentials.userName().toWideChar() + delimeterOffset + 1);
        }
    }

    std::vector<char> base64EncodedCredentials;
    if(!m_NtlmImplementation.makeChallenge(base64EncodedCredentials,
        domain.toAscii(), user.toAscii(), m_credentials.password().toAscii()))
    {
        TRACE_MESSAGE("Failed to construct NTLM authorization request");
        return false;
    }

    result = std::string(g_authorizationPrefixNTLM) + " " + &base64EncodedCredentials[0];
    return true;
}
bool KLUPD::HttpAuthorizationDriver::makeNtlmResponse(std::string &result)
{
    result.erase();

    std::vector<char> base64EncodedCredentials;
    if(!m_NtlmImplementation.makeResponse(m_serverAuthorizationToken.c_str(), base64EncodedCredentials))
    {
        TRACE_MESSAGE2("Failed to make NTLM authorization response for user '%S'",
            m_credentials.toString().toWideChar());
        return false;
    }

    result = std::string(g_authorizationPrefixNTLM) + " " + &base64EncodedCredentials[0];
    return true;
}
