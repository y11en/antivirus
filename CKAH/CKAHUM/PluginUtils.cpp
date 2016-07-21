#include "stdafx.h"
#include "PluginUtils.h"
#include "Unpacker.h"
#include "UnicodeConv.h"
#include "RebootCheckHlp.h"
#include "Utils.h"
#include "Services.h"
#include "StartedPlugins.h"

extern StartedPluginsList g_StartedPlugins;
extern int g_bUseKL1;

bool CheckAndUnpack(const std::wstring &sSourceFileName,
					const std::wstring &sSourceDirectory,
					const std::wstring &sDestDirectory,
                    DWORD dwSourceVersionMS,
                    DWORD dwSourceVersionLS,
					bool &bReplaced)
{
	log.WriteFormat (_T("[CKAHUM::Reload] CheckAndUnpack '%ls' from '%ls' (%d.%d.%d.%d) to '%ls'"), PEL_INFO,
		sSourceFileName.c_str(), sSourceDirectory.c_str(), 
		HIWORD(dwSourceVersionMS), LOWORD(dwSourceVersionMS),
		HIWORD(dwSourceVersionLS), LOWORD(dwSourceVersionLS),
		sDestDirectory.c_str());

	bool bRetValue = false;
	bReplaced = false;

    __int64 verSrc = MAKEVER64(dwSourceVersionMS, dwSourceVersionLS);

    const WCHAR *szwDestExtension = g_bIsNT ? (g_bUseKL1 ? L".dat" : L".sys") : L".vxd";

    std::wstring szwSrcPath = sSourceDirectory + sSourceFileName;
	std::wstring szwDstPath = sDestDirectory + RemoveExtension (sSourceFileName) + szwDestExtension;

	// check version
	__int64 verDst = GetFileVersion64 (szwDstPath.c_str ());

	if(verDst)
	{
		log.WriteFormat (_T("[CKAHUM::Reload] Version of '%ls' is %d.%d.%d.%d"), PEL_INFO,
			szwDstPath.c_str(),
			HIWORD(GETVER64HI(verDst)), LOWORD(GETVER64HI(verDst)),
			HIWORD(GETVER64LO(verDst)), LOWORD(GETVER64LO(verDst)));
	}
	else
	{
		log.WriteFormat (_T("[CKAHUM::Reload] Version of '%ls' is unknown"), PEL_INFO, szwDstPath.c_str());
	}

	if (verSrc && verDst && verSrc == verDst) // только равенство версий!
	{
		// no need to copy
		log.WriteFormat (_T("[CKAHUM::Reload] No need to unpack '%ls' (%d.%d.%d.%d) to '%ls' (%d.%d.%d.%d)"), PEL_INFO,
								szwSrcPath.c_str (),
									HIWORD (GETVER64HI (verSrc)), LOWORD (GETVER64HI (verSrc)),
									HIWORD (GETVER64LO (verSrc)), LOWORD (GETVER64LO (verSrc)),
								szwDstPath.c_str (),
									HIWORD (GETVER64HI (verDst)), LOWORD (GETVER64HI (verDst)),
									HIWORD (GETVER64LO (verDst)), LOWORD (GETVER64LO (verDst)));
		bRetValue = true;
	}
    else
    {
        if (UnpackOSFile (szwSrcPath.c_str (), sSourceFileName.c_str (), sDestDirectory.c_str ()))
        {
		    log.WriteFormat (_T("[CKAHUM::Reload] '%ls' successfully unpacked to '%ls'"), PEL_INFO,
								    sSourceFileName.c_str (), szwDstPath.c_str ());

			bRetValue = true;
			bReplaced = true;
		}
	    else
	    {
		    log.WriteFormat (_T("[CKAHUM::Reload] Failed to unpack '%ls' to '%ls'"), PEL_ERROR,
								    sSourceFileName.c_str (), szwDstPath.c_str ());
	    }
	}

	return bRetValue;
}

static bool StartPlugin (const std::wstring &szwPluginName,
									const std::wstring &szwDirectory,
									bool bUnregisterBeforeRegister)
{
	bool bRetResult = false;

	std::wstring sBaseName = RemoveExtension (szwPluginName);

    const WCHAR *szwPluginExtension = g_bIsNT ? (g_bUseKL1 ? L".dat" : L".sys") : L".vxd";
	std::wstring szwPluginPath = szwDirectory + sBaseName + szwPluginExtension;

	bool bIsPluginRegistered = false;
	CKAHUM_IsPluginRegistered (sBaseName.c_str (), szwPluginPath.c_str (), bIsPluginRegistered);

	if (bIsPluginRegistered && bUnregisterBeforeRegister)
	{
		CKAHUM_UnregisterPlugin (sBaseName.c_str ());
		CKAHUM_IsPluginRegistered (sBaseName.c_str (), szwPluginPath.c_str (), bIsPluginRegistered);
	}			

	if (bIsPluginRegistered || CKAHUM_RegisterPlugin (sBaseName.c_str(), szwPluginPath.c_str ()))
		bRetResult = CKAHUM_StartPlugin (sBaseName.c_str(), szwPluginPath.c_str ());

	return bRetResult;
}

