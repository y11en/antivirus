#ifndef __KLLICSRVSTORE_H__
#define __KLLICSRVSTORE_H__

#include <srvp/csp/klcsp.h>
#include <srvp/licsrv/licsrv.h>
#include <srvp/klola/klola.h>
#include <common/kllicinfo.h>

namespace KLLICSRV
{
    //! key app id
    const wchar_t c_szwKeyAppId[] = KLLIC_APP_ID;   //INT_T

    //! key product suite id
    const wchar_t c_szwProdSuiteId[] = KLLIC_PROD_SUITE_ID;   //INT_T

    //! key creation time
    const wchar_t c_szwKeyCreationTime[] = KLLIC_CREATION_DATE;   //DATE_TIME_T

    //! key limit expiration time time
    const wchar_t c_szwKeyLimitTime[] = KLLIC_LIMIT_DATE;   //DATE_TIME_T
    
    //! key serial number
    const wchar_t c_szwKeySerial[] = KLLIC_SERIAL;   //STRING_T
    
    //! key name
    const wchar_t c_szwKeyName[] = KLLIC_PROD_NAME;   //STRING_T

    //! key type
    const wchar_t c_szwKeyType[] = KLLIC_KEY_TYPE;   //INT_T

    //! key major version
    const wchar_t c_szwKeyMajVer[] = KLLIC_MAJ_VER;   //STRING_T

    /*! 
        key data (for methods SrvLicences2::InstallKey and 
        SrvLicences2::ExportKey only)
        contains fields KLOLA::c_szwOLAKey_KeyFile (required) and 
        KLOLA::c_szwOLAKey_CustomerInfo (optional)
    */
    const wchar_t c_szwKeyData[] = L"KLLICSRV_KEYDATA";       //PARAMS_T

    //! if key is a flexible key
    const wchar_t c_szwKeyIsFlexible[] = L"KLLICSRV_FLEXIBLE_KEY";  //BOOL_T

    //! key validity period (days)
    const wchar_t c_szwKeyPeriod[] = KLLIC_LICENSE_PERIOD;     //INT_T

    //! license count
    const wchar_t c_szwKeyLicNumber[] = KLLIC_LIC_COUNT;      //INT_T

    //! Effective period
    const wchar_t c_szwKeyEffectivePeriod[] = KLLIC_EFFECTIVE_LICENSE_PERIOD;         //INT_T

    //! effective license number
    const wchar_t c_szwKeyEffectiveLicNumber[] = KLLIC_EFFECTIVE_LIC_COUNT;   //INT_T

    //! maximal effective license number (if key is flexible )
    const wchar_t c_szwKeyEffectiveLicNumberMax[] = L"KLLICSRV_KEY_EFFECTIVE_LICNUMBER_MAX";   //INT_T

    //! minimal effective license number (if key is flexible )
    const wchar_t c_szwKeyEffectiveLicNumberMin[] = L"KLLICSRV_KEY_EFFECTIVE_LICNUMBER_MIN";   //INT_T

    //! if key is installed
    const wchar_t c_szwKeyInstalled[] = L"KLLICSRV_KEY_INSTALLED";   //BOOL_T

    //! if key is installed
    const wchar_t c_szwEnumOpt_InstalledOnly[] = L"KLLICSRV_ENOPT_INSTALLED_ONLY";   //BOOL_T
    

    class KLSTD_NOVTABLE SrvLicStore
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Installs key.

          \param	pData data for key to install, must contain following attributes:
                    required
                        c_szwKeyData
                    optional
                        c_szwKeyIsFlexible
                        
        */
        virtual std::wstring InstallKey(KLPAR::Params* pData) = 0;

        /*!
          \brief	Uninstalls key. 

          \param	szwSerial key serial number
        */
        virtual void UninstallKey(const wchar_t* szwSerial) = 0;

