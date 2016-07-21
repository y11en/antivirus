/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file HST/Hosts.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Интерфейс для получения информации о компьютерах в логической сети 
 *        Сервера Администрирования.
 *
 */

#ifndef KLHST_HOSTS_H
#define KLHST_HOSTS_H

#include <time.h>
#include <string>
#include <vector>
#include <map>

#include <std/base/klbase.h>
#include <std/hstd/hostdomain.h>
#include <std/par/params.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prts/tasksstorage.h>
#include <kca/prcp/agentproxy.h>

#include <srvp/evp/eventpropertiesclient.h>
#include <transport/ev/ev_general.h>

#include <srvp/admsrv/admsrv.h>
#include <srvp/grp/groups.h>
#include <srvp/updsrv/updsrv.h>
#include <srvp/licsrv/licsrv.h>
#include <srvp/pol/policies.h>

//#include <../apps/server/errors.h"
//#include "../naginst/errors.h"

namespace KLHST
{
	const wchar_t c_szwUnknownDomain[]=L"*";
    //! Перечисление возможных типов доменов в сети
	typedef enum
	{
		DT_WIN_DOMAIN=KLSTD::KLDT_WIN_DOMAIN,
		DT_WIN_WORKGROUP=KLSTD::KLDT_WIN_WORKGROUP
	}DomainType;

//    enum DomainType {
  //      DT_WIN_DOMAIN,  ///< Windows NT домен
    //    DT_WIN_WORKGROUP,  ///< Рабочая группа Windows NT
//    };

    //! Перечисление возможных состояний компьютера в сети.
    enum ComputerStatus {
        CS_VISIBLE =            0x00000001, ///< Компьютер включен и присутствует в сети.
        CS_ADDEDTOGROUP =       0x00000002, ///< Компьютер добавлен в группу
        CS_AGENTINSTALLED =     0x00000004, ///< На компьютер инсталлирован Агент администрирования
        CS_AGENTISALIVE =       0x00000008, ///< Агент администрирования нормально функционирует 
        CS_PRODUCTINSTALLED =   0x00000010, ///< На компьютер инсталлирован rtp.
        CS_INROAMINGMODE =      0x00000020  ///< Компьютер в режиме roaming mode
    };

    //! Перечисление возможных идентификаторов статуса компьютера.
    typedef enum
    {
        CSID_OK =        0x00000000,
        CSID_CRITICAL =  0x00000001,
        CSID_WARNING =   0x00000002
    }ComputerStatusID;
    
    typedef enum
    {
        SMC_HOST_OUT_OF_CONTROL = 0x00000001,
        SMC_RPT_NOT_RUNNING     = 0x00000002,
        SMC_AV_NOT_RUNNING      = 0x00000004,
        SMC_VIRUSES             = 0x00000008,
        SMC_RPT_DIFFERS         = 0x00000010,
        SMC_NO_AV_SOFTWARE      = 0x00000020,
        SMC_OLD_FSCAN           = 0x00000040,
        SMC_OLD_AV_BASES        = 0x00000080,
        SMC_OLD_LAST_CONNECT    = 0x00000100,
        SMC_OLD_LICENSE         = 0x00000200,
        SMC_UNCURED             = 0x00000400
    }StatusMaskCondition;
    

    typedef enum
    {
        MaskRtpState_Stopped                = 0x00010000,
        MaskRtpState_Suspended              = 0x00020000,
        MaskRtpState_Starting               = 0x00040000,
        MaskRtpState_Running                = 0x00080000,
        MaskRtpState_Running_MaxProtection  = 0x00100000,
        MaskRtpState_Running_MaxSpeed       = 0x00200000,
        MaskRtpState_Running_Recomended     = 0x00400000,
        MaskRtpState_Running_Custom         = 0x00800000,
        MaskRtpState_Failure                = 0x01000000
    }RtpStateMask;


	const wchar_t c_hst_array_hosts[] = L"KLHST_WKS_HOSTS";  // Имя параметра 'массив хостов'
	const wchar_t c_hst_array_results[] = L"KLHST_WKS_RESULTS";  // 

//! Имена полей в контейнере Params с информация о рабочей станции в сети.

