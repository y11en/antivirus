#define _WIN32_WINNT 0x0500
#include <stdio.h>

#include "rdb_smgr.h"
#include "rdb_helper_win32.h"
#include <Abstract/al_file_win.h>


typedef bool (*cbOnProcessDirHandler)(void* pContext, const WCHAR* sPath, WIN32_FIND_DATAW* fd);
bool OnProcessDirHandler(void* pContext, const WCHAR* sPath, WIN32_FIND_DATAW* fd);
bool ProcessDirW(const WCHAR* sPath, const WCHAR* sMask, cbOnProcessDirHandler OnProcessDirHandler, void* pContext);

#include <report.h>
#include <windows.h>
//
//tERROR InitEvent (
//		tEvent* pEvent,
//		tDateTime Timestamp, // 0 - current
//		enEventSeverity Severity,
//		uint16  AdminKitEventID, // (?) AdminKit
//		tNativePid PID,
//		enTaskGroup TaskGroup,
//		enTaskID TaskID,
//		enEventAction Action,
//		enVerdict Verdict
//		)
//{
//	if (!pEvent)
//		return errPARAMETER_INVALID;
//	memset(pEvent, 0, sizeof(tEvent));
////	if (!Timestamp)
////		Timestamp = m_pHelper->GetLocalTime();
//	pEvent->Timestamp = Timestamp;
//	pEvent->Severity = Severity;
//	pEvent->AdminKitEventID = AdminKitEventID;
////	m_procdb->ConvertPid2Uniq(PID, &pEvent->PID, &pEvent->AppID);
//	pEvent->Action = Action;
//	pEvent->TaskGroup = TaskGroup;
//	pEvent->TaskID = TaskID;
//	pEvent->Verdict = Verdict;
//	return errOK;
//}
//	
//
//
//tERROR InitFileEvent (
//		tEvent* pEvent,
//		tDateTime Timestamp, // 0 - current
//		enEventSeverity Severity,
//		uint16  AdminKitEventID, // (?) AdminKit
//		tNativePid PID,
//		enObjectType ObjectType,
//		const wchar_t* sFullPath,
//		const wchar_t* sOldFullPath,
//		enEventAction Action,
//		enTaskGroup TaskGroup,
//		enTaskID TaskID,
//		enVerdict Verdict,
//		enDecisionReason DecisionReason,
//		enDetectType DetectType,
//		const wchar_t* sDetectName,
//		enDetectDanger DetectDanger,
//		enIsExactDetect IsExact,
//		uint64  ExtraInfo
//		)
//{
//	if (!sFullPath)
//		return errPARAMETER_INVALID;
//	InitEvent(pEvent, Timestamp, Severity, AdminKitEventID, PID, TaskGroup, TaskID, Action, Verdict);
//	pEvent->ObjectType = ObjectType;
//	FindOrCreateObject(true, ObjectType, sFullPath, -1, &pEvent->ObjectID);
//	if (sOldFullPath)
//		FindOrCreateObject(true, ObjectType, sOldFullPath, -1, &pEvent->OldObjectID);
//	pEvent->DecisionReason = DecisionReason;
//	pEvent->DetectType = DetectType;
//	if (sDetectName)
//		FindOrCreateObject(false, eDetectName, sDetectName, -1, &pEvent->DetectName);
//	pEvent->DetectDanger = DetectDanger;
//	pEvent->Action = IsExact;
//	pEvent->ExtraInfo = ExtraInfo;
//	return errOK;
//}