        /*!
          \brief	Returns info for specified key

          \param	szwSerial key serial number
          \retval	ppData contains following attributes:
                        c_szwKeyAppId
                        c_szwProdSuiteId
                        c_szwKeyCreationTime
                        c_szwKeyLimitTime
                        c_szwKeySerial
                        c_szwKeyName
                        c_szwKeyType
                        c_szwKeyMajVer
                        c_szwKeyIsFlexible
                        c_szwKeyPeriod
                        c_szwKeyLicNumber
                        c_szwKeyEffectivePeriod
                        c_szwKeyEffectiveLicNumber
                        c_szwKeyInstalled,
                        c_szwKeyEffectiveLicNumberMax
                        c_szwKeyEffectiveLicNumberMin
        */
        virtual void GetKeyData(
                    const wchar_t*    szwSerial, 
                    KLPAR::Params**   ppData) = 0;

        /*!
          \brief	Exports key. 

          \param	szwSerial key serial number
          \retval	ppData contains following attributes:
                        c_szwKeyData
        */
        virtual void ExportKey(
                    const wchar_t*    szwSerial, 
                    KLPAR::Params**   ppData) = 0;

       
        /*!
          \brief	Recalculates effective KeyPeriod

          \param	pLimits flexibility limits as returned by GetKeyInfo

          \param	lNewCountOfLicenses number of licenses required by user, 
                    must be more or equal to between c_szwKeyEffectiveLicNumberMin 
                    and less or equal to c_szwKeyEffectiveLicNumberMax
          \retval	lNewKeyPeriod resulting new key period
          \return	false if lNewCountOfLicenses provided is not permitted
          \exception throws exception in case of error (invalid arguments and so on)
        */
        virtual bool TryToAdjustKey(
                            KLPAR::Params*  pLimits, 
                            long            lNewCountOfLicenses, 
                            long&           lNewKeyPeriod) = 0;

        /*!
          \brief	Updates effective license count or changes key flexibility state

          \param	szwSerial key serial number

          \param	pData key data, must contain following attributes:
                        
                    1. to change turn on flexibility state
                    required
                        c_szwKeyIsFlexible = true
                    optional
                        c_szwKeyEffectivePeriod
                        c_szwKeyEffectiveLicNumber -- must be calculated with TryToAdjustKey

                    2. to change turn off flexibility state
                    required
                        c_szwKeyIsFlexible = false
                    
                    3. to change flexibility state 
                    required
                        c_szwKeyEffectivePeriod
                        c_szwKeyEffectiveLicNumber -- must be calculated with TryToAdjustKey
        */
        virtual void AdjustKey(
                            const wchar_t*  szwSerial, 
                            KLPAR::Params*  pData) = 0;

        /*!
          \brief	Enumerates installed keys

          \param	vect_fields             attributes to return, possible values
                                                c_szwKeyAppId
                                                c_szwProdSuiteId
                                                c_szwKeyCreationTime
                                                c_szwKeyLimitTime
                                                c_szwKeySerial
                                                c_szwKeyName
                                                c_szwKeyType
                                                c_szwKeyMajVer
                                                c_szwKeyIsFlexible
                                                c_szwKeyPeriod
                                                c_szwKeyLicNumber
                                                c_szwKeyEffectivePeriod
                                                c_szwKeyEffectiveLicNumber
                                                c_szwKeyInstalled
          \param	vect_fields_to_order    attributes to sort by
          \param	pOptions                options, possible attributes:
                                                c_szwEnumOpt_InstalledOnly -- if to enumerate only installed keys or all known keys
          \param	ppAccessor              iterator
          \param    lTimeout                iterator lifetime, seconds
          \return                           records found
        */
        virtual long EnumKeys(
                const std::vector<std::wstring>&            vect_fields,
                const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
                KLPAR::Params*                              pOptions,
                long                                        lTimeout,
                KLCSP::ChunkAccessor**                      ppAccessor) = 0;
    };

};

#endif //__KLLICSRVSTORE_H__
