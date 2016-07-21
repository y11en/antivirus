#include "IniDownloadProgress.h"

IniDownloadProgress::IniDownloadProgress()
{
}

bool IniDownloadProgress::requestAuthorizationCredentials(KLUPD::Credentials &)
{
    // interaction with user is not implemented
    return false;
}

bool IniDownloadProgress::authorizationTargetEnabled(const KLUPD::DownloadProgress::AuthorizationTarget &)
{
    return true;
}

void IniDownloadProgress::percentCallback(const size_t currentPercentValue)
{
    printf(".");
}

