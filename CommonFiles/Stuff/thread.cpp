////////////////////////////////////////////////////////////////////
//
//  thread.cpp
//  Simple Win32 thread implementation
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <tchar.h>
#include <process.h>
#include <stdlib.h>
#include <Stuff\thread.h>
#include <Stuff\cpointer.h>

//----------------------------------------------------------------------------
// Semaphores Win32
//

//----------------------------------------
// CMutex WIN32

CMutex::~CMutex()
{
	Release();
}

void CMutex::Release()
{
  ::ReleaseMutex(*this);
}

//----------------------------------------
// CCountedSemaphore WIN32

CCountedSemaphore::~CCountedSemaphore()
{
	Release();
}

void CCountedSemaphore::Release()
{
  ::ReleaseSemaphore(*this, 1, 0);  
}

//----------------------------------------
// CEventSemaphore WIN32

CEventSemaphore::~CEventSemaphore()
{
	Release();
}

void CEventSemaphore::Release()
{
  // Events don't need to be released
}

//----------------------------------------
// CSemaphoreSet & its CLock for Win32

//
CSemaphoreSet::CSemaphoreSet(const CASemaphore* sems[], int size)
:
  Sems(0)
{
  int count = 0;
  if (sems)
    while (sems[count])
      count++;
  Count = count;
  Size = size >= 0 ? size : count;
  if (Size) {
    Sems = new CSemaphorePtr[Size];
    int i = 0;
    if (sems)
      for (; i < Count; i++)
        Sems[i] = const_cast<CASemaphore *>(sems[i]);
    for (; i < Size; i++)
      Sems[i] = 0;
  }
}

//
CSemaphoreSet::~CSemaphoreSet()
{
  delete[] Sems;
}

//
void CSemaphoreSet::Add(const CASemaphore& sem)
{
  if (Count < Size)
    Sems[Count++] = const_cast<CASemaphore *>(&sem);
}

//
void CSemaphoreSet::Remove(const CASemaphore& sem)
{
  _ASSERT(Count <= Size);
  for (int i = 0; i < Count; i++)
    if (Sems[i] == &sem) {
      for (int j = i; j < Count-1; j++)  // Shift rest down to keep packed
        Sems[j] = Sems[j+1];
      Sems[Count-1] = 0;
      return;
    }
}

void CSemaphoreSet::Release(int index)
{
  if (index >= 0)
    const_cast<CASemaphore*>(Sems[index])->Release();
  else
    for (int i = 0; i < Count; i++)
      const_cast<CASemaphore*>(Sems[i])->Release();
}

static HANDLE* newHandles(const CSemaphoreSet& set)
{
  HANDLE* handles = new HANDLE[set.GetCount()];
  for (int i = 0; i < set.GetCount(); i++) {
    _ASSERT(set[i]);
    handles[i] = *set[i];  // Assumes non-0 since i is in set range
  }
  return handles;
}

//
CSemaphoreSet::CLock::CLock(const CSemaphoreSet& set, CWaitWhat wait,
                            ULONG timeOut, bool alertable)
:
  Set(0)
{
  CAPointer<CHandle> handles = newHandles(set);

  if (InitLock(set.Count, wait,
               ::WaitForMultipleObjectsEx(set.Count, handles, wait, timeOut, alertable)))
    Set = &set;
}

//
CSemaphoreSet::CLock::CLock(ULONG msgMask, const CSemaphoreSet& set,
                            CWaitWhat wait, ULONG timeOut)
{
  CAPointer<CHandle> handles = newHandles(set);

  if (InitLock(set.Count, wait,
               ::MsgWaitForMultipleObjects(set.Count, handles, wait, timeOut, msgMask)))
    Set = &set;
}

