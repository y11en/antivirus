// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  30 March 2005,  16:07 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- avspm
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- avspm.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Interface version,  )
#define AVSPM_VERSION ((tVERSION)1)
// AVP Prague stamp end

#pragma warning(disable: 4786)

#define _WIN32_WINNT 0x0500

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>

#include <AV/plugin/p_avspm.h>
#include <AV/structs\s_avs.h>
// AVP Prague stamp end

#include "../avs.h"

#include <windows.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <psapi.h>
#include <pdh.h>
#include <list>


#define THREAD_WAIT_TIMEOUT	        1000

#define CHECK_GLOBAL_MEMORY_COUNTER	1
#define RESET_WORKING_SET_TIMEOUT	30

#define BIG_WORKING_MEM_USAGE		0x6400000
#define MAX_WORKING_MEM_USAGE		0xA00000
#define MIN_WORKING_PERCENT			1

#define TOO_LOW_GLOBAL_MEM_AVAIL    0x100000

#define THREAD_EXCEPT_TIMEOUT		30000

struct AVSThreadInfo
{
	AVSThreadInfo(tDWORD nThreadId) :
		m_nThreadId(nThreadId), m_hDupThread(NULL), m_pEngine(0){}

	tDWORD	m_nThreadId;
	tDWORD	m_nYieldTime;
	tPTR	m_pEngine;

	HANDLE	m_hDupThread;
};
typedef std::list<AVSThreadInfo> tThreadsList;


static struct cPMAPI
{
	cPMAPI()
	{
		memset(this, 0, sizeof(cPMAPI));

		m_hPSApi  = LoadLibrary("psapi.dll");
		HMODULE hNTDll = GetModuleHandle("ntdll.dll");
		HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
		HMODULE hUser32 = GetModuleHandle("user32.dll");

		if( m_hPSApi )
			*(void**)&m_fnGetProcessMemoryInfo = GetProcAddress(m_hPSApi, "GetProcessMemoryInfo");

		if( hNTDll )
			*(void**)&m_fnNtQuerySystemInformation = GetProcAddress(hNTDll, "NtQuerySystemInformation");

		if( hKernel32 )
		{
			*(void**)&m_fnGetProcessTimes = GetProcAddress(hKernel32, "GetProcessTimes");
			*(void**)&m_fnGetThreadTimes = GetProcAddress(hKernel32, "GetThreadTimes");
			*(void**)&m_fnGlobalMemoryStatusEx = GetProcAddress(hKernel32, "GlobalMemoryStatusEx");
			*(void**)&m_fnCreateToolhelp32Snapshot = GetProcAddress(hKernel32, "CreateToolhelp32Snapshot");
			*(void**)&m_fnThread32First = GetProcAddress(hKernel32, "Thread32First");
			*(void**)&m_fnThread32Next = GetProcAddress(hKernel32, "Thread32Next");
			*(void**)&m_fnOpenThread = GetProcAddress(hKernel32, "OpenThread");
		}

		if( hUser32 )
			*(void**)&m_fnGetLastInputInfo = GetProcAddress(hUser32, "GetLastInputInfo");

		SYSTEM_INFO si;
		GetSystemInfo(&si);
		m_nProcessorsNum = si.dwNumberOfProcessors;

		m_pPPPI = new SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[m_nProcessorsNum];
		InitializeCriticalSection(&m_lock);
	}

	~cPMAPI()
	{
		if( m_pPPPI )
			delete[] m_pPPPI;

		if( m_hPSApi )
			FreeLibrary(m_hPSApi);

		InitIdleThreadsList(true);
		DeleteCriticalSection(&m_lock);
	}

	tDWORD GetWorkingSetSize()
	{
		PROCESS_MEMORY_COUNTERS sMemInfo;
		if( !m_fnGetProcessMemoryInfo || !m_fnGetProcessMemoryInfo(GetCurrentProcess(), &sMemInfo, sizeof(sMemInfo)) )
			return 0;

		return sMemInfo.WorkingSetSize;
	}

