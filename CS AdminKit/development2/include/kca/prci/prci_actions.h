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

#define KLAUTH_COMPONENT_START				0x0001	//< Start component
#define KLAUTH_COMPONENT_CONTROL			0x0002	//< Stop, Pause, Resume
#define KLAUTH_COMPONENT_QUERY_STATE		0x0004	//< State
#define KLAUTH_COMPONENT_QUERY_INFORMATION	0x0008	//< Statistics, list of running tasks

#define KLAUTH_TASK_START					0x0001	//< RunTask
#define KLAUTH_TASK_CONTROL					0x0002	//< StopTask, SuspendTask, ResumeTask, ReloadTask
#define KLAUTH_TASK_QUERY_STATE				0x0004	//< GetTaskState, GetTaskCompletion
#define KLAUTH_TASK_QUERY_INFORMATION		0x0008	//< GetTaskParams, GetTaskName

#endif //__KLPRCI_ACTIONS_H__
