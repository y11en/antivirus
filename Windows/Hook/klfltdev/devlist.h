#ifndef __DEVLIST_H
#define __DEVLIST_H

/*!
*		
*		
*		(C) 2007 Kaspersky Lab 
*		
*		\file	devlist.h
*		\brief	хранение списока устройств к которым в данный момент присоединен фильтр
*		
*		\author Aleksey Ryaskin
*		\date	24.07.2007 00:00:00
*		
*		
*		
*		
*/		

#include "ntifs.h"
#include "ioctl.h"

typedef struct _DEVICE_NAME
{
	UNICODE_STRING usGuid;
	UNICODE_STRING usDeviceType;

} DEVICE_NAME, *PDEVICE_NAME;

//элемент списка шаблона
typedef struct 
{
	LIST_ENTRY		ListEntry;
	PDEVICE_NAME	pDevName;
} DEVICE_LIST_ENTRY, *PDEVICE_LIST_ENTRY;


#endif

NTSTATUS
InsertDeviceList( 
	__in PDEVICE_NAME pDevName
);

BOOLEAN
IsAttachedDevice(
	__in PUNICODE_STRING pusGuid,
	__in PUNICODE_STRING pusDeviceType
);

NTSTATUS
RemItemFromDeviceList(
	__in PDEVICE_NAME pDevName
);

void GlobalInitDeviceList();

void GlobalDoneDeviceList();