	tQWORD GetAvailMemorySize()
	{
		if( m_fnGlobalMemoryStatusEx )
		{
			MEMORYSTATUSEX ms; ms.dwLength = sizeof(MEMORYSTATUSEX);
			if( m_fnGlobalMemoryStatusEx(&ms) )
				return ms.ullAvailPageFile;
		}

		MEMORYSTATUS ms; ms.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&ms);
		return ms.dwAvailPageFile;
	}

	tDWORD GetProcessorUsage()
	{
		if( !m_fnNtQuerySystemInformation || !m_fnGetProcessTimes )
			return 0;

		LONGLONG qwTemp;
		DWORD nTotalPersent, nProcessPersent, nCurTicks, nDiff;

		EnterCriticalSection(&m_lock);

		nCurTicks = GetTickCount();
		nDiff = (nCurTicks - m_nLastRequest)*100;
		m_nLastRequest = nCurTicks;

		if( !nDiff )
		{
			LeaveCriticalSection(&m_lock);
			return 0;
		}

		FILETIME tmDummy, tmUserTime, tmKernelTime;
		m_fnGetProcessTimes(GetCurrentProcess(), &tmDummy, &tmDummy, &tmDummy, &tmUserTime);

		qwTemp = (((LONGLONG)tmUserTime.dwHighDateTime << 32) | tmUserTime.dwLowDateTime)/m_nProcessorsNum;
		nProcessPersent = (DWORD)((qwTemp - m_qwProcessorProcess) / nDiff);
		m_qwProcessorProcess = qwTemp;

		m_fnNtQuerySystemInformation(SystemProcessorPerformanceInformation, 
			m_pPPPI, sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)*m_nProcessorsNum, NULL);

		qwTemp = 0;
		for(DWORD i = 0; i < m_nProcessorsNum; i++)
			qwTemp += m_pPPPI[i].IdleTime.QuadPart;

		qwTemp /= m_nProcessorsNum;
		nTotalPersent = 100 - (DWORD)((qwTemp - m_qwProcessorTotal) / nDiff);
		m_qwProcessorTotal = qwTemp;

		if( m_nIdleThreadsNum )
		{
			qwTemp = 0;
			for(DWORD i = 0; i < m_nIdleThreadsNum; i++)
			{
				m_fnGetThreadTimes(m_hIdleThreads[i], &tmDummy, &tmDummy, &tmKernelTime, &tmUserTime);

				qwTemp += ((((LONGLONG)tmUserTime.dwHighDateTime + tmKernelTime.dwHighDateTime) << 32) |
					(tmKernelTime.dwLowDateTime + tmUserTime.dwLowDateTime))/m_nProcessorsNum;
			}
			nTotalPersent -= (DWORD)((qwTemp - m_qwProcessorThreads) / nDiff);
			m_qwProcessorThreads = qwTemp;
		}

		nTotalPersent -= nProcessPersent;
		if( nTotalPersent > (90 / m_nProcessorsNum) )
		{
			if( ++m_nIdleCheckCounter == 10 )
				InitIdleThreadsList(false);
		}
		else
			m_nIdleCheckCounter = 0;

		LeaveCriticalSection(&m_lock);
		return nTotalPersent;
	}

	void InitIdleThreadsList(bool bReset)
	{
		if( !m_fnCreateToolhelp32Snapshot || !m_fnOpenThread )
			return;

		for(DWORD i = 0; i < m_nIdleThreadsNum; i++)
			if( m_hIdleThreads[i] )
				CloseHandle(m_hIdleThreads[i]);

		m_nIdleThreadsNum = 0;
		m_nIdleCheckCounter = 0;

		if( bReset )
			return;

		HANDLE hSnap = m_fnCreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
		if( hSnap == INVALID_HANDLE_VALUE )
			return;

		THREADENTRY32 th32;
		th32.dwSize = sizeof(THREADENTRY32);
		for(BOOL res = m_fnThread32First(hSnap, &th32); res; res = m_fnThread32Next(hSnap, &th32) )
		{
			if( th32.tpBasePri != 1 )
				continue;

			HANDLE hThread = m_fnOpenThread(THREAD_QUERY_INFORMATION, FALSE, th32.th32ThreadID);
			if( hThread )
			{
				m_hIdleThreads[m_nIdleThreadsNum++] = hThread;
				if( m_nIdleThreadsNum == 10 )
					break;
			}
		}

		CloseHandle(hSnap);
	}

	LONG GetLastUserInputPeriod()
	{
		if( !m_fnGetLastInputInfo )
			return false;

		LASTINPUTINFO lif = {sizeof(LASTINPUTINFO)};
		m_fnGetLastInputInfo(&lif);
		
		LONG dwDiff = GetTickCount() - m_nLastUserInput;
		m_nLastUserInput = lif.dwTime;

		return dwDiff;
	}

