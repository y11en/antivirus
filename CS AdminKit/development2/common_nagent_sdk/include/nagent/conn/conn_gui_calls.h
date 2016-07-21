#ifndef __KL_CONN_GUI_CALLS_H__
#define __KL_CONN_GUI_CALLS_H__

#include "./conn_types.h"

namespace KLCONN
{
    enum GuiCallResult
    {
        GCR_SUCCESS = 0,    //! method completed successfully
        GCR_FAILURE = -1,   //! method failed, extra information is contained in pErrorData
        GCR_NO_SUCH_CALL = KLSTD::STDE_NOFUNC   //! no such gui call (gui call is not implemented)
    };

    /*!
        Интерфейс реализуется системой администрирования.
        Интерфейс используется библиотекой интеграции.

        Вызовы методов интерфейса могут производится на разных нитях.
    */
    class GuiCall : public KLSTD::KLBaseQI
    {
    public:
    /*!
      \brief	Completes GUI call execution and returns results or error
                information to caller.

      \param	nResult     IN result code
      \param	pData       IN GUI call output data
      \param	pErrorData  IN error data
    */
        virtual void FinishGuiCall(
                        GuiCallResult   nResult, 
                        KLPAR::Params*  pData, 
                        KLERR::Error*   pErrorData) = 0;
    };

    /*!
        Support for direct call from the Administration Console plugin.
        Interface is quired by the Network agent from AppSynchronizer 
        interface.
        Интерфейс реализуется библиотекой интеграции
        Интерфейс используется системой администрирования.

        Вызовы методов интерфейса могут производится на разных нитях, но 
        при этом сериализуются вызывающей стороной.
    */
    class AppGuiCalls : public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Starts GUI call execution.

          \param	szwCallName         IN name of GUI call
          \param	szwConnectionId     IN id of GUI connection
          \param	pCallFinish         IN instance of GuiCall
          \param	pData               IN GUI call input data
        */
        virtual void StartGuiCall(
                        const wchar_t*  szwCallName,
                        const wchar_t*  szwConnectionId,
                        GuiCall*        pCallFinish,
                        KLPAR::Params*  pData) = 0;

        /*!
          \brief	Called when gui connection is closed. Connector may 
                    deallocate here some data allocated earlier in 
                    StartGuiCall method.

          \param	szwConnectionId IN id of GUI connection
        */
        virtual void OnGuiConnectionClosed(
                        const wchar_t* szwConnectionId) = 0; 
    };
};

#endif //__KL_CONN_GUI_CALLS_H__