//
// Init the Set and Locked members after a system wait call
//
bool CSemaphoreSet::CLock::InitLock(int count, CWaitWhat wait, int index)
{
  if (DWORD(index) >= WAIT_OBJECT_0 && DWORD(index) < WAIT_OBJECT_0+count ||
      DWORD(index) >= WAIT_ABANDONED_0 && DWORD(index) < WAIT_ABANDONED_0+count)
  {
    if (wait == WaitAny) {
      if (index >= WAIT_ABANDONED_0)
        index -= WAIT_ABANDONED_0;
      Locked = index;      // Just this one is locked
    }
    else
      Locked = AllAquired;         // They are all locked
    return true;
  }
  else if (DWORD(index) == WAIT_OBJECT_0+count)
    Locked = MsgWaiting;
  else if (DWORD(index) == WAIT_TIMEOUT)
    Locked = TimedOut;
  else if (DWORD(index) == WAIT_IO_COMPLETION)
    Locked = IoComplete;
  return false;
}

//
CSemaphoreSet::CLock::~CLock()
{
  Release();
}

//
void CSemaphoreSet::CLock::Release(bool relinquish)
{
  if (Set) {
    const_cast<CSemaphoreSet*>(Set)->Release(Locked);
    if (relinquish)
      Set = 0;
  }
}

//----------------------------------------------------------------------------
// CThread Win32

//
// CThread constructors
//
CThread::CThread(  unsigned int uiStackSize )
:
  ThreadId(0),
  Handle(0),
  Stat(Created),
  TerminationRequested(0),
  Attached(false),
	RunningEvent(true),
	RunEvent(true),
	FinishedEvent(true),
	StackSize( uiStackSize )
{
}

//
// Attach to a running thread
//
CThread::CThread(CCurrent)
:
  ThreadId(::GetCurrentThreadId()),
  Handle(0),
  Stat(Running),
  TerminationRequested(0),
  Attached(true),
	RunningEvent(true),
	RunEvent(true),
	FinishedEvent(true),
	StackSize( 0 )
{
  ::DuplicateHandle(::GetCurrentProcess(), ::GetCurrentThread(),
                    ::GetCurrentProcess(), &Handle,
                    0, false, DUPLICATE_SAME_ACCESS);
}

//
//
//
CThread::CThread(const CThread&)
:
  ThreadId(0),
  Handle(0),
  Stat(Created),
  TerminationRequested(0),
  Attached(false),
	RunningEvent(true),
	RunEvent(true),
	FinishedEvent(true)
{
}

//
// CThread assignment operator
//
// Used when assigning derived objects. Attempting to assign from a running
// object is an error, since the data fields in the running object can be
// changing asynchronously.
//
const CThread& CThread::operator =(const CThread& thread)
{
  switch (GetStatus()) {
    case Created:
    case Suspended:
    case Finished: {
      if (this != &thread) {
        Handle = 0;
        ThreadId = 0;
        Stat = Created;
        TerminationRequested = false;
        Attached = false;
      }
      return *this;
    }
    default:
      throw new CThreadException(CThreadException::AssignError);
  }
}

//
// CThread destructor
//
// If the thread hasn't finished, destroying its control object is an error.
//
CThread::~CThread()
{
  if (!Attached && IsRunning() /*GetStatus() != Finished */&& GetStatus() != Invalid)
    throw new CThreadException(CThreadException::DestroyBeforeExit);
  if (Handle)
	::CloseHandle(Handle);
}

//extern int errno;
//
// Starts the thread executing. The actual call depends on the operating system.
// After the system call we check status.
//
CThread::CHandle CThread::Start()
{
  // If this thread is already started, just return the current handle
  //
	FinishedEvent.Reset();
	RunningEvent.Set();

  if (Handle)
    return Handle;

//# if defined(BI_MULTI_THREAD_RTL)
  Handle = (HANDLE)::_beginthreadex(NULL, StackSize/*4096*/, &CThread::Execute, this, 0, (UINT *)&ThreadId);
//# else
//  Handle = ::CreateThread(0, 0, &CThread::Execute, this, 0, &ThreadId);
//# endif

  if (Handle) {
    //TRACEX(Threads, 1, "Thread started [id:" << Handle << ']');
    Stat = Running;
		RunEvent.Set();
  }
  else {
    //TRACEX(Threads, 2, "Thread failed to start");
    Stat = Invalid;
		RunningEvent.Reset();
		FinishedEvent.Set();
		//DWORD nError = errno;//::GetLastError();
    throw new CThreadException(CThreadException::CreationFailure);
  }

  return Handle;
}

