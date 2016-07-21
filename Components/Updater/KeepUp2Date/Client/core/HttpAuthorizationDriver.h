#ifndef HTTPAUTHORIZATIONDRIVER_H_INCLUDED_7CC2759E_4879_4f24_836D_5A28FA74B634
#define HTTPAUTHORIZATIONDRIVER_H_INCLUDED_7CC2759E_4879_4f24_836D_5A28FA74B634

#if _MSC_VER > 1000
    #pragma once
#endif

#include "../helper/stdinc.h"
#include "win32/updater_ntlm.h"

namespace KLUPD {

// this helper class provides data and helper funcitons to deal with HTTP authorization
class KAVUPDCORE_API HttpAuthorizationDriver
{
public:
    HttpAuthorizationDriver(const AuthorizationTypeList &authorizationTypeSupportedByClient,
        const Credentials &proxyAuthorizationCredentials, Log *);
    void resetAuthorizatoinState(const AuthorizationTypeList &authorizationTypeSupportedByClient);
    void resetNtlmState();

    void credentials(const Credentials &);
    Credentials credentials()const;
    void setNtlmAuthorizationToken(const char *);
    AuthorizationType currentAuthorizationType()const;
    void currentAuthorizationType(const AuthorizationType &);

    // server may support different authorization methods
    void addAuthorizationTypeSupportedByServer(const AuthorizationType &);
    AuthorizationTypeList supportedAuthorizationTypesByServer()const;
    AuthorizationTypeList supportedAuthorizationTypesByClient()const;

    // set state to authorized
    void authorized(const bool fileReceivedSuccessfully);

    // return current authorization header
    bool makeProxyAuthorizationHeader(std::string &);

    bool switchToNextAuthorization(bool &needUpdateCredentials, bool &ntlmAuthorizationTriedAlready);


private:
    enum NtlmState
    {
        challenge,
        response,
    };

    // return false switch to next authorization type must be performed
    bool switchToNextAuthorizationState();
    // return false in case there is no authorization type left
    bool switchToNextAuthorizationType(const bool searchMethodWithCredentials, bool &ntlmAuthorizationTriedAlready);
    // request authorization credentials if needed
    bool switchToNextCredentialsNeeded(bool &ntlmAuthorizationTriedAlready);

    bool authorizationMethodSupported(const AuthorizationType &)const;

    // makes basic authorization encoded string to pass on HTTP server
    bool makeBasicAuthorizationString(std::string &)const;
    bool makeNtlmChallenge(std::string &);
    bool makeNtlmResponse(std::string &);



    bool m_authorized;
    AuthorizationType m_currentAuthorizationType;

    NtlmState m_ntlmState;
    NtlmImplementation m_NtlmImplementation;

    Credentials m_credentials;
    std::string m_serverAuthorizationToken;

    AuthorizationTypeList m_supportedTypesByServer;
    AuthorizationTypeList m_supportedTypesByClient;

    Log *pLog;
};

}   // namespace KLUPD

#endif  // HTTPAUTHORIZATIONDRIVER_H_INCLUDED_7CC2759E_4879_4f24_836D_5A28FA74B634
