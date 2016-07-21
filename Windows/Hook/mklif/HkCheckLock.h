#ifndef __HKCHECKLOCK_H
#define	__HKCHECKLOCK_H

#include "pch.h"

#ifndef _WIN64
VOID
MakeCheckLockAccess();
VOID
MakeIoIsOperationSync();
#endif

NTSTATUS
DoDirectOpen (
	PHANDLE pHandle,
	PWCHAR pOrigName,
	USHORT OrigNameLenInTchar
	);

VOID
CheckLockLists_Init();

VOID
SwitchOnChLkOn();

VOID
SwitchOnChLkOff();

BOOLEAN
IsChLkOn();


#ifndef _WIN64
VOID
HookCheckLock();
#endif

VOID
AddSkipLockItem(PFILE_OBJECT pFileObject);

VOID
DelSkipLockItem(PFILE_OBJECT pFileObject);


#endif //__HKCHECKLOCK_H