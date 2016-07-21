#ifndef HTTP_TRANSPORT_H_INCLUDED_1C8A8BE0_600B_4d25_A27F_DE2931B30A9B
#define HTTP_TRANSPORT_H_INCLUDED_1C8A8BE0_600B_4d25_A27F_DE2931B30A9B

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include <core/NetFacHTTP.h>
#include <helper/updaterStaff.h>


#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

#include "i_transport.h"

class HttpTransport : public KLUPD::HttpProtocol
{
public:
    HttpTransport(cTransport &,
        const KLUPD::AuthorizationTypeList &authorizationTypeSupportedByClient,
        const KLUPD::Credentials &proxyAuthorizationCredentials,
        const size_t downloadTimeout,
        KLUPD::DownloadProgress &, KLUPD::Log *);

    cIO *m_destinationIO;
    size_t m_currentOffset;

protected:
    virtual void fullHttpHeaderReceived(const std::string &httpHeader);
    virtual KLUPD::CoreError dataReceived(const unsigned char *data, const size_t &);

private:
    cTransport &m_transport;
};


#endif  // HTTP_TRANSPORT_H_INCLUDED_1C8A8BE0_600B_4d25_A27F_DE2931B30A9B