    //! INT_T, Идентификатор записи в базе данных
    const wchar_t c_wks_id[]			= L"KLHST_WKS_ID";  
    //! INT_T, Идентификатор группы, в которую включен данный компьютер.
    const wchar_t c_wks_groupId[]		= L"KLHST_WKS_GROUPID"; 
    //! TIME_T, Когда компьютер последний раз появлялся в сети
    const wchar_t c_wks_lastVisible[]	= L"KLHST_WKS_LAST_VISIBLE"; 
    //! TIME_T, Последнее время обновления информации о компьютере
    const wchar_t c_wks_lastInfoUpdate[]= L"KLHST_WKS_LAST_INFOUDATE"; 
    //! INT_T, Текущее состояние компьютера из перечисления ComputerStatus.
    const wchar_t c_wks_status[]		= L"KLHST_WKS_STATUS";
    //! TIME_T Последнее время получения обновлений компьютером
    const wchar_t c_wks_lastUpdate[]	= L"KLHST_WKS_LAST_UPDATE"; 
    //! TIME_T последнее время соединения с сетевым агентом
    const wchar_t c_wks_lastNagentConnected[] = L"KLHST_WKS_LAST_NAGENT_CONNECTED";
    //! BOOL_T Если false, то сервер будет рвать соединение с сетевым агентом после каждого пинга
    const wchar_t c_wks_keepConnection[]	= L"KLHST_WKS_KEEP_CONNECTION";
    //! отображаемое имя компьютера, предназначенное для чтения человеком
    const wchar_t c_wks_display_name[]	= L"KLHST_WKS_DN";	// STRING_T
    //! имя компьютера
    const wchar_t c_wks_name[]			= L"KLHST_WKS_HOSTNAME"; // STRING_T
    //! NetBIOS-имя компьютера     
    const wchar_t c_wks_win_name[]		= L"KLHST_WKS_WINHOSTNAME";			// STRING_T
    //! Имя Windows-домена
    const wchar_t c_wks_domain[]		= L"KLHST_WKS_WINDOMAIN";		// STRING_T
    const wchar_t c_wks_win_domain[]	= L"KLHST_WKS_WINDOMAIN";	// STRING_T
    //!
    const wchar_t c_wks_win_domain_type[]	= L"KLHST_WKS_WINDOMAIN_TYPE";		// STRING_T
    //! DNS-домен
    const wchar_t c_wks_dnsDomain[]		= L"KLHST_WKS_DNSDOMAIN";		// STRING_T
    //! DNS - имя
    const wchar_t c_wks_dnsName[]		= L"KLHST_WKS_DNSNAME";		// STRING_T
    //! тип компьютера (см. )
    const wchar_t c_wks_ctype[]			= L"KLHST_WKS_CTYPE";			// INT_T
    //! тип платформы (см. )
    const wchar_t c_wks_ptype[]			= L"KLHST_WKS_PTYPE";			// INT_T
    //! Старшая часть версии ОС
    const wchar_t c_wks_versionMajor[]	= L"KLHST_WKS_OS_VER_MAJOR";	// INT_T
    //! Младшая часть версии ОС
    const wchar_t c_wks_versionMinor[]	= L"KLHST_WKS_OS_VER_MINOR";	// INT_T
    //! Адрес хоста with network byte order
    const wchar_t c_wks_ip[]			= L"KLHST_WKS_IP";			// INT_T
    //! Предполагаемое имя хоста
    const wchar_t c_wks_presumed_groupname[]= L"KLHST_WKS_PRESUMED_GROUPNAME";			// INT_T
    //! Время последнего полного сканирования компьютера
    const wchar_t c_wks_lastFullScan [] = L"KLHST_WKS_LAST_FULLSCAN"; //DATE_TIME_T
    //! Количество обнаружений вирусов
    const wchar_t c_wks_VirusCount[] = L"KLHST_WKS_VIRUS_COUNT";//LONG_T
    //! Статус rtp-защиты
    const wchar_t c_wks_RtpState[] = L"KLHST_WKS_RTP_STATE";//INT_T
    //! Код ошибки rtp-защиты
    const wchar_t c_wks_RtpErrorCode[] = L"KLHST_WKS_RTP_ERROR_CODE";//INT_T
    //! Hosts's public key
    const wchar_t c_wks_public_key[] = L"KLHST_WKS_PKEY";

    //! User comments for host
    const wchar_t c_wks_comment[] = L"KLHST_WKS_COMMENT";   //STRING_T
    
    //! Адрес хоста (беззнаковый) with host byte order
    const wchar_t c_wks_ip_long[]			= L"KLHST_WKS_IP_LONG";			// LONG_T
    
    //! Адрес хоста, полученный при коннекте (беззнаковый) with host byte order
    const wchar_t c_wks_connect_ip_long[]			= L"KLHST_WKS_CONNECT_IP_LONG";			// LONG_T
    
    //! (starting from Mantainance Pack 3)
    const wchar_t c_wks_from_unassigned[] = L"KLHST_WKS_FROM_UNASSIGNED"; // BOOL_T

    //! (starting from v 7)
    const wchar_t c_wsk_UncuredCount[] = L"KLHST_WKS_UNCURED_COUNT"; //LONG_T

    //! ID статуса хоста (ComputerStatusID)
    const wchar_t c_wks_status_id[]		= L"KLHST_WKS_STATUS_ID"; //INT_T

    /*!
        Маска статуса хоста. Бит i в маске статуса означает, 
        что i-ое условие было нарушено.
        Возможные условия:
        Бит Условие
        0.  Потеряна связь с клиентским компьютером (он виден в сети, но агент не соединяется с сервером).
        1.  Не работает RTP защита на компьютере, и при этом установлен антивирус.
        2.  На компьютере не стартовал антивирус (он инсталлирован, но не запустился).
        3.  Число найденных на компьютере вирусов более чем N.  Число N задается администратором (KLSRV_HSTSTAT_SP_VIRUSES). 
        4.  Уровень RTP защиты отличается от заданного администратором, и при этом установлен антивирус (KLSRV_HSTSTAT_SP_RPT_DIFFERS).
        5.  На компьютер не установлена антивирусная защита (агент либо антивирус для WKS, FS). 
        6.  Компьютер не сканировался более N дней, и при этом установлен антивирус. Число N настраивается администратором (KLSRV_HSTSTAT_OLD_FSCAN).
        7.  Антивирусные базы не обновлялись N дней, и при этом установлен антивирус. Число N настраивается администратором (KLSRV_HSTSTAT_OLD_AV_BASES).
        8.  Компьютер не подсоединялся к серверу в течении N дней, и при этом установлен сетевой агент.  Число N настраивается администратором (KLSRV_HSTSTAT_OLD_LAST_CONNECT).
        9.  Лицензия итстекла (для статуса Critical), до истечения лицензии осталось менее N дней. Число N настраивается администратором (KLSRV_HSTSTAT_OLD_LICENSE).
        10. Число невылеченных на компьютере объектов более чем N. Число N задается администратором (KLSRV_HSTSTAT_SP_UNCURED). 

        В случае необходимости выбора одного условия, следует брать
        условие, выдаваемое функцией GetPrimaryStatusCondition.
    */
    const wchar_t c_wks_status_mask[]		= L"KLHST_WKS_STATUS_MASK"; //INT_T
    

    //! Full version of network agent installed on host (KLADMSRV_SV70 and higher)
    const wchar_t c_wks_nagent_version[]		= L"KLHST_WKS_NAG_VERSION"; //STRING_T

    //! Full version of anti-virus with rtp installed on host (KLADMSRV_SV70 and higher)
    const wchar_t c_wks_rtp_av_version[]		= L"KLHST_WKS_RTP_AV_VERSION"; //STRING_T

