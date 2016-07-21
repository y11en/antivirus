#include "PragueDownloadProgress.h"

PragueDownloadProgress::PragueDownloadProgress(CUpdater2005 &updateTask, KLUPD::Log *log)
 : m_saveProxyAuthorizationCredetianlsCancelledByUser(false),
   m_updateTask(updateTask),
   pLog(log)
{
}

bool PragueDownloadProgress::requestAuthorizationCredentials(KLUPD::Credentials &credentials)
{
    if(m_saveProxyAuthorizationCredetianlsCancelledByUser || !m_updateTask.m_interactionWithUserEnabled)
        return false;

    cProxySettings proxySettings;
    proxySettings.m_strProxyLogin = credentials.userName().toWideChar();
    {
        std::string resultExplanation;
        proxySettings.m_strProxyPassword = PRAGUE_HELPERS::cryptPassword(PRAGUE_HELPERS::encrypt,
            m_updateTask.m_hTM, credentials.password().toWideChar(), resultExplanation).toWideChar();
        if(!resultExplanation.empty())
            TRACE_MESSAGE(resultExplanation.c_str());
    }

    cProxyCredentialsRequest settingsRequester;
    settingsRequester.m_settings = &proxySettings;
    const tERROR settingsRequesterCreateResult = m_updateTask.sendImpersonatedMessage("Credentials request",
        pmcASK_ACTION, transportRequest_ProxyCredentials, 0, &settingsRequester, SER_SENDMSG_PSIZE);

    if(PR_SUCC(settingsRequesterCreateResult))
    {
        credentials.userName(proxySettings.m_strProxyLogin.data());
        std::string resultExplanation;
        credentials.password(PRAGUE_HELPERS::cryptPassword(PRAGUE_HELPERS::decrypt,
            m_updateTask.m_hTM, proxySettings.m_strProxyPassword, resultExplanation));
        if(!resultExplanation.empty())
            TRACE_MESSAGE(resultExplanation.c_str());
        return true;
    }

    TRACE_MESSAGE2("Proxy Credentials request from product failed, create request object result '%s'",
        PRAGUE_HELPERS::toStringPragueResult(settingsRequesterCreateResult).c_str());

    m_saveProxyAuthorizationCredetianlsCancelledByUser = true;
    return false;
}

bool PragueDownloadProgress::authorizationTargetEnabled(const KLUPD::DownloadProgress::AuthorizationTarget &)
{
    return true;
}

void PragueDownloadProgress::percentCallback(const size_t currentPercentValue)
{
    cAutoCS autoCSStatistics(m_updateTask.m_hCSStatistics, true);
    m_updateTask.m_Statistics.m_qwDownloadedBytesCount = bytesTransferred();
    m_updateTask.m_Statistics.m_qwTotalDownloadBytesCount = expectedDownloadSize();
    m_updateTask.m_Statistics.m_nPercentCompleted = currentPercentValue;

    // calculate update speed in Killobytes/sec
    const double speed = transferRate();
    m_updateTask.m_Statistics.m_dbSpeed = speed / 1024.0;

    // can do no task completion approximation
    /// task complete approximation
    if(speed && (bytesTransferred() < expectedDownloadSize()))
    {
        const double bytesRest = expectedDownloadSize() - bytesTransferred();
        const double secondsRest = bytesRest / speed;
        const double currentTimestamp = static_cast<double>(static_cast<tDWORD>(cDateTime::now_utc()));
        m_updateTask.m_Statistics.m_timeFinish = static_cast<tDWORD>(currentTimestamp + secondsRest);
    }
    else
    {
        // can not calculate completion time
        m_updateTask.m_Statistics.m_timeFinish = 0;
    }
}

bool PragueDownloadProgress::checkCancel()
{
    tDWORD newState = TASK_STATE_UNKNOWN;
    tDWORD oldState = TASK_STATE_UNKNOWN;
    {
        cAutoCS autoCSState(m_updateTask.m_hCSState, false);
        cAutoCS autoCSNewState(m_updateTask.m_hCSNewState, false);

        oldState = m_updateTask.m_dwState;
        newState = m_updateTask.m_dwNewState;
    }
    
    if(newState == oldState)
        return false;

    if(newState == TASK_STATE_PAUSED)
    {
        TRACE_MESSAGE("Task paused");

        m_updateTask.setState(TASK_STATE_PAUSED);
        m_updateTask.m_hPauseEvent->Wait(cSYNC_INFINITE);
        m_updateTask.m_hPauseEvent->SetState(cFALSE);

        TRACE_MESSAGE("Task resumed");
    }

    {
        cAutoCS autoCSState(m_updateTask.m_hCSState, false);
        cAutoCS autoCSNewState(m_updateTask.m_hCSNewState, false);
        
        oldState = m_updateTask.m_dwState;
        newState = m_updateTask.m_dwNewState;
    }

    if(newState == TASK_STATE_STOPPED)
        return true;

    m_updateTask.setState(TASK_STATE_RUNNING);
    return false;
}
