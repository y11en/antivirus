// -*- C++ -*- Time-stamp: <03/07/14 17:54:15 ptr>

#include "build/general.h"

#include "std/thr/sync.h"
#include "std/base/klbase.h"
#include "std/io/klio.h"

#include "std/err/klerrors.h"

#ifdef _WIN32
#  define STRICT
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <tchar.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include <stl/_threads.h>

#ifdef N_PLAT_NLM
#  include <nwsemaph.h>
#  include <nwthread.h>
#  include <time.h>
#include <nwconio.h>
#endif

#ifdef _WIN32
#  include <crtdbg.h>
#endif

#ifdef USE_BOOST_THREADS
	#include <boost/thread/thread.hpp>
	#include <boost/thread/mutex.hpp>
	#include <boost/thread/recursive_mutex.hpp>
	#include <boost/thread/condition.hpp>
	#include <boost/thread/xtime.hpp>

	typedef boost::detail::thread::lock_ops<boost::recursive_mutex> recursive_mutex_lock_ops;	
	
	#define KLSTD_FILL_XTIME(xt,msec)\
			xt.sec += msec / 1000;\
			xt.nsec += ( msec%1000 ) * 1000 * 1000;\
			if ( xt.nsec>=1000000000 ) { xt.sec++; xt.nsec -=1000000000; }
#endif

