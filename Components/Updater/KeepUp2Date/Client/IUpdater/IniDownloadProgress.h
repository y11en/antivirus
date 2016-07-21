#ifndef INIDOWNLOADPREGRESS_H_INCLUDED_58A619B0_459D_4dbe_B2A9_7F29D3F492BD
#define INIDOWNLOADPREGRESS_H_INCLUDED_58A619B0_459D_4dbe_B2A9_7F29D3F492BD

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "ini_log.h"

class IniDownloadProgress : public KLUPD::DownloadProgress
{
public:
    IniDownloadProgress();

    virtual bool requestAuthorizationCredentials(KLUPD::Credentials &);
    virtual bool authorizationTargetEnabled(const KLUPD::DownloadProgress::AuthorizationTarget &);
    virtual void percentCallback(const size_t currentPercentValue);
};


#endif  // INIDOWNLOADPREGRESS_H_INCLUDED_58A619B0_459D_4dbe_B2A9_7F29D3F492BD
