#include "useklin.h"
#include "../hook/HookSpec.h"

#include "../hook/klin_api.h"
PWCHAR pwcKLIN_NAME = L"\\Device\\"KLIN_NAME;

BOOLEAN GeneralStart_ExternalDrv(ULONG Ioctl_GetVersion, ULONG Ioctl_StartFiltering, PCWSTR pwcDrvNAME, ULONG ApiVersion, ULONG HookID, BOOLEAN *pbStarted);
BOOLEAN GeneralStop_ExternalDrv(ULONG Ioctl_StopFiltering, PCWSTR pwcDrvNAME, ULONG HookID, BOOLEAN *pbStarted);

//+ ------------------------------------------------------------------------------------------
BOOLEAN Klin = FALSE;

BOOLEAN KlinStart(VOID)
{
	return GeneralStart_ExternalDrv(KLIN_GET_VERSION, KLIN_START_FILTER, pwcKLIN_NAME, KLIN_API_VERSION, FLTTYPE_KLIN, &Klin);
}

BOOLEAN KlinStop(VOID)
{
	return GeneralStop_ExternalDrv(KLIN_STOP_FILTER, pwcKLIN_NAME, FLTTYPE_KLIN, &Klin);	
}
