#ifndef __KLCONNAPP_APPINST_H__
#define __KLCONNAPP_APPINST_H__

#include <nagent/conn/connector.h>
#include <std/base/klbaseqi_imp.h>
#include <kca/prci/componentinstance.h>
#include <common/measurer.h>

#define KLCONNAPP_COMPONENT_NAME   KLCONN_COMPONENT_NAME
#define KLCONNAPP_MAKE_WELLKNOWN   KLCONN_MAKE_WELLKNOWN


//!Event published
#define KLCONNAPP_EV_APP_CHANGE_STATE   L"KLCONNAPP_EV_APP_CHANGE_STATE"
    #define KLCONNAPP_EVP_PRODUCT           L"KLCONNAPP_EVP_PRODUCT"
    #define KLCONNAPP_EVP_VERSION           L"KLCONNAPP_EVP_VERSION"
    #define KLCONNAPP_EVP_APPSTATE          L"KLCONNAPP_EVP_APPSTATE"

#define KLCONNAPP_EV_APP_CHANGED_AVDB   L"KLCONNAPP_EV_APP_CHANGED_AVDB"

namespace KLCONNAPP
{
    class KLSTD_NOVTABLE AppInst
        :   public KLSTD::KLBaseQI
    {
    public:        
        virtual void Create() = 0;
        KLSTD_NOTHROW virtual void Destroy() throw() = 0;

        /*!
          \brief	Returns current application state

          \return	current application state
        */
        virtual KLCONN::AppState GetAppState() = 0;

        /*!
          \brief	Starts the product

          \return	false if already running
        */
        virtual bool Start() = 0;

        /*!
          \brief	Stops the product

          \return	false if already stopped
        */
        virtual bool Stop() = 0;
        

        /*!
          \brief	Returns rtp states. The method is implemented only by 
                    connectors

          \param	LCONN::AppRtpState&    nRtpState
          \param	int&                    nErrorCode
          \return	virtual void 
        */
        virtual void GetRtpState(
                        KLCONN::AppRtpState&    nRtpState,
                        int&                    nErrorCode) = 0;
        
        /*!
          \brief	Checks if integration is implemented via connector
          \return	true if connector is used
        */
        virtual bool IsConnectorDriven() = 0;
    };

    typedef KLSTD::CAutoPtr<KLCONNAPP::AppInst> AppInstPtr;
};

#endif //__KLCONNAPP_APPINST_H__

