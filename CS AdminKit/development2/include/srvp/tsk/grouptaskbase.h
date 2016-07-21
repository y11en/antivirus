/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file srvp\tsk\GroupTaskBase.h
 * \author Andrew Lashchenkov
 * \date 2005
 * \brief Интерфейс для работы с групповыми задачами.
 *
 */

#ifndef __TSK_GROUPTASKBASE_H__
#define __TSK_GROUPTASKBASE_H__

// Tasks names

#define GTN_REMOTE_INSTALL					L"Remote Installation"
#define GTN_REMOTE_UNINSTALL				L"Remote Deinstallation"
#define GTN_SYNC_PACKAGE					L"SyncPackage"
#define GTN_SRV_UPGRADE						L"SrvUpgrade"

//! BEGIN of remote installation task settings

#define KLTSK_IS_APPLIED_TO_SUBGROUPS       L"IsAppliedToSubgroups"
#define KLTSK_IS_DESCENDANT_CAN_FORBID      L"IsDescendantCanForbid"

#define KLTSK_HOST_LIST                     L"HostList"
#define KLTSK_HOST_NAME                     L"HostName"
#define KLTSK_HOST_DISP_NAME                L"HostDispName"
#define KLTSK_DOMAIN_NAME                   L"DomainName"
//#define KLTSK_HOST_USER                     L"HostUser" // Optional
//#define KLTSK_HOST_PWD                      L"HostPwd"  // Optional
//#define KLTSK_HOST_SHARE                    L"HostShare"  // Optional
#define KLTSK_DOMAIN_NAME					L"DomainName"

#define KLTSK_HOST_USER_KEY                 L"HostUserKey" // Optional
#define KLTSK_HOST_PWD_KEY                  L"HostPwdKey"  // Optional

#define KLTSK_DISPL_NAME_FIELD_SIZE			100


//! END of remote installation task settings

#define GTS_ID_SERVER_PREFIX    L"server_side_"
#define GTS_ID_NOT_FOUND        -1

#include <transport/wat/common.h>

namespace KLTSK
{
	/*!
        Событие c_EventGroupTaskSyncState публикуется при изменении синхронизационного 
            состояния задачи
        Параметры:
            KLPRCI::c_evpTaskNewState,  INT_T - Новое состояние задачи, принимает значение KLTSK::hsScheduled или KLTSK::hsNone
            KLEVP::c_er_severity,  INT_T - severity события
    */
    const wchar_t c_EventGroupTaskSyncState[]=L"KLEVP_GroupTaskSyncState";

    /*!
    * \brief TaskState - тип состояния задачи на одной машине.
    */
    typedef long HostState;

    /*!
    * \brief константы hs* - состояние задачи на одном хосте. Это bitmask type, значения могут 
        комбинироватся битовыми операторами.
        Реальные значения храняться также в БД, так что менять их рекомендуется с острожностью.
    */
    const HostState hsNone            = 0x01;
    const HostState hsInProgress      = 0x02;
    const HostState hsFinishedOk      = 0x04;
    const HostState hsFinishedWarning = 0x08;
    const HostState hsFinishedFail    = 0x10;
    const HostState hsScheduled       = 0x20;
    const HostState hsPaused          = 0x40;

	/*!
        Событие c_EventGroupTaskStats публикуется при изменении статистики задачи
        Параметры:
            KLTSK::c_evpGrpTaskId,  STRING_T - идентификатор задачи;
			KLTSK::c_evpGrpTaskStatsParam, PARAMS_T - статистика задачи, аналогично GroupTaskControl::GetTaskStatistics();
    */
    const wchar_t c_EventGroupTaskStats[]	= L"KLEVP_EventGroupTaskStats";
	const wchar_t c_evpGrpTaskId[]			= L"KLTSK_GRP_TSK_ID";	// STRING_T
	const wchar_t c_evpGrpTaskStatsParam[]	= L"KLTSK_GRP_TSK_STATS_PARAMS"; // PARAMS_T

	/*!
        Событие c_EventTasksCompletionPercents публикуется кумулятивно с заданным периодом при изменении процента выполнения задач
        Параметры:
            KLTSK::c_evpTskPercents,  PARAMS_T - процент выполнения (имя параметра - ид-р задачи в TS, значение (INT_T) - процент);
            KLTSK::c_evpTskTimes,  PARAMS_T - время изменения состояния задачи (имя параметра - ид-р задачи в TS, значение (DATETIME_T) - время)
            KLTSK::c_evpTskTimesMS,  PARAMS_T - мс. составляющая времени изменения состояния задачи (имя параметра - ид-р задачи в TS, значение (DATETIME_T) - время)
    */
    const wchar_t c_EventTasksCompletionPercents[]	= L"KLEVP_EventTasksCompletionPercents";
	const wchar_t c_evpTskPercents[]					= L"KLEVP_TSK_PERCENTS";	// PARAMS_T
	const wchar_t c_evpTskTimes[]						= L"KLEVP_TSK_TIMES"; // PARAMS_T
	const wchar_t c_evpTskTimesMS[]						= L"KLEVP_TSK_TIMES_MS"; // PARAMS_T

    /*!
        \brief hsMaskAll - битовая комбинация всех констант hs*. Рекомендуеться использовать ее,
         когда нужно скомбинировать все флаги, а не делать это вручную.
    */
    const HostState hsMaskAll =
        hsNone | hsInProgress | hsFinishedOk | hsFinishedWarning | hsFinishedFail | hsScheduled;

    // Имя параметра 'массив состояний'
    const wchar_t c_host_state_array[] = L"statuses";

    //! Имена полей в контейнере Params с информацией о статусе хоста.
    const wchar_t c_hs_domainName[]        = L"domain_name";  // STRING_T, имя домена
    const wchar_t c_hs_hostName[]          = L"hostname";  // STRING_T, имя хоста
    const wchar_t c_hs_hostDisplayName[]   = L"hostdn";  // STRING_T, windows-имя хоста
    const wchar_t c_hs_hostState[]         = L"state_code";  // INT_T, статус (см.тип HostState и соотв. константы)
    const wchar_t c_hs_tryCount[]          = L"try_count";  // INT_T, количество сделанных попыток выполнить задачу
    const wchar_t c_hs_stateDescr[]        = L"state_descr";  // STRING_T, МОЖЕТ ОТСУТСТВОВАТЬ ЕСЛИ ЗНАЧЕНИЕ NULL, описание состояния в случае Fail или Warning
    const wchar_t c_hs_startTime[]         = L"start_time";  // TIME_T, МОЖЕТ ОТСУТСТВОВАТЬ ЕСЛИ ЗНАЧЕНИЕ NULL, время начала работы задачи
    const wchar_t c_hs_finishTime[]        = L"finish_time";  // TIME_T, МОЖЕТ ОТСУТСТВОВАТЬ ЕСЛИ ЗНАЧЕНИЕ NULL, время конца работы задачи
    const wchar_t c_hs_completedPercent[]  = L"completed_percent";  // INT_T, выполненный процент
	
	const wchar_t c_szwRemoteInstallSign[]			= L"KLEVP_REMOTE_INSTALL_SIGN"; // BOOL_T
	const wchar_t c_szwRebootState[]				= L"KLEVP_REMOTE_INSTALL_REBOOT_STATE"; // INT_T, см enum KLTSK::RebootState
	const wchar_t c_szwRemoteInstallCheckNextSubTask[]   = L"KLEVP_REMOTE_INSTALL_CHK_NXT_TSK"; // BOOL_T
}

#endif // __TSK_GROUPTASKBASE_H__
