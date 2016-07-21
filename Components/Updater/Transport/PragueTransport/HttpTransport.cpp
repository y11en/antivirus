#include "HttpTransport.h"

#include "../../SharedCode/PragueHelpers/PragueHelpers.h"

HttpTransport::HttpTransport(cTransport &transport,
                             const KLUPD::AuthorizationTypeList &authorizationTypeSupportedByClient,
                             const KLUPD::Credentials &proxyAuthorizationCredentials,
                             const size_t downloadTimeout,
                             KLUPD::DownloadProgress &downloadProgress, KLUPD::Log *log)
 : HttpProtocol(authorizationTypeSupportedByClient, proxyAuthorizationCredentials,
            downloadTimeout, downloadProgress, log),
   m_transport(transport),
   m_destinationIO(0),
   m_currentOffset(0)
{
}

void HttpTransport::fullHttpHeaderReceived(const std::string &httpHeader)
{
    m_transport.set_pgTRANSPORT_HTTP_HEADER(reinterpret_cast<void *>(const_cast<char *>(httpHeader.c_str())),
        httpHeader.size() + 1);
}

KLUPD::CoreError HttpTransport::dataReceived(const unsigned char *data, const size_t &size)
{
    if(!m_destinationIO)
    {
        TRACE_MESSAGE("Warning: unable to write received data, no destination file is specified'");
		return KLUPD::CORE_NO_ERROR;
    }

    const tERROR writeFileResult = m_destinationIO->SeekWrite(0, m_currentOffset,
        const_cast<void *>(reinterpret_cast<const void *>(data)), size);
	if(PR_SUCC(writeFileResult))
    {
        m_currentOffset += size;
		return KLUPD::CORE_NO_ERROR;
    }

    TRACE_MESSAGE2("Failed to write received data with result '%s'",
        PRAGUE_HELPERS::toStringPragueResult(writeFileResult).c_str());
    return KLUPD::CORE_GenericFileOperationFailure;
}
