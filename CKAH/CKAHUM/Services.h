#if !defined(AFX_SERVICES_H__B095A6A5_E9BA_404F_9C5B_87A3F402B4EE__INCLUDED_)
#define AFX_SERVICES_H__B095A6A5_E9BA_404F_9C5B_87A3F402B4EE__INCLUDED_

#include <CKAH/ckahumdefs.h>
#include "CKAHManifest.h"

bool CKAHUM_RegisterPlugin (LPCWSTR szPluginServiceName, LPCWSTR szPluginFileName, DWORD dwStartType = SERVICE_DEMAND_START);
bool CKAHUM_UnregisterPlugin (LPCWSTR szPluginServiceName, bool bStopBeforeUnregister = true);
bool CKAHUM_IsPluginRegistered (LPCWSTR szPluginServiceName, bool &bIsRegistered);
bool CKAHUM_IsPluginRegistered (LPCWSTR szPluginServiceName, LPCWSTR szPluginFileName, bool &bIsRegistered);

bool CKAHUM_StartPlugin (LPCWSTR szPluginServiceName, LPCWSTR szPluginFileName);
bool CKAHUM_StopPlugin (LPCWSTR szPluginServiceName);

bool CKAHUM_RegisterBaseDrv (LPCWSTR szBaseDrvServiceName, LPCWSTR szBaseDrvFileName);
bool CKAHUM_UnregisterBaseDrv (LPCWSTR szBaseDrvServiceName, bool bStopBeforeUnregister = true);
bool CKAHUM_IsBaseDrvRegistered (LPCWSTR szBaseDrvServiceName, LPCWSTR szBaseDrvFileName, bool &bIsRegistered);
bool CKAHUM_IsBaseDrvRegistered (LPCWSTR szBaseDrvServiceName, bool &bIsRegistered);
bool CKAHUM_SetBaseDrvStartType (LPCWSTR szBaseDrvServiceName, DWORD dwStartType);
bool CKAHUM_GetBaseDrvStartType (LPCWSTR szBaseDrvServiceName, DWORD *pdwStartType);
bool CKAHUM_EnableBaseDriver (LPCWSTR szBaseDrvServiceName, LPCWSTR szBaseDrvFileName, bool bEnable, bool *pbNeedReboot = NULL);

bool CKAHUM_PauseBaseDrv (LPCWSTR szBaseDrvServiceName);
bool CKAHUM_ResumeBaseDrv (LPCWSTR szBaseDrvServiceName);

#endif // !defined(AFX_SERVICES_H__B095A6A5_E9BA_404F_9C5B_87A3F402B4EE__INCLUDED_)
