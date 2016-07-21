/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	srvp/rpt/admsrvstat.h
 * \author	Andrew Lashchenkov
 * \date	22.08.2005 12:00:00
 * \brief	интерфейс для получения статистики и обобщенных отчетов
 * 
 */

#ifndef __KLPRT_ADMSRVSTAT_H__
#define __KLPRT_ADMSRVSTAT_H__

#pragma warning  (disable : 4786 )
#include <vector>

#include <std/base/klstd.h>
#include <srvp/rpt/reports.h>

namespace KLRPT
{
    /*!
        \brief AdmSrvStatistics - интерфейс получения статистики и обобщенных отчетов */
    class AdmSrvStatistics : public KLSTD::KLBaseQI{
    public:
	    /*!
        \brief запрос информации 
			\param pRequestParams - Params с перечислением запрашиваемых параметров;
									каждый элемент имеет тип Params и опционально содержит параметры запроса;
			\param wstrRequestId - уникальный идентификатор запроса;
		*/
        virtual void RequestStatisticsData(
			const KLSTD::CAutoPtr<KLPAR::Params> pRequestParams,
            const std::wstring& wstrRequestId,
			KLEV::AdviseEvSink* pSink,
            KLEV::HSINKID& hSink) = 0;

	    /*!
        \brief запрос информации 
			\param pRequestParams - отмена запроса;
			\param wstrRequestId - идентификатор запроса;
		*/
        virtual void CancelStatisticsRequest(
            const std::wstring & wstrRequestId) = 0;

	    /*!
        \brief запрос получение результата запроса;
			\param wstrRequestId - идентификатор запроса;
			\param ppParamsData - полученные данные.
		*/
        virtual void GetStatisticsData(
            const std::wstring& wstrRequestId,
			KLPAR::Params** ppParamsData) = 0;
	};

	class AdmSrvStatistics2 : public AdmSrvStatistics{
	public:
	    /*!
        \brief принудительный сброс данных статистики, например, сброс статуса "Вирусная атака" или 
			"Ошибка выполнения задачи Сервера администрирования" после ознакомления с детальной информацией. 
			\param pRequestParams - Params с перечислением статусов, которые надо "сбросить";
		*/
        virtual void ResetStatisticsData(
			const KLSTD::CAutoPtr<KLPAR::Params> pRequestParams) = 0;
	};

	// Counters parameters:
	const wchar_t c_rptStatPeriodInSec[]=L"KLPPT_StatPeriodInSec"; // INT_T
	
	const wchar_t c_rptStatTimeGridSec[]=L"KLPPT_StatTimeGridSec"; // ARRAY_T

	const wchar_t c_rptReturnDataAsSeries[]=L"KLPPT_ReturnDataAsSeries"; // BOOL_T

	const wchar_t c_rptMaxSubitemsCount[]=L"KLPPT_MaxSubitemsCount"; // INT_T

	const wchar_t c_rptStatFinishTime[]=L"KLPPT_StatFinishTime"; // DATETIME_T

	// Counters:
	const wchar_t c_rptUnassignedHostsCount[]=L"KLPPT_UnassignedHostsCount"; // INT_T

	const wchar_t c_rptNetScanPercent[]=L"KLPPT_NetScanPercent"; // INT_T

	const wchar_t c_rptNewHostsCount[]=L"KLPPT_NewHostsCount"; // INT_T
		// c_rptStatPeriodInSec

	const wchar_t c_rptLastNetScanTime[]=L"KLPPT_LastNetScanTime"; // DATE_TIME_T
	
	const wchar_t c_rptDomainBeingScanned[]=L"KLRPT_DomainBeingScanned"; //STRING_T

	const wchar_t c_rptGroupsHostsCount[]=L"KLPPT_GroupsHostsCount"; // INT_T

	const wchar_t c_rptCrtHostsCount[]=L"KLPPT_CrtHostsCount"; // INT_T

	const wchar_t c_rptWrnHostsCount[]=L"KLPPT_WrnHostsCount"; // INT_T

	const wchar_t c_rptSrvConnectionsCount[]=L"KLPPT_SrvConnectionsCount"; // INT_T

	const wchar_t c_rptLastSrvConnTimeHistogram[]=L"KLPPT_LastSrvConnTimeHistogram"; // PARAMS_T
		// c_rptStatTimeGridSec
		// c_rptReturnDataAsSeries

	const wchar_t c_rptInfectedObjectsCount[]=L"KLPPT_InfectedObjectsCount"; // INT_T
		// c_rptStatPeriodInSec

	const wchar_t c_rptLastUpdateTime[]=L"KLPPT_LastUpdateTime"; // DATE_TIME_T

	const wchar_t c_rptUpdTskState[]=L"KLPPT_UpdTskState"; // INT_T

	const wchar_t c_rptUpdTskStateDescr[]=L"KLPPT_UpdTskStateDescr"; // STRING_T

	const wchar_t c_rptAVSBasesAgeHistogram[]=L"KLPPT_AVSBasesAgeHistogram"; // PARAMS_T

	const wchar_t c_rptRTPStateHistogram[]=L"KLPPT_RTPStateHistogram"; // PARAMS_T

	const wchar_t c_rptFullScanTimeHistogram[]=L"KLPPT_FullScanTimeHistogram"; // PARAMS_T

	const wchar_t c_rptVirusesHistogram[]=L"KLPPT_VirusesHistogram"; // PARAMS_T

	const wchar_t c_rptStatViractPeriodInSec[]=L"KLPPT_StatViractPeriodInSec"; // INT_T

	// Рабочие станции: // за последние c_rptStatViractPeriodInSec секунд

	const wchar_t c_rptVirWKSFound[]=L"KLPPT_VirWKSFound"; // INT_T

	const wchar_t c_rptVirWKSCured[]=L"KLPPT_VirWKSCured"; // INT_T

	const wchar_t c_rptVirWKSDeleted[]=L"KLPPT_VirWKSDeleted"; // INT_T

	const wchar_t c_rptVirWKSNotCured[]=L"KLPPT_VirWKSNotCured"; // INT_T

	const wchar_t c_rptVirWKSNetAttacks[]=L"KLPPT_VirWKSNetAttacks"; // INT_T

	// Файловые сервера: // за последние c_rptStatViractPeriodInSec секунд

	const wchar_t c_rptVirFSFound[]=L"KLPPT_VirFSFound"; // INT_T

	const wchar_t c_rptVirFSCured[]=L"KLPPT_VirFSCured"; // INT_T

	const wchar_t c_rptVirFSDeleted[]=L"KLPPT_VirFSDeleted"; // INT_T

	const wchar_t c_rptVirFSNotCured[]=L"KLPPT_VirFSNotCured"; // INT_T

	const wchar_t c_rptVirFSNetAttacks[]=L"KLPPT_VirFSNetAttacks"; // INT_T

	// Почтовые сервера: // за последние c_rptStatViractPeriodInSec секунд

	const wchar_t c_rptVirEMLFound[]=L"KLPPT_VirEMLFound"; // INT_T

	const wchar_t c_rptVirEMLCured[]=L"KLPPT_VirEMLCured"; // INT_T

	const wchar_t c_rptVirEMLDeleted[]=L"KLPPT_VirEMLDeleted"; // INT_T

	const wchar_t c_rptVirEMLNotCured[]=L"KLPPT_VirEMLNotCured"; // INT_T

	const wchar_t c_rptVirEMLNetAttacks[]=L"KLPPT_VirEMLNetAttacks"; // INT_T

	// Защита периметра: // за последние c_rptStatViractPeriodInSec секунд

	const wchar_t c_rptVirPHFound[]=L"KLPPT_VirPHFound"; // INT_T

	const wchar_t c_rptVirPHCured[]=L"KLPPT_VirPHCured"; // INT_T

	const wchar_t c_rptVirPHDeleted[]=L"KLPPT_VirPHDeleted"; // INT_T

	const wchar_t c_rptVirPHNotCured[]=L"KLPPT_VirPHNotCured"; // INT_T

