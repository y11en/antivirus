#include "stdafx.h"
#include "mchandleops.h"

HANDLE_EX::HANDLE_EX()
{
	hHandle = NULL;
	hInitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	assert(hInitEvent);

	hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	assert(hStopEvent);

}

HANDLE_EX::~HANDLE_EX()
{
	::CloseHandle(hInitEvent);
	::CloseHandle(hStopEvent);
}

const HANDLE_EX& HANDLE_EX::operator=(HANDLE_EX& handle)
{
	hHandle = handle.hHandle;
	if(hHandle != NULL)
	{
		::SetEvent(hInitEvent);
	}else
	{
		::SetEvent(hStopEvent);
	}
	return *this;
}

const HANDLE_EX& HANDLE_EX::operator=(HANDLE handle)
{
	hHandle = handle;

	if(hHandle != NULL)
	{
		::SetEvent(hInitEvent);
	} else
	{
		::SetEvent(hStopEvent);
	}
	return *this;
}

HANDLE_EX::operator HANDLE() const
{
	return hHandle;
}

HANDLE HANDLE_EX::GetInitializeEvent()
{
	return hInitEvent;
}

HANDLE HANDLE_EX::GetStopEvent()
{
	return hStopEvent;
}

bool CloseValidHandle(HANDLE &hHandle)
{
	if( IS_VALID_HANDLE(hHandle) ) 
	{
		bool bRes = (CloseHandle(hHandle) != 0);
		hHandle = NULL;

		return bRes;
	}

	return false;
}

//! \fn				: IsThreadAlive
//! \brief			: Определяет статус потока 	
//! \return			: bool 
//! \param          : void *hHandle
bool IsThreadAlive(HANDLE hHandle) 
{	
	if( IS_VALID_HANDLE(hHandle) )
	{
		DWORD dwExitCode = STILL_ACTIVE; 
		if ( GetExitCodeThread(hHandle, &dwExitCode) )
		{
			PR_TRACE( (NULL, MC_TRACE_LEVEL, "IsThreadAlive => Thread returned %d", dwExitCode) );
			return dwExitCode == STILL_ACTIVE;
		}
		else
			TRACE_LASTERROR( "IsThreadAlive => GetExitCodeThread failed" );
	}

	PR_TRACE( (NULL, MC_TRACE_LEVEL, "IsThreadAlive => Invalid handle of the thread: 0x%X", hHandle) );
	return false;
}

//static COwnSync g_ownsync;

//! \fn				: StopThread
//! \brief			: Останавливает поток (в том числе и аварийно)
//! \return			: bool
//! \param          : HANDLE &hThread
//! \param          : DWORD dwMSec
bool StopThread(HANDLE &hThread, DWORD dwMSec)
{	
//	CAutoLock cs(g_ownsync);
	PR_TRACE( (NULL, MC_TRACE_LEVEL, "StopThread => : wait for 0x%X", hThread) );
	
	if( IsThreadAlive(hThread) )
	{
		if( WaitForSingleObject(hThread, dwMSec) == WAIT_TIMEOUT ) 
			TerminateThread(hThread, -1); // return false; //TerminateThread(hThread, -1);

		return CloseValidHandle(hThread);		
	}

	return true;
}