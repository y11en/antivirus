#ifndef __CONN_AUTOKEYS_H__
#define __CONN_AUTOKEYS_H__
/*KLCSAK_PUBLIC_HEADER*/

#include "./conn_types.h"
#include <kca/prss/settingsstorage.h>
#include <common/kllicinfo.h>

namespace KLCONN
{
    /*!
        \brief  Allows connector to acquire automatically distriburted 
                license key.
                Interface is implemented by network agent.
                Interface is acquired with QueryInterface from KLCONN::Notifier.
    */

    class KLSTD_NOVTABLE AutomaticKeys
        :   public KLSTD::KLBaseQI
    {
    public:

        /*!
          \brief	Automatic keys info. Available only for applications 
                    that have CIF_SUPPORTS_AUTOKEYS flag. 

                    If application decides to use one of that automatic keys 
                    as its first license key it must further return it in 
                    AppSynchronizer::GetApplicationProperties call. 
    
                    As such decision means changing of Application properties
                    calling Notifier::ApplicationPropertiesChange is also required. 

                    Application that uses automatic key must stop using it 
                    if that key is not listed by AcquireAutomaticKeysAvailable any more !!!

                    +--c_szwDisallowedAutoKeys  ARRAY_T|STRING_T
                    |
                    +--c_szwAllowedAutoKeys     PARAMS_T
                        |
                        +--<license ID #1>      PARAMS_T
                            |
                            +---KLLICSRV_KEY_INSTALLED
                            +---KLLIC_APP_ID
                            +---KLLIC_EFFECTIVE_LICENSE_PERIOD
                            +---KLLIC_EFFECTIVE_LIC_COUNT
                            +---KLLIC_KEY_TYPE
                            +---KLLIC_LICENSE_PERIOD
                            +---KLLIC_LIC_COUNT  
                            +---KLLIC_MAJ_VER 
                            +---KLLIC_PROD_NAME
                            +---KLLIC_PROD_SUITE_ID
                            +---KLLIC_SERIAL

          \retval	ppData automatic keys list. 
        */
        virtual void AcquireAutomaticKeysAvailable(KLPAR::Params** ppData) = 0;



        /*!
          \brief	Returns body of specified allowed key if available.

          \retval	ppData container with key body
          
                        +--KLLIC_LICFILE    BINARY_T
        */
        virtual void AcquireKeyBody(const wchar_t* szwSerial, KLPAR::Params** ppData) = 0;
    };


    /*!
        \brief  Allows connector to acquire automatically distributed 
                license key.
                Interface is used only if CIF_SUPPORTS_AUTOKEYS flag is 
                set in installation info.
                Interface is implemented by connector.
                Interface is acquired with QueryInterface from KLCONN::AppSynchronizer.
    */
    class KLSTD_NOVTABLE AppAutomaticKeys
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Hints the connector that list of available automatic 
                    keys might have been changed.
        */
        virtual void LookAtAutomaticKeys() = 0;
    };
};

#endif //__CONN_AUTOKEYS_H__
