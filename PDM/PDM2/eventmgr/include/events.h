// ----------------- PRIMARY EVENTS -----------------

DECLARE_EVENT( OnProcessCreatePre, (cEvent& event, cFile& image_path, tcstring cmd_line, uint32_t creation_flags), (event, image_path, cmd_line, creation_flags));
DECLARE_EVENT( OnProcessCreatePost, (cEvent& event, tPid new_proc_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line, uint32_t creation_flags), (event, new_proc_pid, image_path, working_folder, cmd_line, creation_flags));
DECLARE_EVENT( OnProcessExitPre, (cEvent& event), (event));
DECLARE_EVENT( OnProcessExitPost, (cEvent& event), (event));
DECLARE_EVENT( OnProcessTerminatePre, (cEvent& event, tPid dest_pid), (event, dest_pid));

DECLARE_EVENT( OnThreadCreatePre, (cEvent& event, tPid dest_pid, tAddr eip, tAddr thread_base), (event, dest_pid, eip, thread_base));
DECLARE_EVENT( OnThreadCreatePost, (cEvent& event, tPid dest_pid, tTid new_tid, tAddr eip, tAddr thread_base), (event, dest_pid, new_tid, eip, thread_base));
DECLARE_EVENT( OnThreadExitPre, (cEvent& event), (event));
DECLARE_EVENT( OnThreadExitPost, (cEvent& event), (event));
DECLARE_EVENT( OnThreadTerminatePost, (cEvent& event, tPid dest_pid, tTid dest_tid), (event, dest_pid, dest_tid));
DECLARE_EVENT( OnThreadSetContextPre, (cEvent& event, tPid dest_pid, tTid dest_tid, tAddr eip), (event, dest_pid, dest_tid, eip));

DECLARE_EVENT( OnOpenProcessPost, (cEvent& event, tPid dest_pid), (event, dest_pid));
DECLARE_EVENT( OnWriteProcessMemoryPre, (cEvent& event, tPid dest_pid, tAddr addr, void* pData, size_t nDataSize), (event, dest_pid, addr, pData, nDataSize));
DECLARE_EVENT( OnMapViewOfSectionPost, (cEvent& event, tPid dest_pid, tAddr addr, size_t nSize), (event, dest_pid, addr, nSize));
DECLARE_EVENT( OnOpenSectionPre, (cEvent& event, tcstring section_name, uint32_t desired_access), (event, section_name, desired_access));
DECLARE_EVENT( OnPhysicalMemoryAccess, (cEvent& event, bool bWrite), (event, bWrite));

DECLARE_EVENT( OnFileCreatePre, (cEvent& event, cFile& file, uint32_t desired_access, uint32_t share_mode, uint32_t creation_disposition), (event, file, desired_access, share_mode, creation_disposition));
DECLARE_EVENT( OnFileCreatePost, (cEvent& event, cFile& file, uint32_t desired_access, uint32_t share_mode, uint32_t creation_disposition), (event, file, desired_access, share_mode, creation_disposition));
DECLARE_EVENT( OnFileCreateFail, (cEvent& event, cFile& file, uint32_t desired_access, uint32_t share_mode, uint32_t creation_disposition), (event, file, desired_access, share_mode, creation_disposition));

DECLARE_EVENT( OnFileModifyPre, (cEvent& event, cFile& file), (event, file));
DECLARE_EVENT( OnFileModifyPost, (cEvent& event, cFile& file), (event, file));
DECLARE_EVENT( OnFileModifyFail, (cEvent& event, cFile& file), (event, file));

DECLARE_EVENT( OnFileCloseModifiedPre,  (cEvent& event, cFile& file), (event, file));
DECLARE_EVENT( OnFileCloseModifiedPost, (cEvent& event, cFile& file), (event, file));

DECLARE_EVENT( OnFileDeletePre, (cEvent& event, cFile& file), (event, file));
DECLARE_EVENT( OnFileDeletePost, (cEvent& event, cFile& file), (event, file));
DECLARE_EVENT( OnFileDeleteFail, (cEvent& event, cFile& file), (event, file));

