#include <windows.h>

#include "eventmgr\include\eventmgr.h"
#include "pdm\envhelper_win32w.h"

#include "../EventHandler/eh_syswatch.h"
#include "al_file_win.h"


class cEHSysWatchWin : public cEHSysWatch
{
	bool GetProcessInfo(tNativePid native_pid, uint64* pstart_time, tNativePid* pparent_pid)
	{ 
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)native_pid);
		if (!hProcess)
			return false;
		FILETIME ftCreated;
		FILETIME ftDummy;
		if (GetProcessTimes(hProcess, &ftCreated, &ftDummy, &ftDummy, &ftDummy))
		{
			if (pstart_time)
				*pstart_time = *(uint64*)&ftCreated;
		}
		CloseHandle(hProcess);
		return false; 
	};
};

void main()
{
	alAutoRelease<alFileWin32*> objdb = new_alFileWin32();
	if (!objdb)
		return;
	if (!objdb->CreateFileW(_T("db_hashes.dat"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0))
		return;
	alAutoRelease<alFileWin32*> evtdb = new_alFileWin32();
	if (!evtdb)
		return;
	if (!evtdb->CreateFileW(_T("db_events.dat"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0))
		return;
	alAutoRelease<alFileWin32*> procdb = new_alFileWin32();
	if (!procdb)
		return;
	if (!procdb->CreateFileW(_T("db_procs.dat"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0))
		return;
	cEHSysWatchWin syswatch;
	if (!syswatch.cSystemWatcher::Init(evtdb, objdb, procdb))
		return;
	cEnvironmentHelperWin32W envhelper_win32;
	cEventMgr eventmgr(&envhelper_win32);
	if (!eventmgr.RegisterHandler(&syswatch, false))
		return;

	FILETIME time;
	GetSystemTimeAsFileTime(&time);

	cEvent event(0, GetCurrentProcessId());
	cFile image_path(&envhelper_win32, _T("c:\\runme.exe"));
	cPath working_folder(&envhelper_win32, _T("c:\\"));
	eventmgr.OnProcessCreatePost(event, 0, image_path, working_folder, _T("c:\\runme.exe"), 0);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(_T("c:\\windows\\system32\\*.*"), &fd);
	if (hFind)
	{
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			GetSystemTimeAsFileTime(&time);
			syswatch.AddEvent(*(uint64*)&time, GetCurrentProcessId(), evtCreate, objFile, fd.cFileName, (_tcslen(fd.cFileName)+1)*sizeof(fd.cFileName[0]), 0, 0); 
		} while(FindNextFile(hFind, &fd));
		FindClose(hFind);
	}

	PEVENT_FIND_DATA pFind = syswatch.FindFirstEvent();
	if (pFind)
	{
		HANDLE hFind = FindFirstFile(_T("c:\\windows\\system32\\*.*"), &fd);
		if (hFind)
		{
			do {
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					continue;
				TCHAR szFile[MAX_PATH];
				assert(syswatch.GetHashData(pFind->ObjId, 0, szFile, _countof(szFile), 0, 0, 0, 0));
				assert(0==_tcscmp(szFile, fd.cFileName));
				syswatch.FindNextEvent(pFind);
			} while(FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
		syswatch.FindEventClose(pFind);
	}
}