    //! AV bases date of anti-virus with rtp installed on host (KLADMSRV_SV70 and higher)
    const wchar_t c_wks_rtp_av_bases_time[]		= L"KLHST_WKS_RTP_AV_BASES_TIME"; //DATETIME_T
    
   
    //!Following fields may be used only while searching for hosts    
        //!product name
        const wchar_t c_wks_product_name[] = L"KLHST_WKS_PRODUCT_NAME";//STRING_T
        //!product version
        const wchar_t c_wks_product_version[] = L"KLHST_WKS_PRODUCT_VERSION";//STRING_T
        //!product display version
        const wchar_t c_wks_product_display_version[] = L"KLHST_WKS_PRODUCT_DISPLAY_VERSION";//STRING_T
        //!product AV bases record count
        const wchar_t c_wks_product_avbases_records[] = L"KLHST_WKS_AV_BASES_RECORDS";//INT_T
        //!product AV bases date
        const wchar_t c_wks_product_avbases_date[] = L"KLHST_WKS_AV_BASES_DATE";//DATETIME_T
		//!competitor_product name
        const wchar_t c_wks_competitor_product_name[] = L"KLHST_WKS_COMPETITOR_PRODUCT_NAME";//STRING_T
		//!inventory_product strId
        const wchar_t c_inventory_product_strid[] = L"KLHST_INVENTORY_PRODUCT_STRID";//STRING_T
		//!inventory_product name
        const wchar_t c_inventory_product_name[] = L"KLHST_INVENTORY_PRODUCT_NAME";//STRING_T
		//!inventory_product version
        const wchar_t c_inventory_product_display_version[] = L"KLHST_INVENTORY_PRODUCT_DISPLAY_VERSION";//STRING_T
		//!inventory_product publisher
        const wchar_t c_inventory_product_publisher[] = L"KLHST_INVENTORY_PRODUCT_PUBLISHER";//STRING_T
		//!inventory_product_install_date
        const wchar_t c_inventory_product_install_date[] = L"KLHST_INVENTORY_PRODUCT_INSTALL_DATE";//DATE_TIME_T
		//!inventory_product_install_dir
        const wchar_t c_inventory_product_install_dir[] = L"KLHST_INVENTORY_PRODUCT_INSTALL_DIR";//STRING_T
		//!inventory_patch name
        const wchar_t c_inventory_patch_name[] = L"KLHST_INVENTORY_PATCH_NAME";//STRING_T
		//!inventory_patch version
        const wchar_t c_inventory_patch_display_version[] = L"KLHST_INVENTORY_PATCH_DISPLAY_VERSION";//STRING_T
		//!inventory_patch publisher
        const wchar_t c_inventory_patch_publisher[] = L"KLHST_INVENTORY_PATCH_PUBLISHER";//STRING_T
        //!ads OU
        const wchar_t c_wks_ad_org_unit[] = L"KLHST_AD_ORGUNIT";//INT_T
        
        //! hosts belongs to the specified unit or it's subunit
        const wchar_t c_wks_ad_org_unit_grandparent[] = L"KLHST_AD_ORGUNIT_GP";//INT_T

        //! hosts belongs to the specified group or it's subgroup
        const wchar_t c_wks_groupId_grandparent[] = L"KLHST_WKS_GROUPID_GP";//INT_T
        
        //! network agent installation id (see KLNAG_INSTALLATION_ID), search-only
        const wchar_t c_wks_nagent_instid[] = L"KLHST_NAG_INSTID";//STRING_T
        
        //! host has uncured objects (starting from v 7)
        const wchar_t c_wks_uncured_present[] = L"KLHST_UNCURED_PRESENT"; //BOOL_T

        /*! status bits (for search only)
            c_wks_status_mask_bit* is non-zero if corrsponding bit is set 
            otherwise c_wks_status_mask_bit* is zero

            DEPRECATED, use biwise operation instead
        */
        const wchar_t c_wks_status_mask_bit0[]		= L"KLHST_WKS_STATUS_MASK_0"; //INT_T
        const wchar_t c_wks_status_mask_bit1[]		= L"KLHST_WKS_STATUS_MASK_1"; //INT_T
        const wchar_t c_wks_status_mask_bit2[]		= L"KLHST_WKS_STATUS_MASK_2"; //INT_T
        const wchar_t c_wks_status_mask_bit3[]		= L"KLHST_WKS_STATUS_MASK_3"; //INT_T
        const wchar_t c_wks_status_mask_bit4[]		= L"KLHST_WKS_STATUS_MASK_4"; //INT_T
        const wchar_t c_wks_status_mask_bit5[]		= L"KLHST_WKS_STATUS_MASK_5"; //INT_T
        const wchar_t c_wks_status_mask_bit6[]		= L"KLHST_WKS_STATUS_MASK_6"; //INT_T
        const wchar_t c_wks_status_mask_bit7[]		= L"KLHST_WKS_STATUS_MASK_7"; //INT_T
        const wchar_t c_wks_status_mask_bit8[]		= L"KLHST_WKS_STATUS_MASK_8"; //INT_T

    /*!
        Имя компьютера, введенное пользователем
        Может являться (проверяется в указанном порядке)
               1. ip-адресом
               2. иденитфикатором хоста
               3. отображаемым именем
               4. именем NetBIOS
               5. именем DNS
    */
    const wchar_t   c_wks_anyname_array[]     = L"KLHST_WKS_ANYNAME";     //SRING_T
    
    /*! \brief  Информация о статусе запущенных задач на хосте.
    Структура:

		c_spRtTsksInfo	//PARAMS_T
			<ProductName>	//PARAMS_T
				KLHST::c_spProductVersion	//STRING_T
				KLHST::c_spRtTskArray		//PARAMS_T|ARRAY_T 
					KLHST::c_tsk_State		//INT_T
					KLHST::c_tsk_StorageId	// STRING_T
    */    
    
    /*! \brief  Информация о статусе продуктов на хосте.
    Структура:
        c_spHstAppInfo
            <product>
                <version>
                    c_spHstAppState - см. ComponentInstanceState
                    c_spHstAppStateTime - дата и время последнего обновления статуса                            
    */
    const wchar_t c_spHstAppInfo[] = L"KLHST_APP_INFO";    //PARAMS_T
    const wchar_t c_spHstAppState[] = L"KLHST_APP_STATE";    //INT_T
    const wchar_t c_spHstAppStateTime[] = L"KLHST_APP_STATE_TIME";    //DATE_TIME_T
    
