#ifndef _MAILCHECKER_HANDLES_OPERATIONS_H_
#define _MAILCHECKER_HANDLES_OPERATIONS_H_

#include <assert.h>

class HANDLE_EX
{
public:
	HANDLE_EX();
	virtual ~HANDLE_EX();

	const HANDLE_EX& operator=(HANDLE_EX& handle);
	const HANDLE_EX& operator=(HANDLE handle);
	operator HANDLE() const;

	HANDLE GetInitializeEvent();
	HANDLE GetStopEvent();

protected:
	HANDLE hHandle;
	HANDLE hInitEvent;
	HANDLE hStopEvent;
};

bool CloseValidHandle(HANDLE &hHandle);

//! \fn				: IsThreadAlive
//! \brief			: Определяет статус потока 	
//! \return			: bool 
//! \param          : void *hHandle
bool IsThreadAlive(HANDLE hHandle); 

//! \fn				: StopThread
//! \brief			: Останавливает поток (в том числе и аварийно)
//! \return			: void 
//! \param          : HANDLE &hThread
//! \param          : DWORD dwMSec
bool StopThread(HANDLE &hThread, DWORD dwMSec = INFINITE /*WAITSERVER_TIMEOUT*/);

#endif