CKAHUM::OpResult CKAHUM_InternalStart (CCKAHPlugin::PluginType PluginType, LPCKAHSTORAGE lpStorage)
{
    Guard::Lock lock(g_guardStorage);

	log.WriteDWORD (_T("[CKAHUM::Start] Starting type %d plugin..."), PluginType);

	if (CKAHSTP::IsPluginStarted (PluginType))
	{
		log.Write (_T("[CKAHUM::Start] Already started"), PEL_WARNING);
		return CKAHUM::srAlreadyStarted;
	}

	if (lpStorage == NULL)
	{
		log.Write (_T("[CKAHUM::Start] No persistent storage provided"), PEL_ERROR);
		return CKAHUM::srNoPersistentManifest;
	}

	CKAHUM_AfterRebootCheck (lpStorage);

	CCKAHManifest manifest;

	if (!manifest.ReadFromStorage (lpStorage, szwPersistentManifestRoot))
	{
		log.Write (_T("[CKAHUM::Start] Failed to read manifest from storage"), PEL_ERROR);
		return CKAHUM::srErrorReadingPersistentManifest;
	}

	if (manifest.m_Directory.empty ())
	{
		log.Write (_T("[CKAHUM::Start] No working directory in manifest, probably first start"), PEL_INFO);
		return CKAHUM::srNeedReload;
	}

	CKAHUM::OpResult RetResult = CKAHUM::srOpFailed;

	unsigned i;
	for (i = 0; i < manifest.m_Plugins.size (); ++i)
	{
		if (manifest.m_Plugins[i]->GetGroupType () == PluginType)
		{
			log.WriteFormat (_T("[CKAHUM::Start] Starting plugin '%ls'..."), PEL_INFO, manifest.m_Plugins[i]->m_sName.c_str ());

			if (StartPlugin (manifest.m_Plugins[i]->m_sName, manifest.m_Directory, false))
			{
				CKAHSTP::AddPluginToStarted (manifest.m_Plugins[i]);
				manifest.m_Plugins[i]->LoadData (manifest.m_Directory.c_str ());
				RetResult = CKAHUM::srOK;
			}
		}
	}

	log.WriteDWORD (_T("[CKAHUM::Start] Starting done (%d)"), RetResult);

	return RetResult;
}

static void StopPlugin (CCKAHPlugin &plugin)
{
	std::wstring szwPluginName = RemoveExtension (plugin.m_sName);
	
	log.WriteFormat (_T("[CKAHUM::Stop] Stopping plugin '%ls'"), PEL_INFO, szwPluginName.c_str ());

	CKAHUM_StopPlugin (szwPluginName.c_str ());
	CKAHUM_UnregisterPlugin (szwPluginName.c_str ());
}

static void StopPluginByType (CCKAHPlugin::PluginType PluginType)
{
	StartedPluginsList::CLock lock (&g_StartedPlugins);

	StartedPluginsList::iterator i;

	for (i = g_StartedPlugins.begin (); i != g_StartedPlugins.end ();)
	{
		if ((*i)->GetGroupType () == PluginType || PluginType == CCKAHPlugin::ptLast)
		{
			StopPlugin (**i);
			i = g_StartedPlugins.erase (i);
		}
		else
			++i;
	}
}

static void StopPluginByName (LPCWSTR szwName)
{
	StartedPluginsList::CLock lock (&g_StartedPlugins);

	StartedPluginsList::iterator i;

	for (i = g_StartedPlugins.begin (); i != g_StartedPlugins.end ();)
	{
		if (RemoveExtension ((*i)->m_sName) == szwName)
		{
			StopPlugin (**i);
			i = g_StartedPlugins.erase (i);
		}
		else
			++i;
	}
}