    const wchar_t c_spHostLocation[]=L"KLHST_LOCATION";
    const wchar_t c_spHostInstanceId[]=L"KLHST_INSTANCEID";
	
	//! информация о задачах
	const wchar_t c_tsk_Type[]=L"taskType";				//STRING_T
	const wchar_t c_tsk_State[]=L"taskState";			//INT_T
    const wchar_t c_tsk_Completion[]=L"taskCompletion"; //INT_T
	const wchar_t c_tsk_Id[]=L"taskId";					//INT_T
	const wchar_t c_tsk_StorageId[]=L"taskStorageId";	//STRING_T

	//! информация об экземплярах
	const wchar_t c_inst_State[]=L"instState";			//INT_T
	const wchar_t c_inst_Statistics[]=L"instStatistics";//PARAMS_T
	
	const wchar_t c_spProductName[]=L"klhst-ProductName";		//STRING_T
	const wchar_t c_spProductVersion[]=L"klhst-ProductVersion";	//STRING_T
	const wchar_t c_spComponentName[]=L"klhst-ComponentName";	//STRING_T
	const wchar_t c_spInstanceId[]=L"klhst-InstanceId";			//STRING_T

	const wchar_t c_spRtTsksInfo[]=L"klhst-rt-TskInfo";	//PARAMS_T|ARRAY_T	
	const wchar_t c_spRtTskCmpIndex[]=L"klhst-rt-TskCmpIndex";//INT_T
	const wchar_t c_spRtTskInstIndex[]=L"klhst-rt-TskInstIndex";//INT_T	
	const wchar_t c_spRtTskArray[]=L"klhst-rt-TskArray";//PARAMS_T|ARRAY_T
	const wchar_t c_spRtInstInfo[]=L"klhst-rt-InstInfo";	//PARAMS_T
	const wchar_t c_spRtInstInfoArray[]=L"klhst-rt-InstInfoArray";	//PARAMS_T|ARRAY_T
	const wchar_t c_spRtCmpArray[]=L"klhst-rt-CmpArray";//STRING_T|ARRAY_T
	const wchar_t c_spRtInstArray[]=L"klhst-rt-InstArray";//STRING_T|ARRAY_T
	const wchar_t c_spRtPrdNamesArray[]=L"klhst-rt-PrdNamesArray";//STRING_T|ARRAY_T		

    //! TIME_T время добавления хоста в базу
    const wchar_t c_wks_Created[]	= L"KLHST_WKS_CREATED"; 

    //! host has update agent installed, BOOL_T
    const wchar_t c_wks_HasUpdateAgent[] = L"HST_HAS_UPDATE_AGENT";

    /*!
    * \brief Интерфейс для получения информации о компьютерах в сети 
    *        Сервера Администрирования.
    *
    */

	typedef struct
	{
		std::wstring	wstrName;
		DomainType		nType;
	}domain_info_t;

	typedef struct
	{
        std::wstring	wstrName;       //host id
		std::wstring	wstrWinHostName;    //windows host name
		ComputerStatus	nType;
	}host_info_t;

	typedef struct
	{
		std::wstring	wstrName;
		std::wstring	wstrVersion;
	}product_info_t;


    /*!
      \brief	Функция возвращает наиболее приоритетное

      \param	unsigned mask
      \return	inline int 
    */
    inline int GetPrimaryStatusCondition(unsigned mask)
    {
        if(!mask || mask > 2047)
            return -1;
        size_t i;
        for(i = 0; i < 10 && !(mask & 1); ++i, mask >>= 1);
        return i;
    };

    class KLSTD_NOVTABLE Hosts : public KLSTD::KLBaseQI
    {
    public:

    /*!
      \brief Возвращает список Windows доменов в сети
       
        \param domains [out] Список доменов в сети.
    */
        virtual void GetDomains (std::vector<domain_info_t>& domains)  = 0;


    /*!
      \brief Возвращает список имен рабочих станций в домене.

        Информация получается из контроллера домена. Данный вызов
        возвращает полный список рабочих станций в домене, даже если
        рабочая станция сейчас выключена.

        \param domain       [in]  Имя домена.
        \param hosts        [out] Список рабочих станций, зарегистрированных в домене.
       
    */
        virtual void GetDomainHosts(
                            const std::wstring&         domain,
                            std::vector<host_info_t>&   hosts)=0;


    /*!
      \brief Возвращает информацию о компьютере

        \param wstrHostName [in]  имя компьютера (НЕ имя NetBIOS !!! ).
        \param fields       [in]  Список полей, которые требуется получить (в т.ч. c_spHstAppInfo и c_spHstTskInfo)
        \param info         [out] Информация о компьютере.
		
		\exception STDE_NOTFOUND указанный компьютер не найден
       
    */
        virtual void GetHostInfo(						
						const std::wstring& wstrName,
						const wchar_t**		fields,
						int					size,
						KLPAR::Params**     info) = 0;

    /*!
     \brief Добавляет новый домен к базе данных.
	 
	   \param domain       [in]  Имя домена.
	   \param type       [in]  Тип домена.
	 \exception STDE_EXIST домен с указанным именем уже существует.
    */		
		virtual void AddDomain(
						const std::wstring&	domain,
						DomainType			nType)=0;

    /*!
      \brief Удаляет домен из базы данных.

        \param domain       [in]  Имя домена.
    */
		virtual void DelDomain(const std::wstring&	domain)=0;

    /*!
      \brief Добавляет новый компьютер к базе данных.

        \param info         [in] Информация о компьютере.
						Контейер может содержать следующие атрибуты.
							Обязательные:
                                c_wks_display_name,
                                c_wks_groupId,
							Необязательные :
                                c_wks_win_domain,
                                c_wks_dnsName,
                                c_wks_dnsDomain
								c_wks_lastVisible,
								c_wks_lastInfoUpdate,
								c_wks_lastUpdate,
								c_wks_status,
								c_wks_ctype,
								c_wks_ptype,
								c_wks_versionMajor,
								c_wks_versionMinor,
								c_wks_ip,
                                c_wks_keepConnection
							Прочие атрибуты игнорируются.
        \return идентификатор добавленного хоста
    */
        virtual std::wstring AddHost(KLPAR::Params* pInfo)=0;
		
