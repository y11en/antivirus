#include "updater_ntlm.h"

#include "../../helper/updaterStaff.h"


KLUPD::NtlmImplementation::NtlmImplementation(Log *log)
 :
#ifdef WIN32    // no implementation for non-Windows platforms
   m_securityDll(0),
   m_funcionTable(0),
   m_credentialsObtained(false),
   m_securityContextObtained(false),
#endif  // WIN32
   pLog(log)
{
#ifdef WIN32    // no implementation for non-Windows platforms
    memset(&m_context, 0, sizeof(m_context));
    memset(&m_credentials, 0, sizeof(m_credentials));
#endif  // WIN32
}

KLUPD::NtlmImplementation::~NtlmImplementation()
{
#ifdef WIN32    // no implementation for non-Windows platforms
    releasePreviousCredentialsIfNeeded();
    releasePreviousSecurityContextIfNeeded();

    if(m_securityDll)
        FreeLibrary(m_securityDll);
    m_securityDll = 0;
#endif  // WIN32
}

bool KLUPD::NtlmImplementation::makeChallenge(std::vector<char> &challenge, const std::string &domain, const std::string &user, const std::string &password)
{
#ifdef WIN32    // no implementation for non-Windows platforms

    if(!loadSecurityLibrary())
        return false;

    TimeStamp expiration;
    if(needAquireCredentials(domain, user, password))
    {
        _SEC_WINNT_AUTH_IDENTITY *authorizationIdentityPointer = 0;
        _SEC_WINNT_AUTH_IDENTITY authorizationIdentity;

        if(getAuthorizationIdentity(authorizationIdentity, domain, user, password))
            authorizationIdentityPointer = &authorizationIdentity;

        releasePreviousCredentialsIfNeeded();

        SECURITY_STATUS result = m_funcionTable->AcquireCredentialsHandle(0, _T("NTLM"), SECPKG_CRED_OUTBOUND, 0, authorizationIdentityPointer, 0, 0, &m_credentials, &expiration);
        if(result != SEC_E_OK)
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE3("Failed to make NTLM challenge: failed to acquire credentials, result 0x%x, last error '%S'",
                result, errnoToString(lastError, windowsStyle).toWideChar());
            return false;
        }
        m_credentialsObtained = true;
    }

    SecBuffer secBuffer;
    // the ISC_REQ_ALLOCATE_MEMORY content requirement does not
    //   work on Windows 98, that is why buffer is allocated on stack
    char buffer[2048];
    secBuffer.cbBuffer = 2048;
    secBuffer.pvBuffer = buffer;
    secBuffer.BufferType = SECBUFFER_TOKEN;

    SecBufferDesc messageType1;
    messageType1.cBuffers = 1;
    messageType1.pBuffers = &secBuffer;
    messageType1.ulVersion = SECBUFFER_VERSION;


    unsigned long attr = 0;
    const SECURITY_STATUS result = m_funcionTable->InitializeSecurityContext(
            &m_credentials,         // handle to credentials
            0,                      // handle to partial context (should be 0 on first call)
            0,                      // target (optional)
            0,                      // context requirements
            0,                      // reserved
            SECURITY_NETWORK_DREP,  // data representation
            0,                      // security buffer description
            0,                      // reserved
            &m_context,             // handle of new context
            &messageType1,          // security buffer description
            &attr,                  // dst context attributes
            &expiration);           // expiration time (local)

    if(result != SEC_I_CONTINUE_NEEDED  // SEC_I_CONTINUE_NEEDED is not error code - reply from server is expected to perform NTLM authorization protocol
        && result != SEC_E_OK)
    {
        const int lastError = GetLastError();
        TRACE_MESSAGE3("Failed to make NTLM challenge: failed to initialize security context, result 0x%x, last error '%S'",
            result, errnoToString(lastError, windowsStyle).toWideChar());
        return false;
    }
    m_securityContextObtained = true;

    challenge = CBase64::encodeBuffer(reinterpret_cast<unsigned char *>(secBuffer.pvBuffer), secBuffer.cbBuffer);
    return true;

