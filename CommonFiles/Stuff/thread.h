////////////////////////////////////////////////////////////////////
//
//  thread.h
//  Simple Win32 thread definitions
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#if !defined(__THREAD_H__)
#define __THREAD_H__

#include <windows.h>
#include <crtdbg.h>

#define _USERENTRY __cdecl

//
// class CASemaphore
// ~~~~~ ~~~~~~~~~~
// Base class for handle-based thread synchronization classes, CMutex,
// CCountedSemaphore and CEventSemaphore. Defines some types & constants, as
// well as holding the system object handle for Win32.
//

#define CSemaphore CASemaphore

class CASemaphore
{
  public:

   virtual ~CASemaphore();

    enum { NoWait = 0, NoLimit = INFINITE };
    typedef HANDLE CHandle;
    operator CHandle() const;

    class CLock {
      public:
        CLock(const CASemaphore&, ULONG timeOut = NoLimit, bool alertable = false);
       ~CLock();

        bool WasAquired() const;  // See if really aquired, or just timed-out

        // Release lock early & relinquish, i.e. before destructor. Or, just
        // drop the lock count on an exiting semaphore & keep locked 'til dtor
        //
        void Release(bool relinquish = false);

      private:
        const CASemaphore* Sem;
    };
    friend class CLock;
    friend class CSemaphoreSet;

  private:
    virtual void Release() = 0;  // Derived class must provide release

  protected:
    CHandle Handle;              // Derived class must initialize
};

//
// class CMutex
// ~~~~~ ~~~~~~
// Mutual-exclusive semaphore
//
// CMutex provides a system-independent interface to critical sections in
// threads. With suitable underlying implementations the same code can be used
// under OS/2 and Windows NT.
//
// An object of type CMutex can be used in conjunction with objects of type
// CMutex::CLock (inherited from CASemaphore) to guarantee that only one thread
// can be executing any of the code sections protected by the lock at any
// given time.
//
// The differences between the classes CCriticalSection and CMutex are that a
// timeout can be specified when creating a Lock on a CMutex object, and that
// a CMutex object has a HANDLE which can be used outside the class. This
// mirrors the distinction made in Windows NT between a CRITICALSECTION and a
// Mutex. Under NT a CCriticalSection object is much faster than a CMutex
// object. Under operating systems that don't make this distinction a
// CCriticalSection object can use the same underlying implementation as a
// CMutex, losing the speed advantage that it has under NT.
//
class CMutex : public CASemaphore
{
  public:

    CMutex(const TCHAR* name = 0, LPSECURITY_ATTRIBUTES sa = 0);
    CMutex(const TCHAR* name, bool inherit, DWORD access = MUTEX_ALL_ACCESS);
    CMutex(CHandle handle);

		~CMutex();
		
    typedef CLock Lock;  // For compatibility with old T-less typename

  private:
    CMutex(const CMutex&);
    const CMutex& operator =(const CMutex&);

    virtual void Release();  // Release this mutex semaphore
};

//
// class CCountedSemaphore
// ~~~~~ ~~~~~~~~~~~~~~~~~
// Counted semaphore. Currently Win32 only
//
class CCountedSemaphore : public CASemaphore
{
  public:
    CCountedSemaphore(int initialCount, int maxCount, const TCHAR* name = 0,
                      LPSECURITY_ATTRIBUTES sa = 0);
    CCountedSemaphore(const TCHAR* name, bool inherit,
                      DWORD access = SEMAPHORE_ALL_ACCESS);
    CCountedSemaphore(CHandle handle);

		~CCountedSemaphore();

  private:
    CCountedSemaphore(const CCountedSemaphore&);
    const CCountedSemaphore& operator =(const CCountedSemaphore&);

    virtual void Release();  // Release this counted semaphore
};

//
// class CEventSemaphore
// ~~~~~ ~~~~~~~~~~~~~~~
//
class CEventSemaphore : public CASemaphore
{
  public:

    CEventSemaphore(bool manualReset=false, bool initialState=false,
                    const TCHAR* name = 0, LPSECURITY_ATTRIBUTES sa = 0);
    CEventSemaphore(const TCHAR* name, bool inherit,
                    DWORD access = SEMAPHORE_ALL_ACCESS);
    CEventSemaphore(CHandle handle);