private:
	HMODULE  m_hPSApi;
	DWORD    m_nLastUserInput;
	DWORD    m_nLastRequest;
	DWORD    m_nProcessorsNum;
	LONGLONG m_qwProcessorTotal;
	LONGLONG m_qwProcessorProcess;
	LONGLONG m_qwProcessorThreads;
	PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION m_pPPPI;

	CRITICAL_SECTION m_lock;
	DWORD    m_nIdleCheckCounter;
	DWORD    m_nIdleThreadsNum;
	HANDLE   m_hIdleThreads[10];

	BOOL     (WINAPI* m_fnGetProcessMemoryInfo)(HANDLE Process, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb);
	BOOL     (WINAPI* m_fnGlobalMemoryStatusEx)(LPMEMORYSTATUSEX lpBuffer);
	BOOL     (WINAPI* m_fnGetProcessTimes)(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime);
	BOOL     (WINAPI* m_fnGetThreadTimes)(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime);
	BOOL     (WINAPI* m_fnGetLastInputInfo)( PLASTINPUTINFO plii);
	NTSTATUS (WINAPI* m_fnNtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

	HANDLE   (WINAPI* m_fnCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);
	BOOL     (WINAPI* m_fnThread32First)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
	BOOL     (WINAPI* m_fnThread32Next)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
	HANDLE   (WINAPI* m_fnOpenThread)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);

} g_PSApi;


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable AVSPMImpl : public cAVSPM 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations

public:
// External function declarations
	tERROR pr_call ProcessBegin(tPTR engine);
	tERROR pr_call ProcessEnd();
	tERROR pr_call ProcessYield();
	tERROR pr_call GetPerformance( cSerializable* p_stat );
	tERROR pr_call GetProcessorUsage( tDWORD* p_usage );

// Data declaration
// AVP Prague stamp end

	ULONG  DoWork();
	void   CheckThread(AVSThreadInfo *pInfo);
	void   DetachThread();

	static ULONG WINAPI WorkingThread(PVOID pArg){ return ((AVSPMImpl*)pArg)->DoWork(); }

private:
	AVSImpl *        m_pAvs;
	HANDLE		     m_hThread;
	HANDLE		     m_hStop;
	tDWORD			 m_tlsThreads;
	CRITICAL_SECTION m_lock;
	tThreadsList	 m_Threads;

	tBOOL            m_bLowMemAvail;
	cAVSPerformance	 m_Performance;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(AVSPMImpl)
};
// AVP Prague stamp end

