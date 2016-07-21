#if !defined(AFX_PLUGINUTILS_H__2113593E_ADAA_4C40_9F9A_983BC8953F67__INCLUDED_)
#define AFX_PLUGINUTILS_H__2113593E_ADAA_4C40_9F9A_983BC8953F67__INCLUDED_

#include "CKAHManifest.h"
#include <CKAH/ckahum.h>

CKAHUM::OpResult CKAHUM_InternalStart (CCKAHPlugin::PluginType PluginType, LPCKAHSTORAGE lpStorage);
CKAHUM::OpResult CKAHUM_InternalStop (CCKAHPlugin::PluginType PluginType);

CKAHUM::OpResult CKAHUM_InternalPause (CCKAHPlugin::PluginType PluginType);
CKAHUM::OpResult CKAHUM_InternalResume (CCKAHPlugin::PluginType PluginType);

CKAHUM::OpResult ReloadFromManifest (CCKAHManifest &manifest, LPCKAHSTORAGE lpStorage);

CKAHUM::OpResult RemoveBaseDrivers (LPCKAHSTORAGE lpStorage);

CKAHUM::OpResult CKAHUM_AfterRebootCheck (LPCKAHSTORAGE lpStorage);

CKAHUM::OpResult SendPluginModuleMessage (CCKAHPlugin::PluginType PluginType,
											DWORD dwIoControlCode,
											PVOID pInBuffer, ULONG nInBufferSize,
											PVOID pOutBuffer, ULONG nOutBufferSize, ULONG *nRetBufferSize = NULL);

CKAHUM::OpResult SendPluginMessage (CCKAHPlugin::PluginType PluginType, 
                                      DWORD dwIoControlCode, CHAR *szPluginName, ULONG nMessageID,
                                      PVOID pInBuffer, ULONG nInBufferSize,
                                      PVOID pOutBuffer, ULONG nOutBufferSize, ULONG *nRetBufferSize = NULL);

bool IsPluginStarted (CCKAHPlugin::PluginType PluginType, DWORD *pdwPluginVersion = NULL);

CKAHUM::OpResult StopAllPlugins ();

void GetDriverInstallPath (TCHAR *szPath, UINT nPathSize);
bool IsBaseDriverRunning(const std::wstring &szwBaseDrvName);

#endif // !defined(AFX_PLUGINUTILS_H__2113593E_ADAA_4C40_9F9A_983BC8953F67__INCLUDED_)
