#ifndef __HST_CONN_GUI_CALLS_H__
#define __HST_CONN_GUI_CALLS_H__

#include <std/base/klstd.h>
#include <std/par/params.h>
#include <transport/ev/sinks_base.h>
#include <kca/prcp/componentproxy.h>
#include <nagent/conn/conn_gui_calls.h>

namespace KLHST
{
    class KLSTD_NOVTABLE GuiCallResults : public KLSTD::KLBaseQI
    {
    public:
        virtual KLCONN::GuiCallResult GetCallResult() = 0;
        virtual void GetOutData(KLPAR::Params**     ppOutData) = 0;
        virtual void GetErrorData(KLERR::Error**    ppError) = 0;
    };

    class KLSTD_NOVTABLE GuiCallsProxy2 : public KLSTD::KLBaseQI
    {
    public:
        virtual void CallConnectorSync(
                        const wchar_t*      szwCallName,
                        KLPAR::Params*      pInData,
                        long                lTimeout,
                        GuiCallResults**    ppResults) = 0;

        virtual void CallConnectorAsync(
                        const wchar_t*      szwCallName,
                        KLPAR::Params*      pInData,
                        KLEV::AdviseEvSink* pSink,
                        KLEV::HSINKID&      hSink) = 0;

        KLSTD_NOTHROW virtual bool Unadvise(KLEV::HSINKID hSink) throw () = 0;
    };
};

#endif //__HST_CONN_GUI_CALLS_H__