		~CEventSemaphore();

    void Set();
    void Reset();
    void Pulse();

  private:
    CEventSemaphore(const CMutex&);
    const CEventSemaphore& operator =(const CEventSemaphore&);

    virtual void Release();  // Release this event semaphore
};

//
// class CSemaphoreSet
// ~~~~~ ~~~~~~~~~~~~~
// Semaphore object aggregator. Used to combine a set of semaphore objects so
// that they can be waited upon (locked) as a group. The lock can wait for any
// one, or all of them. The semaphore objects to be aggregated MUST live at
// least as long as this CSemaphoreSet, as it maintains pointers to them.
//
class CSemaphoreSet
{
  public:
    // sems is initial array of sem ptrs, may be 0 to add sems later,
    // size is maximum sems to hold, -1 means count the 0-terminated array
    // Passing (0,-1) is not valid
    //
    CSemaphoreSet(const CASemaphore* sems[], int size = -1);
   ~CSemaphoreSet();

    void Add(const CASemaphore& sem);
    void Remove(const CASemaphore& sem);
    int  GetCount() const;
    const CASemaphore* operator [](int index) const;

    enum CWaitWhat { WaitAny = false, WaitAll = true };
    enum { NoWait = 0, NoLimit = INFINITE };

    class CLock {
      public:
        // Assumes that set is not modified while locked
        //
        CLock(const CSemaphoreSet& set, CWaitWhat wait,
              ULONG timeOut = NoLimit, bool alertable = false);
        CLock(ULONG msgMask, const CSemaphoreSet& set, CWaitWhat wait,
              ULONG timeOut = NoLimit);
       ~CLock();

        bool WasAquired() const;    // See if one or more was aquired
        enum {
          AllAquired = -1,   // All semaphores were aquired
          TimedOut   = -2,   // None aquired: timed out
          IoComplete = -3,   //               IO complate alert
          MsgWaiting = -4,   //               Msg waiting
        };
        int  WhichAquired() const;  // See which was aquired >=0, or enum
       
        void Release(bool relinquish = false);

      protected:
        bool InitLock(int count, CWaitWhat wait, int index);

      private:
        const CSemaphoreSet* Set;
        int   Locked;    // Which one got locked, or wait code
    };
    friend CLock;

  private:
    void Release(int index = -1);

    typedef CASemaphore* CSemaphorePtr;
    typedef HANDLE CHandle;

    CASemaphore** Sems;    // Array of ptrs to semaphores, packed at head

    int   Size;   // Size of array, i.e. maximum object count
    int   Count;  // Count of objects currently in array
};

//
// class CCriticalSection
// ~~~~~ ~~~~~~~~~~~~~~~~
// Lightweight intra-process thread synchronization. Can only be used with
// other critical sections, and only within the same process.
//
class CCriticalSection
{
  public:
    CCriticalSection();
   ~CCriticalSection();

    class CLock {
      public:
        CLock(const CCriticalSection&);
       ~CLock();
      private:

        const CCriticalSection& CritSecObj;
    };
    friend CLock;
    typedef CLock Lock;  // For compatibility with old T-less typename

  private:

    CRITICAL_SECTION CritSec;

    CCriticalSection(const CCriticalSection&);
    const CCriticalSection& operator =(const CCriticalSection&);
};