AVSPMImpl * g_avspm = NULL;

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AVSPM". Static(shared) property table variables
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR AVSPMImpl::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSPM::ObjectInitDone method" );

	// Place your code here
	InitializeCriticalSection(&m_lock);
	m_tlsThreads = TlsAlloc();

	if( g_avspm )
	{
		PR_TRACE( (this, prtALWAYS_REPORTED_MSG, "AVSPM already exists") );
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	m_pAvs = (AVSImpl*)sysGetParent(IID_ANY);

	m_hStop = CreateEvent(0, TRUE, FALSE, NULL);

	DWORD nThreadId;
	m_hThread = CreateThread(NULL, 0, WorkingThread, this, 0, &nThreadId);

	g_PSApi.GetProcessorUsage();
	g_avspm = this;
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR AVSPMImpl::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSPM::ObjectPreClose method" );

	// Place your code here
	g_avspm = NULL;

	if( m_hThread )
	{
		SetEvent(m_hStop);
		WaitForSingleObject(m_hThread, INFINITE);

		CloseHandle(m_hStop);
		CloseHandle(m_hThread);
		m_hThread = m_hStop = NULL;
	}

	DeleteCriticalSection(&m_lock);
	TlsFree(m_tlsThreads);
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessBegin )
// Parameters are:
tERROR AVSPMImpl::ProcessBegin(tPTR engine)
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSPM::ProcessBegin method" );

	// Place your code here
	AVSThreadInfo * pThreadInfo = (AVSThreadInfo*)TlsGetValue(m_tlsThreads);

	EnterCriticalSection(&m_lock);
	if( !pThreadInfo )
	{
		m_Threads.push_back(GetCurrentThreadId());
		pThreadInfo = &*m_Threads.rbegin();

		DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &pThreadInfo->m_hDupThread,
			THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_TERMINATE, FALSE, 0);

		TlsSetValue(m_tlsThreads, pThreadInfo);
	}

	pThreadInfo->m_nYieldTime = GetTickCount();
	pThreadInfo->m_pEngine = engine;
	LeaveCriticalSection(&m_lock);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessEnd )
// Parameters are:
tERROR AVSPMImpl::ProcessEnd()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSPM::ProcessEnd method" );

	// Place your code here
	AVSThreadInfo * pThreadInfo = (AVSThreadInfo*)TlsGetValue(m_tlsThreads);

	EnterCriticalSection(&m_lock);
	if( pThreadInfo )
		pThreadInfo->m_pEngine = NULL;
	LeaveCriticalSection(&m_lock);
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessYield )
// Parameters are:
tERROR AVSPMImpl::ProcessYield()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSPM::ProcessYield method" );

	// Place your code here
	AVSThreadInfo * pThreadInfo = (AVSThreadInfo*)TlsGetValue(m_tlsThreads);
	if( pThreadInfo )
		pThreadInfo->m_nYieldTime = GetTickCount();

//	if( m_bLowMemAvail )
//		return errNOT_ENOUGH_MEMORY;

	return error;
}
// AVP Prague stamp end

void AVSPMImpl::DetachThread()
{
	AVSThreadInfo * pThreadInfo = (AVSThreadInfo*)TlsGetValue(m_tlsThreads);
	if( !pThreadInfo )
		return;

	EnterCriticalSection(&m_lock);
	for(tThreadsList::iterator it = m_Threads.begin(); it != m_Threads.end(); ++it)
	{
		AVSThreadInfo * itInfo = &*it;
		if( itInfo == pThreadInfo )
		{
			if( itInfo->m_hDupThread )
				CloseHandle(itInfo->m_hDupThread);

			m_Threads.erase(it);
			break;
		}
	}
	LeaveCriticalSection(&m_lock);
}

// AVP Prague stamp begin( External (user called) interface method implementation, GetPerformance )
// Parameters are:
//! tERROR (null)::GetPerformance( cSerializable* p_stat )
tERROR AVSPMImpl::GetPerformance( cSerializable* p_stat )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSPM::GetPerformance method" );

	// Place your code here
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetProcessorUsage )
// Parameters are:
tERROR AVSPMImpl::GetProcessorUsage( tDWORD* p_usage )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSPM::GetProcessorUsage method" );

	// Place your code here
	*p_usage = g_PSApi.GetProcessorUsage();

	if( g_PSApi.GetLastUserInputPeriod() > 5000 )
		return warnFALSE;

	return error;
}
// AVP Prague stamp end

__declspec(naked) void ThreadException()
{
	__asm { int 3 }
}