void main()
{

//	OpenFile
//	alFileWin32* pDBEvents = new_alFileWin32();
//	pDBEvents->CreateFileW()
	cReportDBHelperWin32 reportHelper;
	cReportDBSegmentMgr report;
	reportHelper.Init(L"data", false);
	report.Init(&reportHelper, sizeof(tReportEvent), true, NULL);
	WCHAR szVolumeName[0x100];
	DWORD dwLogicalDrives, _dwLogicalDrives = GetLogicalDrives();
	dwLogicalDrives = _dwLogicalDrives;
	for (size_t i=0; i<32; i++, dwLogicalDrives>>=1)
	{
		if (0 == (dwLogicalDrives & 1))
			continue;
		WCHAR wcDriveName[4] = L"?:\\";
		wcDriveName[0] = i + 'A';
		tObjectId nLogicalDrive, nDevice;
		tERROR error = report.FindObject(true, eLogicalDisk, RF_UNICODE | RF_CASE_INSENSETIVE, wcDriveName, cSIZE_WSTR, true, 0, &nLogicalDrive);
		if (PR_FAIL(error))
			continue;
		WCHAR szVolumePath[0x1000];
		wcDriveName[2] = 0;
		if (!QueryDosDeviceW(wcDriveName, szVolumePath, 0x1000))
			continue;
		if (0 == _wcsnicmp(szVolumePath, L"\\Device\\", 8))
		{
			wcscat_s(szVolumePath, _countof(szVolumePath), L"\\");
			error = report.FindObject(true, eDevice, RF_UNICODE | RF_CASE_INSENSETIVE, szVolumePath, cSIZE_WSTR, true, 0, &nDevice);
			if (PR_SUCC(error))
				error = report.SetObjectData(nDevice, &nLogicalDrive, sizeof(nLogicalDrive), eObjectMapping);
			if (PR_SUCC(error))
				error = report.SetObjectData(nLogicalDrive, &nDevice, sizeof(nDevice), eObjectMapping);
			continue;
		}
	}
	dwLogicalDrives = _dwLogicalDrives;
	for (size_t i=0; i<32; i++, dwLogicalDrives>>=1)
	{
		if (0 == (dwLogicalDrives & 1))
			continue;
		WCHAR wcDriveName[4] = L"?:\\";
		wcDriveName[0] = i + 'A';
		tObjectId nLogicalDrive, nVolume;
		DWORD dwFSFlags = 0;
		tERROR error = report.FindObject(true, eLogicalDisk, RF_UNICODE | RF_CASE_INSENSETIVE, wcDriveName, cSIZE_WSTR, true, 0, &nLogicalDrive);
//		if (GetVolumeInformation(wcDriveName, szVolumeName, 0x100, 0, 0, &dwFSFlags, 0, 0))
//		{
//			ddata.bCaseSensitive = ((dwFSFlags & FS_CASE_SENSITIVE) == FS_CASE_SENSITIVE ? 1 : 0);
//		}
		if (PR_FAIL(error))
			continue;
		if (!GetVolumeNameForVolumeMountPointW(wcDriveName, szVolumeName, 0x100))
			continue;
		error = report.FindObject(true, eVolume, RF_UNICODE | RF_CASE_INSENSETIVE, szVolumeName, cSIZE_WSTR, true, 0, &nVolume);
		if (PR_SUCC(error))
			error = report.SetObjectData(nVolume, &nLogicalDrive, sizeof(nLogicalDrive), eObjectMapping);
		if (PR_SUCC(error))
			error = report.SetObjectData(nLogicalDrive, &nVolume, sizeof(nVolume), eObjectMapping);
	}
	dwLogicalDrives = _dwLogicalDrives;
	for (size_t i=0; i<32; i++, dwLogicalDrives>>=1)
	{
		if (0 == (dwLogicalDrives & 1))
			continue;
		WCHAR wcDriveName[4] = L"?:\\";
		wcDriveName[0] = i + 'A';
		tObjectId nLogicalDrive, nDevice;
		tERROR error = report.FindObject(true, eLogicalDisk, RF_UNICODE | RF_CASE_INSENSETIVE, wcDriveName, cSIZE_WSTR, true, 0, &nLogicalDrive);
		if (PR_FAIL(error))
			continue;
		WCHAR szVolumePath[0x1000];
		wcDriveName[2] = 0;
		if (!QueryDosDeviceW(wcDriveName, szVolumePath, 0x1000))
			continue;
		if (0 == _wcsnicmp(szVolumePath, L"\\??\\", 4) && szVolumePath[4] && szVolumePath[5]==':')
		{
			wcscat_s(szVolumePath, _countof(szVolumePath), L"\\");
			error = report.FindObject(true, eDirectory, RF_SPLIT_PATH | RF_UNICODE | RF_CASE_INSENSETIVE, szVolumePath + 4, cSIZE_WSTR, true, 0, &nDevice);
			if (PR_SUCC(error))
				error = report.SetObjectData(nLogicalDrive, &nDevice, sizeof(nDevice), eObjectMapping);
			continue;
		}
	}
	ProcessDirW(L"O:\\KAV8\\Windows", L"*.*", OnProcessDirHandler, &report);
	report.Clear();
	ProcessDirW(L"O:\\KAV8\\Windows", L"*.*", OnProcessDirHandler, &report);
	report.Clear();
}


bool ProcessDirW(const WCHAR* sPath, const WCHAR* sMask, cbOnProcessDirHandler OnProcessDirHandler, void* pContext)
{
	size_t len = wcslen(sPath);
	WCHAR* lPath = (WCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (len+MAX_PATH+10)*sizeof(WCHAR));
	if (!lPath)
		return false;
	wcscpy(lPath, sPath);
	wcscpy(lPath+len, L"\\");
	wcscpy(lPath+len+1, sMask);
	WIN32_FIND_DATAW fd;
	HANDLE hFind = FindFirstFileW(lPath, &fd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		do 
		{
			wcscpy(lPath+len+1, fd.cFileName);
			if (!OnProcessDirHandler(pContext, lPath, &fd))
				break;
		} while (FindNextFileW(hFind, &fd));
		FindClose(hFind);
	}
	HeapFree(GetProcessHeap(), 0, lPath);
	return true;
}

bool OnProcessDirHandler(void* pContext, const WCHAR* sPath, WIN32_FIND_DATAW* fd)
{
	cReportDBSegmentMgr* pReport = (cReportDBSegmentMgr*)pContext;
	// dir
	if (fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		WCHAR* ptr = fd->cFileName;
		while (*ptr == '.')
			ptr++;
		if (0 == *ptr)
			return true;
		printf("%S\n", sPath);
		return ProcessDirW(sPath, L"*.*", OnProcessDirHandler, pContext);
	}
	// file
	wcscat((WCHAR*)sPath, L"/aa");
	tReportEvent event = {0};	
	event.m_Timestamp = pReport->m_pHelper->GetLocalTime();
	event.m_Severity = 0;
	event.m_ObjectType = eFile;
	pReport->FindObject(true, eFile, RF_DONT(RF_MAP_SUBST), sPath, cSIZE_WSTR, true, 0, &event.m_ObjectID);
	event.m_Action = evtCreate;
	pReport->AddEvent(&event, sizeof(event), NULL);
	wchar_t sPath2[MAX_PATH*10];
	pReport->GetObjectName(event.m_ObjectID, sPath2, sizeof(sPath2), NULL, RF_DONT(RF_MAP_VOLUME), NULL);
	if (0!=wcscmp(sPath, sPath2))
		__asm int 3;
	return true;	
}
	