//
// class CSync
// class CSync::CLock
// ~~~~~ ~~~~~~~~~~~
// CSync provides a system-independent interface to build classes that act like
// monitors, i.e., classes in which only one member can execute on a particular
// instance at any one time. CSync uses CCriticalSection, so it is portable to
// all platforms that CCriticalSection has been ported to.
//
// CSync Public Interface
// ~~~~~~~~~~~~~~~~~~~~~~
// None. CSync can only be a base class.
//
// CSync Protected Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//     CSync(const CSync&);
//                             Copy constructor. Does not copy the
//                             CCriticalSection object.
//
//     const CSync& operator =(const CSync&);
//                             Assignment operator. Does not copy the
//                             CCriticalSection object.
//
//     class CLock;            Handles locking and unlocking of member
//                             functions.
//
// Example
// ~~~~~~~
//     class ThreadSafe : private CSync
//     {
//       public:
//         void f();
//         void g();
//       private:
//         int i;
//     };
//
//     void ThreadSafe::f()
//     {
//       CLock lock(this);
//       if (i == 2)
//         i = 3;
//     }
//
//     void ThreadSafe::g()
//     {
//       CLock lock(this);
//       if (i == 3)
//         i = 2;
//     }
//
class CSync
{
  protected:
    CSync();
    CSync(const CSync&);
    const CSync& operator =(const CSync&);

    class CLock : private CCriticalSection::CLock
    {
      public:
        CLock(const CSync*);
      private:
        static const CCriticalSection& GetRef(const CSync*);
    };
    friend CLock;
    typedef CLock Lock;  // For compatibility with old T-less typename

  private:
    CCriticalSection CritSec;
};

//
// template <class T> class CStaticSync
// template <class T> class CStaticSync::CLock
// ~~~~~~~~ ~~~~~~~~~ ~~~~~ ~~~~~~~~~~~~~~~~~
// CStaticSync provides a system-independent interface to build sets of classes
// that act somewhat like monitors, i.e., classes in which only one member
// function can execute at any one time regardless of which instance it is
// being called on. CStaticSync uses CCriticalSection, so it is portable to all
// platforms that CCriticalSection has been ported to.
//
// CStaticSync Public Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// None. CStaticSync can only be a base class.
//
// CStaticSync Protected Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     CStaticSync<T>(const CStaticSync<T>&);
//                             Copy constructor. Does not copy the
//                             CCriticalSection object.
//
//     const CStaticSync<T>& operator =(const CStaticSync<T>&);
//                             Assignment operator. Does not copy the
//                             CCriticalSection object.
//
//     class CLock;            Handles locking and unlocking of member
//                             functions.
//
// Example
// ~~~~~~~
//     class ThreadSafe : private CStaticSync<ThreadSafe>
//     {
//       public:
//         void f();
//         void g();
//       private:
//         static int i;
//     };
//
//     void ThreadSafe::f()
//     {
//       CLock lock;
//       if (i == 2)
//         i = 3;
//     }
//
//     void ThreadSafe::g()
//     {
//       CLock lock;
//       if (i == 3)
//         i = 2;
//     }
//
template <class T> class CStaticSync
{
  protected:
    CStaticSync();
    CStaticSync(const CStaticSync<T>&);
    const CStaticSync<T>& operator =(const CStaticSync<T>&);
   ~CStaticSync();

    class CLock : private CCriticalSection::CLock
    {
      public:
        CLock() : CCriticalSection::CLock(*CStaticSync<T>::CritSec) {}
    };
    friend CLock;
    typedef CLock Lock;  // For compatibility with old T-less typename

  private:
    static CCriticalSection* CritSec;
    static ULONG Count;
};

