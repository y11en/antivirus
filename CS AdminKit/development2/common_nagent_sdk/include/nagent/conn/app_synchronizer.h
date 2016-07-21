#ifndef __KL_APP_SYNCHRONIZER_H__
#define __KL_APP_SYNCHRONIZER_H__

#include "./conn_types.h"
#include <kca/prss/settingsstorage.h>
#include <common/kllicinfo.h>

#define KLCONN_UPDATE_INFO_1            L"KLCONN_UPDATE_INFO_1"
#define KLCONN_LIC_INFO_1               L"KLCONN_LIC_INFO_1"
#define KLCONN_COMPONENTS_INFO_1        L"KLCONN_COMPONENTS_INFO_1"
#define KLCONN_CRITICAL_FIXES_INFO_1    L"KLCONN_CRITICAL_FIXES_INFO_1"
#define KLCONN_VAL_FIXES                L"KLCONN_VAL_FIXES"


namespace KLCONN
{
	/*!
      \brief Состояния приложения
    */    
    typedef enum
    {
        APP_STATE_INACTIVE,
        APP_STATE_RUNNING
    }AppState;

	/*!
      \brief Команды, посылаемые приложению системой администрирования.
    */    
    typedef enum
    {
        APP_ACTION_START,
        APP_ACTION_STOP
    }AppAction;


	/*!
      \brief Команды, посылаемые приложению системой администрирования.
    */    
    typedef enum
    {
        APP_RTP_STATE_UNKNOWN = 0,
        APP_RTP_STATE_STOPPED,
        APP_RTP_STATE_SUSPENDED,
        APP_RTP_STATE_STARTING,
        APP_RTP_STATE_RUNNING,
        APP_RTP_STATE_RUNNING_MAX_PROTECTION,
        APP_RTP_STATE_RUNNING_MAX_SPEED,
        APP_RTP_STATE_RUNNING_RECOMMENDED,
        APP_RTP_STATE_RUNNING_CUSTOM,
        APP_RTP_STATE_FAILURE
    }AppRtpState;

    /*!
    \brief Интерфейс, отвечающий за синхронизацию состояния приложения
	    с состоянием в системе администрирования.

        Интерфейс реализуется библиотекой интеграции
        Интерфейс используется системой администрирования.

        Вызовы методов интерфейса могут производится на разных нитях, но при
        этом сериализуются вызывающей стороной.

    */

    class KLSTD_NOVTABLE AppSynchronizer : public KLSTD::KLBaseQI
    {
    public:

        /*!
          \brief	Возвращает статус приложения
            В случае ошибки выбрасывается исключение типа KLERR::Error*

            \param	nState      [out] - состояние приложения
            \param ppResult     [out]   дополнительные параметры.
            \param tmRiseTime   [out]   время публикации события в UTC.
        */
        virtual void GetAppStatus(
                                KLCONN::AppState&   nState,
                                KLPAR::Params**     ppResult,
                                raise_time_t&       tmRiseTime) = 0;

        /*!
          \brief	Возвращает статистику приложения.
            В случае ошибки выбрасывается исключение типа KLERR::Error*

          \param	pFilter - фильтр; возвращаются значения полей, указанные в фильтре.
          \param	ppStatistics - возвращаемая статистика приложения
        */
        virtual void GetAppStatistics(
                                KLPAR::Params*  pFilter,
                                KLPAR::Params** ppStatistics) = 0;


        /*!
          \brief	Посылает приложению команду.
            В случае ошибки выбрасывается исключение типа KLERR::Error*

          \param	nAction - команда
          \param	pData - данные

          \exception STDE_NOTPERM - данная команда не поддерживается
        */
        virtual void SendAppAction(
                                KLCONN::AppAction   nAction,
                                KLPAR::Params*      pData) = 0;

