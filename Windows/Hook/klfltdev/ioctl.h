#ifndef __IOCTL_H
#define __IOCTL_H

#include "const.h"

#define IOCTL_KLFLTDEV_ADDRULE		(ULONG) CTL_CODE( CTL_DEVICE_KLFLTDEV, 0x01, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define ACCESS_OFFSET	0x0
#define LOG_OFFSET		0x2
typedef struct _KLFLTDEV_RULE
{
	ULONG		m_mask;
	//дополнително выделяется 
	// ULONG	GuidLen;
	// WCHAR	Guid[GuidLen] //не содержит нулевой символ
	// ULONG	DeviceTypeLen
	// WCHAR	DeviceType[DeviceTypeLen] //не содержит нулевой символ
} KLFLTDEV_RULE, *PKLFLTDEV_RULE;

typedef enum _REPLUG_STATUS
{
	Need_Noting = 0,
	Need_Replug = 1,
	Need_Reboot = 2
}REPLUG_STATUS, *PREPLUG_STATUS;

#define IOCTL_KLFLTDEV_APPLYRULES   (ULONG) CTL_CODE( CTL_DEVICE_KLFLTDEV, 0x02, METHOD_BUFFERED, FILE_ANY_ACCESS )

// #define IOCTL_KLFLTDEV_RESETRULES   (ULONG) CTL_CODE( CTL_DEVICE_KLFLTDEV, 0x03, METHOD_BUFFERED, FILE_ANY_ACCESS )
// typedef struct _KLFLTDEV_RESET_RULES
// {
// 	ULONG		m_clientID;
// } KLFLTDEV_RESET_RULES, *PKLFLTDEV_RESET_RULES;

#define IOCTL_KLFLTDEV_GETRULES_SIZE   (ULONG) CTL_CODE( CTL_DEVICE_KLFLTDEV, 0x04, METHOD_BUFFERED, FILE_ANY_ACCESS )
typedef struct _KLFLTDEV_RULES_SIZE
{
	ULONG		m_size;
} KLFLTDEV_RULES_SIZE, *PKLFLTDEV_RULES_SIZE;

#define IOCTL_KLFLTDEV_GETRULES   (ULONG) CTL_CODE( CTL_DEVICE_KLFLTDEV, 0x05, METHOD_BUFFERED, FILE_ANY_ACCESS )
typedef struct _KLFLTDEV_RULES
{
	ULONG		m_ver;			//версия протокола
	ULONG		m_size;			//общий размер структуры
	ULONG		m_RulesCount;	// количество правил
// далее m_RulesCount правил (KLFLTDEV_RULE)
	// ULONG		m_ItemSize;		//размер KLFLTDEV_RULE
	// KLFLTDEV_RULE
} KLFLTDEV_RULES, *PKLFLTDEV_RULES;

#endif