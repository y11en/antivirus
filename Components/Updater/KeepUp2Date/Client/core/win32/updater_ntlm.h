#ifndef UPDATER_NTLM_H_INCLUDED_5DC15883_C5F6_4fb1_8988_915078513F10
#define UPDATER_NTLM_H_INCLUDED_5DC15883_C5F6_4fb1_8988_915078513F10


#include "../../helper/comdefs.h"

#ifdef WIN32
    #define SECURITY_WIN32

    #include <sspi.h>
    #include <winnt.h>
#endif


namespace KLUPD {

class NtlmImplementation
{
public:
    NtlmImplementation(Log *);
    ~NtlmImplementation();

    bool makeChallenge(std::vector<char> &challenge, const std::string &domain, const std::string &user, const std::string &password);
    bool makeResponse(const char *challenge, std::vector<char> &response);


#ifdef WIN32    // no implementation for non-Windows platforms

private:
    bool loadSecurityLibrary();

    bool getAuthorizationIdentity(_SEC_WINNT_AUTH_IDENTITY &, const std::string &domain, const std::string &user, const std::string &password);

    void releasePreviousCredentialsIfNeeded();
    void releasePreviousSecurityContextIfNeeded();

    /// new security context is only obtained in case credentials changed or not obtained before
    bool needAquireCredentials(const std::string &domain, const std::string &user, const std::string &password);


    HINSTANCE m_securityDll;
    PSecurityFunctionTable m_funcionTable;
    
    bool m_credentialsObtained;
    CredHandle m_credentials;
    bool m_securityContextObtained;
    CtxtHandle m_context;

    // internal buffer to store data for Windows API
    std::vector<char> m_internalDomainBuffer;
    std::vector<char> m_internalUserBuffer;
    std::vector<char> m_internalPasswordBuffer;

#endif  // WIN32

    Log *pLog;
};

}   // namespace KLUPD

#endif  // #ifndef UPDATER_NTLM_H_INCLUDED_5DC15883_C5F6_4fb1_8988_915078513F10