        /*!
          \brief	Возвращает свойства приложения.

          \param	KLPAR::Params** ppData -- контейнер со свойствами
                        приложения. Контейнер имеет следующую структуру:
                    KLCONN_UPDATE_INFO_1
	                    KLPRSS_PRODVAL_INSTALLTIME      -- дата/время инсталляции приложения, DATE_TIME_T
	                    KLPRSS_PRODVAL_LASTUPDATETIME   -- дата/время последнего обновления, DATE_TIME_T
                        KLPRSS_PRODVAL_DISPLAYNAME      -- отображаемое имя продукта для чтения человеком, STRING_T
                        KLPRSS_PRODVAL_VERSION          -- полная версия продукта,  STRING_T
	                    KLPRSS_PRODVAL_BASEDATE         -- дата/время антивирусных баз, DATE_TIME_T
	                    KLPRSS_PRODVAL_BASEINSTALLDATE  -- дата/время установки антивирусных баз, DATE_TIME_T
	                    KLPRSS_PRODVAL_BASERECORDS      -- количество записей в антивирусных базах, INT_T

                    KLCONN_LIC_INFO_1
	                    KLPRSS_VAL_LIC_KEY_CURRENT          -- информация о текущем ключе
		                    KLLIC::c_szwKeyType             -- тип ключа, INT_T
		                    KLLIC::c_szwKeyProdSuiteID      -- идентификатор продукта, INT_T
		                    KLLIC::c_szwKeyAppID            -- идентификатор приложения, INT_T
		                    KLLIC::c_szwKeyInstallDate      -- дата/время инсталляции ключа, DATE_TIME_T
		                    KLLIC::c_szwKeyExpirationDate   -- дата/время истечения срока ключа, DATE_TIME_T
		                    KLLIC::c_szwKeyLicPeriod        -- время жизни ключа, INT_T
		                    KLLIC::c_szwKeyLicCount         -- количетсво лицензий, INT_T
		                    KLLIC::c_szwKeySerial           -- серийный номер, STRING_T
		                    KLLIC::c_szwKeyMajVer           -- версия продукта, STRING_T
		                    KLLIC::c_szwKeyProdName		    -- имя продукта, STRING_T
	                    KLPRSS_VAL_LIC_KEY_NEXT
		                    KLLIC::c_szwKeyType
		                    KLLIC::c_szwKeyProdSuiteID
		                    KLLIC::c_szwKeyAppID
		                    KLLIC::c_szwKeyInstallDate
		                    KLLIC::c_szwKeyExpirationDate
		                    KLLIC::c_szwKeyLicPeriod
		                    KLLIC::c_szwKeyLicCount
		                    KLLIC::c_szwKeySerial
		                    KLLIC::c_szwKeyMajVer
		                    KLLIC::c_szwKeyProdName
                    KLCONN_COMPONENTS_INFO_1
                        KLPRSS_VAL_PRODCOMPS                -- список компонентов, имеющих базы, нуждающиеся в обновлении, ARRAY_T|PARAMS_T
                            KLPRSS_VAL_NAME                 -- имя компонента, STRING_T
                            KLPRSS_VAL_VERSION              -- версия компонента, STRING_T
                    KLCONN_CRITICAL_FIXES_INFO_1
                        KLCONN_VAL_FIXES             -- список критических обновлений в порядке их установки, ARRAY_T|PARAMS_T
                            KLPRSS_PRODVAL_DISPLAYNAME      -- локализованное отображаемое имя критического обновления -- строка произвольных символов Unicode длиной не более 256 символов, STRING_T
                            KLPRSS_VAL_NAME                 -- независимый от локализации идентификатор критического обновления -- строка символов первой половины кодовой таблицы длиной не более 64 символа, STRING_T

        */
        virtual void GetApplicationProperties(
                                KLPAR::Params** ppData) = 0;


        /*!
          \brief	Возвращает состояние защиты реального времени. Метод 
                    вызывается для приложений, которые такую защиту 
                    обеспечивают (у которых выставлен флаг 
                    CIF_PROVIDES_RTP в переменной ConnectorFlags в записи 
                    регистрации коннектора).

          \param	nRtpState  OUT состояние защиты реального времени
          \param	nErrorCode OUT код ошибки ( для состояния of APP_RTP_STATE_FAILURE)
        */
        virtual void GetRtpState(
                        AppRtpState&    nRtpState,
                        int&            nErrorCode) = 0;
    };
}
#endif //__KL_APP_SYNCHRONIZER_H__
