/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	agent_info.h
 * \author	Andrew Kazachkov
 * \date	03.03.2003 12:29:51
 * \brief	
 * 
 */

#ifndef __KLAGENT_INFO_H__
#define __KLAGENT_INFO_H__

#define KLAG_SETTINGS	KLCS_COMPONENT_AGENT

#define KLAG_TASKSTORAGE_PATH	            L"TaskStoragePath"
#define KLAG_EVENTSTORAGE_PATH	            L"EventStoragePath"
#define KLAG_EVENTS_FOLDER		            L"EventFolder"
#define KLAG_WAIT_SCHED_FOR_START_EVENT		L"KLAG_WAIT_SCHED_FOR_START_EVENT"  //BOOL_T
#define KLAG_TSID_2_ACTION_ID_SECTION       L"KLAG_TSID_2_ACTION_ID"

#define KLTSK_GROUP_TASK_START_TIMEOUT 30000
#define KLAGINST_COMPONENT_START_TIMEOUT 180000
#define AGINST_CLOSE_WRITE_OPENED_JOURNAL_TIMEOUT 3000

#endif //__KLAGENT_INFO_H__

// Local Variables:
// mode: C++
// End:
