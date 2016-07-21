#ifndef  __KLFLTDEV_H
#define	__KLFLTDEV_H

#include "pch.h"

NTSTATUS 
KLFltDev_SetRule(
	__in PVOID InputBuffer,
	__in ULONG InputBufferLength,
	__out PVOID OutputBuffer,
	__in ULONG OutputBufferLength, 
	__out ULONG* pRetSize
);

NTSTATUS 
KLFltDev_GetRulesSize(
	__out PVOID OutputBuffer,
	__in ULONG OutputBufferLength, 
	__out ULONG* pRetSize
);

NTSTATUS 
KLFltDev_GetRules(
	__out PVOID OutputBuffer,
	__in ULONG OutputBufferLength, 
	__out ULONG* pRetSize
);

NTSTATUS 
KLFltDev_ApplyRules();

#endif