DECLARE_EVENT( OnFileRenamePre,  (cEvent& event, cFile& src_file, cFile& dst_file), (event, src_file, dst_file));
DECLARE_EVENT( OnFileRenamePost, (cEvent& event, cFile& src_file, cFile& dst_file), (event, src_file, dst_file));
DECLARE_EVENT( OnFileRenameFail, (cEvent& event, cFile& src_file, cFile& dst_file), (event, src_file, dst_file));

DECLARE_EVENT( OnCopyFilePre, (cEvent& event, cFile& src_file, cFile& dst_file, bool bFailIfExists), (event, src_file, dst_file, bFailIfExists));

DECLARE_EVENT( OnRegKeyCreatePost, (cEvent& event, const cRegKey& key), (event, key) );
DECLARE_EVENT( OnRegKeyRenamePost, (cEvent& event, const cRegKey& src_key, const cRegKey& dst_key), (event, src_key, dst_key) );
DECLARE_EVENT( OnRegKeyDeletePost, (cEvent& event, const cRegKey& key), (event, key) );
DECLARE_EVENT( OnRegValueSetPre,  (cEvent& event, const cRegKey& key, tcstring sValueName, uint32_t nNewType, const void* pNewData, size_t nNewDataSize, uint32_t nValueType, const void* pValueData, size_t nValueDataSize), (event, key, sValueName, nNewType, pNewData, nNewDataSize, nValueType, pValueData, nValueDataSize));
DECLARE_EVENT( OnRegValueSetPost, (cEvent& event, const cRegKey& key, tcstring sValueName, uint32_t nNewType, const void* pNewData, size_t nNewDataSize, uint32_t nValueType, const void* pValueData, size_t nValueDataSize), (event, key, sValueName, nNewType, pNewData, nNewDataSize, nValueType, pValueData, nValueDataSize));
DECLARE_EVENT( OnRegValueSetStrPre,  (cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sNewData, tcstring sValueData), (event, key, sValueName, sNewData, sValueData));
DECLARE_EVENT( OnRegValueSetStrPost, (cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sNewData, tcstring sValueData), (event, key, sValueName, sNewData, sValueData));
DECLARE_EVENT( OnRegValueDeletePre,  (cEvent& event, const cRegKey& key, tcstring sValueName, uint32_t nValueType, const void* pValueData, size_t nValueDataSize), (event, key, sValueName, nValueType, pValueData, nValueDataSize));
DECLARE_EVENT( OnRegValueDeletePost, (cEvent& event, const cRegKey& key, tcstring sValueName, uint32_t nValueType, const void* pValueData, size_t nValueDataSize), (event, key, sValueName, nValueType, pValueData, nValueDataSize));
DECLARE_EVENT( OnRegValueDeleteStrPre, (cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sValueData), (event, key, sValueName, sValueData));
DECLARE_EVENT( OnRegValueDeleteStrPost, (cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sValueData), (event, key, sValueName, sValueData));

DECLARE_EVENT( OnDownloadFile, (cEvent& event, cFile& file), (event, file));

DECLARE_EVENT( OnProtectedStorageAccessPre, (cEvent& event), (event));

DECLARE_EVENT( OnSetWindowsHook, (cEvent& event, uint32_t idHook, tAddr lphf, tHANDLE hMod, tTid dwThreadId), (event, idHook, lphf, hMod, dwThreadId));
DECLARE_EVENT( OnFindWindowPre, (cEvent& event, tcstring sClassName, tcstring sTitle), (event, sClassName, sTitle));
DECLARE_EVENT( OnFindWindowPost, (cEvent& event, tcstring sClassName, tcstring sTitle, tHANDLE hwnd), (event, sClassName, sTitle, hwnd));
DECLARE_EVENT( OnFindWindowFail, (cEvent& event, tcstring sClassName, tcstring sTitle), (event, sClassName, sTitle));

// --------------- SECONDARY EVENTS ---------------------


DECLARE_EVENT( OnFileCreateNewPre,  (cEvent& event, cFile& file), (event, file));
DECLARE_EVENT( OnFileCreateNewPost, (cEvent& event, cFile& file), (event, file));
DECLARE_EVENT( OnFileCreateNewFail, (cEvent& event, cFile& file), (event, file));

// HEURISTIC events

