// swdrv.cpp : Defines the entry point for the console application.
//
#include <windows.h>

#include "eventmgr\include\eventmgr.h"
#include "pdm\envhelper_win32w.h"

#include "../EventHandler/eh_syswatch.h"
#include "al_file_win.h"

#include "../../../Windows/hook/mklif/mklapi/mklapi.h"
#pragma comment(lib, "mklapi.lib")

LONG g_MemCounter = 0;

void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	if (ptr)
		InterlockedIncrement( &g_MemCounter );
	
	return ptr;
};

void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & g_MemCounter );
};

class cCS : protected CRITICAL_SECTION
{
public:
	cCS()        { InitializeCriticalSection(this); }
	~cCS()       { DeleteCriticalSection(this); }
	void Enter() { EnterCriticalSection(this); }
	void Leave() { LeaveCriticalSection(this); }
};

class cAutoLockerCS
{
public:
	cAutoLockerCS(cCS &cs) { cs.Enter(); m_pCS = &cs; }
	~cAutoLockerCS()       { UnlockAndDetach(); }
	void UnlockAndDetach() { if( cCS *pCS = m_pCS ) { m_pCS = NULL; pCS->Leave(); } }

protected:
	cCS * m_pCS;
};

//////////////////////////////////////////////////////////////////////////
// cSystemWatcherDrv

class cSystemWatcherDrv : public cEHSysWatch
{
public:
	cSystemWatcherDrv() :
		m_eventmgr(&m_envhelper),
		m_driverContext(NULL),
		m_eventHandlerThread(NULL)
	{}
	
	bool Initialize();
	bool WatchToSystem();
	bool BreakWatching();

protected:
	struct cMessageInfo
	{
		PVOID msg;
		ULONG msgSize;
		PMKLIF_EVENT_HDR eventHdr;
	};
	typedef std::vector<cMessageInfo> cMessageInfos;

	struct cEventInfo
	{
		uint64      time;
		tNativePid  pid;
		eEventType  eventType;
		eObjectType objectType;
		tcstring    name;
		tcstring    name2;
		uint16      userId;
		uint32      extraData;
	};
	typedef std::vector<cEventInfo> cEventInfos;

	bool HandleEvent(cMessageInfo &msg, cEventInfo &evi);
	void EventsHandler();
	static DWORD WINAPI EventsHandlerLink(void *ctx) { ((cSystemWatcherDrv *)ctx)->EventsHandler(); return 0; }

protected:
	cEventMgr                   m_eventmgr;
	cEnvironmentHelperWin32W    m_envhelper;

	cCS           m_csEvents;
	cEventInfos   m_events;
	PVOID         m_driverContext;
	HANDLE        m_eventHandlerThread;
	volatile bool m_stopWorking;
};

