#ifndef __NAG_REGPRT_H__
#define __NAG_REGPRT_H__

#include <nagent/conn/app_synchronizer.h>
#include <kca/prci/componentinstance.h>
#include <kca/prci/prci_const.h>

namespace KLNAG_REGPRT
{
    class KLSTD_NOVTABLE AvPrtStatePublisher
        :   public KLSTD::KLBaseQI
    {
    public:
        KLSTD_NOTHROW virtual void  AvPrt_UpdateAvInstalled(bool) throw() = 0;
        KLSTD_NOTHROW virtual void  AvPrt_UpdateAvRunning(bool) throw() = 0;
        KLSTD_NOTHROW virtual void  AvPrt_UpdateRtpState(
                    KLCONN::AppRtpState,
                    bool) throw() = 0;
        KLSTD_NOTHROW virtual void  AvPrt_UpdateLastFScan(KLSTD::precise_time_t) throw() = 0;
        KLSTD_NOTHROW virtual void  AvPrt_UpdateAvBasesDate(KLSTD::precise_time_t) throw() = 0;
        KLSTD_NOTHROW virtual void  AvPrt_UpdateLastConnected(KLSTD::precise_time_t) throw() = 0;
        KLSTD_NOTHROW virtual void  AvPrt_ApplicationStateChanged(
                    const KLPRSS::product_version_t&    prod,
                    KLCONN::AppState                    state) throw() = 0;
        KLSTD_NOTHROW virtual void  AvPrt_ApplicationBasesChanged(
                    const KLPRSS::product_version_t& prod,
                    KLSTD::precise_time_t x) throw() = 0;
        //KLSTD_NOTHROW virtual void Create() throw() = 0;
        //KLSTD_NOTHROW virtual void Destroy() throw() = 0;
    };
};

#endif //__NAG_REGPRT_H__