DECLARE_EVENT( OnSelfCopy, (cEvent& event, cFile& copy), (event, copy));
DECLARE_EVENT( OnRegAutoRunRegister, (cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sValueData, tcstring sValueDataPart), (event, key, sValueName, sValueData, sValueDataPart));
DECLARE_EVENT( OnRegAutoRunRegisterResolved, (cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sValueData, tcstring sValueDataPart, tcstring sFilename), (event, key, sValueName, sValueData, sValueDataPart, sFilename));

// DETECT events
DECLARE_EVENT( OnSelfCopyNot2Temp, (cEvent& event, cFile& copy), (event, copy));
DECLARE_EVENT( OnSelfCopyMultiple, (cEvent& event, cFile& copy), (event, copy));
DECLARE_EVENT( OnSelfCopyToNetwork, (cEvent& event, cFile& copy), (event, copy));
DECLARE_EVENT( OnSelfCopyToPeerToPeerFolder, (cEvent& event, cFile& copy), (event, copy));
DECLARE_EVENT( OnSelfCopyToStartup, (cEvent& event, tcstring sFilename), (event, sFilename));
DECLARE_EVENT( OnSelfCopyAutoRun, (cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sValueData, tcstring sValueDataPart, tcstring sFilename), (event, key, sValueName, sValueData, sValueDataPart, sFilename));

DECLARE_EVENT( OnProcessInvader, (cEvent& event, tPid dest_pid, tAddr addr), (event, dest_pid, addr));
DECLARE_EVENT( OnDllInvader, (cEvent& event, tPid dest_pid, cFile& dll_file), (event, dest_pid, dll_file));
DECLARE_EVENT( OnRegAvpImageFileExecOptions, (cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sValueData), (event, key, sValueName, sValueData));
DECLARE_EVENT( OnKillService, (cEvent& event, tcstring service), (event, service));
DECLARE_EVENT( OnHostsAdded, (cEvent& event, const char* domain), (event, domain));

DECLARE_EVENT( OnAntiAV, (cEvent& event, int32_t av_id), (event, av_id));
DECLARE_EVENT( OnAntiAVMultiple, (cEvent& event, int32_t av_id), (event, av_id));
DECLARE_EVENT( OnDownloader, (cEvent& event, tcstring url), (event, url));
DECLARE_EVENT( OnKillSystemFiles, (cEvent& event, long nDeletedSystemFiles), (event, nDeletedSystemFiles));
DECLARE_EVENT( OnInfector, (cEvent& event, cFile& last_infected_file), (event, last_infected_file));

DECLARE_EVENT( OnInstallDriver, (cEvent& event, cPath& driver), (event, driver));
DECLARE_EVENT( OnInstall, (cEvent& event, const cRegKey& key, tcstring sValueName, cPath& file), (event, key, sValueName, file));
DECLARE_EVENT( OnInstallNotSelf, (cEvent& event, const cRegKey& key, tcstring sValueName, cPath& file), (event, key, sValueName, file));
DECLARE_EVENT( OnInstallInNTFSStream, (cEvent& event, cPath& file), (event, file));
DECLARE_EVENT( OnStartSystemFileSuspended, (cEvent& event, cFile& image_file, tcstring cmd_line), (event, image_file, cmd_line));
DECLARE_EVENT( OnStartWithSystemName, (cEvent& event, cFile& image_file, tcstring cmd_line), (event, image_file, cmd_line));

DECLARE_EVENT( OnPswAccess, (cEvent& event, cFile& file), (event, file));
DECLARE_EVENT( OnPswAccessMultiple, (cEvent& event, cFile& file), (event, file));

DECLARE_EVENT( OnChangeSystemRegistry, (cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sNewData, tcstring sValueData), (event, key, sValueName, sNewData, sValueData));
DECLARE_EVENT( OnDeleteSelf, (cEvent& event, cFile& helper_file), (event, helper_file));

// TODO: replace these dumbs with more subtle ones
DECLARE_EVENT( OnIEStartPage, (cEvent& event, tcstring sValueData), (event, sValueData));
DECLARE_EVENT( OnSecurityViolation, (cEvent& event, int severity), (event, severity));
//	DECLARE_EVENT( OnRegAutoRunRegisterDropped, (cEvent& event), (event));

// Custom notification events
DECLARE_EVENT( OnEmulatorEvent, (cEvent& event, int32_t event_code, const void* pdata, size_t size), (event, event_code, pdata, size));
