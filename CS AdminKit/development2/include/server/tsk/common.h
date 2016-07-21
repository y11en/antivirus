/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	tsk/common.h
 * \author	Mikhail Karmazine
 * \date	24.06.2003 13:13:29
 * \brief	ќбщие объ€влени€ дл€ серверной и клиентской частей,
            в т.ч. дл€ частей, работающих на агенте
 * 
 */


#ifndef __TSK_COMMON_H__
#define __TSK_COMMON_H__

namespace KLTSK
{
    /* ¬ случае необходимости запустить, остановить, или приостановить удаленную
        групповую задачу в ее параметр TaskInfo записывютс€ следующие данные (с
        полседующей синхронизацией): */

    enum GroupTaskAction { gtaRun, gtaStop, gtaPause, gtaResume, gtaNone };

    // PARAMS_T, содержит в себе данные дл€ передачи (см.отсальные
    // константы c_szwGroupTaskAction*).
    // —одержитьс€ в TaskInfo:
    // +---<c_szwGroupTaskActionId>
    // |        c_szwGroupTaskActionType
    // |        c_szwGroupTaskActionDate
    const wchar_t c_szwGroupTaskActionData[] = L"KLTSK_GROUP_TASK_ACTION_DATA";

    // STRING_T, содержит в себе ID, уникальный дл€ каждой синхронизации - с тем,
    // чтобы отследить его в случае повторной синхронизации.
    const wchar_t c_szwGroupTaskActionId[] = L"KLTSK_GROUP_TASK_ACTION_ID";

    // INT_T, содержит в себе инф. о том, что нужно делать. ћожет быть одним из 
    // значений enum GroupTaskAction.
    const wchar_t c_szwGroupTaskActionType[] = L"KLTSK_GROUP_TASK_ACTION_TYPE";

    // DATE_TIME_T, содержит в себе инф. о дате ручного запуска
    const wchar_t c_szwGroupTaskActionDate[] = L"KLTSK_GROUP_TASK_ACTION_DATE";

    // BOOL_T, содержит в признак, нужно ли действие распростран€ть по иерархии.
    const wchar_t c_szwGroupTaskActionSkipOnSlaves[] = L"KLTSK_GROUP_TASK_ACTION_SKIP_SLV";
	const bool c_bGroupTaskActionSkipOnSlavesDflt = false;
}

#endif //__TSK_COMMON_H__

// Local Variables:
// mode: C++
// End:
