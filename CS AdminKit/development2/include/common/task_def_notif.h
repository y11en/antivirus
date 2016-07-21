/*!
 * (C) 2003-2006 Kaspersky Lab 
 * 
 * \file	task_def_notif.h
 * \author	Mikhail Karmazine & Andrew Lashchenkov
 * \date	05.12.2003 15:05:50
 * \brief	
 * 
 */

#ifndef __TASK_DEF_NOTIF_H__
#define __TASK_DEF_NOTIF_H__

namespace KLTSK{

	enum EStoreType{
		DO_NOT_STORE = 0,
		STORE_RESULTS = 1,
		STORE_STATES = 2,
		STORE_ALL_STATES = STORE_STATES | STORE_RESULTS,
		STORE_SYNC_STATES = 4,
		STORE_ALL_EVENTS = STORE_STATES | STORE_RESULTS | STORE_SYNC_STATES
	};

	bool GetTaskHistorySettings(
		KLPAR::ParamsPtr parTaskInfo,
		EStoreType& eStoreType,
		long& nDaysToStoreInDb,
		bool& bStoreInPRES,
		bool& bStoreInClientEventLog,
		bool& bStoreInServerEventLog);

	void SetTaskHistorySettings(
		EStoreType eStoreType,
		long nDaysToStoreInDb,
		bool bStoreInPRES,
		bool bStoreInClientEventLog,
		bool bStoreInServerEventLog,
		KLPAR::ParamsPtr parTaskInfo,
		bool bDeliverToServerAlways = true);

	void SetDefaultTaskHistorySettings(
		KLPAR::ParamsPtr parTaskInfo);

	bool GetTaskNotificationSettings(
		KLPAR::ParamsPtr parTaskInfo,
		bool& bErrorsOnly,
		KLPAR::Params** pparNotifications);

	void SetTaskNotificationSettings(
		bool bErrorsOnly,
		const KLPAR::ParamsPtr parNotifications,
		KLPAR::ParamsPtr parTaskInfo);

	void SetDefaultTaskNotificationSettings(
		KLPAR::ParamsPtr parTaskInfo);
};

#endif //__TASK_DEF_NOTIF_H__