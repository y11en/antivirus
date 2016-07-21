#ifndef __KLHST_GUI_CALLS_H__
#define __KLHST_GUI_CALLS_H__

#include "./hst_conn_gui_calls.h"

namespace KLHST
{
    //! obsolete
    class KLSTD_NOVTABLE GuiCallsProxy : public KLSTD::KLBaseQI
    {
    public:
        virtual void CallConnectorSync(
                        const std::wstring&     wstrCallName,
                        KLPAR::Params*          pInData,
                        long                    lTimeout,
                        GuiCallResults**        ppResults) = 0;

        virtual void CallConnectorAsync(
                        const std::wstring& wstrCallName,
                        KLPAR::Params*      pInData,
                        KLEV::AdviseEvSink* pSink,
                        KLEV::HSINKID&      hSink) = 0;

        KLSTD_NOTHROW virtual bool Unadvise(KLEV::HSINKID hSink) throw () = 0;
    };
};

//! obsolete
void KLCSSRVP_DECL KLHST_CreateGuiCallsProxy( 
                                KLPRCP::ComponentProxy*     pConnector,
                                KLHST::GuiCallsProxy**      ppGuiCallsProxy);

void KLCSSRVP_DECL KLHST_CreateGuiCallsProxy2( 
                                KLPRCP::ComponentProxy*     pConnector,
                                KLHST::GuiCallsProxy2**     ppGuiCallsProxy);

#endif //__KLHST_GUI_CALLS_H__