CKAHUM::OpResult CKAHUM_InternalStop (CCKAHPlugin::PluginType PluginType)
{
	log.WriteDWORD (_T("[CKAHUM::Stop] Stopping type %d plugin..."), PluginType);
	
	StopPluginByType (PluginType);

	log.Write (_T("[CKAHUM::Stop] Stopping done"));

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHUM_InternalPause (CCKAHPlugin::PluginType PluginType)
{
	log.WriteDWORD (_T("[CKAHUM::Pause] Pausing type %d plugin..."), PluginType);

	CKAHUM::OpResult Result = SendPluginModuleMessage (PluginType, PLUG_MOD_PAUSE, NULL, 0, NULL, 0);
	if (Result == CKAHUM::srOK)
		CKAHSTP::MarkResumed (PluginType, false);

	log.WriteFormat (_T("[CKAHUM::Pause] Pausing type %d plugin done (%d)"), PEL_INFO, PluginType, Result);

	return Result;
}

CKAHUM::OpResult CKAHUM_InternalResume (CCKAHPlugin::PluginType PluginType)
{
	log.WriteDWORD (_T("[CKAHUM::Resume] Resuming type %d plugin..."), PluginType);

	CKAHUM::OpResult Result = SendPluginModuleMessage (PluginType, PLUG_MOD_RESUME, NULL, 0, NULL, 0);
	if (Result == CKAHUM::srOK)
		CKAHSTP::MarkResumed (PluginType, true);

	log.WriteFormat (_T("[CKAHUM::Resume] Resuming type %d plugin done (%d)"), PEL_INFO, PluginType, Result);

	return Result;
}

CKAHUM::OpResult CKAHUM_AfterRebootCheck (LPCKAHSTORAGE lpStorage)
{
	CCKAHManifest reboot_manifest;
	CKAHUM::OpResult RetResult = CKAHUM::srOK;

	if (reboot_manifest.ReadFromStorage (lpStorage, szwPersistentRebootManifestRoot) &&
			!reboot_manifest.IsEmpty () &&
			!CRebootCheckHlp::IsRebootQueried ())
	{
		log.Write (_T("[CKAHUM::Start] Reloading after system reboot..."), PEL_INFO);

		RetResult = ReloadFromManifest (reboot_manifest, lpStorage);

		lpStorage->ClearParams (szwPersistentRebootManifestRoot);

		log.Write (_T("[CKAHUM::Start] Reloading after system reboot done"), PEL_INFO);
	}

	return RetResult;
}

//////////////////////////////////////////////////////////////////////////

void GetDriverInstallPath (TCHAR *szPath, UINT nPathSize)
{
	GetSystemDirectory (szPath, nPathSize);
	if (g_bIsNT)
		_tcscat (szPath, _T("\\Drivers"));
	_tcscat (szPath, _T("\\"));
}

CKAHUM::OpResult ReloadFromManifest (CCKAHManifest &manifest, LPCKAHSTORAGE lpStorage)
{
	CWow64FsRedirectorDisabler Wow64FsRedirectorDisabler;
	CCKAHManifest persistent_manifest;

	persistent_manifest.ReadFromStorage (lpStorage, szwPersistentManifestRoot);

	persistent_manifest.m_Directory = manifest.m_Directory;

	std::vector<CEnvelope<CCKAHBaseDrv> > eliminated_basedrivers;
	std::vector<CEnvelope<CCKAHPlugin> > eliminated_plugins;

	unsigned i, j;

    // Author : Roschin
    // Включить, если требуется использовать только нужные базовые драйвера.
    // для поддержки требуются изменения в инсталляторе.
    // manifest.EliminateUnused( lpStorage );

	manifest.EliminateIdentical (persistent_manifest, eliminated_basedrivers, eliminated_plugins);

	TCHAR szDriverInstallPath[MAX_PATH + 1 + 9];
	GetDriverInstallPath (szDriverInstallPath, sizeof (szDriverInstallPath));
	std::wstring sDriverInstallPath (__LPWSTR (szDriverInstallPath));

    const WCHAR *szwDestExtension = g_bIsNT ? ( g_bUseKL1 ? L".dat" : L".sys") : L".vxd";

	WCHAR szPersDesc[0x400], szNewDesc[0x400];

	bool bNeedReboot = false;
	std::vector <CEnvelope<CCKAHBaseDrv> > newBaseDrvs;

	bool bIsRebootWasQueried = CRebootCheckHlp::IsRebootQueried ();

	// base drivers

	bool bBaseDriversEnabled = true;
	CKAHUM::IsBaseDriversEnabled(&bBaseDriversEnabled);
	
	// проверим, что с неизмененными все OK
	for (i = 0; i < eliminated_basedrivers.size (); ++i)
	{
		CCKAHBaseDrv &drv = *eliminated_basedrivers[i];
		
		drv.WriteToString (szPersDesc);

		log.WriteFormat (_T("[CKAHUM::Reload] Processing basedriver '%ls' (%ls)"), PEL_INFO,
			drv.m_sName.c_str (), szPersDesc);
		
		bool bReplaced = false;

		if (CheckAndUnpack (drv.m_sName, manifest.m_Directory, sDriverInstallPath, drv.m_dwVersionMS, drv.m_dwVersionLS, bReplaced))
		{
			std::wstring sBaseName = RemoveExtension (drv.m_sName);

			std::wstring szwDstPath = sDriverInstallPath + sBaseName + szwDestExtension;

			bool bIsBaseDrvRegistered = false;
			CKAHUM_IsBaseDrvRegistered (sBaseName.c_str (), szwDstPath.c_str (), bIsBaseDrvRegistered);
			
			if (bIsBaseDrvRegistered)
				bNeedReboot |= bReplaced; // надо перегружаться, если действительно было копирование
            else if (bBaseDriversEnabled)
            {
                CKAHUM_RegisterBaseDrv (sBaseName.c_str(), szwDstPath.c_str ());
                bNeedReboot = true;
            }

			IsBaseDriverRunning(sBaseName); // to trace base driver state
		}
        else
        {
		    log.WriteFormat (_T("[CKAHUM::Reload] Processing of basedriver '%ls' failed"), PEL_ERROR,
								    drv.m_sName.c_str ());
            return CKAHUM::srOpFailed;
        }

		log.WriteFormat (_T("[CKAHUM::Reload] Processing of basedriver '%ls' done"), PEL_INFO,
								drv.m_sName.c_str ());
	}

	// обновление base drivers
	for (i = 0; i < manifest.m_BaseDrvs.size (); ++i)
	{
		for (j = 0; j < persistent_manifest.m_BaseDrvs.size (); ++j)
		{
			if (manifest.m_BaseDrvs[i]->m_sName == persistent_manifest.m_BaseDrvs[j]->m_sName)
			{
				CCKAHBaseDrv &persdrv = *persistent_manifest.m_BaseDrvs[j];
				CCKAHBaseDrv &newdrv = *manifest.m_BaseDrvs[i];

				persdrv.WriteToString (szPersDesc);
				newdrv.WriteToString (szNewDesc);

				log.WriteFormat (_T("[CKAHUM::Reload] Processing basedriver '%ls': persistent (%ls), new (%ls)"), PEL_INFO,
					newdrv.m_sName.c_str (), szPersDesc, szNewDesc);

				bool bReplaced = false;
				
				log.WriteFormat (_T("[CKAHUM::Reload] Processing basedriver '%ls': update needed"), PEL_INFO,
										newdrv.m_sName.c_str ());

				if (CheckAndUnpack (newdrv.m_sName, manifest.m_Directory, sDriverInstallPath, newdrv.m_dwVersionMS, newdrv.m_dwVersionLS, bReplaced))
                {
			        std::wstring sBaseName = RemoveExtension (newdrv.m_sName);

			        std::wstring szwDstPath = sDriverInstallPath + sBaseName + szwDestExtension;

			        bool bIsBaseDrvRegistered = false;
			        CKAHUM_IsBaseDrvRegistered (sBaseName.c_str (), szwDstPath.c_str (), bIsBaseDrvRegistered);

			        if (bIsBaseDrvRegistered)
					    bNeedReboot |= bReplaced;
                    else if (bBaseDriversEnabled)
                    {
                        CKAHUM_RegisterBaseDrv (sBaseName.c_str(), szwDstPath.c_str ());
                        bNeedReboot = true;
                    }
                }
                else
                {
				    log.WriteFormat (_T("[CKAHUM::Reload] Processing basedriver '%ls' failed"), PEL_ERROR,
					    newdrv.m_sName.c_str ());
                    return CKAHUM::srOpFailed;
                }

				newBaseDrvs.push_back (manifest.m_BaseDrvs[i]);

				log.WriteFormat (_T("[CKAHUM::Reload] Processing basedriver '%ls' done"), PEL_INFO,
					newdrv.m_sName.c_str ());

				manifest.m_BaseDrvs.erase (manifest.m_BaseDrvs.begin () + i--);
				persistent_manifest.m_BaseDrvs.erase (persistent_manifest.m_BaseDrvs.begin () + j);

				break;
			}
		}
	}

	// удаление ненужных base drivers
	for (i = 0; i < persistent_manifest.m_BaseDrvs.size (); ++i)
	{
		CCKAHBaseDrv &persdrv = *persistent_manifest.m_BaseDrvs[i];

		persdrv.WriteToString (szPersDesc);

		std::wstring sPersName = RemoveExtension (persdrv.m_sName);

		log.WriteFormat (_T("[CKAHUM::Reload] Processing basedriver '%ls' (%ls): need to be removed"), PEL_INFO,
								persdrv.m_sName.c_str (), szPersDesc);

		bool bIsBaseDrvRegistered = false;
		CKAHUM_IsBaseDrvRegistered (sPersName.c_str (), bIsBaseDrvRegistered);

		if (bIsBaseDrvRegistered)
		{
			if (CKAHUM_UnregisterBaseDrv (sPersName.c_str (), false))
			{
				std::wstring szwDrvPath = sDriverInstallPath + sPersName + szwDestExtension;
				RemoveAttribsDeleteFileW (szwDrvPath.c_str ());

				bNeedReboot = true;
			}
			else
				newBaseDrvs.push_back (persistent_manifest.m_BaseDrvs[i]);
		}

		log.WriteFormat (_T("[CKAHUM::Reload] Remove of basedriver '%ls' done"), PEL_INFO,
								persdrv.m_sName.c_str ());
	}

	// добавление новых base drivers
	for (i = 0; i < manifest.m_BaseDrvs.size (); ++i)
	{
		CCKAHBaseDrv &newdrv = *manifest.m_BaseDrvs[i];

		newdrv.WriteToString (szPersDesc);

		log.WriteFormat (_T("[CKAHUM::Reload] Processing basedriver '%ls' (%ls): need to be added"), PEL_INFO,
								newdrv.m_sName.c_str (), szPersDesc);

        bool bReplaced = false;

		if (CheckAndUnpack (newdrv.m_sName, manifest.m_Directory, sDriverInstallPath, newdrv.m_dwVersionMS, newdrv.m_dwVersionLS, bReplaced))
		{
			std::wstring sBaseName = RemoveExtension (newdrv.m_sName);

			std::wstring szwDstPath = sDriverInstallPath + sBaseName + szwDestExtension;

			bool bIsBaseDrvRegistered = false;
			CKAHUM_IsBaseDrvRegistered (sBaseName.c_str (), szwDstPath.c_str (), bIsBaseDrvRegistered);
			
			if (bIsBaseDrvRegistered)
				bNeedReboot |= bReplaced; // надо перегружаться, если действительно было копирование
            else if (bBaseDriversEnabled)
			{
				CKAHUM_RegisterBaseDrv (sBaseName.c_str (), szwDstPath.c_str ());
				bNeedReboot = true;
			}
			newBaseDrvs.push_back (manifest.m_BaseDrvs[i]);
		}
        else
        {
		    log.WriteFormat (_T("[CKAHUM::Reload] Adding of basedriver '%ls' failed"), PEL_ERROR,
								    manifest.m_BaseDrvs[i]->m_sName.c_str ());
            return CKAHUM::srOpFailed;
        }

		log.WriteFormat (_T("[CKAHUM::Reload] Adding of basedriver '%ls' done"), PEL_INFO,
								manifest.m_BaseDrvs[i]->m_sName.c_str ());
	}

	// добавим исключенные base drivers
	newBaseDrvs.insert (newBaseDrvs.end (), eliminated_basedrivers.begin (), eliminated_basedrivers.end ());

	persistent_manifest.m_BaseDrvs = newBaseDrvs;

	if (bNeedReboot || bIsRebootWasQueried)
	{
		// пометим, что нужна перезагрузка
		if (bNeedReboot && !bIsRebootWasQueried)
			CRebootCheckHlp::QueryReboot ();

		// запишем модифицированный persistent в реестр
		persistent_manifest.m_Plugins.insert (persistent_manifest.m_Plugins.end (), eliminated_plugins.begin (), eliminated_plugins.end ());
		if (!persistent_manifest.WriteToStorage (lpStorage, szwPersistentManifestRoot))
			log.Write (_T("[CKAHUM::Reload] Failed to write manifest to storage"));

		// ...а тот, который должен быть после перезагрузки запишем рядом
		manifest.m_BaseDrvs = newBaseDrvs;
		manifest.m_Plugins.insert (manifest.m_Plugins.end (), eliminated_plugins.begin (), eliminated_plugins.end ());

		if (!manifest.WriteToStorage (lpStorage, szwPersistentRebootManifestRoot))
			log.Write (_T("[CKAHUM::Reload] Failed to write reboot manifest to storage"));
		
		log.Write (_T("[CKAHUM::Reload] Reloading done. System need to be rebooted."));

		return CKAHUM::srNeedReboot;
	}

	// plugins

	CKAHSTP::PluginTypeList resumed_plugin_list;
	CKAHSTP::GetResumedPluginIfaceTypeList (resumed_plugin_list);

	// перегрузим старые плагины, которые запущены
	for (i = 0; i < eliminated_plugins.size (); ++i)
	{
		log.WriteFormat (_T("[CKAHUM::Reload] Processing plugin '%ls': reloading"), PEL_INFO,
								eliminated_plugins[i]->m_sName.c_str ());

		// распакуем плагин и его данные
		bool bReplaced = false;
		if(!CheckAndUnpack (eliminated_plugins[i]->m_sName, manifest.m_Directory,
						manifest.m_Directory, eliminated_plugins[i]->m_dwVersionMS, eliminated_plugins[i]->m_dwVersionLS, bReplaced))
		{
			log.WriteFormat (_T("[CKAHUM::Reload] Failed to unpack plugin '%ls'"), PEL_ERROR,
									eliminated_plugins[i]->m_sName.c_str ());
			continue;
		}
		if(!eliminated_plugins[i]->CopyData(manifest.m_Directory.c_str()))
		{
			log.WriteFormat (_T("[CKAHUM::Reload] Failed to copy plugin '%ls' data"), PEL_ERROR,
									eliminated_plugins[i]->m_sName.c_str ());
		}

		CCKAHPlugin::PluginType PluginType = eliminated_plugins[i]->GetIfaceType ();

		// если плагин такого типа был запущен, то этот тоже надо запустить
		if (CKAHSTP::IsPluginStarted (PluginType))
		{
			log.WriteFormat (_T("[CKAHUM::Reload] Starting plugin '%ls'"), PEL_INFO,
									eliminated_plugins[i]->m_sName.c_str ());

			bool bNeedLoadData = true;

			if (bReplaced)
			{
				StopPluginByName (eliminated_plugins[i]->m_sName.c_str());
				bNeedLoadData = StartPlugin (eliminated_plugins[i]->m_sName, manifest.m_Directory, false);
			}

			if(bNeedLoadData)
				eliminated_plugins[i]->LoadData (manifest.m_Directory.c_str ());

			log.WriteFormat (_T("[CKAHUM::Reload] Reloading of plugin '%ls' done"), PEL_INFO,
									eliminated_plugins[i]->m_sName.c_str ());
		}
		else
		{
			log.WriteFormat (_T("[CKAHUM::Reload] Processing plugin '%ls': type %d plugin not started, skipped"), PEL_INFO,
									eliminated_plugins[i]->m_sName.c_str (), PluginType);
		}
	}

	// добавление новых
	for (i = 0; i < manifest.m_Plugins.size (); ++i)
	{
		log.WriteFormat (_T("[CKAHUM::Reload] Processing plugin '%ls': need to be added"), PEL_INFO,
								manifest.m_Plugins[i]->m_sName.c_str ());
		
		// распакуем плагин и его данные
		bool bReplaced = false;
		if(!CheckAndUnpack (manifest.m_Plugins[i]->m_sName, manifest.m_Directory,
						manifest.m_Directory, manifest.m_Plugins[i]->m_dwVersionMS, manifest.m_Plugins[i]->m_dwVersionLS, bReplaced))
		{
			log.WriteFormat (_T("[CKAHUM::Reload] Failed to unpack plugin '%ls'"), PEL_ERROR,
									manifest.m_Plugins[i]->m_sName.c_str ());
			continue;
		}
		if(!manifest.m_Plugins[i]->CopyData(manifest.m_Directory.c_str()))
		{
			log.WriteFormat (_T("[CKAHUM::Reload] Failed to copy plugin '%ls' data"), PEL_ERROR,
									manifest.m_Plugins[i]->m_sName.c_str ());
		}

		CCKAHPlugin::PluginType PluginType = manifest.m_Plugins[i]->GetIfaceType ();

		// если плагин такого типа был запущен, то этот тоже надо запустить
		if (CKAHSTP::IsPluginStarted (PluginType))
		{
			log.WriteFormat (_T("[CKAHUM::Reload] Starting plugin '%ls'"), PEL_INFO,
									manifest.m_Plugins[i]->m_sName.c_str ());

			if (StartPlugin (manifest.m_Plugins[i]->m_sName, manifest.m_Directory, true))
			{
				CKAHSTP::AddPluginToStarted (manifest.m_Plugins[i]);
				manifest.m_Plugins[i]->LoadData (manifest.m_Directory.c_str ());
			}

			log.WriteFormat (_T("[CKAHUM::Reload] Adding of plugin '%ls' done"), PEL_INFO,
									manifest.m_Plugins[i]->m_sName.c_str ());
		}
		else
		{
			log.WriteFormat (_T("[CKAHUM::Reload] Processing plugin '%ls': type %d plugin not started, skipped"), PEL_INFO,
									manifest.m_Plugins[i]->m_sName.c_str (), PluginType);
		}
	}

	// удаление ненужных
	for (i = 0; i < persistent_manifest.m_Plugins.size (); ++i)
	{
		std::wstring szwPluginName = RemoveExtension (persistent_manifest.m_Plugins[i]->m_sName);

		log.WriteFormat (_T("[CKAHUM::Reload] Processing plugin '%ls': need to be removed"), PEL_INFO,
								szwPluginName.c_str ());

//		CKAHUM_InternalStop (persistent_manifest.m_Plugins[i]->GetGroupType ());
		StopPluginByName (szwPluginName.c_str ());
		
		std::wstring sFileToDelete;

		//sFileToDelete = persistent_manifest.m_Directory + persistent_manifest.m_Plugins[i]->m_sName;
		//log.WriteFormat (_T("[CKAHUM::Reload] Deleting file '%ls'"), PEL_INFO, sFileToDelete.c_str ());
		//RemoveAttribsDeleteFileW (sFileToDelete.c_str ());

		sFileToDelete = persistent_manifest.m_Directory + szwPluginName + szwDestExtension;
		log.WriteFormat (_T("[CKAHUM::Reload] Deleting file '%ls'"), PEL_INFO, sFileToDelete.c_str ());
		RemoveAttribsDeleteFileW (sFileToDelete.c_str ());

		log.WriteFormat (_T("[CKAHUM::Reload] Removing plugin '%ls' data"), PEL_INFO, szwPluginName.c_str ());
		persistent_manifest.m_Plugins[i]->RemoveData (persistent_manifest.m_Directory.c_str ());

		lpStorage->ClearParams ((std::wstring (szwPersistentManifestRoot) + L'\\' + szwPluginName).c_str ());

		log.WriteFormat (_T("[CKAHUM::Reload] Removing of plugin '%ls' done"), PEL_INFO,
								szwPluginName.c_str ());
	}

	// перезапустим плагины, которые были запущены до обновления
	for (i = 0; i < resumed_plugin_list.size (); ++i)
		CKAHUM_InternalResume (resumed_plugin_list[i]);

	// заменим плагины

	// добавим исключенные
	manifest.m_Plugins.insert (manifest.m_Plugins.end (), eliminated_plugins.begin (), eliminated_plugins.end ());
	persistent_manifest.m_Plugins = manifest.m_Plugins;

	// еще раз распакуем данные, так как они могли быть удалены при удалении плагинов,
	// а имена файлов данных могут иметь одинаковые имена
	for (i = 0; i < manifest.m_Plugins.size (); ++i)
	{
		//log.WriteFormat (_T("[CKAHUM::Reload] Copying plugin '%ls' data"), PEL_INFO,
		//						manifest.m_Plugins[i]->m_sName.c_str ());
		
		if(!manifest.m_Plugins[i]->CopyData(manifest.m_Directory.c_str()))
		{
			//log.WriteFormat (_T("[CKAHUM::Reload] Failed to copy plugin '%ls' data"), PEL_ERROR,
			//						manifest.m_Plugins[i]->m_sName.c_str ());
		}
	}

	// запишем модифицированный persistent в реестр
	if (!persistent_manifest.WriteToStorage (lpStorage, szwPersistentManifestRoot))
		log.Write (_T("[CKAHUM::Reload] Failed to write manifest to storage"));
	
	log.Write (_T("[CKAHUM::Reload] Reloading done"));

	return CKAHUM::srOK;
}


CKAHUM::OpResult RemoveBaseDrivers (LPCKAHSTORAGE lpStorage)
{
	CWow64FsRedirectorDisabler Wow64FsRedirectorDisabler;
	CCKAHManifest persistent_manifest;

	persistent_manifest.ReadFromStorage (lpStorage, szwPersistentManifestRoot);

	TCHAR szDriverInstallPath[MAX_PATH + 1 + 9];
	GetDriverInstallPath (szDriverInstallPath, sizeof (szDriverInstallPath));
	std::wstring sDriverInstallPath (__LPWSTR (szDriverInstallPath));

    const WCHAR *szwBaseDrvExtension = g_bIsNT ? (g_bUseKL1 ? L".dat" : L".sys") : L".vxd";

	WCHAR szPersDesc[0x400];
    unsigned i;

	for (i = 0; i < persistent_manifest.m_BaseDrvs.size (); ++i)
	{
		CCKAHBaseDrv &persdrv = *persistent_manifest.m_BaseDrvs[i];

		persdrv.WriteToString (szPersDesc);

		std::wstring sPersName = RemoveExtension (persdrv.m_sName);

		log.WriteFormat (_T("[CKAHUM::RemoveBaseDrv] Processing basedriver '%ls' (%ls): need to be removed"), PEL_INFO,
								persdrv.m_sName.c_str (), szPersDesc);

		bool bIsBaseDrvRegistered = false;
		CKAHUM_IsBaseDrvRegistered (sPersName.c_str (), bIsBaseDrvRegistered);

		if (bIsBaseDrvRegistered)
		{
		    log.WriteFormat (_T("[CKAHUM::RemoveBaseDrv] Unregistering basedriver '%ls'"), PEL_INFO,
								    persdrv.m_sName.c_str ());

			CKAHUM_UnregisterBaseDrv (sPersName.c_str (), false);
		}

		std::wstring szwDrvPath = sDriverInstallPath + sPersName + szwBaseDrvExtension;
		RemoveAttribsDeleteFileW (szwDrvPath.c_str ());

		log.WriteFormat (_T("[CKAHUM::RemoveBaseDrv] Remove of basedriver '%ls' done"), PEL_INFO,
								persdrv.m_sName.c_str ());
    }

	return CKAHUM::srOK;
}


CKAHUM::OpResult SendPluginModuleMessage (CCKAHPlugin::PluginType PluginType,
											DWORD dwIoControlCode,
											PVOID pInBuffer, ULONG nInBufferSize,
											PVOID pOutBuffer, ULONG nOutBufferSize, ULONG *nRetBufferSize)
{
	TCHAR szName [MAX_PATH + 5];
	HANDLE hPlugin;

	bool bOk = false, bPluginFound = false;
    CKAHUM::OpResult res = CKAHUM::srOpFailed;

	StartedPluginsList::CLock lock (&g_StartedPlugins);

	StartedPluginsList::iterator i;

	for (i = g_StartedPlugins.begin (); i != g_StartedPlugins.end (); ++i)
	{
		if ((*i)->GetIfaceType () != PluginType)
			continue;

		bPluginFound = true;

		std::wstring sName = RemoveExtension ((*i)->m_sName);

		wsprintf (szName, _T("\\\\.\\%s"), __LPCTSTR (sName.c_str ()));

		if (g_bIsNT)
			hPlugin = CreateFile (szName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		else
			hPlugin = CreateFile (szName, 0, 0, NULL, 0, 0, NULL);

		if (hPlugin == INVALID_HANDLE_VALUE)
		{
			log.WriteFormat (_T("[SendPluginModuleMessage] Unable to get '%ls' plugin handle (0x%08x)"), PEL_ERROR, 
								sName.c_str (), GetLastError ());
			continue;
		}

		DWORD dwBytesReturned = 0;
		if (!DeviceIoControl (hPlugin, dwIoControlCode, pInBuffer, nInBufferSize, pOutBuffer, nOutBufferSize, &dwBytesReturned, NULL))
		{
            if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
            {
                res = CKAHUM::srInsufficientBuffer;
            }
			else
			{
				log.WriteFormat (_T("[SendPluginModuleMessage] DeviceIoControl failed (0x%08x)"), PEL_ERROR, GetLastError ());
			}
		}
		else
		{
			bOk = true;
            res = CKAHUM::srOK;
		}

        if (nRetBufferSize) *nRetBufferSize = dwBytesReturned;

		CloseHandle (hPlugin);

		break;
	}
		
	if (!bOk && !bPluginFound)
		log.WriteFormat (_T("[SendPluginModuleMessage] No plugin of type %d found"), PEL_ERROR, PluginType);

	return res;
}


CKAHUM::OpResult SendPluginMessage (CCKAHPlugin::PluginType PluginType, DWORD dwIoControlCode, CHAR *szPluginName, ULONG nMessageID,
									   PVOID pInBuffer, ULONG nInBufferSize,
										PVOID pOutBuffer, ULONG nOutBufferSize, ULONG *nRetBufferSize)
{
	std::vector<BYTE> vec_buf (sizeof (PLUGIN_MESSAGE) + nInBufferSize, 0);

	PLUGIN_MESSAGE *pPluginMessage = (PLUGIN_MESSAGE *)&vec_buf[0];

	strcpy (pPluginMessage->PluginName, szPluginName);
	pPluginMessage->MessageID = nMessageID;
	pPluginMessage->MessageSize = nInBufferSize;

	if (pInBuffer && nInBufferSize > 0)
		memcpy (&vec_buf [sizeof (PLUGIN_MESSAGE)], pInBuffer, nInBufferSize);

	return SendPluginModuleMessage (PluginType, dwIoControlCode, &vec_buf[0], vec_buf.size (), pOutBuffer, nOutBufferSize, nRetBufferSize);
}

bool IsPluginStarted (CCKAHPlugin::PluginType PluginType, DWORD *pdwPluginVersion /* = NULL */)
{
	log.WriteDWORD (_T("[IsPluginStarted] Checking start of type %d plugin..."), PluginType);

	DWORD dwModVersion = 0;

	bool bIsStarted = SendPluginModuleMessage (PluginType,
											PLUG_GET_MOD_VERSION,
											NULL, 0,
											&dwModVersion, sizeof (dwModVersion)) == CKAHUM::srOK;

	if (bIsStarted && pdwPluginVersion)
		*pdwPluginVersion = dwModVersion;

	if (bIsStarted)
		log.WriteDWORD (_T("[IsPluginStarted] Type %d plugin is started"), PluginType);
	else
		log.WriteDWORD (_T("[IsPluginStarted] Type %d plugin is not started"), PluginType);

	return bIsStarted;
}

CKAHUM::OpResult StopAllPlugins ()
{
	StopPluginByType (CCKAHPlugin::ptLast);
	return CKAHUM::srOK;
}

bool IsBaseDriverRunning(const std::wstring &szwBaseDrvName)
{
	bool bIsRunning = false;

	TCHAR szName [MAX_PATH + 5];
	HANDLE hBaseDrv;

	wsprintf (szName, _T("\\\\.\\%s"), __LPCTSTR (szwBaseDrvName));

	if (g_bIsNT)
		hBaseDrv = CreateFile (szName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		hBaseDrv = CreateFile (szName, 0, 0, NULL, 0, 0, NULL);

	bIsRunning = hBaseDrv != INVALID_HANDLE_VALUE;

	if (bIsRunning)
		log.WriteFormat (_T("[CKAHUM::CHECKBDRUN] '%ls' is running (0x%08x)"), PEL_INFO, szwBaseDrvName.c_str (), hBaseDrv);
	else
		log.WriteFormat (_T("[CKAHUM::CHECKBDRUN] Unable to open '%ls' (0x%08x)"), PEL_ERROR, szwBaseDrvName.c_str (), GetLastError ());

	if (bIsRunning)
		CloseHandle (hBaseDrv);

	return bIsRunning;
}
