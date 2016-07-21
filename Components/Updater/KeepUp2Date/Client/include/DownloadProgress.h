#ifndef DOWNLOADPROGRESS_H_INCLUDED_B769374B_CC62_43a5_8AF4_9F091A7B17B0
#define DOWNLOADPROGRESS_H_INCLUDED_B769374B_CC62_43a5_8AF4_9F091A7B17B0

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "../helper/comdefs.h"
#include "cfg_updater2.h"

namespace KLUPD {

class KAVUPDCORE_API DownloadProgress
{
public:
    enum AuthorizationTarget
    {
        // TODO: WARNING authorization on server is not implemented (bug 18099)
        server,
        proxyServer,
    };
    static std::string toString(const AuthorizationTarget &);

    typedef double BytesPerSecond;


    DownloadProgress();
    virtual ~DownloadProgress();

    // request credentials from product for authorization on proxy server
    virtual bool requestAuthorizationCredentials(Credentials &) = 0;
// TODO: authorization on server is not implemented correclty,
 // but there is a workaround for Updater for some proxies which return 401 HTTP code instead 407
    virtual bool authorizationTargetEnabled(const AuthorizationTarget &) = 0;

    // informs about current download progress
    virtual void percentCallback(const size_t currentPercentValue);
    // check if user wants to cancel update
    virtual bool checkCancel();


    // returns transfer rate
    BytesPerSecond transferRate()const;
    // return total number of bytes transferred
    size_t bytesTransferred()const;
    // expected number of bytes to expected download (the size of update)
    size_t expectedDownloadSize()const;


///////////////////////////////////////////////////////////////////
//// Implementetion to calculate transfer rate
private:
    // the Socket, FileTransferProtocol, AdministrationKitProtocol
    //  classes transfer data and used for bandwidth calculation
    friend class Socket;
    friend class FileTransferProtocol;
    friend class AdministrationKitProtocol;


    // start timer to calculate download speed
    void updateSpeedStartTimer();
    // stop timer to calculate download speed
    void updateSpeedStopTimer();

    // number of bytes transferred via network or other transport (Administration Kit, file tranfer)
    void bytesTransferred(const size_t);

    size_t m_expectedDownloadSize;
    size_t m_bytesTransferred;

    // timestamp of current download operation begin
    clock_t m_timerStart;
    // time taken by transfer operations up to current moment
    clock_t m_clockPassed;


///////////////////////////////////////////////////////////////////
//// Implementetion to disply update percentage progress smoothly
private:
    // Updater and UpdaterTransaction class manages states
    friend class Updater;
    friend class UpdaterTransaction;


    // the percent Adjust State is used to make progress bar smooth
    enum PercentAdjustState
    {
        checkingLocalFiles,
        downloadingIndex,
        indexDownloaded,
        replacingFiles,
    };


    // control state function
    void percentAdjustState(const PercentAdjustState &);
    // shows percent to user to avoid sitiation as if update freezes while building list of files to download
    void tickPercent();

    PercentAdjustState m_percentAdjustState;



    // number of files already enumerated (because there are more then 300 files in
     // common cases, that is why percent tick should not be performed for each file)
    size_t m_fileNumberEnumerated;

    // the progress bar range for building file list to download
    size_t m_percentRestForEnumeratingLocalFiles;
    // the progress bar range for building file list to download
    size_t m_percentRestForDownloadIndex;
    // the progress bar range for replacing files after download
    size_t m_percentRestForReplacingFiles;

    // cache of maximum percent value to avoid situation when percent goes back
    size_t m_maximumPercentValueReached;


    size_t m_percentGetNotByDownloadedBytes;

    // files needed to be enumerated to move progress bar on 1 tick
     // (consider there are approximatelly 300 local files)
    static const size_t s_filesNeededForOneTickForEnumeration;
};

// no implementation Download Progress helper class
class KAVUPDCORE_API EmptyDownloadProgress : public DownloadProgress
{
public:
    virtual bool requestAuthorizationCredentials(Credentials &);
    virtual bool authorizationTargetEnabled(const AuthorizationTarget &);
};


}   // namespace KLUPD

#endif  // DOWNLOADPROGRESS_H_INCLUDED_B769374B_CC62_43a5_8AF4_9F091A7B17B0