    /*!
      \brief Удаляет компьютер из базы данных.

        \param name         [in]  имя компьютера (НЕ имя NetBIOS !!! ).
       
    */
        virtual void DelHost(
                    const std::wstring & name )=0;

    /*!
      \brief Обновляет информацию о комьютере в базе данных.

        \param name         [in]  имя компьютера (НЕ имя NetBIOS !!! ).
        \param info         [in]  Информация о компьютере.
						Контейнер может содержать следующие атрибуты.
								c_wks_name,
								c_wks_dnsName,
								c_wks_win_domain,
								c_wks_dnsDomain
								c_wks_lastVisible,
								c_wks_lastInfoUpdate,
								c_wks_lastUpdate,
								c_wks_status,
								c_wks_ctype,
								c_wks_ptype,
								c_wks_versionMajor,
								c_wks_versionMinor,
								c_wks_ip,
                                c_wks_keepConnection
							Прочие атрибуты игнорируются.
			
			 \except STDE_NOTFOUND домен не найден
       
    */
        virtual void UpdateHost(
              const std::wstring & name,
              KLPAR::Params  * info )=0;


    /*!
      \brief Возвращает информацию о списке продуктов на узле.

        \param name         [in]  имя компьютера (НЕ имя NetBIOS !!! ).
        \param ppProducts   [out] 
                c_hst_array_hosts   //ARRAY_T|PARAMS_T
                    Каждый элемент массива содержит поля c_spProductName и
                    c_spProductVersion, кроме того, могут присутствовать
                    следующие поля (см. prss/settingsstorage.h):
                        KLPRSS_PRODVAL_INSTALLTIME
                        KLPRSS_PRODVAL_LASTUPDATETIME
                        KLPRSS_PRODVAL_DISPLAYNAME
                        KLPRSS_PRODVAL_BASEDATE
                        KLPRSS_PRODVAL_BASEINSTALLDATE
                        KLPRSS_PRODVAL_BASERECORDS
                        KLPRSS_PRODVAL_VERSION
    */
        virtual void GetHostProducts(
                const std::wstring&     name,
                KLPAR::Params**         ppProducts)=0;

    /*!
      \brief Возвращает информацию о настройках продуктов на узле.

        \param name         [in]  имя компьютера (НЕ имя NetBIOS !!! ).
		\param type         [in]  Тип хранилища.
        \param settings     [out] Настройки компьютера на удаленном узле.
       
    */
        virtual void GetHostSettings(
						const std::wstring & name,
						const std::wstring & type,
						KLPRSS::SettingsStorage  ** settings )=0;


    /*!
      \brief Возвращает информацию о задачах, определенных для данного узла.

        \param name         [in]  имя компьютера (НЕ имя NetBIOS !!! ).
        \param tasks        [out] Задачи, определенные для компьютера на удаленном узле.
       
    */
        virtual void GetHostTasks(
				const std::wstring&		name,
				const std::wstring&		product,
				const std::wstring&		version,
				KLPRTS::TasksStorage**	tasks,
				long					lLifetime = 20) = 0;
		
    /*!
      \brief Возвращает информацию реального времени
        \param name         [in]  имя компьютера (НЕ имя NetBIOS !!! ).
		\param pFilter      [in]  Фильтр. Имеет следующую структуру:
			1. требуется информация о запущенных задачах
				c_spRtTsksInfo	//PARAMS_T
					-Требуется информация по задачам продуктов, если перемнная
					 с_spRtPrdNamesArray отсутствует, возвращается информация
					 по задачам всех продуктов
					 с_spRtPrdNamesArray		//STRING_T|ARRAY_T
					-Требуется информация по c_spProductVersion
					 KLHST::c_spProductVersion	//STRING_T
					-Требуется информация по c_tsk_Type
					 KLHST::c_tsk_Type			//STRING_T
					-Требуется информация по c_tsk_State
					 KLHST::c_tsk_State			//INT_T
					-Требуется информация по c_tsk_Id
					 KLHST::c_tsk_Id				//INT_T
					-Требуется информация по c_tsk_StorageId
					 KLHST::c_tsk_StorageId		// STRING_T
			2. требуется информация о запущенных экземплярах компонентов
				c_spRtInstInfo	//PARAMS_T
					-Требуется информация по экземплярам компонентов продуктов,
					 если перемнная с_spRtPrdNamesArray отсутствует,
					 возвращается информация по компонентам всех продуктов
					 с_spRtPrdNamesArray		//STRING_T|ARRAY_T
					-Требуется информация по c_spProductVersion
					 KLHST::c_spProductVersion	//STRING_T
					-Требуется информация по c_spComponentName
					 KLHST::c_spComponentName	//STRING_T
					-Требуется информация по c_spInstanceId
					 KLHST::c_spInstanceId		//STRING_T
					-Требуется информация по c_inst_Statistics. 
					 KLHST::c_inst_Statistics	//PARAMS_T
					-Требуется информация по c_inst_State
					 KLHST::c_inst_State		//INT_T
            3. требуется информация о запущенных приложениях
                c_spHstAppInfo
		\param ppTasksInfo  [out] Информация реального времени. Имеет
			следующую структуру:		
			1. о запущенных задачах
				c_spRtTsksInfo	//PARAMS_T
					<ProductName>	//PARAMS_T
						KLHST::c_spProductVersion	//STRING_T
						KLHST::c_spRtTskArray		//PARAMS_T|ARRAY_T 
							KLHST::c_tsk_Type		//STRING_T
							KLHST::c_tsk_State		//INT_T
							KLHST::c_tsk_Id			//INT_T
							KLHST::c_tsk_StorageId	// STRING_T
							KLHST::c_spRtTskCmpIndex	//INT_T
							KLHST::c_spRtTskInstIndex	//INT_T
					KLHST::c_spRtCmpArray			//ARRAY_T|STRING_T
					KLHST::c_spRtInstArray			//ARRAY_T|STRING_T
			2. о запущенных экземплярах компонентов
				c_spRtInstInfo	//PARAMS_T
					<ProductName>	//PARAMS_T
						KLHST::c_spProductVersion	//STRING_T
						KLHST::c_spRtInstInfoArray	//PARAMS_T|ARRAY_T 
							KLHST::c_spComponentName//STRING_T
							KLHST::c_spInstanceId	//STRING_T
							KLHST::c_inst_Statistics//PARAMS_T
							KLHST::c_inst_State		//INT_T
            3. о запущенных приложениях
                c_spHstAppInfo
                <product>
                    <version>
                        c_spHstAppState - см. ComponentInstanceState
                        c_spHstAppStateTime - дата и время последнего обновления статуса                            


          \exception    KLSRV::KLSRV_ERR_NO_HOST_ADDRESS - серверу неизвестен адрес хоста.
          \exception    KLSRV::KLSRV_ERR_HOST_NOT_RESPONDING - хост не отвечает
    */
		virtual void GetHostRuntimeInfo(
								const std::wstring&	name,
								KLPAR::Params*		pFilter,
								KLPAR::Params**		ppTasksInfo,
                                KLSTD::KLBaseQI*    pConnDesc = NULL) = 0;


    /*!
      \brief Возвращает объект AgentProxy для данного узла.

        \param name         [in]  Имя компьютера.
        \param agent        [out] AgentProxy.

          \exception    KLSRV::KLSRV_ERR_NO_HOST_ADDRESS - серверу неизвестен адрес хоста.
          \exception    KLSRV::KLSRV_ERR_HOST_NOT_RESPONDING - хост не отвечает
          \exception    KLSRV::KLNAG_ERR_APP_NOT_RUNNING - приложение (product v. version) не запущено       
    */
        virtual void GetHostAgentProxy(
						const std::wstring&		name,
						const std::wstring&		product,
						const std::wstring&		version,
						KLPRCP::AgentProxy**	agent,
                        KLSTD::KLBaseQI*        pConnDesc = NULL)=0;

        /*!
          \brief    
			\param  vectSeverities - фильтр по важности событий (см. c_NFXXXXDescr)					
					!!! поддерживается только локальной реализацией интерфейса Hosts !!!
		*/
        virtual void GetHostEventPropertiesProxy(
            const std::wstring&		                    domain,
			const std::wstring&		                    name,
			const std::wstring&		                    product,
			const std::wstring&		                    version,
            const std::vector<std::wstring>&            vect_fields,
	        const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	        long                                        lifetime,
	        KLEVP::EventProperties**                    ppEventProperties,
			const std::vector<std::wstring> &			vectSeverities) =0;

        virtual void GetGroups(KLGRP::Groups** ppGroups) =0;
        virtual void GetUpdates(KLUPDSRV::Updates** ppUpdates) =0;
        virtual void GetLicSrv(KLLICSRV::SrvLicences** ppLicSrv) =0;
        virtual void GetPol(KLPOL::Policies** ppPolicies) =0;
        
        virtual void GetComponentTransportName(std::wstring& wstrTransportName)=0;

        /*!
          \brief    Форсирует синхронизацию указанного в параметре wstrSSType
                    хранилища. Вызов синхронный, т.е. метод не возвращает
                    управление до тех пор, пока не выполнится любое из
                    следующих условий: синхронизация не пройдёт до
                    конца или или не произойдёт ошибка.

          \param    name - имя компьютера (НЕ имя NetBIOS !!! ).
          \param    wstrSSType - идентификатор хранилища (пустая строка
                        означает, что необходмо провести синхронизацию для всех
                        хранилищ)
          \param    lWaitTimeout - максимальное время ожидания в мс - число
                        большее нуля
          \param    bCancelWait - запись в переменную, на которую указывает
                        данный указатель, значения true приводит к прерыванию
                        ожидания завершения и выбрасыванию исключения
                        STDE_CANCELED.

          \exception    KLSTD::STDE_CANCELED - ожидание было прервано
          \exception    KLSTD::STDE_TIMEOUT - истёк таймаут lWaitTimeout
          \exception    KLSRV::KLSRV_ERR_NO_HOST_ADDRESS - серверу неизвестен адрес хоста.
          \exception    KLSRV::KLSRV_ERR_HOST_NOT_RESPONDING - хост не отвечает
        */
        virtual void ForceSynchronizationSync(
						const std::wstring&		name,
                        const std::wstring&     wstrSSType,
                        long                    lWaitTimeout,
                        volatile bool*          bCancelWait=NULL) = 0;

        /*!
          \brief    Стартует указанное приложение на удалённом хосте.

          \param    name - имя хоста
          \param    wstrProductName - имя приложения
          \param    wstrProductName - версия приложения
          \param    lWaitTimeout - максимальное время ожидания в мс - число
                        большее нуля
          \param    bCancelWait - запись в переменную значения true приводит к
                        прерыванию ожидания и выбрасыванию исключения
                        STDE_CANCELED.

          \exception    KLSRV::KLSRV_ERR_NO_HOST_ADDRESS - серверу неизвестен адрес хоста.
          \exception    KLSRV::KLSRV_ERR_HOST_NOT_RESPONDING - хост не отвечает
          \exception    KLSTD::STDE_CANCELED - ожидание было прервано
          \exception    KLSTD::STDE_TIMEOUT - истёк таймаут lWaitTimeout
          \exception    KLNAG::KLNAG_ERR_APP_NOTINSTALLED - приложение не установлено
          \exception    KLNAG::KLNAG_ERR_APP_NOT_RUNNABLE - приложение не поддерживает ручной старт
          \exception    KLNAG::KLNAG_ERR_APP_STOPPED_UNEXPECTEDLY - приложение запустилось и неожиданно завершилось

          \return   true, если приложение было запущено;
                    false - если приложение уже запущено
        */
        virtual bool StartProduct(
					const std::wstring&		name,
                    const std::wstring&     wstrProductName,
                    const std::wstring&     wstrProductVersion,
                    long                    lWaitTimeout,
                    volatile bool*          bCancelWait=NULL) = 0;