//
bool CThread::IsRunning()
{
  return ::WaitForSingleObject(RunningEvent, 0) == WAIT_OBJECT_0;
}

//
bool CThread::IsRun()
{
  return ::WaitForSingleObject(RunEvent, 0) == WAIT_OBJECT_0;
}

//
bool CThread::IsFinished()
{
  return ::WaitForSingleObject(FinishedEvent, 0) == WAIT_OBJECT_0;
}

//
// It's an error to try to suspend a thread that hasn't been started or that
// has already terminated.
//
ULONG CThread::Suspend()
{
  switch (GetStatus()) {
    case Created:
      //TRACEX(Threads, 2, "Illegal Created thread suspension [id:" << Handle << ']');
      throw new CThreadException(CThreadException::SuspendBeforeRun);
    case Finished:
      //TRACEX(Threads, 2, "Illegal Finished thread suspension [id:" << Handle << ']');
      throw new CThreadException(CThreadException::SuspendAfterExit);
    default:
      ULONG res = ::SuspendThread(Handle);
      if (res < MAXIMUM_SUSPEND_COUNT)  // Else a problem
        Stat = Suspended;
      //TRACEX(Threads, 0, "Thread suspended [id:" << Handle << ", Count:" << res << ']');
      return res;
  }
}

//
// It's an error to try to resume a thread that isn't suspended.
//
ULONG CThread::Resume()
{
  switch (GetStatus()) {
    case Created:
      //TRACEX(Threads, 2, "Illegal Created thread resumption [id:" << Handle << ']');
      throw new CThreadException(CThreadException::ResumeBeforeRun);
    case Running:
      //TRACEX(Threads, 2, "Illegal Running thread resumption [id:" << Handle << ']');
      throw new CThreadException(CThreadException::ResumeDuringRun);
    case Finished:
      //TRACEX(Threads, 2, "Illegal Finished thread resumption [id:" << Handle << ']');
      throw new CThreadException(CThreadException::ResumeAfterExit);
    default:
      ULONG res = ::ResumeThread(Handle);
      //TRACEX(Threads, 0, "Thread resumed [id:" << Handle << ", Count:" << res << ']');
      if (res == 0 || res == 1)
        Stat = Running;
      return res;
    }
}

//
// Mark the thread for termination.
//
void CThread::Terminate()
{
  //TRACEX(Threads, 1, "Thread termination requested [handle:" << Handle << ']');
	WaitForStart( 30000 );
  TerminationRequested = true;
}

// ---
BOOL CThread::WaitForStart(ULONG timeout)
{
	if ( !IsFinished() ) {
		HANDLE aHandles[2] = { RunningEvent, FinishedEvent };
		if ( ::WaitForMultipleObjects( 2, aHandles, FALSE, timeout ) == WAIT_OBJECT_0 ) {
			aHandles[0] = RunEvent;
			return ::WaitForMultipleObjects( 2, aHandles, FALSE, timeout ) == WAIT_OBJECT_0;
		}
	}
  return FALSE;
}

//
// Block until the thread terminates.
//
// IMPORTANT: the meaning of the 'timeout' parameter is different for NT and
// OS/2. Under NT it specifies how long to wait for termination. Under OS/2 it
// specifies whether to wait or to return immediately if the thread hasn't
// terminated.
//
ULONG CThread::WaitForExit(ULONG timeout) {
  //TRACEX(Threads, 1, "Waiting for thread exit [id:" << Handle << ']');
  ULONG ulResult = ::WaitForSingleObject(FinishedEvent/*Handle*/, timeout);
	Sleep( 5 );
	return ulResult;
}

