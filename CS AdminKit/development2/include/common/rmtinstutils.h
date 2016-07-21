/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file tsk/RmtInstUtils.h
 * \author јндрей Ћащенков
 * \date 14:10 10.09.2004
 * \brief ќбщие утилиты, используемые подсистемой удаленной инсталл€ции.
 */

#ifndef __TSK_REMOTE_INSTALL_UTILSH__
#define __TSK_REMOTE_INSTALL_UTILSH__

#include <string>
#include <vector>
#include <common/ricommon.h>

#define LS_EXEC_FILE_NAME L"lsexec.exe"
#define KLTSK_KAV_Remote_Installations_FOLDER  L"KAV Remote Installations"
#define KLTSK_KAV_Remote_Installations_FOLDER_9x  L"KLRI"

#define KLTSKRI_STUPID_MAGIC_WORD 0x54c3

namespace KLTSK
{
	enum RebootState {
		REBOOT_NOT_NEEDED,
		REBOOT_NEEDED,
		REBOOT_DONE
	};

	typedef bool (*FGetAppEventStopCallback)(void* pParam);

	bool Setup(
		const std::wstring& wstrFolder, 
		const std::wstring& wstrParams, 
		const std::wstring& wstrWorkingFolder, 
		long lTimeoutMS, 
		FGetAppEventStopCallback pGetAppEventStopCallback, 
		void* pGetAppEventStopCallbackParam,
		bool bWaitForProcess, 
		const std::vector<std::wstring>& vecFilesToDelete,
		bool bStartRbtAgt,
		long& lResultCode, 
		const std::wstring& wstrExecutable = L"",
		bool bInteractWithDesktop = true);
	
	bool Setup(
		const std::wstring& wstrFolder, 
		const std::wstring& wstrParams, 
		const std::wstring& wstrWorkingFolder, 
		long lTimeoutMS, 
		FGetAppEventStopCallback pGetAppEventStopCallback, 
		void* pGetAppEventStopCallbackParam,
		bool bWaitForProcess, 
		bool bStartRbtAgt,
		long& lResultCode, 
		const std::wstring& wstrExecutable = L"",
		bool bInteractWithDesktop = true);

	bool GetCurrentInstallState(const std::wstring& wstrFolder, std::wstring& wstrProcessResultCode,
		std::wstring& wstrFirstResultCode, std::wstring& wstrSecondResultCode, std::wstring& wstrReportFileContent,
		KLTSK::RebootState& rebootState);

	std::wstring CreateISSetupParameters(
		const std::vector<std::pair<std::wstring, std::wstring> >& params, 
		const std::wstring& wstrLogPath,
		bool bReplaceApostr);

	bool IsWellKnownISSetupProcessError(long lProcessResultCode);

	bool GetISSetupProcessErrorString(long lProcessResultCode, std::wstring& wstrErrorDescr);

	bool FormatISSetupProcessError(long lProcessResultCode, std::wstring& wstrErrorDescr);

	std::wstring GetISLogFilePath();

	std::wstring GetApplicationFolderName();

	std::wstring FindFirstKpdFile(const std::wstring& wstrDirPath);

	bool GetTaskId(const std::wstring& wstrCmdLine, std::wstring& wstrTaskId);
	
	void WriteDistribPathToE2S(const std::wstring& wstrTaskId, const std::wstring& wstrSvrFileIdentity);
	
	std::wstring GetLocalWritableFolder(bool bFindInRegistryIfNotFound, bool bUseDefault, bool bThrowNotFound);
	
	bool GetCmdLineParam(const std::wstring& wstrCmdLine, const std::wstring& wstrParam, std::wstring& wstrValue);
	
	void RemoveTaskIdFromE2S(const std::wstring& wstrTaskId, bool bAll);

	std::wstring EncryptUninstallPswd(const std::wstring& wstrPswd);

	std::wstring DecryptUninstallPswd(const std::wstring& wstrPswd);
	
	std::wstring FindFirstFileByExt(const std::wstring& wstrDirPath, const std::wstring& wstrExt, bool bFullPath = true);
	
	bool CheckZombieFile(const std::wstring& wstrZombieFilePath);
	
	void UpdateFile(const std::wstring& wstrSourceFile, const std::wstring& wstrDestFile, bool bUpdateExisting, bool bAddNew = true);

	bool IsRemoteInstallationTaskAlreadyStartedHere(const std::wstring& wstrSection, const std::wstring& wstrStartID);

	std::wstring GetSubscriptionDataFilePath();
}

void KLSTD_CopyDirectory(const wchar_t* szwDir, const wchar_t* szwDest);

#endif // __TSK_REMOTE_INSTALL_UTILSH__