        /*!
          \brief    Останавливает указанное приложение на удалённом хосте.

          \param    name - имя хоста
          \param    wstrProductName - имя приложения
          \param    wstrProductName - версия приложения
          \param    lWaitTimeout - максимальное время ожидания в мс - число
                        большее нуля
          \param    bCancelWait - запись в переменную значения true приводит к
                        прерыванию ожидания и выбрасыванию исключения
                        STDE_CANCELED.

          \exception    KLSRV::KLSRV_ERR_NO_HOST_ADDRESS - серверу неизвестен адрес хоста.
          \exception    KLSRV::KLSRV_ERR_HOST_NOT_RESPONDING - хост не отвечает
          \exception    KLSTD::STDE_CANCELED - ожидание было прервано
          \exception    KLSTD::STDE_TIMEOUT - истёк таймаут lWaitTimeout
          \exception    KLNAG::KLNAG_ERR_APP_NOTINSTALLED - приложение не установлено
          \exception    KLNAG::KLNAG_ERR_APP_NOT_STOPPABLE - приложение не подерживает ручную остановку

          \return   true, если приложение было успешно остановлено;
                    false - если приложение не было запущено
        */
        virtual bool StopProduct(
					const std::wstring&		name,
                    const std::wstring&     wstrProductName,
                    const std::wstring&     wstrProductVersion,
                    long                    lWaitTimeout,
                    volatile bool*          bCancelWait=NULL) = 0;

		
        /*!
          \brief    

			\param  vectSeverities - фильтр по важности событий (см. c_NFXXXXDescr)					
					!!! поддерживается только локальной реализацией интерфейса Hosts !!!
		*/
        virtual void GetHostEventPropertiesProxy(
            const std::wstring&		                    domain,
			const std::wstring&		                    name,
			const std::wstring&		                    product,
			const std::wstring&		                    version,
	        const KLPAR::Params*                        pParamsIteratorDescr,
            const std::vector<std::wstring>&            vect_fields,
	        const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	        long                                        lifetime,
	        KLEVP::EventProperties**                    ppEventProperties,
			const std::vector<std::wstring> &			vectSeverities) =0;


        //OBSOLETE
        inline void GetHostAgentProxy(
                        const std::wstring&		domain,
						const std::wstring&		name,
						const std::wstring&		product,
						const std::wstring&		version,
						KLPRCP::AgentProxy**	agent)
        {
            GetHostAgentProxy(
                            name,
                            product,
                            version,
                            agent,
                            NULL);
        };

		inline void GetHostRuntimeInfo(
                                const std::wstring&	domain,
								const std::wstring&	name,
								KLPAR::Params*		pFilter,
								KLPAR::Params**		ppTasksInfo)
        {
            GetHostRuntimeInfo(name, pFilter, ppTasksInfo);
        }

        inline void ForceSynchronizationSync(
                        const std::wstring&	    domain,
						const std::wstring&		name,
                        const std::wstring&     wstrSSType,
                        long                    lWaitTimeout,
                        volatile bool*          bCancelWait=NULL)
        {
            ForceSynchronizationSync(
                                    name,
                                    wstrSSType,
                                    lWaitTimeout,
                                    bCancelWait);
        }

        inline bool StartProduct(
                    const std::wstring&	    domain,
					const std::wstring&		name,
                    const std::wstring&     wstrProductName,
                    const std::wstring&     wstrProductVersion,
                    long                    lWaitTimeout,
                    volatile bool*          bCancelWait=NULL)
        {
            return StartProduct(
                            name,
                            wstrProductName,
                            wstrProductVersion,
                            lWaitTimeout,
                            bCancelWait);
        };

        inline bool StopProduct(
                    const std::wstring&	    domain,
					const std::wstring&		name,
                    const std::wstring&     wstrProductName,
                    const std::wstring&     wstrProductVersion,
                    long                    lWaitTimeout,
                    volatile bool*          bCancelWait=NULL)
        {
            return StopProduct(
                            name,
                            wstrProductName,
                            wstrProductVersion,
                            lWaitTimeout,
                            bCancelWait);
        }

        inline void AddHosts(KLPAR::Params* pData, KLPAR::Params** ppOutData)
        {
            KLSTD::CAutoPtr<KLGRP::Groups> pGroups;
            if(!QueryInterface(KLSTD_IIDOF(KLGRP::Groups), (void**)&pGroups))
                return;

            std::vector<std::wstring> vecNames;
            long idGroup = -1;
            {
                KLSTD::CAutoPtr<KLPAR::ArrayValue> pArray;
                pData->GetValue(c_hst_array_hosts, (KLPAR::Value**)&pArray);
                if(!pArray || pArray->GetType() != KLPAR::Value::ARRAY_T)
                    return;

                const int nSize = pArray->GetSize();
                vecNames.reserve(nSize);                
            
                for(int i=0; i < nSize; ++i)
                {
                    KLSTD::CAutoPtr<KLPAR::ParamsValue> pEntry;
                    pArray->GetAt(i, (KLPAR::Value**)&pEntry);
                    if(!pEntry || pEntry->GetType() != KLPAR::Value::PARAMS_T)
                        continue;
                    KLSTD::CAutoPtr<KLPAR::Params> pItem=pEntry->GetValue();
                    if(!pItem)
                        continue;                
                    idGroup = KLPAR::GetIntValue(pItem, KLHST::c_wks_groupId);
                    const std::wstring& wstrName =
                           KLPAR::GetStringValue(pItem, KLHST::c_wks_name);
                    vecNames.push_back(wstrName);
                }
                if(idGroup == -1 || vecNames.size() == 0)
                    return;
            };
            KLSTD::CAutoPtr<KLPAR::Params> pInData;
            {
                KLSTD::CAutoPtr<KLPAR::ArrayValue> pArray;
                KLPAR::CreateStringValueArray(vecNames, &pArray);

                KLPAR::param_entry_t pars[]=
                {
                    KLPAR::param_entry_t(KLHST::c_wks_anyname_array, pArray),
                    KLPAR::param_entry_t(KLHST::c_wks_groupId, idGroup)
                };
                KLPAR::CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pInData);
            };