	const wchar_t c_rptVirPHNetAttacks[]=L"KLPPT_VirPHNetAttacks"; // INT_T

	// Число компьютеров с истекшим сроком лицензионного ключа
	const wchar_t c_rptLicExpiredCnt[]=L"KLPPT_LicExpiredCnt"; // INT_T

	// Число компьютеров без антивирусной защиты
	const wchar_t c_rptNoAvpCnt[]=L"KLPPT_NoAvpCnt"; // INT_T

	// Число компьютеров, управление которыми потеряно
	const wchar_t c_rptControlLostCnt[]=L"KLPPT_ControlLostCnt"; // INT_T
};

namespace KLSTS
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Статусы Системы администрирования /////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Признак того, что требуются все каунтеры, а не только определяющие текущий статус:
	const wchar_t c_sts_NeedAllCounters[]=L"KLSTS_NeedAllCounters"; // BOOL_T
	const bool c_sts_bNeedAllCountersDflt = false;

	// Признак того, что требуются все причины, определяющие текущий статус:
	const wchar_t c_sts_NeedAllReasons[]=L"KLSTS_NeedAllReasons"; // BOOL_T
	const bool c_sts_bNeedAllReasonsDflt = true;
	
	enum EStatus{
		STATUS_OK  = 0,
		STATUS_INFO = 1,
		STATUS_WARNING = 2,
		STATUS_CRITICAL = 3
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Общие параметры статусов:

		// Статус (см. enum EStatus):
		const wchar_t c_sts_Par_Status[]=L"KLSTS_Par_Status"; // INT_T

		// Причина статуса (битовая маска):
		const wchar_t c_sts_Par_StatusReasonMask[]=L"KLSTS_Par_StatusReasonMask"; // INT_T

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cтатусы:
	// (у всех статусов как минимум 2 обязательных параметра - c_sts_Par_Status и c_sts_Par_StatusReasonMask)

	// Раздел "Установка приложений":
	const wchar_t c_sts_Deployment[]=L"KLSTS_Deployment"; // PARAMS_T
		// Параметры:
		const wchar_t c_sts_Par_Dpl_HostsInGroups[]=L"KLSTS_Par_Dpl_HostsInGroups"; // INT_T
		const wchar_t c_sts_Par_Dpl_HostsWithAVP[]=L"KLSTS_Par_Dpl_HostsWithAVP"; // INT_T
		const wchar_t c_sts_Par_Dpl_RITaskStrId[]=L"KLSTS_Par_Dpl_RITaskStrId"; // STRING_T
		const wchar_t c_sts_Par_Dpl_RITaskName[]=L"KLSTS_Par_Dpl_RITaskName"; // STRING_T
		const wchar_t c_sts_Par_Dpl_RITaskPercent[]=L"KLSTS_Par_Dpl_RITaskPercent"; // INT_T
		const wchar_t c_sts_Par_Dpl_RITaskOkCnt[]=L"KLSTS_Par_Dpl_RITaskOkCnt"; // INT_T
		const wchar_t c_sts_Par_Dpl_RITaskFailedCnt[]=L"KLSTS_Par_Dpl_RITaskFailedCnt"; // INT_T
		const wchar_t c_sts_Par_Dpl_RITaskNeedRbtCnt[]=L"KLSTS_Par_Dpl_RITaskNeedRbtCnt"; // INT_T
		const wchar_t c_sts_Par_Dpl_LicExparingSerial[]=L"KLSTS_Par_Dpl_LicExparingSerial"; // STRING_T
		const wchar_t c_sts_Par_Dpl_LicExparingDays[]=L"KLSTS_Par_Dpl_LicExparingDays"; // INT_T
		const wchar_t c_sts_Par_Dpl_LicExparingCnt[]=L"KLSTS_Par_Dpl_LicExparingCnt"; // INT_T
		const wchar_t c_sts_Par_Dpl_LicExparedCnt[]=L"KLSTS_Par_Dpl_LicExparedCnt"; // INT_T
		
		// Пространство значений c_sts_Par_StatusReasonMask:
		enum{
			// На все компьютеры в группах администрирования установлена антивирусная защита
			STAT_DPL__OK = 0x0000,

			// Компьютеров в группах администрирования: N, из них с установленным антивирусом: M (N > M)
			// N - c_sts_Par_Dpl_HostsInGroups
			// M - c_sts_Par_Dpl_HostsWithAVP
			STAT_DPL__NO_AV = 0x0001,

			// Работает задача установки приложений NNN, процент выполнения: N
			// NNN - c_sts_Par_Dpl_RITaskName
			// N - c_sts_Par_Dpl_RITaskPercent
			// {Id задачи} - c_sts_Par_Dpl_RITaskStrId
			STAT_DPL__RI_RUNNING = 0x0002,

			// Задача установки приложений NNN завершилась успешно на N компьютерах, неуспешно на M компьютерах
			// NNN - c_sts_Par_Dpl_RITaskName
			// N - c_sts_Par_Dpl_RITaskOkCnt
			// M - c_sts_Par_Dpl_RITaskFailedCnt
			// {Id задачи} - c_sts_Par_Dpl_RITaskStrId
			STAT_DPL__RI_FAILED = 0x0004,

			// Задача установки приложений NNN завершилась успешно на N компьютерах, требуется перезагрузка на M компьютерах
			// NNN - c_sts_Par_Dpl_RITaskName
			// N - c_sts_Par_Dpl_RITaskOkCnt
			// M - c_sts_Par_Dpl_RITaskNeedRbtCnt
			// {Id задачи} - c_sts_Par_Dpl_RITaskStrId
			STAT_DPL__RI_NEED_REBOOT = 0x0008,

			// До окончания срока действия лицензии NNN на N компьютерах осталось M дней. 
			// NNN - c_sts_Par_Dpl_LicExparingSerial
			// N - c_sts_Par_Dpl_LicExparingCnt
			// M - c_sts_Par_Dpl_LicExparingDays
			STAT_DPL__LIC_EXPARING = 0x0010,

			// Лицензия окончила действие на N компьютерах
			// N - c_sts_Par_Dpl_LicExparedCnt
			STAT_DPL__LIC_EXPARED = 0x0020
		};

	// Раздел "Обновления":
	const wchar_t c_sts_Updates[]=L"KLSTS_Updates"; // PARAMS_T
		// Параметры:
		const wchar_t c_sts_Par_Upd_LastSrvUpdateTime[]=L"KLSTS_Par_Upd_LastSrvUpdateTime"; // DATETIME_T
		const wchar_t c_sts_Par_Upd_NotUpdatesCnt[]=L"KLSTS_Par_Upd_NotUpdatesCnt"; // INT_T
		const wchar_t c_sts_Par_Upd_SrvCompletedPercent[]=L"KLSTS_Par_Upd_SrvCompletedPercent"; // INT_T
		// Пространство значений c_sts_Par_StatusReasonMask:
		enum{
			// Последнее обновление сервера администрирования: N минут назад
			// N - c_sts_Par_Upd_LastSrvUpdateTime
			STAT_UPD__OK = 0x0000,

			// Сервер администрирования давно не обновлялся
			STAT_UPD__SRV_NOT_UPDATED = 0x0001,

			// N компьютеров в группах администрирования давно не обновлялись
			// N - c_sts_Par_Upd_NotUpdatesCnt
			STAT_UPD__HOSTS_NOT_UPDATED = 0x0002,

			// Работает задача обновления Сервера администрирования, процент выполнения: N
			// N - c_sts_Par_Upd_SrvCompletedPercent
			STAT_UPD__SRV_UPDATE_IN_PROGRESS = 0x0004
		};

	// Раздел "Защита":
	const wchar_t c_sts_Protection[]=L"KLSTS_Protection"; // PARAMS_T
		// Параметры:
		const wchar_t c_sts_Par_Prt_AvpNotRunningCnt[]=L"KLSTS_Par_Prt_AvpNotRunningCnt"; // INT_T
		const wchar_t c_sts_Par_Prt_RtpNotRunningCnt[]=L"KLSTS_Par_Prt_RtpNotRunningCnt"; // INT_T
		const wchar_t c_sts_Par_Prt_RtpLevelChangedCnt[]=L"KLSTS_Par_Prt_RtpLevelChangedCnt"; // INT_T
		const wchar_t c_sts_Par_Prt_NoncuresHstCnt[]=L"KLSTS_Par_Prt_NoncuresHstCnt"; // INT_T
		const wchar_t c_sts_Par_Prt_TooManyThreadsCnt[]=L"KLSTS_Par_Prt_TooManyThreadsCnt"; // INT_T
		// Пространство значений c_sts_Par_StatusReasonMask:
		enum{
			// Антивирусная защита работает на всех управляемых компьютерах
			STAT_PRT__OK = 0x0000,
				
			// Не запущено антивирусное приложение на N компьютерах
			// N - c_sts_Par_Prt_AvpNotRunningCnt
			STAT_PRT__AV_NOT_RUNNING = 0x0001,
			
			// Не работает постоянная защита на N компьютерах
			// N - c_sts_Par_Prt_RtpNotRunningCnt
			STAT_PRT__RTP_NOT_RUNNING = 0x0002,
			
			// Уровень постоянной защиты отличается от установленного администратором на N компьютерах
			// N - c_sts_Par_Prt_RtpLevelChangedCnt
			STAT_PRT__RPT_LEVEL_CHANGED = 0x0004,
			
			// На N компьютерах имеются невылеченные объекты
			// N - c_sts_Par_Prt_NoncuresHstCnt
			STAT_PRT__NONCURED_FOUND = 0x0008,

			// На N компьютерах найдено слишком много вирусов
			// N - c_sts_Par_Prt_TooManyThreadsCnt
			STAT_PRT__TOO_MANY_THREATS = 0x0010,
			
			// Вирусная атака
			STAT_PRT__VIRUS_OUTBREAK = 0x0020
		};

	// Раздел "Проверка":
	const wchar_t c_sts_FullScan[]=L"KLSTS_FullScan"; // PARAMS_T
		// Параметры:
		const wchar_t c_sts_Par_Scn_NotScannedLatelyCnt[]=L"KLSTS_Par_Scn_NotScannedLatelyCnt"; // INT_T
		const wchar_t c_sts_Par_Scn_NeverScannedCnt[]=L"KLSTS_Par_Scn_NeverScannedCnt"; // INT_T
		// Пространство значений c_sts_Par_StatusReasonMask:
		enum{
			// Полная антивирусная проверка проводилась на всех компьютерах 
			STAT_SCN__OK = 0x0000,

			// Полная антивирусная проверка давно не проводилась на N компьютерах
			// N - c_sts_Par_Scn_NotScannedLatelyCnt
			STAT_SCN__NOT_SCANNED_LATELY = 0x0001,
				
			// Полная антивирусная проверка ни разу не проводилась на N компьютерах
			// N - c_sts_Par_Scn_NeverScannedCnt
			STAT_SCN__NEVER_SCANNED = 0x0002
		};

	// Раздел "Организация логической сети":
	const wchar_t c_sts_Administration[]=L"KLSTS_Administration"; // PARAMS_T
		// Параметры:
		const wchar_t c_sts_Par_Adm_FoundHstCnt[]=L"KLSTS_Par_Adm_FoundHstCnt"; // INT_T
		const wchar_t c_sts_Par_Adm_GrpCnt[]=L"KLSTS_Par_Adm_GrpCnt"; // INT_T
		const wchar_t c_sts_Par_Adm_GrpHstCnt[]=L"KLSTS_Par_Adm_GrpHstCnt"; // INT_T
		const wchar_t c_sts_Par_Adm_NotConnectedLongTimeCnt[]=L"KLSTS_Par_Adm_NotConnectedLongTimeCnt"; // INT_T
		const wchar_t c_sts_Par_Adm_ControlLostCnt[]=L"KLSTS_Par_Adm_ControlLostCnt"; // INT_T
		// Пространство значений c_sts_Par_StatusReasonMask:
		enum{
			// Обнаружено N компьютеров, в M группах администрирования размещено K компьютеров
			// N - c_sts_Par_Adm_FoundHstCnt
			// M - c_sts_Par_Adm_GrpCnt
			// K - c_sts_Par_Adm_GrpHstCnt
			STAT_ADM__OK = 0x0000,

			// N компьютеров давно не соединялись с сервером администрирования
			// N - c_sts_Par_Adm_NotConnectedLongTimeCnt
			STAT_ADM__NOT_CONNECTED_LONG_TIME = 0x0001,

			// Потеряно управление компьютерами
			// N - c_sts_Par_Adm_ControlLostCnt
			STAT_ADM__CONTROL_LOST = 0x0002,
		
			// Конфликт NETBIOS-имен клиентских компьютеров
			STAT_ADM__NAME_CONFLICT = 0x0004
		};

	// Раздел "Отчеты и уведомления":
	const wchar_t c_sts_Events[]=L"KLSTS_Events"; // PARAMS_T
		// Параметры:
		const wchar_t c_sts_Par_Ev_CriticalSrvEventsCnt[]=L"KLSTS_Par_Ev_CriticalSrvEventsCnt"; // INT_T
		const wchar_t c_sts_Par_Ev_FailedSrvTskName[]=L"KLSTS_Par_Ev_FailedSrvTskName"; // STRING_T
		const wchar_t c_sts_Par_Ev_FailedSrvTskStrId[]=L"KLSTS_Par_Ev_FailedSrvTskStrId"; // STRING_T
		// Пространство значений c_sts_Par_StatusReasonMask:
		enum{
			// Новые критические события не происходили
			STAT_EV__OK = 0,

			// На сервере администрирования зарегистрировано N критических событий
			// N - c_sts_Par_Ev_CriticalSrvEventsCnt
			STAT_EV__EVENTS_OCCURED = 0x0001,

			// Задача NNN Сервера администрирования завершились с ошибкой
			// NNN - c_sts_Par_Ev_FailedSrvTskName
			// {ID задачи} - c_sts_Par_Ev_FailedSrvTskStrId
			STAT_EV__TASK_FAILED = 0x0002
		};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

namespace KLDSH
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Dashboard data
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const wchar_t c_dshDashboard[]=L"KLPPT_DASHBOARD"; // ARRAY_T // of PARAMS_T
	// |- KLRPT::c_dshEntryId; // STRING_T; Идентификатор показателя.
	//		|- KLDSH::c_dshType; // INT_T; EDashboardType
	//		|- KLRPT::c_rptStatFinishTime; // DATETIME_T
	//		|- KLRPT::c_rptStatPeriodInSec; // INT_T
	//		|- Доп. параметры, специфичные для данного показателя.

	enum EDashboardType{
		DSHT_UNKNOWN = 0,
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Гистограммы:
		DSHT_HIST_EV_BY_COND = 1, // Распределение событий, удовлетворяющих заданным условиям
		// In - Параметры (доп.):
			// - KLEVP::c_er_product_name // Имя продукта (может отсутствовать или м.б. пустая строка);
			// - KLEVP::c_er_product_version // Внутр. версия продукта (может отсутствовать или м.б. пустая строка);
			// - KLEVP::c_er_event_type // Тип события (может отсутствовать или м.б. пустая строка);
			// - KLEVP::c_er_severity // Приоритет события (может отсутствовать или м.б. < 0 - для всех);
		// Out - Параметры: 
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Finish; // Конец временного интервала;
			//		|- c_dshPar_Start; // Начало временного интервала;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_HIST_VIRACT = 2,  // Распределение вирусной активности
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Finish; // Конец временного интервала;
			//		|- c_dshPar_Start; // Начало временного интервала;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_HIST_VIRACT_PRD = 3, // Распределение вирусной активности по типам продуктов
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Finish; // Конец временного интервала;
			//		|- c_dshPar_Start; // Начало временного интервала;
			//		|- c_dshPar_Count; // Каунтер;
			//		|- c_dshPar_AvpPrdType; // Тип продукта (KLRPT::EAVProdType);
		DSHT_HIST_NET_ATTACK = 4, // Распределение сетевых атак
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Finish; // Конец временного интервала;
			//		|- c_dshPar_Start; // Начало временного интервала;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_HIST_NEW_HST_FND = 5, // Распределение обнаружения новых хостов
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Finish; // Конец временного интервала;
			//		|- c_dshPar_Start; // Начало временного интервала;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_HIST_GRP_TSK = 6, // Распределение состояния групповой задачи
		// In - Параметры (доп.):
			// - KLTSK::c_evpGrpTaskId;
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Finish; // Конец временного интервала;
			//		|- c_dshPar_Start; // Начало временного интервала;
			//		|- KLEVP::c_er_task_new_state; // Состояние задачи на хосте;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_HIST_AV_REC = 7, // Распределение числа записей в антивирусных базах Сервера
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Finish; // Конец временного интервала;
			//		|- c_dshPar_Start; // Начало временного интервала;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_HIST_AVP = 8, // Распределение состояния антивирусной защиты.
		// Out - Параметры:
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Finish; // Конец временного интервала;
			//		|- c_dshPar_Start; // Начало временного интервала;
			//		|- c_dshPar_CountCrt; // Число хостов со статусом CRT;
			//		|- c_dshPar_CountWrn; // Число хостов со статусом WRN;
			//		|- c_dshPar_CountOk; // Число хостов со статусом OK;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Диаграммы:
		DSHT_VIRACT_ACT_BY_VIR = 9, // Диаграмма по типам вирусов и результатам лечения.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_VirType; // INT_T - EDetectionType;
			//		|- c_dshPar_Result; // INT_T - EViractResult;
			//		|- c_dshPar_Count; // INT_T;
		DSHT_VIRACT_VIR_BY_AVP = 10,// Диаграмма по типам вирусов, отловленных отдельными типами продуктов.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_AvpPrdType; // INT_T - Тип продукта (KLRPT::EAVProdType);
			//		|- c_dshPar_VirType; // INT_T - EDetectionType;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_VIRACT_ACT_BY_AVP = 11, // Диаграмма результатов лечения вирусов, отловленных отдельными типами продуктов.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_AvpPrdType; // INT_T - Тип продукта (KLRPT::EAVProdType);
			//		|- c_dshPar_Result; // INT_T - EViractResult;
			//		|- c_dshPar_Count; // INT_T;
		DSHT_VIRACT_MI_HOSTS = 12, // Диаграмма наиболее заражаемых компьютеров.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Name; // Имя компьютера;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_VIRACT_MI_USERS = 13, // Диаграмма наиболее заражающих пользователей.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Name; // Аккаунт пользователя;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_AVP_STATE = 14, // Текущее состояние антивирусной защиты.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshPar_CountCrt; // Число хостов со статусом CRT;
			// - c_dshPar_CountWrn; // Число хостов со статусом WRN;
			// - c_dshPar_CountOk; // Число хостов со статусом OK;
		DSHT_PROD_VER = 15, // Диаграмма, показывающая распределение продуктов по версиям.
		// In - Параметры (доп.):
			// - KLEVP::c_er_product_name // Имя продукта (может отсутствовать или м.б. пустая строка);
			// - KLEVP::c_er_product_version // Внутр. версия продукта (может отсутствовать или м.б. пустая строка);
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Name; // Версия;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_AVDB_VER = 16, // Диаграмма, показывающая распределение по версиям антивирусных баз.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshPar_CountActual; // Каунтер;
			// - c_dshPar_CountDay; // Каунтер;
			// - c_dshPar_Count3Days; // Каунтер;
			// - c_dshPar_Count7Days; // Каунтер;
			// - c_dshPar_CountOld; // Каунтер;
		DSHT_ERR = 17, // Диаграмма по типам ошибок.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Name; // Ошибка;
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_RTP_STATE = 18, // Диаграмма по текущему состоянию RTP защиты.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Type; // Состояние; KLCONN::AppRtpState
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_NET_ATTACKS = 19, // Диаграмма по типам сетевых атак.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Name; // Имя атаки
			//		|- c_dshPar_Count; // Каунтер;
		DSHT_LIC = 20, // Диаграмма использования лицензий.
		// Out - Параметры:
			// - все In - Параметры;
			// - c_dshData; // ARRAY_T of PARAMS_T
			//		|- c_dshPar_Name; // Лицензия
			//		|- c_dshPar_Count; // Использовано;
			//		|- c_dshPar_Limit; // Лимит;
		DSHT_SRV_UPD = 21, // Текущий статус задачи получения обновлений сервером администрирования.
		// Out - Параметры:
			// - все In - Параметры;
			// - KLEVP::c_er_task_new_state; // Состояние задачи на хосте;
			// - KLEVP::c_er_rise_time; // Время;
			// - KLEVP::c_er_descr; // Описание;
			// - KLEVP::c_er_completedPercent; // Процент выполнения;
		DSHT_SRV_BKP = 22, // Текущий статус задачи резервного копирования сервера администрирования.
		// Out - Параметры:
			// - все In - Параметры;
			// - KLEVP::c_er_task_new_state; // Последнее состояние задачи;
			// - KLEVP::c_er_rise_time; // Время;
			// - KLEVP::c_er_descr; // Описание;
		DSHT_TYPE_END
	};

	enum EViractResult{
		VR_NA  = 0,
		VR_CURED = 1,
		VR_DELETED = 2,
		VR_NOTCURED = 3
	};

	const wchar_t c_dshType[] = L"KLRPT_DSH_TYPE"; // INT_T; Тип показателя (EDashboardType).
	const wchar_t c_dshEntryId[] = L"KLRPT_DSH_ENTRY_ID"; // STRING_T; Идентификатор показателя.
	const wchar_t c_dshData[]=L"DSHT_DATA"; // ARRAY_T
	const wchar_t c_dshPar_Finish[] = L"tmFinish"; // DATETIME_T - Конец временного интервала;
	const wchar_t c_dshPar_Start[] = L"tmStart"; // DATETIME_T - Начало временного интервала;
	const wchar_t c_dshPar_Count[] = L"nCount"; // INT_T - Каунтер;
	const wchar_t c_dshPar_Count64[] = L"nCount64"; // LONG_T - 64-битный Каунтер;
	const wchar_t c_dshPar_CountCrt[] = L"nCrtCount"; // INT_T - Каунтер объектов со статусом CRT;
	const wchar_t c_dshPar_CountWrn[] = L"nWrnCount"; // INT_T - Каунтер объектов со статусом WRN;
	const wchar_t c_dshPar_CountOk[] = L"nOkCount"; // INT_T - Каунтер объектов со статусом OK;
	const wchar_t c_dshPar_AvpPrdType[] = L"nAvpPrdType"; // INT_T - Тип продукта (KLRPT::EAVProdType);
	const wchar_t c_dshPar_VirType[] = L"nVirType"; // INT_T - EDetectionType;
	const wchar_t c_dshPar_Result[] = L"nVirRslt"; // INT_T - EViractResult;
	const wchar_t c_dshPar_Name[] = L"wstrName"; // STRING_T - имя (компьютера, аккаунта, etc.);
	const wchar_t c_dshPar_Type[] = L"nType"; // INT_T - состояние, тип (RTP, тип сетевой атаки, etc.);
	const wchar_t c_dshPar_Limit[] = L"nLimit"; // INT_T - лимит;
	const wchar_t c_dshPar_CountActual[] = L"nCountActual"; // Каунтер;
	const wchar_t c_dshPar_CountDay[] = L"nCountDay"; // Каунтер;
	const wchar_t c_dshPar_Count3Days[] = L"nCount3Days"; // Каунтер;
	const wchar_t c_dshPar_Count7Days[] = L"nCount7Days"; // Каунтер;
	const wchar_t c_dshPar_CountOld[] = L"nCountOld"; // Каунтер;
}

#endif //__KLPRT_ADMSRVSTAT_H__
