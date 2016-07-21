/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prci_actions.h
 * \author	Andrew Kazachkov
 * \date	18.03.2003 13:29:54
 * \brief	Права доступа модуля PRCI.
 * 
 */

#ifndef __KLPRCI_ACTIONS_H__
#define __KLPRCI_ACTIONS_H__

#define KLAUTH_COMPONENT_START				KLAVT_EXECUTE	//< Start component
#define KLAUTH_COMPONENT_CONTROL			KLAVT_CONTROL	//< Stop, Pause, Resume
#define KLAUTH_COMPONENT_QUERY_STATE		KLAVT_READ	//< State
#define KLAUTH_COMPONENT_QUERY_INFORMATION	KLAVT_READ	//< Statistics, list of running tasks

#define KLAUTH_TASK_START					KLAVT_EXECUTE	//< RunTask
#define KLAUTH_TASK_CONTROL					KLAVT_CONTROL	//< StopTask, SuspendTask, ResumeTask, ReloadTask
#define KLAUTH_TASK_QUERY_STATE				KLAVT_READ	//< GetTaskState, GetTaskCompletion
#define KLAUTH_TASK_QUERY_INFORMATION		KLAVT_READ	//< GetTaskParams, GetTaskName

#endif //__KLPRCI_ACTIONS_H__