//
// class CThread
// ~~~~~ ~~~~~~~
// CThread provides a system-independent interface to threads. With
// suitable underlying implementations the same code can be used under
// OS/2 and Win32.
//
// CThread Public Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~
//     Start();                Begins execution of the thread. Returns
//                             the handle of the thread.
//
//     Suspend();              Suspends execution of the thread.
//     Resume();               Resumes execution of a suspended thread.
//
//     Terminate();            Sets an internal flag that indicates that the 
//                             thread should exit. The derived class can check
//                             the state of this flag by calling
//                             ShouldTerminate().
//
//     WaitForExit(ULONG timeout = NoLimit);
//                             Blocks the calling thread until the internal
//                             thread exits or until the time specified by
//                             timeout, in milliseconds,expires. A timeout of
//                             NoLimit says to wait indefinitely.
//
//     TerminateAndWait(ULONG timeout = NoLimit);
//                             Combines the behavior of Terminate() and
//                             WaitForExit(). Sets an internal flag that
//                             indicates that the thread should exit and blocks
//                             the calling thread until the internal thread
//                             exits or until the time specified by timeout, in
//                             milliseconds, expires. A timeout of NoLimit says
//                             to wait indefinitely.
//
//     GetStatus();            Gets the current status of the thread.
//                             See CThread::Status for possible values.
//
//     GetPriority();          Gets the priority of the thread.
//     SetPriority();          Sets the priority of the thread.
//
//     enum CStatus;           Identifies the states that the class can be in.
//
//         Created             The class has been created but the thread has 
//                             not been started.
//         Running             The thread is running.
//
//         Suspended           The thread has been suspended.
//
//         Finished            The thread has finished execution.
//
//         Invalid             The object is invalid. Currently this happens
//                             only when the operating system is unable to
//                             start the thread.
//
//     class CThreadException;     The error class that defines the objects that 
//                             are thrown when an error occurs.
//
// CThread::ThreadError Public Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     enum CErrorType;        Identifies the type of error that occurred.
//
//         SuspendBeforeRun    The user called Suspend() on an object
//                             before calling Start().
//
//         ResumeBeforeRun     The user called Resume() on an object
//                             before calling Start().
//
//         ResumeDuringRun     The user called Resume() on a thread that
//                             was not Suspended.
//
//         SuspendAfterExit    The user called Suspend() on an object
//                             whose thread had already exited.
//
//         ResumeAfterExit     The user called Resume() on an object
//                             whose thread had already exited.
//
//         CreationFailure     The operating system was unable to create
//                             the thread.
//
//         DestroyBeforeExit   The object's destructor was invoked
//                             its thread had exited.
//
//         AssignError         An attempt was made to assign to an
//                             object that was not in either the
//                             Created or the Finished state.
//
//     CErrorType GetErrorType() const;
//                             Returns a code indicating the type of
//                             error that occurred.
//
//     string why();           Returns a string that describes the
//                             error. Inherited from xmsg.
//
// CThread Protected Interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     CThread();              Creates an object of type CThread.
//     virtual ~CThread();     Destroys the object.
//
//     const CThread& operator =(const CThread&);
//                             The target object must be in either the
//                             Created state or the Finished state. If
//                             so, puts the object into the Created
//                             state. If the object is not in either the
//                             Created state or the Finished state it
//                             is an error and an exception will be
//                             thrown.
//
//     CThread(const CThread&);
//                             Pts the object into the Created
//                             state, just like the default constructor.
//                             Does not copy any of the internal details
//                             of the thread being copied.
//
//     virtual ULONG Run() = 0;
//                             The function that does the work. Calling
//                             Start() creates a thread that begins
//                             executing Run() with the 'this' pointer
//                             pointing to the CThread-based object.
//
//     int ShouldTerminate() const;
//                             Returns a non-zero value to indicate
//                             that Terminate() or TerminateAndWait()
//                             has been called. If this capability is
//                             being used, the thread should call
//                             ShouldTerminate() regularly, and if it
//                             returns a non-zero value the thread
//                             finish its processing and exit.
//
// Example
// ~~~~~~~
//     class TimerThread : public CThread
//     {
//       public:
//         TimerThread() : Count(0) {}
//       private:
//         int Run();
//         int Count;
//     };
//
//     int TimerThread::Run()
//     {
//       // loop 10 times
//       while (Count++ < 10) {
//         Sleep(1000);    // delay 1 second
//         cout << "Iteration " << Count << endl;
//       }
//       return 0;
//     }
//
//     int main()
//     {
//       TimerThread timer;
//       timer.Start();
//       Sleep(20000);     // delay 20 seconds
//       return 0;
//     }
//
// Internal States
// ~~~~~~~~~~~~~~~
// Created:    the object has been created but its thread has not been
//             started. The only valid transition from this state is
//             to Running, which happens on a call to Start(). In
//             particular, a call to Suspend() or Resume() when the
//             object is in this state is an error and will throw an
//             exception.
//
// Running:    the thread has been started successfully. There are two
//             transitions from this state:
//
//                 When the user calls Suspend() the object moves into
//                 the Suspended state.
//
//                 When the thread exits the object moves into the
//                 Finished state.
//
//             Calling Resume() on an object that is in the Running
//             state is an error and will throw an exception.
//
// Suspended:  the thread has been suspended by the user. Subsequent
//             calls to Suspend() nest, so there must be as many calls
//             to Resume() as there were to Suspend() before the thread
//             actually resumes execution.
//
// Finished:   the thread has finished executing. There are no valid
//             transitions out of this state. This is the only state
//             from which it is legal to invoke the destructor for the
//             object. Invoking the destructor when the object is in
//             any other state is an error and will throw an exception.
//
class CThread
{
  public:

