#ifndef __DEVRUL_H
#define __DEVRUL_H

/*!
*		
*		
*		(C) 2007 Kaspersky Lab 
*		
*		\file	devrul.h
*		\brief	хранение правил для устройств
*		
*		\author Aleksey Ryaskin
*		\date	24.07.2007 00:00:00
*		
*		
*		
*		
*/		

#include "ntifs.h"
#include "const.h"
#include "ioctl.h"

#define DEV_RUL_VERSION	0x00000001

//часть элемента списка шаблона
typedef struct 
{
	UNICODE_STRING	usGuid;			// GUID устройства
	UNICODE_STRING	usDeviceType;	// тип устройства
	ULONG			mask;	

}DEV_RUL, *PDEV_RUL;

//элемент списка шаблона
typedef struct 
{
	LIST_ENTRY	ListEntry;
	ULONG		clientID;
	DEV_RUL		dev_rul;
} DEV_RUL_ENTRY, *PDEV_RUL_ENTRY;



NTSTATUS
ResetClientRulesFromList(
	__in ULONG clientID
);


BOOLEAN 
IsAllowAccess(
	__in UNICODE_STRING	usGuid,
	__in UNICODE_STRING	deviceType
);

/*
Записываем правило в реестр
*/
NTSTATUS
SaveDevRuleToReg(
	__in PUNICODE_STRING RegistryPath,
	__in UNICODE_STRING usGuid,
	__in UNICODE_STRING usDeviceType,
	__in ULONG mask
);

NTSTATUS
GetDevRulSize (
	__out PULONG psize
);

NTSTATUS
GetDevRules (
	__in ULONG max_size,
	__out PKLFLTDEV_RULES prules
);

/* 
Заносит правилА в список из реестра
RegistryPath - ПУТЬ к драйверу (получаем в DriverEntry) \REGISTRY\MACHINE\SYSTEM\ControlSet001\Services\klfltdev
*/
NTSTATUS
RebuildDevRulListFromReg( __in PUNICODE_STRING RegistryPath );

//должна запускаться при инициализации драйвера
void InitDevRul();

// запуск при выгрузке драйвера
void DoneDevRul();

#define RUL_KEY_NAME		L"Rules"
#define REGVAR_NAME_MAX_SIZE   260

#endif