namespace KLSTD
{
#ifdef _WIN32
    class CReadWriteLock
        :   public KLSTD::KLBaseImpl<KLSTD::ReadWriteLock>
    {
    public:
	    CReadWriteLock()
		    :	m_hReaderEvent(NULL)
		    ,	m_hMutex(NULL)
		    ,	m_hWriterMutex(NULL)
		    ,	m_lCounter(-1)
	    {
		    m_hReaderEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
		    m_hMutex = CreateEvent(NULL,FALSE,TRUE,NULL);
		    m_hWriterMutex = CreateMutex(NULL,FALSE,NULL);		
	    };

	    virtual ~CReadWriteLock()
	    {
            KLSTD_ASSERT(m_lCounter == -1);
		    CloseHandle(m_hReaderEvent);
		    CloseHandle(m_hMutex);
		    CloseHandle(m_hWriterMutex);
	    };

	    void Lock(bool bWriter)
	    {
		    if (bWriter)
		    {
			    WaitForSingleObject(m_hWriterMutex,INFINITE);
			    WaitForSingleObject(m_hMutex, INFINITE);
		    }
		    else
		    {
			    if (InterlockedIncrement(&m_lCounter) == 0)
			    {
				    WaitForSingleObject(m_hMutex, INFINITE);
				    SetEvent(m_hReaderEvent);
			    };
			    WaitForSingleObject(m_hReaderEvent,INFINITE);
		    };
	    };

	    void Unlock(bool bWriter)
	    {
		    if(bWriter)
		    {
			    SetEvent(m_hMutex);
			    ReleaseMutex(m_hWriterMutex);
		    }
		    else
		    {
			    if(InterlockedDecrement(&m_lCounter) < 0)
			    {
				    ResetEvent(m_hReaderEvent);
				    SetEvent(m_hMutex);
			    };
		    };
	    };
    protected:
	    HANDLE	m_hReaderEvent, m_hMutex, m_hWriterMutex;
	    volatile long	m_lCounter;
    };    
#endif

class CCriticalSection :
        public CriticalSection
{
  public:
    CCriticalSection()
      {
#ifdef USE_BOOST_THREADS
#else
	#ifdef _WIN32
        memset(&m_cs, 0, sizeof(m_cs));
        ::InitializeCriticalSection(&m_cs);
	#elif defined(N_PLAT_NLM) // Novell
        m_sem = OpenLocalSemaphore(1);
        m_semi = OpenLocalSemaphore(1);
        counter =0;
	#endif
#endif
      }
		
    virtual ~CCriticalSection()
      {
#ifdef USE_BOOST_THREADS
#else
	#ifdef _WIN32	
	   DeleteCriticalSection(&m_cs);
		memset(&m_cs, 0xFF, sizeof(m_cs));
	#elif defined(N_PLAT_NLM) // Novell
        CloseLocalSemaphore( m_semi );
        CloseLocalSemaphore( m_sem );
	#endif
#endif
      }

    void Enter()
      {
#ifdef USE_BOOST_THREADS		
		recursive_mutex_lock_ops::lock( m_mutex );
		locked = true;
#else
	#ifdef _WIN32	
        ::EnterCriticalSection(&m_cs);
	#elif defined(N_PLAT_NLM) // Novell
	
			ThreadSwitch();
	  unsigned int id_local = GetThreadID();
          WaitOnLocalSemaphore( m_semi);
           if (counter!=0 && threadID==id_local){
            counter++;
            SignalLocalSemaphore(m_semi);
           }
           else{
            SignalLocalSemaphore(m_semi);
            WaitOnLocalSemaphore( m_sem );
			WaitOnLocalSemaphore( m_semi);
            counter = 1;
            threadID = id_local;
            SignalLocalSemaphore(m_semi);          
           }
			ThreadSwitch();

	#elif defined(__unix)
        _lock.lock();
	#endif
#endif
      }

    void Leave()
      {
#ifdef USE_BOOST_THREADS
		recursive_mutex_lock_ops::unlock( m_mutex );
		locked = false;
#else
	#ifdef _WIN32	
        ::LeaveCriticalSection(&m_cs);
	#elif defined(N_PLAT_NLM) // Novell
	WaitOnLocalSemaphore( m_semi);
	if (counter==1) {
         counter=0;
         SignalLocalSemaphore( m_sem );
        }
        else counter--;
	SignalLocalSemaphore(m_semi);

	#elif defined(__unix)
        _lock.unlock();
	#endif
#endif
      }

  private:
#ifdef USE_BOOST_THREADS
	  boost::recursive_mutex m_mutex;
	  bool locked;
#else
	#ifdef _WIN32	
		CRITICAL_SECTION m_cs;
	#elif defined(N_PLAT_NLM) // Novell
		LONG m_sem;
                LONG m_semi;
                unsigned int counter;
                unsigned int threadID;
	#elif defined(__unix)
		__impl::MutexSDS _lock;
	#else
		#  error "Unsupported platform!"
	#endif
#endif
};

class CFileSemaphoreNoReadLock :
        public KLBaseImpl<FileSemaphoreNoReadLock>
{
  public:

    CFileSemaphoreNoReadLock( const std::wstring& wstrFileName )
      { m_wstrFileName=wstrFileName; }

    virtual ~CFileSemaphoreNoReadLock()
    {        
        /*
            If we try to open file being deleted we shall get
            ERROR_ACCESS_DENIED error, so it's better not to delete file at all
        */
        //::KLSTD_Unlink( m_wstrFileName.c_str(), false );
    }

    void Enter(bool bReadOnly, long lTimeout)
      {
        KLSTD_ASSERT(!m_pFile);
        ::KLSTD_CreateFile(
          m_wstrFileName,
          bReadOnly?SF_READ:0,
          CF_OPEN_ALWAYS,
          bReadOnly?AF_READ:(AF_READ|AF_WRITE),
          0,
          &m_pFile,
          lTimeout);						
      }
    
    void EnterForDelete(long lTimeout)
    {
#ifdef _WIN32
        KLSTD_ASSERT(!m_pFile);
        
        ::KLSTD_CreateFile(
              m_wstrFileName,
              0,
              CF_OPEN_ALWAYS,
              AF_READ|AF_WRITE,
              EF_DELETE_ON_CLOSE,
              &m_pFile,
              lTimeout);
#else
        Enter(false, lTimeout);
#endif
    };

    void Leave()
      {
//	KLSTD_ASSERT(m_pFile);
        m_pFile=NULL;
      }
    
  protected:
    CAutoPtr<File>	m_pFile;
    std::wstring	m_wstrFileName;
};

class SemaphoreImp :
    public KLBaseImpl<Semaphore>
{
  public:
    SemaphoreImp( int initCount ) :
        KLBaseImpl<Semaphore>()
    {
#ifdef USE_BOOST_THREADS
		m_value = initCount;
#else
	#ifdef _WIN32
        semaphore = CreateSemaphore( NULL, initCount, INT_MAX, _T("") );
	#endif
	#ifdef N_PLAT_NLM
        semaphore = OpenLocalSemaphore( initCount );
	#endif
#endif
    }
		
    virtual ~SemaphoreImp()
      {
#ifdef USE_BOOST_THREADS
#else
	#ifdef _WIN32
        CloseHandle( semaphore );
	#endif
	#ifdef N_PLAT_NLM
        CloseLocalSemaphore( semaphore );
	#endif
#endif
    }

    bool Wait( int msec )
    {
#ifdef USE_BOOST_THREADS
		boost::xtime waittime;
		if ( msec>=0 )
		{
			boost::xtime_get(&waittime, boost::TIME_UTC);
			KLSTD_FILL_XTIME( waittime, msec );
		}

		boost::mutex::scoped_lock lock( m_wait_mutex );
		
		int waitcondres = 0;
		while( m_value==0 )
		{
			if ( msec>=0 )
			{
				if ( !m_cond_wait.timed_wait( lock, waittime ) ) 
					return false;
			}
			else
			{
				m_cond_wait.wait( lock );
			}
		}

		--m_value;		
		return true;
#else
	#ifdef _WIN32
        DWORD res = msec < 0 ?
          WaitForSingleObject( semaphore, INFINITE ) :
          WaitForSingleObject( semaphore, msec );
        
        return res == WAIT_OBJECT_0 ? true : false;
	#endif
	#ifdef N_PLAT_NLM
        if ( msec < 0 ) {
          WaitOnLocalSemaphore( semaphore );
          return true;
        }
        bool tres = !TimedWaitOnLocalSemaphore( semaphore, msec );
		return tres;
	#endif
	#ifdef __unix
        if ( msec >= 0 ) {
          timespec t;
          KLSTD_FILL_XTIME( t, msec );
          return _s.wait_delay( &t ) == 0 ? true : false;
        }
        return _s.wait() == 0 ? true : false;
	#endif
#endif
    }

    void Post()
    {
#ifdef USE_BOOST_THREADS
		{
			boost::mutex::scoped_lock lock( m_wait_mutex );

			m_value++;
		}
		m_cond_wait.notify_one();		
#else
	#ifdef _WIN32
        long prevCount;
        ReleaseSemaphore( semaphore, 1, &prevCount );
	#endif
	#ifdef N_PLAT_NLM
        SignalLocalSemaphore( semaphore );
	#endif
	#ifdef __unix
        _s.post();
	#endif
#endif
    }

    bool IsValid()
    {
#ifdef USE_BOOST_THREADS
		return true;
#else
	#ifdef _WIN32
        return ( semaphore == NULL ) ? false : true;
	#endif
	#ifdef N_PLAT_NLM
        return ( semaphore != 0 );
	#endif
	#ifdef __unix
        return true;
	#endif
#endif
    }

  private:
 
#ifdef USE_BOOST_THREADS
		int						m_value;
		boost::mutex			m_wait_mutex;
		boost::condition		m_cond_wait;
#else
	#ifdef _WIN32
		HANDLE semaphore;
	#endif
	#ifdef N_PLAT_NLM
		LONG semaphore;
	#endif
#endif
};	


} // namespace KLSTD