    enum { NoLimit = INFINITE };
    typedef HANDLE CHandle;

    // Attach to the current running (often primary) thread
    //
    enum CCurrent {Current};
    CThread(CCurrent);

    enum CStatus { Created, Running, Suspended, Finished, Invalid };

    CHandle Start();
    ULONG   Suspend();
    ULONG   Resume();
    bool    Sleep(long timeMS, bool alertable = false);

    virtual void    Terminate();
    ULONG   WaitForExit(ULONG timeout = NoLimit);
		BOOL    WaitForStart(ULONG timeout = NoLimit);
    ULONG   TerminateAndWait(ULONG timeout = NoLimit);

		void		MsgWaitForExit(HWND lookAfter, ULONG timeout = NoLimit);

    CStatus GetStatus() const;
    DWORD  GetExitCode() const;

		operator CHandle() { return Handle; }

    enum CPriority {
      Idle         = THREAD_PRIORITY_IDLE,          // -15
      Lowest       = THREAD_PRIORITY_LOWEST,        //  -2
      BelowNormal  = THREAD_PRIORITY_BELOW_NORMAL,  //  -1
      Normal       = THREAD_PRIORITY_NORMAL,        //   0
      AboveNormal  = THREAD_PRIORITY_ABOVE_NORMAL,  //   1
      Highest      = THREAD_PRIORITY_HIGHEST,       //   2
      TimeCritical = THREAD_PRIORITY_TIME_CRITICAL, //  15
    };
      
    int GetPriority() const;
    int SetPriority(int);   // Can pass a CPriority for simplicity

    bool    ShouldTerminate() const;

		bool    IsRunning();
		bool    IsRun();
		bool    IsFinished();

  protected:
    CThread( unsigned int uiStackSize = 0 );          // Create a thread. Derived class overrides Run()

    // Copying a thread puts the target into the Created state
    //
    CThread(const CThread&);
    const CThread& operator =(const CThread&);

    virtual ~CThread();

    void    Exit(ULONG code);  // Alternative to returning from Run()

  private:
    virtual int Run();         

    CStatus CheckStatus() const;

//#if defined(BI_MULTI_THREAD_RTL)
//    static void _USERENTRY Execute(void* thread);
//#elif defined(BI_PLAT_WIN32)
    static UINT __stdcall Execute(void* thread);

    DWORD ThreadId;
    CHandle Handle;
    mutable CStatus Stat;
    bool TerminationRequested;
    bool Attached;
		CEventSemaphore RunningEvent;
		CEventSemaphore RunEvent;
		CEventSemaphore FinishedEvent;
		unsigned int    StackSize;
};

class CThreadException// : public CException
{
//	DECLARE_DYNAMIC(CThreadException)
  public:
    enum CErrorType {
      SuspendBeforeRun,
      ResumeBeforeRun,
      ResumeDuringRun,
      SuspendAfterExit,
      ResumeAfterExit,
      CreationFailure,
      DestroyBeforeExit,
      AssignError,
      NoMTRuntime,
    };
    typedef CErrorType ErrorType;
    CErrorType GetErrorType() const;
		virtual int ReportError( UINT nType = MB_OK, UINT nMessageID = 0 );

  private:
    CThreadException(CErrorType type);
    static TCHAR *MakeString(ErrorType type);
    CErrorType Type;
  friend CThread;
};

//----------------------------------------------------------------------------
// Inline implementation
//

//----------------------------------------
// CASemaphore Win32

//
inline CASemaphore::~CASemaphore()
{
  if (Handle)
    ::CloseHandle(Handle);
}

//
inline CASemaphore::operator CASemaphore::CHandle() const
{
  return Handle;
}

//
inline CASemaphore::CLock::CLock(const CASemaphore& sem, ULONG timeOut, bool alertable)
:
  Sem(0)
{
  if (::WaitForSingleObjectEx(sem, timeOut, alertable) == 0)
    Sem = &sem;
}

//
inline CASemaphore::CLock::~CLock()
{
  Release();
}

//
inline bool CASemaphore::CLock::WasAquired() const
{
  return Sem != 0;
}

//
// Release but hang on to the semaphore
//
inline void CASemaphore::CLock::Release(bool relinquish)
{
  if (Sem) {
    const_cast<CASemaphore*>(Sem)->Release();
    if (relinquish)
      Sem = 0;
  }
}

//----------------------------------------
// CMutex Win32

//
inline CMutex::CMutex(const TCHAR* name, LPSECURITY_ATTRIBUTES sa)
{
  Handle = ::CreateMutex(sa, false, name);  // don't aquire now
}

//
// Open an existing mutex. Fails if not there.
//
inline CMutex::CMutex(const TCHAR* name, bool inherit, DWORD access)
{
  Handle = ::OpenMutex(access, inherit, name);
}

//
//
//
inline CMutex::CMutex(CHandle handle)
{
  ::DuplicateHandle(::GetCurrentProcess(), handle,
                    ::GetCurrentProcess(), &Handle,
                    0, false, DUPLICATE_SAME_ACCESS);
}

//----------------------------------------
// CCountedSemaphore Win32

//
inline CCountedSemaphore::CCountedSemaphore(int initialCount, int maxCount,
                                            const TCHAR* name,
                                            LPSECURITY_ATTRIBUTES sa)
{
  Handle = ::CreateSemaphore(sa, initialCount, maxCount, name);
}

//
inline CCountedSemaphore::CCountedSemaphore(const TCHAR* name, bool inherit,
                                            DWORD access)
{
  Handle = ::OpenSemaphore(access, inherit, name);
}

//
inline CCountedSemaphore::CCountedSemaphore(CHandle handle)
{
  ::DuplicateHandle(::GetCurrentProcess(), handle,
                    ::GetCurrentProcess(), &Handle,
                    0, false, DUPLICATE_SAME_ACCESS);
}

//----------------------------------------
// CEventSemaphore Win32

inline CEventSemaphore::CEventSemaphore(bool manualReset, bool initialState,
                                        const TCHAR* name,
                                        LPSECURITY_ATTRIBUTES sa)
{
  Handle = ::CreateEvent(sa, manualReset, initialState, name);
}

//
inline CEventSemaphore::CEventSemaphore(const TCHAR* name, bool inherit,
                                        DWORD access)
{
  Handle = ::OpenEvent(access, inherit, name);
}

//
inline CEventSemaphore::CEventSemaphore(CHandle handle)
{
  ::DuplicateHandle(::GetCurrentProcess(), handle,
                    ::GetCurrentProcess(), &Handle,
                    0, false, DUPLICATE_SAME_ACCESS);
}

inline void CEventSemaphore::Set()
{
  ::SetEvent(*this);
}

inline void CEventSemaphore::Reset()
{
  ::ResetEvent(*this);
}

inline void CEventSemaphore::Pulse()
{
  ::PulseEvent(*this);
}

//----------------------------------------
// CSemaphoreSet Win32

inline int CSemaphoreSet::GetCount() const
{
  return Count;
}

inline const CASemaphore* CSemaphoreSet::operator [](int index) const
{
  return (index >= 0 && index < Count) ? Sems[index] : 0;
}

//
inline bool CSemaphoreSet::CLock::WasAquired() const
{
  return Set != 0;
}

//
// Which one was locked, all locked code, or lock fail code
//
inline int CSemaphoreSet::CLock::WhichAquired() const
{
  return Locked;
}
       
//----------------------------------------------------------------------------
// CCriticalSection Win32
//

//
// Use system call to initialize the CRITICAL_SECTION object.
//
inline CCriticalSection::CCriticalSection()
{
  ::InitializeCriticalSection(const_cast<CRITICAL_SECTION*>(&CritSec));
}

