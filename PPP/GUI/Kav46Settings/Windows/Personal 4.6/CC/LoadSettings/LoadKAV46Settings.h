#ifndef _LOADKAV46SETTINGS_H__INCLUDED_
#define	_LOADKAV46SETTINGS_H__INCLUDED_

#include "defs.h"
#include "Task.h"
#include "../AVPMan/AVPAgent.exe/CCTypes.h"

class CKav46SettingsLoader
{
public:
	//! Загружает настройки KAV 4.6.
	//! \return NULL - в случае ошибки.
	virtual CSettings* LoadKav46Settings(
		const char* pszFilename	//!< [in] Полный путь к файлу policy.dat.
		);

	//! Освобождает настройки, загруженные LoadKav46Settings.
	virtual void FreeSettings(CSettings* pSettings);

	virtual CCODSScanObjects* GetScanObjects(LPCVOID pParamBuffer, DWORD dwParamBufferSize);
	virtual void FreeScanObjects(CCODSScanObjects* pObjs);

	virtual bool DecryptPwd(LPCVOID pCrypted, DWORD dwCryptedSize,
							LPVOID pDecrypted, DWORD dwDecryptedSize);

	virtual bool IsTaskUseIt(const CExcludeList& list, const CExclude& excl, LPCSTR pszTaskName, bool bUseAdminSettings);
	virtual bool IsTaskUseRiskware(const CExcludeList& list, LPCSTR pszTaskName, bool bUseAdminSettings);

	virtual EUserAnswer GetOGAnswer(const COfficeGuardSettings& ogset, EOffGuardEvents ev);

	virtual bool PPro_IsMaximalSpeed(const CUserSettingsMC& mcset);
	virtual bool PPro_IsRecommended(const CUserSettingsMC& mcset);
	virtual bool PPro_IsMaximalProtection(const CUserSettingsMC& mcset);
	virtual bool PPro_IsMaximalSpeed(const CUserSettingsOAS& oasset);
	virtual bool PPro_IsRecommended(const CUserSettingsOAS& oasset);
	virtual bool PPro_IsMaximalProtection(const CUserSettingsOAS& oasset);
	virtual bool PPro_IsMaximalSpeed(const CUserSettingsODS& odsset);
	virtual bool PPro_IsRecommended(const CUserSettingsODS& odsset);
	virtual bool PPro_IsMaximalProtection(const CUserSettingsODS& odsset);

	virtual const CUserSettingsODS& GetBaseODSSettings(const CTaskUserTaskSettings& set);
	virtual const CScheduleSettings* GetBaseScheduleSettings(const CTaskUserTaskSettings& set);
	virtual const std::string& GetNameStdStr(const CTask& task);
	virtual CTask& GetAt(CContainerTasks& cont, int i);
	virtual int GetSize(CContainer& cont);
};

extern "C"
{
LOADKAV46SET_API void* GetLoader();
LOADKAV46SET_API void FreeLoader(void*);
};

#endif // _LOADKAV46SETTINGS_H__INCLUDED_