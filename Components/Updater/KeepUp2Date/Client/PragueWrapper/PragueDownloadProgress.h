#ifndef PRAGUEDOWNLOADPREGRESS_H_INCLUDED_472E7496_26DE_4ea0_96ED_2E5279860B79
#define PRAGUEDOWNLOADPREGRESS_H_INCLUDED_472E7496_26DE_4ea0_96ED_2E5279860B79

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "comdefs.h"
#include "UpdaterTask/task.h"

class PragueDownloadProgress : public KLUPD::DownloadProgress
{
public:
    PragueDownloadProgress(CUpdater2005 &, KLUPD::Log *);

    virtual bool requestAuthorizationCredentials(KLUPD::Credentials &);
    virtual bool authorizationTargetEnabled(const KLUPD::DownloadProgress::AuthorizationTarget &);

    virtual void percentCallback(const size_t currentPercentValue);
    virtual bool checkCancel();


private:
    bool m_saveProxyAuthorizationCredetianlsCancelledByUser;

    CUpdater2005 &m_updateTask;
    KLUPD::Log *pLog;
};


#endif  // PRAGUEDOWNLOADPREGRESS_H_INCLUDED_472E7496_26DE_4ea0_96ED_2E5279860B79
