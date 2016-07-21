#ifndef _APART_THREAD_INCLUDED
#define _APART_THREAD_INCLUDED

///////////////////////////////////////////////////////////
//
// CApartThread
//
// Функциональность данного класса заключается 
// 1 - в возможности создания пареллельно выполняющейся функции
// 2 - имеется возможность приостанавливать ее выполнение
// 3 - что-то при этом делать(например устанавливать новые данные)
// 4 - запускать ее снова
// 5 - узнавать из функции о наличии внешнего запроса на остановку 

// 2  состояния
// 1 - работает, 2 - ждет запуска

class CApartThread 
{
public:
	CApartThread();
	virtual ~CApartThread();

	BOOL	IsReady		();
	BOOL	QueryRestart(BOOL bSync=TRUE, BOOL i_bAutoRestart=FALSE);
	BOOL	QueryRestartEx(DWORD i_dwWaitTimeout = INFINITE, BOOL i_bAutoRestart=FALSE);
	BOOL	Restart		(BOOL i_bStopAfterStart=FALSE);
	void	Terminate	(DWORD i_dwWaitTimeout = 0);
	BOOL	Suspend		();
	BOOL	Resume		();
	DWORD	GetThreadId	();
	void	ComInit		();
	BOOL	IsRestartQueried();
	BOOL	WhileNotReady(DWORD i_dwWaitTimeout = INFINITE);
	BOOL	IsInitiatedWell();
	operator HANDLE	();

	BOOL	SetPriority(int i_nPriority);

	DWORD		m_dwDelay;
protected:
	virtual void OnDestroy	(){}
	virtual BOOL OnInit();
	virtual BOOL OnExit();
	virtual void OnRunException() {}

protected:
	BOOL Create();
	BOOL WaitEvent(HANDLE event, DWORD ms=INFINITE);
	BOOL WaitTimeout(DWORD i_dwMsecs);

	virtual void Run(){}

	// Member variables
	DWORD		m_ThreadId;		// ID of thread
	HANDLE		m_hThread;		// Handle to thread

	HANDLE		m_evRestart;
	HANDLE		m_evDoRestart;
	HANDLE		m_evReady;

	LONG		m_WorkCom;
	BOOL		m_bComInit;
	BOOL		m_bInitResult;

	int			m_Priority;

	static DWORD WINAPI RealThreadProc	(void* pv) ;
	DWORD ClassThreadProc					();
};

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\/
inline 	CApartThread::operator HANDLE	()
{
	return m_hThread;
}

inline BOOL CApartThread::WaitEvent(HANDLE event, DWORD ms)
{
	DWORD ret =	WaitForSingleObject(event,ms);
	if(ret==WAIT_OBJECT_0) 
		return TRUE;
	return FALSE;
}

inline BOOL CApartThread::IsRestartQueried()
{
	return WaitEvent(m_evRestart,0); 			
}

inline BOOL CApartThread::WhileNotReady(DWORD i_dwWaitTimeout)
{
	return WaitEvent(m_evReady,i_dwWaitTimeout);
}

inline BOOL  CApartThread::IsReady()
{
	return WaitEvent(m_evReady,0);  
}

inline DWORD CApartThread::GetThreadId()
{
  return m_ThreadId;
}

inline void CApartThread::ComInit()
{ 
	m_bComInit = TRUE; 
}

inline BOOL CApartThread::IsInitiatedWell()
{
	return m_bInitResult;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\/
#endif