//
// Block until the thread terminates looking after message queue
//
void CThread::MsgWaitForExit(HWND lookAfter, ULONG timeout) {
	HANDLE aHandles[2] = { FinishedEvent, Handle };
	while( TRUE/*Stat != Finished*/ ) {
		DWORD nReply = WAIT_TIMEOUT;
		if ( !lookAfter || ::IsWindow(lookAfter) )
			nReply = ::MsgWaitForMultipleObjects( 2, aHandles, FALSE, timeout, QS_ALLINPUT );
		else
			nReply = ::WaitForMultipleObjects( 2, aHandles, FALSE, timeout );
		if ( nReply == 0xffffffff )
			break;
		if ( nReply == WAIT_OBJECT_0 || nReply == (WAIT_OBJECT_0 + 1) )
			break;
		if ( nReply == (WAIT_OBJECT_0 + 2) ) {
			MSG rcMsg;
			if ( ::PeekMessage(&rcMsg, lookAfter, 0, 0, PM_NOREMOVE) )	{
				if ( rcMsg.message != WM_QUIT )	{
					::PeekMessage( &rcMsg, lookAfter, 0, 0, PM_REMOVE );
					::TranslateMessage( &rcMsg );
					::DispatchMessage( &rcMsg );
				}
			}
		}
	}
	Sleep( 5 );
}

//
// See note for WaitForExit().
//
ULONG CThread::TerminateAndWait(ULONG timeout)
{
  Terminate();
  return WaitForExit(timeout);
}

//
// Set the thread's priority.
//
int CThread::SetPriority(int pri)
{
  //TRACEX(Threads, 1, "Thread priority changed to " << pri << " [id:" << Handle << ']');
  return ::SetThreadPriority(Handle, pri);
}

//
//
//
int
CThread::Run()
{
  //TRACEX(Threads, 1, "Illegal Run() on base CThread [id:" << Handle << ']');
  return -1;
}

//
// Run the thread. This static function is given as the thread start address,
// with 'this' thread object passed as the param. Invoke the Run() method on
// the thread
//
/*
//#if defined(BI_MULTI_THREAD_RTL)
void _USERENTRY CThread::Execute(void* thread)
{
  STATIC_CAST(CThread*,thread)->Run();
  STATIC_CAST(CThread*,thread)->Stat = Finished;
}
//#elif defined(BI_PLAT_WIN32)
*/
UINT _stdcall CThread::Execute(void* thread)
{
  int code = static_cast<CThread *>(thread)->Run();
  static_cast<CThread *>(thread)->Stat = Finished;
	static_cast<CThread *>(thread)->RunningEvent.Reset();
	static_cast<CThread *>(thread)->RunEvent.Reset();
	static_cast<CThread *>(thread)->FinishedEvent.Set();
  return code;
}

//
// Alternative to returning from Run(). Called from within the thread that
// wants to exit early.
//
void
CThread::Exit(ULONG code)
{
  Stat = Finished;
  ::ExitThread(code);
}

//
// Call only when Stat claims that the thread is Running.
//
CThread::CStatus
CThread::CheckStatus() const
{
  DWORD code;
  ::GetExitCodeThread(Handle, &code);
  if (code == STILL_ACTIVE)
    return Running;
  else
    return Finished;
}

//----------------------------------------------------------------------------

//
// CThread::CThreadException constructor
//
/*
CThreadException( const CThreadException &other ) 
:
  CException(false),
  Type(other.GetErrorType())
{
}
*/
//
CThreadException::CThreadException(CErrorType type)
:
//  CException(true),
  Type(type)
{
}

// ---
int CThreadException::ReportError( UINT nType, UINT /*nMessageID*/ ) {
#ifdef _DEBUG
	return ::MessageBox( NULL, MakeString(Type), _T("Thread Error !"), nType | MB_APPLMODAL );
#else
	return MB_OK;
#endif
}

//
// CThread::CThreadException::MakeString()
//
// Translates an error code into a string.
//
TCHAR *CThreadException::MakeString(CErrorType type)
{
  static TCHAR* Names[] = {
    _T("Suspend() before Run()"),
    _T("Resume() before Run()"),
    _T("Resume() during Run()"),
    _T("Suspend() after Exit()"),
    _T("Resume() after Exit()"),
    _T("creation failure"),
    _T("destroyed before Exit()"),
    _T("illegal assignment"),
    _T("Multithreaded Runtime not selected"),
  };
  static TCHAR Msg[256];
	_tcscpy( Msg, _T("Error[thread]: ") );
	_tcscat( Msg, Names[type] );
  return Msg;
}

//IMPLEMENT_DYNAMIC(CThreadException, CException)