            pGroups->ResolveAndMoveToGroup(pInData, ppOutData);
        }

        inline void GetHostSettings(
                        const std::wstring& domain,
						const std::wstring & name,
						const std::wstring & type,
						KLPRSS::SettingsStorage  ** settings )
        {
            GetHostSettings(name, type, settings);
        };

        inline void UpdateHost(
                            const std::wstring& domain,
                            const std::wstring & name,
                            KLPAR::Params  * info )
        {
            UpdateHost(name, info);
        }

        inline void GetHostInfo(
                        const std::wstring& domain,
						const std::wstring& wstrName,
						const wchar_t**		fields,
						int					size,
						KLPAR::Params**     info)
        {
            GetHostInfo(
                        wstrName,
                        fields,
                        size,
                        info);
        };

        inline void GetHostProducts(
                const std::wstring&     domain,
                const std::wstring&     name,
                KLPAR::Params**         ppProducts)
        {
            GetHostProducts(name, ppProducts);
        };

        inline void GetHostTasks(
                const std::wstring&     domain,
				const std::wstring&		name,
				const std::wstring&		product,
				const std::wstring&		version,
				KLPRTS::TasksStorage**	tasks,
				long					lLifetime = 20)
        {
            GetHostTasks(
                        name,
                        product,
                        version,
                        tasks,
                        lLifetime);
        };
    };

    //!obsolete
    const wchar_t c_spHstTskInfo[] = L"KLHST_TSK_INFO";
    const wchar_t c_spHstTskStgId[] = L"KLHST_TSK_STGID";
    const wchar_t c_spHstTskState[] = L"KLHST_TSK_STATE";

    //! Network scanning type
    typedef enum
    {
        NST_AD = 1,     //! AD
        NST_MSN_FAST,   //! Ms network fast scanning
        NST_MSN_FULL,   //! Ms network full scanning
        NST_DPNS        //! Ip diapasons scanning
    }
    NetScanType;

    class KLSTD_NOVTABLE Hosts2 : public KLHST::Hosts
    {
    public:
        /*!
          \brief    Форсирует синхронизацию указанного в параметре wstrSSType
                    хранилища. Вызов синхронный, т.е. метод не возвращает
                    управление до тех пор, пока не выполнится любое из
                    следующих условий: синхронизация не пройдёт до
                    конца или или не произойдёт ошибка.

          \param    name - имя компьютера (НЕ имя NetBIOS !!! ).
          \param    wstrSSType - идентификатор хранилища (пустая строка
                        означает, что необходмо провести синхронизацию для всех
                        хранилищ)

          \exception    KLSTD::STDE_CANCELED - ожидание было прервано
          \exception    KLSTD::STDE_TIMEOUT - истёк таймаут lWaitTimeout
          \exception    KLSRV::KLSRV_ERR_NO_HOST_ADDRESS - серверу неизвестен адрес хоста.
          \exception    KLSRV::KLSRV_ERR_HOST_NOT_RESPONDING - хост не отвечает
        */
        virtual void ForceSync(
						const std::wstring&		name,
                        const std::wstring&     wstrSSType,
                        KLADMSRV::AdmServerAdviseSink*      pSink,
                        KLADMSRV::HSINKID&                  hSink) = 0;

        /*!
          \brief    Добавляет хосты в очередь на синх-цию.
                    Supported IDs for AdmServerAdviseSink::OnNotify
                        c_nAdviseSinkFailure
                        c_nAdviseSinkOK
                        c_nAdviseSinkCompletion
                    В отличие от метода AdmServerAdviseSink::ForceSync
                    ожидание окончания синхронизации не выполняется, 
                     осуществляется только добавление в очередь. 

          \param    vecHosts - имена хостов
          \param    wstrSSType - идентификатор хранилища (пустая строка
                        означает, что необходмо провести синхронизацию для всех
                        хранилищ)
        */
        virtual void AddHostsForSync(
                        const std::vector<std::wstring>&    vecHosts,
                        const std::wstring&                 wstrSSType,
                        KLADMSRV::AdmServerAdviseSink*      pSink,
                        KLADMSRV::HSINKID&                  hSink) = 0;

        /*!
          \brief    Добавляет хосты в очередь на синх-цию.
                    Supported IDs for AdmServerAdviseSink::OnNotify
                        c_nAdviseSinkFailure
                        c_nAdviseSinkOK
                        c_nAdviseSinkCompletion
                    В отличие от метода AdmServerAdviseSink::ForceSync
                    ожидание окончания синхронизации не выполняется, 
                     осуществляется только добавление в очередь. 

          \param    idGroup - id группы 
          \param    wstrSSType - идентификатор хранилища (пустая строка
                        означает, что необходмо провести синхронизацию для всех
                        хранилищ)
        */
        virtual void AddGroupHostsForSync(
                        long                    idGroup,
                        const std::wstring&     wstrSSType,
                        KLADMSRV::AdmServerAdviseSink*      pSink,
                        KLADMSRV::HSINKID&                  hSink) = 0;
       
        /*!
          \brief	Restarts specified network scanning type

          \param	type IN network scanning type
        */
        virtual void RestartNetworkScanning(KLHST::NetScanType type) = 0;
    };
}
/*!
	\brief Создает объект класса KLHST::Hosts

	\param szwAddress [in] Адрес в формате http://<host_name> [: <port_number>]
	\param ppHosts [out] Указатель на переменную, в котрую будет записан
		указатель на объект класса KLHST::Hosts.
	\param pPorts [in] Массив дополнительных портов (завершается 0)
*/
KLCSSRVP_DECL void KLHST_CreateHosts(
						const wchar_t*  szwAddress,
						KLHST::Hosts ** ppHosts,
						const unsigned short* pPorts=NULL,
                        bool            bUseSsl = true);

KLCSSRVP_DECL void KLHST_CreateLocalHosts(KLHST::Hosts ** ppHosts);

#endif // KLHST_HOSTS_H