//
// Use system call to destroy the CRITICAL_SECTION object.
//
inline CCriticalSection::~CCriticalSection()
{
  ::DeleteCriticalSection(const_cast<CRITICAL_SECTION*>(&CritSec));
}

//
// Use system call to lock the CRITICAL_SECTION object.
//
inline CCriticalSection::CLock::CLock(const CCriticalSection& sec)
:
  CritSecObj(sec)
{
  ::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&CritSecObj.CritSec));
}

//
// Use system call to unlock the CRITICAL_SECTION object.
//
inline CCriticalSection::CLock::~CLock()
{
  ::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&CritSecObj.CritSec));
}

//----------------------------------------------------------------------------

//
// Copy constructor does not copy the CCriticalSection object,
// since the new object is not being used in any of its own
// member functions. This means that the new object must start
// in an unlocked state.
//
inline CSync::CSync()
{
}

//
inline CSync::CSync(const CSync&)
{
}

//
// Does not copy the CCriticalSection object, since the new  object is not
// being used in any of its own member functions.
// This means that the new object must start in an unlocked state.
//
inline const CSync& CSync::operator =(const CSync&)
{
  return *this;
}

//
// Locks the CCriticalSection object in the CSync object.
//
inline CSync::CLock::CLock(const CSync* sync)
:
  CCriticalSection::CLock(GetRef(sync))
{
}

//
// Returns a reference to the CCriticalSection object contained in the CSync
// object.
//
// In the diagnostic version, checks for a null pointer.
//
inline const CCriticalSection& CSync::CLock::GetRef(const CSync* sync)
{
  _ASSERT(sync != 0);
  return sync->CritSec;
}

//----------------------------------------------------------------------------

//
// Instantiate the data members.
//
template <class T> CCriticalSection* CStaticSync<T>::CritSec;
template <class T> ULONG CStaticSync<T>::Count;

//
// If this is the first CStaticSync<T> object to be constructed, create the
// semaphore.
//
// The copy constructor only has to increment the count, since there will
// already be at least one CStaticSync<T> object, namely, the one being copied.
//
template <class T> inline CStaticSync<T>::CStaticSync()
{
  if (Count++ == 0)
    CritSec = new CCriticalSection;
}

template <class T> inline CStaticSync<T>::CStaticSync(const CStaticSync<T>&)
{
  Count++;
}

//
// CStaticSync<T> assignment operator
//
template <class T>
inline const CStaticSync<T>& CStaticSync<T>::operator =(const CStaticSync<T>&)
{
  return *this;
}

//
// If this is the only remaining CStaticSync<T> object, destroy the semaphore.
//
template <class T> inline CStaticSync<T>::~CStaticSync()
{
  if (--Count == 0)
    delete CritSec;
}

//----------------------------------------------------------------------------

//
// Put this tread to sleep for a given number of milliseconds, with an optional
// wakeup on an IO completion. Return true if wakeup is due to IO completion.
//
inline bool CThread::Sleep(long timeMS, bool alertable)
{
  return ::SleepEx(timeMS, alertable) == WAIT_IO_COMPLETION;
}

//
// If the thread is marked as Running it may have terminated without our
// knowing it, so we have to check.
//
inline CThread::CStatus CThread::GetStatus() const
{
  if (Stat == Running)
/*
#if defined(BI_NO_MUTABLE)
    CONST_CAST(CThread*,this)->Stat = CheckStatus();
#else
*/
    Stat = CheckStatus();
//#endif
  return Stat;
}

//
inline DWORD  CThread::GetExitCode() const
{
  DWORD code;
  ::GetExitCodeThread(Handle, &code);
  return code;
}

//
// Direct OS call under Win32. Return stored value under OS/2.
//
inline int CThread::GetPriority() const
{
  return ::GetThreadPriority(Handle);
}

//
inline bool CThread::ShouldTerminate() const
{
  return TerminationRequested;
}

//
inline CThreadException::CErrorType CThreadException::GetErrorType() const
{
  return Type;
}

#endif  // CLASSLIB_THREAD_H