#else   // WIN32
    TRACE_MESSAGE("Ntlm authorization is not implemented on this platform");
    return false;
#endif  // WIN32
}

bool KLUPD::NtlmImplementation::makeResponse(const char *challenge, std::vector<char> &response)
{
#ifdef WIN32    // no implementation for non-Windows platforms
    if(!challenge)
    {
        TRACE_MESSAGE("Failed to make NTLM response: invalid input parameter challenge");
        return false;
    }

    std::vector<unsigned char> decodedChallenge = CBase64::decodeBuffer(challenge);
    if(decodedChallenge.empty())
    {
        TRACE_MESSAGE("Failed to make NTLM response: empty decoded string obtained");
        return false;
    }

    SecBuffer challengeBuffer;
    challengeBuffer.BufferType = SECBUFFER_TOKEN;
    challengeBuffer.cbBuffer = decodedChallenge.size();
    challengeBuffer.pvBuffer = &decodedChallenge[0];
    SecBufferDesc messageType2;
    messageType2.cBuffers = 1;
    messageType2.pBuffers = &challengeBuffer;
    messageType2.ulVersion = SECBUFFER_VERSION;


    SecBuffer responseBuffer;
    // the ISC_REQ_ALLOCATE_MEMORY content requirement does not
    //   work on Windows 98, that is why buffer is allocated on stack
    char buffer[2048];
    responseBuffer.cbBuffer = 2048;
    responseBuffer.pvBuffer = buffer;
    responseBuffer.BufferType = SECBUFFER_TOKEN;
    SecBufferDesc messageType3;
    messageType3.cBuffers = 1;
    messageType3.pBuffers = &responseBuffer;
    messageType3.ulVersion = SECBUFFER_VERSION;

    unsigned long attr = 0;
    TimeStamp expiration;
    const SECURITY_STATUS result = m_funcionTable->InitializeSecurityContext(
                    &m_credentials,         // handle to credentials
                    &m_context,             // handle to partial context
                    0,                      // target (ignored on second call)
                    0,                      // context requirements
                    0,                      // reserved
                    SECURITY_NETWORK_DREP,  // data representation
                    &messageType2,          // security buffer description
                    0,                      // reserved
                    &m_context,             // handle of new context
                    &messageType3,          // security buffer description
                    &attr,                  // dst context attributes
                    &expiration);           // expiration time (local)

    if(result != SEC_I_CONTINUE_NEEDED  // SEC_I_CONTINUE_NEEDED is not error code - the data exchange may be continued
        && result != SEC_E_OK)
    {
        const int lastError = GetLastError();
        TRACE_MESSAGE3("Failed to make NTLM response: failed to initialize security context, result 0x%x, last error '%S'",
            result, errnoToString(lastError, windowsStyle).toWideChar());
        return false;
    }

    response = CBase64::encodeBuffer(reinterpret_cast<unsigned char *>(responseBuffer.pvBuffer), responseBuffer.cbBuffer);
    return true;

#else   // WIN32
    TRACE_MESSAGE("Ntlm authorization is not implemented on this platform");
    return false;
#endif  // WIN32
}

#ifdef WIN32    // no implementation for non-Windows platforms

