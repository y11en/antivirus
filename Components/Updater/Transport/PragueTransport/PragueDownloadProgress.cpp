#include "PragueDownloadProgress.h"

#include "../../SharedCode/PragueHelpers/PragueHelpers.h"

#include <Prague/iface/i_root.h>
#include <Updater/transport.h>

#include "transport.h"

extern hROOT g_root;

PragueDownloadProgress::PragueDownloadProgress(KLUPD::Log *log)
 : m_cancelDownloadMutex(0),
   m_cancelDownload(false),
   m_transport(0),
   pLog(log)
{
	const tERROR createMutexResult = g_root->sysCreateObjectQuick(reinterpret_cast<hOBJECT *>(&m_cancelDownloadMutex), IID_MUTEX);
	if(PR_FAIL(createMutexResult))
    {
        m_cancelDownloadMutex = 0;
        TRACE_MESSAGE2("Failed to create cancel HTTP mutex '%s'",
            PRAGUE_HELPERS::toStringPragueResult(createMutexResult).c_str());
    }
}

PragueDownloadProgress::~PragueDownloadProgress()
{
    if(m_cancelDownloadMutex)
        m_cancelDownloadMutex->sysCloseObject();
}

bool PragueDownloadProgress::requestAuthorizationCredentials(KLUPD::Credentials &credentials)
{
    if(!m_transport || !m_transport->m_tm)
    {
        TRACE_MESSAGE("Failed to request authorization credentials, invalid argument");
        return false;
    }

    // request proxy server authorization credentials
    cProxySettings proxySettings;
    proxySettings.m_strProxyLogin = credentials.userName().toWideChar();
    {
        std::string resultExplanation;
        proxySettings.m_strProxyPassword = PRAGUE_HELPERS::cryptPassword(PRAGUE_HELPERS::encrypt,
            m_transport->m_tm, credentials.password().toWideChar(), resultExplanation).toWideChar();
        if(!resultExplanation.empty())
            TRACE_MESSAGE(resultExplanation.c_str());
    }
    cProxyCredentialsRequest settingsRequester;
    settingsRequester.m_settings = &proxySettings;

    tERROR settingsRequesterCreateResult = errOBJECT_NOT_INITIALIZED ;
    if (m_transport->m_task)
    {
        settingsRequesterCreateResult = m_transport->m_task->sysSendMsg(pmcASK_ACTION,
            transportRequest_ProxyCredentials, 0, &settingsRequester, SER_SENDMSG_PSIZE);
    }

    if(PR_FAIL(settingsRequesterCreateResult))
    {
        TRACE_MESSAGE2("Proxy Credentials request from product failed, create request object result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(settingsRequesterCreateResult).c_str());

        return false;
    }

    credentials.userName(proxySettings.m_strProxyLogin.data());
    std::string resultExplanation;
    credentials.password(PRAGUE_HELPERS::cryptPassword(PRAGUE_HELPERS::decrypt,
        m_transport->m_tm, proxySettings.m_strProxyPassword, resultExplanation));
    if(!resultExplanation.empty())
        TRACE_MESSAGE(resultExplanation.c_str());

    return true;
}

bool PragueDownloadProgress::authorizationTargetEnabled(const KLUPD::DownloadProgress::AuthorizationTarget &targetType)
{
    if(targetType == KLUPD::DownloadProgress::proxyServer)
        return true;

    // do not allow to transfer user credentials to some unknown server
     // TODO: implement authorizaiton on update source
    TRACE_MESSAGE("Failed to request authorization credentials for HTTP server, because it is not implemented");
    return false;
}

bool PragueDownloadProgress::checkCancel()
{
    return cancelDownload();
}

bool PragueDownloadProgress::cancelDownload()const
{
	if(m_cancelDownloadMutex)
		m_cancelDownloadMutex->Lock(cSYNC_INFINITE);

    const bool cancelDownload = m_cancelDownload;

    if(m_cancelDownloadMutex)
		m_cancelDownloadMutex->Release();

    return cancelDownload;
}
void PragueDownloadProgress::cancelDownload(const bool cancelDownload)
{
	if(m_cancelDownloadMutex)
		m_cancelDownloadMutex->Lock(cSYNC_INFINITE);

    m_cancelDownload = cancelDownload;

    if(m_cancelDownloadMutex)
		m_cancelDownloadMutex->Release();
}

