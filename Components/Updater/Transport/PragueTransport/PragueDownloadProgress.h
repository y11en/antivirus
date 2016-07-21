#ifndef PRAGUETRANSPORTDOWNLOADPROGRESS_H_INCLUDED_E804A779_BCF0_4e2b_B789_F79F1EF8D2E9
#define PRAGUETRANSPORTDOWNLOADPROGRESS_H_INCLUDED_E804A779_BCF0_4e2b_B789_F79F1EF8D2E9

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include <include/DownloadProgress.h>

#include <Prague/prague.h>
#include <Prague/iface/i_mutex.h>

struct Transport;

class PragueDownloadProgress : public KLUPD::DownloadProgress
{
public:
    PragueDownloadProgress(KLUPD::Log *);
    virtual ~PragueDownloadProgress();

    virtual bool requestAuthorizationCredentials(KLUPD::Credentials &);
    virtual bool authorizationTargetEnabled(const KLUPD::DownloadProgress::AuthorizationTarget &);

    bool cancelDownload()const;
    void cancelDownload(const bool cancelDownload);

    // check if user wants to cancel update
    virtual bool checkCancel();


    Transport *m_transport;

private:
    mutable cMutex *m_cancelDownloadMutex;
    mutable bool m_cancelDownload;

    KLUPD::Log *pLog;
};


#endif  // PRAGUETRANSPORTDOWNLOADPROGRESS_H_INCLUDED_E804A779_BCF0_4e2b_B789_F79F1EF8D2E9
