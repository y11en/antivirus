#ifndef __KLAPP_NOTIFICATION_DEFAULTS_H__
#define __KLAPP_NOTIFICATION_DEFAULTS_H__
/*KLCSAK_PUBLIC_HEADER*/

#include "./conn_types.h"

namespace KLCONN
{

    //! events to store on server for severity "Info"
    const wchar_t c_NfDef_Info[]      = L"INF";
    
    //! events to store on server for severity "Warning"
    const wchar_t c_NfDef_Warning[]   = L"WRN";
    
    //! events to store on server for severity "Error"
    const wchar_t c_NfDef_Error[]     = L"ERR";

    //! events to store on server for severity "Critical"
    const wchar_t c_NfDef_Critical[]  = L"CRT";

    //! event type
    const wchar_t c_NDFld_EventType[]           = L"KLEVP_ND_EVETN_TYPE"; // STRING_T

    //! days to store on the administration server
    const wchar_t c_NDFld_DaysToStore[]           = L"KLEVP_ND_DAYS_TO_STORE_EVENT"; // INT_T

    
    

    /*!
        \brief  Allows set up default event notification settings.
                Interface is used only if CIF_PROVIDES_NOTIFICATIONS_DEFAULTS 
                flag is set in installation info.
                Interface is implemented by connector.
                Interface is acquired with QueryInterface from KLCONN::AppSynchronizer.
    */
    class KLSTD_NOVTABLE AppNotificationDefaults
        :   public KLSTD::KLBaseQI
    {
    public:

    /*!
      \brief Returns notification defaults

      \retval ppData, has following format

                        |
                        +--c_NfDef_Info, type: ARRAY_T of PARAMS_T
                        |    +-[0]
                        |    |  +--c_NDFld_EventType = '<event_type0>', , type: STRING_T
                        |    |
                        |    +-[1]
                        |       +--c_NDFld_EventType = '<event_type1>', , type: STRING_T
                        |       ...
                        |
                        +--c_NfDef_Warning, type: ARRAY_T of PARAMS_T
                        |
                        ...
                        |
                        +--c_NfDef_Error, type: ARRAY_T of PARAMS_T
                        |
                        ...
                        |
                        +--c_NfDef_Critical, type: ARRAY_T of PARAMS_T
    */
        virtual void GetNotificationDefaults(KLPAR::Params** ppData) = 0;
    };
};

#endif //__KLAPP_NOTIFICATION_DEFAULTS_H__