using namespace KLSTD;



#if defined(_WIN32)
    #define KLSTD_HAS_ASM_INTERLOCKED

    #define My_InterlockedIncrement         InterlockedIncrement
    #define My_InterlockedDecrement         InterlockedDecrement
    #define My_InterlockedCompareExchange   InterlockedCompareExchange
#elif defined(__unix) || defined(__i386)
    #define KLSTD_HAS_ASM_INTERLOCKED

    /* Define to a macro to generate an assembly function directive */
    #define __AK_ASM_FUNC(name) ".type " __AK_ASM_NAME(name) ",@function"

    /* Define to a macro to generate an assembly name from a C symbol */
    #define __AK_ASM_NAME(name) name

    #define __AK_ASM_GLOBAL_FUNC(name,code) \
          __asm__( ".text\n\t" \
                   ".align 4\n\t" \
                   ".globl " __AK_ASM_NAME(#name) "\n\t" \
                   __AK_ASM_FUNC(#name) "\n" \
                   __AK_ASM_NAME(#name) ":\n\t" \
                   code \
                   "\n\t.previous" );

    __AK_ASM_GLOBAL_FUNC(My_InterlockedIncrement,
                      "movl 4(%esp),%edx\n\t"
                      "movl $1,%eax\n\t"
                      "lock; xaddl %eax,(%edx)\n\t"
                      "incl %eax\n\t"
                      "ret");
                      //use "ret $4" if stdcall



    __AK_ASM_GLOBAL_FUNC(My_InterlockedDecrement,
                      "movl 4(%esp),%edx\n\t"
                      "movl $-1,%eax\n\t"
                      "lock; xaddl %eax,(%edx)\n\t"
                      "decl %eax\n\t"
                      "ret");
                      //use "ret $4" if stdcall

    __AK_ASM_GLOBAL_FUNC(My_InterlockedCompareExchange,
                  "movl 12(%esp),%eax\n\t"
                  "movl 8(%esp),%ecx\n\t"
                  "movl 4(%esp),%edx\n\t"
                  "lock; cmpxchgl %ecx,(%edx)\n\t"
                  "ret");
                  //use "ret $12" if stdcall


    extern "C" long My_InterlockedIncrement(long volatile *lpAddend);
    extern "C" long My_InterlockedDecrement(long volatile *lpAddend);
    extern "C" long My_InterlockedCompareExchange( long volatile * dest, long xchg, long compare );

#endif  //defined(__unix) && defined(__i386)


#if defined(N_PLAT_NLM)
    static class CSyncInitializer
    {
    public:
        CSyncInitializer()
        {
            m_locker_mutex = OpenLocalSemaphore(1);
        };

        ~CSyncInitializer()
        {
            CloseLocalSemaphore(m_locker_mutex);
        };

        void Lock()
        {
            WaitOnLocalSemaphore( this->m_locker_mutex ); 
        }

        void Unlock()
        {
            SignalLocalSemaphore( this->m_locker_mutex ); 
        };  
        LONG m_locker_mutex;
    }g_Initializer;
#elif defined(USE_BOOST_THREADS) && !defined(_WIN32) && !defined(KLSTD_HAS_ASM_INTERLOCKED)
    
    static class CSyncInitializer
    {
    public:
	    void Lock()
	    {
		    boost::detail::thread::lock_ops<boost::mutex>::lock( m_locker_mutex );
	    }
	    void Unlock()
	    {
		    boost::detail::thread::lock_ops<boost::mutex>::unlock( m_locker_mutex );
	    }
	    boost::mutex			m_locker_mutex;
    } g_Initializer;
#endif

__declspec( naked )
long KLSTD_FASTCALL KLSTD_InterlockedIncrement(long volatile* x)
{
	//KLSTD_CHKINPTR(x);
#if defined(KLSTD_HAS_ASM_INTERLOCKED)
	//return My_InterlockedIncrement(x);
    __asm
    {
        mov     eax,1
        lock    xadd  dword ptr [ecx],eax
        inc     eax
        ret
    };
#else 
	g_Initializer.Lock();
	long lResult=++(*x);
	g_Initializer.Unlock();
	return lResult;
#endif
}

__declspec( naked )
long KLSTD_FASTCALL KLSTD_InterlockedDecrement(long volatile* x)
{
	//KLSTD_CHKINPTR(x);
#if defined(KLSTD_HAS_ASM_INTERLOCKED)
    //return My_InterlockedDecrement(x);
    __asm
    {
        mov     eax,0FFFFFFFFh
        lock    xadd dword ptr [ecx],eax
        dec     eax
        ret
    };
#else
	g_Initializer.Lock();
	long lResult=--(*x);
	g_Initializer.Unlock();
	return lResult;
#endif
}

long KLCSC_DECL KLSTD_InterlockedCompareExchange(
        long volatile*  Destination,
        long            Exchange,
        long            Comperand)
{
#if defined(KLSTD_HAS_ASM_INTERLOCKED)
    return My_InterlockedCompareExchange(Destination, Exchange, Comperand);
#else
	g_Initializer.Lock();
    long lResult = *Destination;
    if(*Destination == Comperand)
        *Destination = Exchange;
	g_Initializer.Unlock();
    return lResult;
#endif
};

long KLCSC_DECL KLSTD_InterlockedIncrementIfGreaterOrEqualThanZero(

        volatile long* plTarget)

{
#if defined(KLSTD_HAS_ASM_INTERLOCKED)
    long lStartValue, lNewValue;
    do{
        lStartValue = *plTarget;
        lNewValue = lStartValue + ((lStartValue >= 0) ? 1 : 0);
    }while(My_InterlockedCompareExchange(plTarget, lNewValue, lStartValue) != lStartValue);
    return(lStartValue);
#else
	g_Initializer.Lock();
    long lResult = *plTarget;
    if(*plTarget >= 0)
        ++(*plTarget);
	g_Initializer.Unlock();
    return lResult;
#endif
};


bool KLSTD_CreateCriticalSection(KLSTD::CriticalSection** ppCritSec)
{
	KLSTD_CHKOUTPTR(ppCritSec);
	*ppCritSec=new RcClassImpl<CCriticalSection>;
	return (*ppCritSec)!=NULL;
}

bool KLSTD_CreateCriticalSectionST(KLSTD::CriticalSection** ppCritSec)
{
	KLSTD_CHKOUTPTR(ppCritSec);
	*ppCritSec=new RcClassImpl<CCriticalSection, false>;
	return (*ppCritSec)!=NULL;
}

void KLSTD_CreateFileSemaphoreNoReadLock(const std::wstring& wsName, KLSTD::FileSemaphoreNoReadLock** ppSem)
{
	KLSTD_CHKOUTPTR(ppSem);
	*ppSem=new CFileSemaphoreNoReadLock(wsName);
	KLSTD_CHKMEM(*ppSem);
};

bool KLSTD_CreateSemaphore( KLSTD::Semaphore** pSem, int initValue )
{
	*pSem = new SemaphoreImp( initValue );
	if ( !((SemaphoreImp *)(*pSem))->IsValid() ) {
		delete (*pSem);
		(*pSem) = NULL;
	}
	return (*pSem)!=NULL;
}

KLSTD_NOTHROW KLCSC_DECL void KLSTD_Sleep(long lTimeout) throw()
{
#ifdef USE_BOOST_THREADS
	boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    KLSTD_FILL_XTIME( xt, lTimeout );	
    boost::thread::sleep(xt); 
#else	
	#ifdef _WIN32
		Sleep(lTimeout);
	#endif
	#ifdef N_PLAT_NLM
		delay( lTimeout );
	#endif
	#ifdef __unix
		timespec t;
		KLSTD_FILL_XTIME( t, lTimeout );		
		__impl::Thread::delay( &t );
	#endif
#endif
};

KLCSC_DECL void KLSTD_CreateReadWriteLock(KLSTD::ReadWriteLock** ppLock)
{
#ifdef _WIN32
    KLSTD_CHKOUTPTR(ppLock);
    KLSTD::CAutoPtr<KLSTD::ReadWriteLock> pLock;
    pLock.Attach(new CReadWriteLock);
    pLock.CopyTo(ppLock);
#else
	KLSTD_NOTIMP();
#endif
};
extern IMPLEMENT_MODULE_LOCK(KLSTD_ATOMIC);

AVP_longlong KLCSC_DECL KLSTD_InterlockedRead64(
                    const AVP_longlong volatile * plData)
{
    //@todo try to create such func without CS
    AVP_longlong llResult;
    g_cKLSTD_ATOMIC_Lock->Enter();
    llResult = *plData;
    g_cKLSTD_ATOMIC_Lock->Leave();
    return llResult;
};

void KLCSC_DECL KLSTD_InterlockedWrite64(                    
                    AVP_longlong volatile*  plData,
                    AVP_longlong            lSrc)
{
    //@todo try to create such func without CS
    g_cKLSTD_ATOMIC_Lock->Enter();
    *plData = lSrc;
    g_cKLSTD_ATOMIC_Lock->Leave();
};
