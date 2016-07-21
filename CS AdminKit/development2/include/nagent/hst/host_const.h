/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file HST/host_const.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Интерфейс для получения информации о компьютерах в логической сети 
 *        Сервера Администрирования.
 *
 */

#ifndef KLHST_HOST_CONST_H
#define KLHST_HOST_CONST_H

#include <time.h>
#include <string>
#include <vector>
#include <map>

#include <std/base/klbase.h>
#include <std/par/params.h>

#include <transport/tr/errors.h>

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
        CS_PRODUCTINSTALLED =   0x00000010, ///< На компьютер инсталлирован продукт.
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
        SMC_OLD_LAST_CONNECT    = 0x00000100
    }StatusMaskCondition;

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
    //! Адрес хоста
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
    
    //! Адрес хоста (беззнаковый)
    const wchar_t c_wks_ip_long[]			= L"KLHST_WKS_IP_LONG";			// LONG_T

    //! ID статуса хоста (ComputerStatusID)
    const wchar_t c_wks_status_id[]		= L"KLHST_WKS_STATUS_ID"; //INT_T

    /*!
        Маска статуса хоста. Бит i в маске статуса означает, 
        что i-ое условие было нарушено.
        Возможные условия:
        Бит Условие
        0	Потеряна связь с клиентским компьютером (он виден в сети, но агент не соединяется с сервером).
        1	Не работает RTP защита на компьютере, и при этом установлен антивирус.
        2	На компьютере не стартовал антивирус (он инсталлирован, но не запустился).
        3	Число найденных на компьютере вирусов более чем N.  Число N задается администратором. 
        4	Уровень RTP защиты отличается от заданного администратором, и при этом установлен антивирус.
        5	На компьютер не установлена антивирусная защита (агент либо антивирус для WKS, FS). 
        6	Компьютер не сканировался более N дней, и при этом установлен антивирус. Число N настраивается администратором.
        7	Антивирусные базы не обновлялись N дней, и при этом установлен антивирус. Число N настраивается администратором.
        8	Компьютер не подсоединялся к серверу в течении N дней, и при этом установлен сетевой агент.  Число N настраивается администратором.         

        В случае необходимости выбора одного условия, следует брать
        условие, соответствующее самому младшему установленному биту в
        маске статуса. Это можно сделать с помощью функции
        GetPrimaryStatusCondition.
    */
    const wchar_t c_wks_status_mask[]		= L"KLHST_WKS_STATUS_MASK"; //INT_T
    
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
        if(!mask || mask > 511)
            return -1;
        size_t i;
        for(i = 0; i < 9 && !(mask & 1); ++i, mask >>= 1);
        return i;
    };

   
    
}

#endif // KLHST_HOST_CONST_H