void AVSPMImpl::CheckThread(AVSThreadInfo *pInfo)
{
	for(int i = 0; i < 10; i++)
	{
		if( GetTickCount() - pInfo->m_nYieldTime < THREAD_EXCEPT_TIMEOUT )
			return;

		if( !pInfo->m_pEngine || !pInfo->m_hDupThread )
			return;

		PR_TRACE((this, prtALWAYS_REPORTED_MSG, "AVS\tno yield on thread\t%03X", pInfo->m_nThreadId));

		g_root->LockKernel();
		if( SuspendThread(pInfo->m_hDupThread) != -1 )
		{
			CONTEXT Context;
			Context.ContextFlags = CONTEXT_CONTROL | CONTEXT_DEBUG_REGISTERS;
			if( GetThreadContext(pInfo->m_hDupThread, &Context) )
				if( PR_SUCC(((CAVSEngine*)pInfo->m_pEngine)->CheckEIP((tPTR)Context.Eip)) )
				{
					Context.Dr0 = Context.Eip; // assign eip to Dr0
					Context.Eip = 0xdead;//(DWORD)ThreadException;
					Context.ContextFlags = CONTEXT_CONTROL | CONTEXT_DEBUG_REGISTERS;
					SetThreadContext(pInfo->m_hDupThread, &Context);
				}

			ResumeThread(pInfo->m_hDupThread);
			Sleep(10);
		}
		g_root->UnlockKernel();
	}
}

ULONG  AVSPMImpl::DoWork()
{
	for(tDWORD dwCounter = 1; ; dwCounter++)
	{
		DWORD dwWaitResult = WaitForSingleObject(m_hStop, THREAD_WAIT_TIMEOUT);
		if( dwWaitResult == WAIT_FAILED || dwWaitResult == WAIT_OBJECT_0 )
			break;

		if( !(dwCounter % CHECK_GLOBAL_MEMORY_COUNTER) )
		{
			tQWORD qwMemSize = g_PSApi.GetAvailMemorySize();
			m_bLowMemAvail = (qwMemSize < TOO_LOW_GLOBAL_MEM_AVAIL);
		}

		if( !(dwCounter % RESET_WORKING_SET_TIMEOUT) )
		{
			tDWORD nWorkingSet = g_PSApi.GetWorkingSetSize();

			if( nWorkingSet > BIG_WORKING_MEM_USAGE )
				PR_TRACE((this, prtALWAYS_REPORTED_MSG, "Memory usage is too big: %d", nWorkingSet));
			
			m_pAvs->GetPerformance(&m_Performance);
			if( m_Performance.m_nPerformance < MIN_WORKING_PERCENT )
			{
				if( nWorkingSet > MAX_WORKING_MEM_USAGE )
				{
					SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
					PR_TRACE((this, prtALWAYS_REPORTED_MSG, "WorkingSet has been reset"));
				}
			}

			EnterCriticalSection(&m_lock);

			for(tThreadsList::iterator it = m_Threads.begin(); it != m_Threads.end(); ++it)
				CheckThread(&*it);

			LeaveCriticalSection(&m_lock);
		}
	}
	return 0;
}


// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AVSPM". Register function
tERROR AVSPMImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AVSPM)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(AVSPM)
// AVP Prague stamp end

// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AVSPM)
	mEXTERNAL_METHOD(AVSPM, ProcessBegin)
	mEXTERNAL_METHOD(AVSPM, ProcessEnd)
	mEXTERNAL_METHOD(AVSPM, ProcessYield)
	mEXTERNAL_METHOD(AVSPM, GetPerformance)
	mEXTERNAL_METHOD(AVSPM, GetProcessorUsage)
mEXTERNAL_TABLE_END(AVSPM)
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "AVSPM::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_AVSPM,                              // interface identifier
		PID_AVSPM,                              // plugin identifier
		0x00000000,                             // subtype identifier
		AVSPM_VERSION,                          // interface version
		VID_MEZHUEV,                            // interface developer
		&i_AVSPM_vtbl,                          // internal(kernel called) function table
		(sizeof(i_AVSPM_vtbl)/sizeof(tPTR)),    // internal function table size
		&e_AVSPM_vtbl,                          // external function table
		(sizeof(e_AVSPM_vtbl)/sizeof(tPTR)),    // external function table size
		NULL,                                   // property table
		0,                                      // property table size
		sizeof(AVSPMImpl)-sizeof(cObjImpl),     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"AVSPM(IID_AVSPM) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AVSPM_Register( hROOT root ) { return AVSPMImpl::Register(root); }
// AVP Prague stamp end

void DetachThread(){ if( g_avspm ) g_avspm->DetachThread(); }

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end

