
/*++

Module Name:

    sfc_int.h

Abstract:

    Header file for private SFC interfaces.

Author:

Revision History:

--*/



#ifndef _SFC_INT_
#define _SFC_INT_


#ifdef __cplusplus
extern "C" {

#endif

#define FILE_ACTION_MASK_FROM_FLAG(flag) (1<<((flag)-1))

#define DISABLE_FILE_ACTION_MODIFIED             FILE_ACTION_MASK_FROM_FLAG(FILE_ACTION_MODIFIED) 
#define DISABLE_FILE_ACTION_ADDED                FILE_ACTION_MASK_FROM_FLAG(FILE_ACTION_ADDED)
#define DISABLE_FILE_ACTION_REMOVED              FILE_ACTION_MASK_FROM_FLAG(FILE_ACTION_REMOVED)
#define DISABLE_FILE_ACTION_RENAMED_OLD_NAME     FILE_ACTION_MASK_FROM_FLAG(FILE_ACTION_RENAMED_OLD_NAME)
#define DISABLE_FILE_ACTION_RENAMED_NEW_NAME     FILE_ACTION_MASK_FROM_FLAG(FILE_ACTION_RENAMED_NEW_NAME)


BOOL
WINAPI
SfcFileException(
    IN HANDLE RpcHandle, // must be NULL
    IN WCHAR* pwcszFileName,
	IN DWORD  dwDisableActionMask // combination of DISABLE_FILE_ACTION_xxx flags
    );

VOID
WINAPI
SfcTerminateWatcherThread(
	VOID
    );

#ifdef __cplusplus
}
#endif

#endif // _SFC_INT_
