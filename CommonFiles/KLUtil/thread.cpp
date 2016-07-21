#include "StdAfx.h"
#include "Thread.h"
#include <process.h>
#include <assert.h>

void KLUTIL::SetDebuggerThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{
#ifdef _DEBUG
	// See MSDN (Setting a Thread Name (Unmanaged))
	struct tagTHREADNAME_INFO
	{
		DWORD dwType; // must be 0x1000
		LPCSTR szName; // pointer to name (in user addr space)
		DWORD dwThreadID; // thread ID (-1=caller thread)
		DWORD dwFlags; // reserved for future use, must be zero
	} info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
	
	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (DWORD*)&info);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
// KLUTIL::CThread
//////////////////////////////////////////////////////////////////////////

KLUTIL::CThread::CThread()
	: m_nThreadId(0)
	, m_hThread(NULL)
{
	m_pThis.reset(this);
}

KLUTIL::CThread::~CThread()
{
	if (m_hThread != NULL)
		CloseHandle(m_hThread);
}

bool KLUTIL::CThread::CreateThread(DWORD dwCreateFlags,
								   UINT nStackSize,
								   LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
	m_hThread = reinterpret_cast<HANDLE>(
		_beginthreadex(lpSecurityAttrs, nStackSize, RunFunc, this, dwCreateFlags, &m_nThreadId));
	return m_hThread != NULL;
}

unsigned __stdcall KLUTIL::CThread::RunFunc(void* pThis)
{
	CThread* _this = static_cast<CThread*>(pThis);
	unsigned result = _this->Run();
	if (_this->m_pThis)
		_this->m_pThis.reset();
	else
		delete _this;
	return result;
}

KLUTIL::CThreadPtr KLUTIL::CThread::GetThisSharedPtr()
{
	if (!m_pThis)
		m_pThis.reset(this);
	return m_pThis;
}

void KLUTIL::CThread::SetThisSharedPtr(KLUTIL::CThreadPtr p)
{
	assert(!m_pThis); // ”казатель уже установлен
	m_pThis = p;
}

//////////////////////////////////////////////////////////////////////////
// KLUTIL::CGuiThread
//////////////////////////////////////////////////////////////////////////

unsigned KLUTIL::CGuiThread::Run()
{
	if (InitInstance())
	{
		MSG msg;
		BOOL bRet;
		while ((bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
		{ 
			if (bRet == -1)
			{
				// handle the error and possibly exit
			}
			else
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
		}
	}
	
	return ExitInstance();
}

BOOL KLUTIL::CGuiThread::InitInstance()
{
	return TRUE;
}

unsigned KLUTIL::CGuiThread::ExitInstance()
{
	return 0;
}