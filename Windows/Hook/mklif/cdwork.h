#ifndef __cdwork
#define __cdwork

NTSTATUS
QueryCdRomState (
	PDEVICE_OBJECT pDevice,
	PBOOLEAN bLockState
	);

#endif // __cdwork