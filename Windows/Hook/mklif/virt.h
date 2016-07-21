#ifndef __VIRT_H
#define __VIRT_H

/*!
*		
*		
*		(C) 2007 Kaspersky Lab 
*		
*		\file	virt.h
*		\brief	виртуализация
*		
*		\author 
*		\date	25.10.2007 00:00:00
*		
*		
*		
*		
*/		

#include "pch.h"
#include "../kldefs.h"
#include "../hook/avpgcom.h"

//список песочниц
//typedef struct _VIRT_SANDBOX 
//{
//	ULONG			Id;
//	UNICODE_STRING	usPath;
//}VIRT_SANDBOX, *PVIRT_SANDBOX;
//
//typedef struct _VIRT_SANDBOX_LIST_ENTRY 
//{
//	LIST_ENTRY		ListEntry;
//	VIRT_SANDBOX	SandBox;	
//}VIRT_SANDBOX_LIST_ENTRY, *PVIRT_SANDBOX_LIST_ENTRY;

//список приложений
typedef struct _VIRT_APPL
{
	UNICODE_STRING	usAppPath;			// Путь к приложению \\Device\\HarddiskVolume2\\far\\far.exe	
	UNICODE_STRING	usSBPath;			// Путь к песочнице \\Device\\HarddiskVolume1\\Sandbox1	
	UNICODE_STRING	usSBVolumeName;		// Название раздела где находится песочница \\Device\\HarddiskVolume1
	UNICODE_STRING	usSBRegistry;		// Путь к разделу реестра где находится песочница для реестровых операций \\REGISTRY\MACHINE\SYSTEM\ControlSet001\Services\klfltdev\SB1
//	ULONG			SandBoxId;
	ULONG			logMask;			
}VIRT_APPL, *PVIRT_APPL;

typedef struct _VIRT_APPL_LIST_ENTRY
{
	LIST_ENTRY		ListEntry;
	VIRT_APPL		appl;
	
}VIRT_APPL_LIST_ENTRY, *PVIRT_APPL_LIST_ENTRY;

//дерево виртуализируемых приложенрий
typedef struct _VIRT_PID_TREE_ENTRY
{
	HANDLE			pid;
	VIRT_APPL		appl;

}VIRT_PID_TREE_ENTRY, *PVIRT_PID_TREE_ENTRY;

typedef struct 
{
	RTL_GENERIC_TABLE  PidTree;
	LONG				readers_cnt;
} VIRT_PID_TREE, *PVIRT_PID_TREE;


NTSTATUS
Virt_InsertApplToList( 
	__in UNICODE_STRING usNativeAppPath,
	__in UNICODE_STRING usNativeSBPath,
	__in UNICODE_STRING usNativeSBVolumeName,
	__in ULONG logMask
);

tefVerdict
GetVirtualPathFile(
	__in PFLT_CALLBACK_DATA Data,
	__in HANDLE pid,
	__in PFLT_INSTANCE Instance,
	__in PUNICODE_STRING pusFilePath,
	__in PUNICODE_STRING pusVolumeName,
	__in ACCESS_MASK DesiredAccess,
	__in ULONG  CreateOptions,
	__in ULONG  CreateDisposition,
	__out PUNICODE_STRING pusNewFilePath
);

BOOLEAN 
Virt_IsProcessInSB(
	__in HANDLE pid
);

NTSTATUS
Virt_RemoveAllApp();

NTSTATUS
Virt_RebuildPidTree();


//должна запускаться при инициализации драйвера
void VirtInit();

// запуск при выгрузке драйвера
void VirtDone();

PFILTER_EVENT_PARAM
Virt_ProcEvContext (
	__in ULONG isCreate,
	__in HANDLE pid
);
  
//виртуализация файлов:
//	реализрван первый этап ( не поддерживается просмотр каталогов с учетом содержимого песочницы )
//виртуализация реестра:
//  только начата -  написаны функции GetVirtualPathRegistryEx и Virt_GetActionRegistry - не запускались ни разу.
// нужно проверить Virt_GetActionRegistry
#endif