bool KLUPD::NtlmImplementation::loadSecurityLibrary()
{
    if(m_securityDll && m_funcionTable)
        return true;

    if(!m_securityDll)
    {
        m_securityDll = LoadLibrary(_T("secur32.dll"));
        if(!m_securityDll)
        {
            // on some platforms (e.g. Windows NT) there is no secur32.dll
            m_securityDll = LoadLibrary(_T("security.dll"));
            if(!m_securityDll)
            {
                const int lastError = GetLastError();
                TRACE_MESSAGE2("Failed to load both 'secur32.dll' and 'security.dll' libraries, last error '%S'",
                    errnoToString(lastError, windowsStyle).toWideChar());
                return false;
            }
        }
    }


    PSecurityFunctionTable (*getFuncTable)();
	getFuncTable = (PSecurityFunctionTable (*)(VOID))GetProcAddress(m_securityDll, tStringToAscii(SECURITY_ENTRYPOINT).c_str());

    if(!getFuncTable)
    {
        const int lastError = GetLastError();
        TRACE_MESSAGE3("Initialization NTLM functionality: failed to get function '%s' address, last error '%S'",
            tStringToAscii(SECURITY_ENTRYPOINT).c_str(), KLUPD::errnoToString(lastError, KLUPD::windowsStyle).toWideChar());
        return false;
    }

    m_funcionTable = getFuncTable();
    if(!m_funcionTable)
    {
        const int lastError = GetLastError();
        TRACE_MESSAGE3("Initialization NTLM functionality: '%s' call failed, last error '%S'",
            tStringToAscii(SECURITY_ENTRYPOINT).c_str(), errnoToString(lastError, windowsStyle).toWideChar());
        return false;
    }

    return true;
}

bool KLUPD::NtlmImplementation::getAuthorizationIdentity(_SEC_WINNT_AUTH_IDENTITY &authorizationIdentity, const std::string &domain, const std::string &user, const std::string &password)
{
#if defined(UNICODE) || defined(_UNICODE)
    typedef unsigned short CharacterType;
#else
    typedef unsigned char CharacterType;
#endif

    m_internalDomainBuffer.assign(domain.begin(), domain.end());
    authorizationIdentity.DomainLength = domain.size();
    if(!m_internalDomainBuffer.empty())
    {
        m_internalDomainBuffer.push_back(0);
        authorizationIdentity.Domain = reinterpret_cast<CharacterType *>(&m_internalDomainBuffer[0]);
    }

    m_internalUserBuffer.assign(user.begin(), user.end());
    authorizationIdentity.UserLength = user.size();
    if(m_internalUserBuffer.empty())
        return false;
    m_internalUserBuffer.push_back(0);
    authorizationIdentity.User = reinterpret_cast<CharacterType *>(&m_internalUserBuffer[0]);

    m_internalPasswordBuffer.assign(password.begin(), password.end());
    authorizationIdentity.PasswordLength = password.size();
    if(!m_internalPasswordBuffer.empty())
    {
        m_internalPasswordBuffer.push_back(0);
        authorizationIdentity.Password = reinterpret_cast<CharacterType *>(&m_internalPasswordBuffer[0]);
    }

    authorizationIdentity.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
    return true;
}

void KLUPD::NtlmImplementation::releasePreviousCredentialsIfNeeded()
{
    if(m_credentialsObtained)
        m_funcionTable->FreeCredentialsHandle(&m_credentials);
    m_credentialsObtained = false;
}
void KLUPD::NtlmImplementation::releasePreviousSecurityContextIfNeeded()
{
    if(m_securityContextObtained)
    {
        m_funcionTable->DeleteSecurityContext(&m_context);
        memset(&m_context, 0, sizeof(m_context));
    }
    m_securityContextObtained = false;
}

bool KLUPD::NtlmImplementation::needAquireCredentials(const std::string &domain, const std::string &user, const std::string &password)
{
    // no credentials were obtained before
    if(!m_credentialsObtained)
        return true;

    // no user name supplied neither at previous no at this attempt.
    //  Authorization uses credentials obtained from Windows
    if(user.empty() && m_internalUserBuffer.empty())
        return false;

    // check size to simplify content check then
    if(domain.size() != m_internalDomainBuffer.size() - 1
        || user.size() != m_internalUserBuffer.size() - 1
        || password.size() != m_internalPasswordBuffer.size() - 1)
    {
        return true;
    }

    // check credentials agains previous cached one
    return !std::equal(domain.begin(), domain.end(), m_internalDomainBuffer.begin())
        || !std::equal(user.begin(), user.end(), m_internalUserBuffer.begin())
        || !std::equal(password.begin(), password.end(), m_internalPasswordBuffer.begin());
}

#endif  // WIN32
