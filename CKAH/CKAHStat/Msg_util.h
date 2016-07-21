#ifndef _MSG_UTIL_H__
#define _MSG_UTIL_H__

#define USER_MODE

#include <CKAH/ckahdefs.h>
#include <CKAH/CKAHStat.h>
#include "../../windows/Hook/klsdk/klstatus.h"
#include "../../windows/Hook/klsdk/PlugAPI.h"
#include "../../windows/Hook/klsdk/BaseAPI.h"

CKAHUM::OpResult CKAHSTAT_Pause ();
CKAHUM::OpResult CKAHSTAT_Resume ();

CKAHUM::OpResult CKAHSTAT_SendPluginMessage (
                                      DWORD dwIoControlCode, 
                                      CKAHSTAT::BaseDrvType BaseDrv,
                                      CHAR *szPluginName, 
                                      ULONG nMessageID,
                                      PVOID pInBuffer, 
                                      ULONG nInBufferSize,
                                      PVOID pOutBuffer, 
                                      ULONG nOutBufferSize, 
                                      ULONG *nRetBufferSize);


#endif