bool cSystemWatcherDrv::Initialize()
{
	alFileWin32 *objdb = new_alFileWin32();
	if( !objdb )
		return false;
	if( !objdb->CreateFileW(L"db_hashes.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0) )
		return false;

	alFileWin32 *evtdb = new_alFileWin32();
	if( !evtdb )
		return false;
	if( !evtdb->CreateFileW(L"db_events.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0) )
		return false;

	alFileWin32 *procdb = new_alFileWin32();
	if( !procdb )
		return false;
	if( !procdb->CreateFileW(L"db_procs.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0) )
		return false;

	if( !cSystemWatcher::Init(evtdb, objdb, procdb) )
		return false;

	if (!m_eventmgr.RegisterHandler(this, false))
		return false;

	return true;
}

bool cSystemWatcherDrv::WatchToSystem()
{
	if( MKL_ClientRegister(&m_driverContext, AVPG_Driver_Specific, AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0, DEADLOCKWDOG_TIMEOUT, pfMemAlloc, pfMemFree, 0) != S_OK )
		return false;
	
	MKL_AddInvisibleThread(m_driverContext);

	DWORD dwThreadId;
	m_stopWorking = false;
	m_eventHandlerThread = CreateThread(NULL, 0, EventsHandlerLink, this, 0, &dwThreadId);

	if( MKL_AddFilter(NULL, m_driverContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_SYSTEM,
		MJ_CREATE_PROCESS_NOTIFY, 0, 0, PostProcessing, NULL, NULL) != S_OK )
		return false;

	if( MKL_AddFilter(NULL, m_driverContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_SYSTEM,
		MJ_EXIT_PROCESS, 0, 0, PostProcessing, NULL, NULL) != S_OK )
		return false;

	if( MKL_AddFilter(NULL, m_driverContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_NFIOR,
		IRP_MJ_CREATE, 0, 0, PostProcessing, NULL, NULL) != S_OK )
		return false;

	if( MKL_AddFilter(NULL, m_driverContext, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_NFIOR,
		IRP_MJ_WRITE, 0, 0, PostProcessing, NULL, NULL) != S_OK )
		return false;

	cMessageInfo dummyMsg;
	MKL_BuildMultipleWait(m_driverContext, 1);
	MKL_GetMessage(m_driverContext, &dummyMsg.msg, &dummyMsg.msgSize, &dummyMsg.eventHdr);
	MKL_ChangeClientActiveStatus(m_driverContext, TRUE);

	for(cMessageInfo msg; true;)
	{
		if( MKL_GetMultipleMessage(m_driverContext, &msg.msg, &msg.msgSize, &msg.eventHdr, INFINITE) != S_OK )
			break;

		cEventInfo evi;
		bool handled = HandleEvent(msg, evi);

		MKLIF_REPLY_EVENT verdict; memset(&verdict, 0, sizeof(verdict));
		MKL_ReplyMessage(m_driverContext, msg.msg, &verdict);
		MKL_GetMessage(m_driverContext, &dummyMsg.msg, &dummyMsg.msgSize, &dummyMsg.eventHdr);
		pfMemFree(NULL, &msg.msg);

		if( handled )
		{
			cAutoLockerCS locker(m_csEvents);
			m_events.push_back(evi);
		}
	}

	MKL_ClientUnregister(&m_driverContext);

	m_stopWorking = true;
	WaitForSingleObject(m_eventHandlerThread, INFINITE);

	return true;
}

bool cSystemWatcherDrv::BreakWatching()
{
	MKL_ChangeClientActiveStatus(m_driverContext, FALSE);
	MKL_ClientBreakConnection(m_driverContext);
	return true;
}

void cSystemWatcherDrv::EventsHandler()
{
	MKL_AddInvisibleThread(m_driverContext);

	for(cEventInfo evi; !m_stopWorking;)
	{
		{
			cAutoLockerCS locker(m_csEvents);
			if( m_events.empty() )
			{
				locker.UnlockAndDetach();
				Sleep(25);
				continue;
			}
			evi = m_events.front();
			m_events.erase(m_events.begin());
		}
		
		cSystemWatcher::AddEvent(evi.time, evi.pid, evi.eventType, evi.objectType,
			evi.name,  evi.name  ? tstrlenbz(evi.name)  : 0,
			evi.name2, evi.name2 ? tstrlenbz(evi.name2) : 0,
			evi.userId, evi.extraData);

		if( evi.name )
			tstrfree(evi.name);

		if( evi.name2 )
			tstrfree(evi.name2);
	}
}

bool cSystemWatcherDrv::HandleEvent(cMessageInfo &msg, cEventInfo &evi)
{
	cEvent event(0, msg.eventHdr->m_ProcessId);
	evi.time = event.GetTime();
	evi.pid = event.m_pid;
	evi.eventType = evtUnknown;
	evi.objectType = objUnknown;
	evi.name = NULL;
	evi.name2 = NULL;
	evi.userId = 0;
	evi.extraData = 0;

	if( msg.eventHdr->m_HookID == FLTTYPE_SYSTEM )
	{
		switch(msg.eventHdr->m_FunctionMj)
		{
		case MJ_CREATE_PROCESS_NOTIFY:
			{
				ULONG Pid = 0;
				PSINGLE_PARAM pParamPid = MKL_GetEventParam(msg.msg, msg.msgSize, _PARAM_OBJECT_SOURCE_ID, FALSE);
				if (pParamPid)
					Pid = *(ULONG*) pParamPid->ParamValue;

				PWCHAR pwchPath = NULL;
				PSINGLE_PARAM pParamUrl = MKL_GetEventParam(msg.msg, msg.msgSize, _PARAM_OBJECT_URL_W, FALSE);
				if (pParamUrl)
					pwchPath = (PWCHAR) pParamUrl->ParamValue;

				PWCHAR pwchFolder = NULL;
				PSINGLE_PARAM pParamFolder = MKL_GetEventParam(msg.msg, msg.msgSize, _PARAM_OBJECT_BASEPATH, FALSE);
				if (pParamFolder)
					pwchFolder = (PWCHAR) pParamFolder->ParamValue;

				PWCHAR pwchCmd = NULL;
				PSINGLE_PARAM pParamCmd = MKL_GetEventParam(msg.msg, msg.msgSize, _PARAM_OBJECT_STARTUP_STR, FALSE);
				if (pParamCmd)
					pwchCmd = (PWCHAR) pParamCmd->ParamValue;

				printf("create process pid %d (parent %d) from '%S'\n", Pid, msg.eventHdr->m_ProcessId, pwchPath);

				if (pwchFolder)
					printf("\tfolder: '%S'\n", pwchFolder);

				if (pwchCmd)
					printf("\tCmdLine: '%S'\n", pwchCmd);

				cFile image_path( &m_envhelper, pwchPath, pwchPath );
				cPath working_folder(&m_envhelper, pwchFolder, pwchFolder);

				evi.eventType = evtProcessStart;
				evi.objectType = objFile;
				evi.extraData = Pid;
				evi.name = tstrdup(image_path.getFull());

				return true;
			}

		case MJ_EXIT_PROCESS:
			{
 				PWCHAR pwchPath = NULL;
 				HRESULT hResult = MKL_GetProcessPath(m_driverContext, msg.msg, &pwchPath);
				
				printf("exit process pid %d, '%S'\n", msg.eventHdr->m_ProcessId, pwchPath);

				cFile image_path(&m_envhelper, pwchPath, pwchPath);

				evi.eventType = evtProcessStop;
				evi.objectType = objFile;
				evi.name = tstrdup(image_path.getFull());

				pfMemFree(NULL, (void**)&pwchPath);
				
				return true;
			}
		}		
	}

	if( msg.eventHdr->m_HookID == FLTTYPE_NFIOR )
	{
		PWCHAR pwchPath = NULL;
		PSINGLE_PARAM pParamUrl = MKL_GetEventParam(msg.msg, msg.msgSize, _PARAM_OBJECT_URL_W, FALSE);
		if (pParamUrl)
			pwchPath = (PWCHAR)pParamUrl->ParamValue;

		if( msg.eventHdr->m_FunctionMj == IRP_MJ_CREATE )
		{
			PSINGLE_PARAM pParamCtx = MKL_GetEventParam(msg.msg, msg.msgSize, _PARAM_OBJECT_CONTEXT_FLAGS, FALSE);
			bool fileCreated = pParamCtx && (*(ULONG *)pParamCtx->ParamValue & _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT);
			if( !fileCreated )
				return false;
			printf("file is created. pid %d, %S\n", msg.eventHdr->m_ProcessId, pwchPath);
			evi.eventType = evtCreate;
		}
		
		if( msg.eventHdr->m_FunctionMj == IRP_MJ_WRITE )
		{
			printf("file is modified. pid %d, %S\n", msg.eventHdr->m_ProcessId, pwchPath);
			evi.eventType = evtModify;
		}

		cFile image_path(&m_envhelper, pwchPath, pwchPath);

		evi.objectType = objFile;
		evi.name = tstrdup(image_path.getFull());
		
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// main

static cSystemWatcherDrv *g_pSW = NULL;

BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
	if( g_pSW )
		g_pSW->BreakWatching();
	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cSystemWatcherDrv sw;
	if( !sw.Initialize() )
		printf("Error initializing system watcher\n");

	printf("Initializing done, start watching to system...\n");
	printf("To stop watching press Ctrl+C\n");

	g_pSW = &sw;
	SetConsoleCtrlHandler(CtrlHandler, TRUE);

	if( !sw.WatchToSystem() )
		printf("Error during watching to system\n");
}