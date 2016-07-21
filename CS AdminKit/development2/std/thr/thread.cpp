/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Thread.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Реализация класса поддержки потоков
 * 
 */
#include "../../include/build/general.h"
#ifdef _WIN32

    #include "std/win32/klos_win32v40.h"
    #include <eh.h>
	#include <time.h>
	#include <process.h>

#ifdef KLUSEFSSYNC
	#ifndef __out
	#define __out
	#define __in
	#define __inout
	#define __inout
	#define __in_opt
	#endif
	#include "../../../Windows/Hook/mklif/fssync/fssync.h"
#endif
#else
#ifdef N_PLAT_NLM 		//  Novell Netware
	#include <nwthread.h>
	#include <nwsemaph.h>
#endif
#endif

#ifdef KL_OPENSSL_SUPPORT
#include "../openssl/err.h"
#endif

#include <string>

#include "std/err/klerrors.h"
#include "std/thr/thread.h"
#include "std/trc/trace.h"
#include "std/thrstore/threadstore.h"
#include <std/thr/thr_callbacks.h>
#include <std/conv/klconv.h>
#include <std/io/klio.h>

#ifdef __unix
    #include <unistd.h>
    
    #define KL_UNIX_STACK_SIZE	1024*1024  // 1 Mb
#endif

#define KLCS_MODULENAME L"KLSTD"

void KLSTD_InitRandom();

#if defined(_WIN32) && defined(KLUSEFSSYNC)

typedef HRESULT (FS_PROC *pFSDrv_InitDeinitPtr)( PVOID* pContext );
typedef HRESULT (FS_PROC *pFSDrv_RegPtr)( PVOID pContext );

static HMODULE	g_hLibfssync = NULL;
pFSDrv_InitDeinitPtr pFSDrv_Init = NULL, pFSDrv_Done = NULL;
pFSDrv_RegPtr pFSDrv_Register = NULL, pFSDrv_UnRegister = NULL;

#endif

void KLSTD_InitThreadRegistration()
{
#if defined(_WIN32) && defined(KLUSEFSSYNC)	
		
	TCHAR szDir[MAX_PATH] = _T("");
    GetModuleFileName(NULL, szDir, KLSTD_COUNTOF(szDir)-1);    
	std::wstring wstrDllPath, wstrName, wstrExt; 
    KLSTD_SplitPath((const wchar_t*)KLSTD_T2CW2(szDir), wstrDllPath, wstrName, wstrExt);
	wstrDllPath += L"fssync.dll";

	g_hLibfssync=LoadLibrary( KLSTD_W2T2(wstrDllPath.c_str()) );
	if ( g_hLibfssync )
	{
		pFSDrv_Init = (pFSDrv_InitDeinitPtr)GetProcAddress(
													g_hLibfssync,
													"FSSync_UNIN");
		pFSDrv_Done = (pFSDrv_InitDeinitPtr)GetProcAddress(
													g_hLibfssync,
													"FSSync_UNDO");

		pFSDrv_Register = (pFSDrv_RegPtr)GetProcAddress(
								g_hLibfssync, "FSSync_UNRI" );

		pFSDrv_UnRegister = (pFSDrv_RegPtr)GetProcAddress(
								g_hLibfssync, "FSSync_UNUR");
	}
	else
	{
		KLSTD::Trace( 1, KLCS_MODULENAME,
            L"KLSTD_InitThreadRegistration: Error in LoadLIbrary - %d\n",			
            GetLastError() );
	}
    KLSTD::Trace( 1, KLCS_MODULENAME,
            L"KLSTD_InitThreadRegistration p1 - %u, p2 - %u, hLib = %u\n",            
			pFSDrv_Init,
			pFSDrv_Register,			
			g_hLibfssync);

#endif
}

void KLSTD_DeInitThreadRegistration()
{
#if defined(_WIN32) && defined(KLUSEFSSYNC)	
	if ( g_hLibfssync!=NULL ) 
	{
		FreeLibrary( g_hLibfssync );
		g_hLibfssync = NULL;
	}
#endif
}

//!\brief Регестрирует текущий поток в драйвере
void RegisterThreadInDriverInternal( bool doRegistration )
{
#if defined(_WIN32) && defined(KLUSEFSSYNC)
	
	HRESULT hInitResult = (-1), hRegisterResult = (-1);

	if ( pFSDrv_Init!=NULL && pFSDrv_Register!=NULL )
	{
		PVOID pContext = NULL;

		hInitResult = pFSDrv_Init( &pContext );
        if (SUCCEEDED( hInitResult ))
        {
			if ( doRegistration )
				hRegisterResult = pFSDrv_Register( pContext );
			else
				hRegisterResult = pFSDrv_UnRegister( pContext );

			pFSDrv_Done( &pContext );
        }
	}

    KLSTD::Trace( 1, KLCS_MODULENAME,
            L"bFSDrv_Init=%d, bFSDrv_RegisterInvisibleThread=%d, doRegistration - %d, p1 - %u, p2 - %u, hLib = %u\n",
            hInitResult, 
            hRegisterResult,
			doRegistration,
			pFSDrv_Init,
			pFSDrv_Register,			
			g_hLibfssync);
#endif
}

//!\brief Регестрирует текущий поток в драйвере
void RegisterThreadInDriver()
{
	RegisterThreadInDriverInternal( true );
}

//!\brief Дерегестрирует текущий поток в драйвере
void UnregisterThreadInDriver()
{
	RegisterThreadInDriverInternal( false );
}

using namespace KLSTD;

#if defined(_WIN32) && ! defined (_MBCS)
    #define CREATE_THREADS_WITH_PROCESS_TOKEN
#endif

#ifdef CREATE_THREADS_WITH_PROCESS_TOKEN
    struct SecurityAttributesHolder
    {
        CAutoPtr<MemoryChunk>   pSD;
        CAutoPtr<MemoryChunk>   pTokenDefaultDacl;
        CAutoPtr<MemoryChunk>   pTokenPrimaryGroup;
        SECURITY_ATTRIBUTES     sa;

        void Clear()
        {
            pSD = NULL;
            pTokenDefaultDacl = NULL;
            pTokenPrimaryGroup = NULL;
            memset( & sa, 0, sizeof( sa ) );
        }
    };

    static SecurityAttributesHolder g_securityAttributesForNewThreads;
#endif

#ifdef _WIN32
# include <crtdbg.h>
#endif

namespace KLSTD {

	struct ThreadStubParams
	{
		void			   *threadArgp;
		ThreadFunctionPtr	threadFunc;
		std::wstring		threadName;
		void			   *osDependData;
        // void *self; // pointer on caller Thread*Imp, still kludge
	};

#if defined(_WIN32) || defined (__unix__)
unsigned int KLSTD_THREADDECL ThreadStubFunction(void *arpg);
#endif

#if defined(_WIN32) || defined (__unix__)
	class ThreadWinImp : public Thread
	{
#ifdef __unix
        static const pthread_t bad_thread_key;
#endif
        friend unsigned int KLSTD_THREADDECL ThreadStubFunction(void *arpg);
	public:


		ThreadWinImp()
#ifdef __unix
            : _id( bad_thread_key )
#endif
		{
#ifndef __unix
			tHandle = NULL;
			threadId = 0;
#endif
		}

		~ThreadWinImp()
		{
#ifndef __unix
			if ( tHandle!=NULL )
              CloseHandle( tHandle );
#else		
#endif
		}

		//!\brief Функция создания и запуска потока.
		void Start( const wchar_t *name, ThreadFunctionPtr func, void *argp = NULL )
		{
#ifndef __unix
			if ( tHandle!=NULL ) CloseHandle( tHandle );
#endif

			ThreadStubParams *stubParams = new ThreadStubParams;
			stubParams->threadArgp = argp;
			stubParams->threadFunc = func;
			stubParams->threadName = name;
			// stubParams->self = static_cast<void *>(this); // kludge

#ifndef __unix

            PSECURITY_ATTRIBUTES pSA = NULL;

    #ifdef CREATE_THREADS_WITH_PROCESS_TOKEN
            bool bThreadImpersonated = false;

            {
                HANDLE hThreadToken = NULL;
                BOOL bRes =
                    ::OpenThreadToken(
                        ::GetCurrentThread(),
                        TOKEN_QUERY,
                        TRUE,
                        & hThreadToken );
                if( bRes )
                {
                    bThreadImpersonated = true;
                    CloseHandle( hThreadToken );
                } else
                {
                    DWORD dwErrCode = ::GetLastError();
                    if( dwErrCode != ERROR_NO_TOKEN )
                    {
                        KLERR_BEGIN
                            KLSTD_TRACE0(
                                1,
                                L"Cannot determine whether current thread impersonated or "
                                L"not - OpenThreadToken returned error "
                                L"(see message below). Will create new thread with "
                                L"security descriptor provided by system.\n" );
                            KLSTD_THROW_LASTERROR_CODE2( dwErrCode );
                        KLERR_ENDT(1)
                    }
                }
            }

            SECURITY_ATTRIBUTES sa;
            if( bThreadImpersonated && ( g_securityAttributesForNewThreads.pSD != NULL ) )
            {
                memset( & sa, 0, sizeof(sa) );
                sa.nLength = sizeof(sa);
                sa.bInheritHandle = FALSE;
                sa.lpSecurityDescriptor =
                    ( PSECURITY_DESCRIPTOR )( g_securityAttributesForNewThreads.pSD->GetDataPtr() );

                pSA = & sa;
            }
    #endif // CREATE_THREADS_WITH_PROCESS_TOKEN

			tHandle = (HANDLE)_beginthreadex( pSA, 0, ThreadStubFunction, stubParams, 0, &threadId );
			if ( tHandle==NULL )
				KLSTD_THROW2( STDE_CREATE_THREAD, name, GetLastError() );
#else		
			pthread_attr_t thr_attr;
			pthread_attr_init( &thr_attr );			
			pthread_attr_setstacksize( &thr_attr, KL_UNIX_STACK_SIZE );
			int ptres = pthread_create( &_id, &thr_attr, (void*(*)(void*))&ThreadStubFunction,stubParams);
			if (  ptres!= 0 ) 
			{
              _id = bad_thread_key; // - ptr
              KLSTD_THROW2( STDE_CREATE_THREAD, name, ptres );
            }
#endif
		}
		
		//!\brief Функция ожидания завершения потока.
		bool Join( int timeOut ) // timeout? point for port problems
		{
#ifndef __unix__
          if ( tHandle==NULL ) {
            KLSTD_NOINIT( L"KLSTD::Thread" );
          }

#else		
          if ( _id == bad_thread_key ) { // - ptr
            KLSTD_NOINIT( L"KLSTD::Thread" );
          }
#endif

#ifndef __unix
			if ( timeOut<0 ) timeOut = INFINITE;
			
			DWORD res = WaitForSingleObject( tHandle, timeOut );
			if ( res==WAIT_OBJECT_0 ) return true;
			else return false;
#else		
			int ptres = pthread_join( _id, 0 );
			if (  ptres!= 0 ) 
			{
              KLSTD_THROW1( STDE_SYSTEM, ptres );
            }
            _id = bad_thread_key;
			return true;
#endif
		}

	private:
#ifndef __unix
		HANDLE tHandle;				// handle потока
		UINT   threadId;			// id потока
#else		
		pthread_t _id;
#endif
		std::wstring threadName;		// имя потока				
	};

#ifdef __unix
const pthread_t ThreadWinImp::bad_thread_key = static_cast<pthread_t>(0);
#endif

	unsigned int KLSTD_THREADDECL ThreadStubFunction(void *arpg)
	{
        KLSTD_InitRandom();
#ifdef _WIN32
        set_terminate(KLSTD_GetTerminateCallback());
        set_unexpected(&terminate);
#endif
	#if defined(_WIN32) && defined(KLUSEFSSYNC)		
        RegisterThreadInDriver();
	#endif

		ThreadStubParams *sParams = (ThreadStubParams *)arpg;
		
		unsigned long res = 0;
		
		KLERR_BEGIN
			ThreadObjectsAutoCleaner tc(KLSTD_GetGlobalThreadStore());
			KLTRACE3( KLCS_MODULENAME, L"Thread started. Name - %ls, ThreadId - %X\n",
#ifndef __unix__
				sParams->threadName.c_str(), GetCurrentThreadId() );
#else				
				sParams->threadName.c_str(), pthread_self() );
#endif

			SetTraceThreadName( sParams->threadName.c_str() );

			res = sParams->threadFunc( sParams->threadArgp );

		KLERR_ENDT( 1 );

		UnregisterTraceThread();

#ifdef __unix
        // really kludge, but better than nothing
        // (see *join in ThreadWinImp::Join and ThreadWinImp::~ThreadWinImp)
        // I comment next line, due to evil not here,
        // but in destructor...
//        ((ThreadWinImp *)sParams->self)->_id = ThreadWinImp::bad_thread_key;
#endif

		void *osDependData = sParams->osDependData;
		delete sParams;

#if defined(_WIN32) && defined(KLUSEFSSYNC)
		UnregisterThreadInDriver();
#endif

#ifdef KL_OPENSSL_SUPPORT
		ERR_remove_state(0);
#endif

#if defined(_WIN32)
		_endthreadex( res );
#endif

		return res;
	}

#endif

#ifdef N_PLAT_NLM 		// Novell Netware

	#  include <nwnspace.h>

	void KLSTD_THREADDECL ThreadStubFunction(void *arpg)
	{
		ThreadStubParams *sParams = (ThreadStubParams *)arpg;		
				
		KLERR_BEGIN

			// swith on long name support
			BYTE oldNameSpace = SetCurrentNameSpace( 4 );

			KLTRACE3( KLCS_MODULENAME, L"Thread started. Name - %ls, ThreadId - %d\n",
				sParams->threadName.c_str(), GetThreadID() );

			SetTraceThreadName( sParams->threadName.c_str() );

			sParams->threadFunc( sParams->threadArgp );
			
            SetCurrentNameSpace( oldNameSpace );

		KLERR_ENDT( 1 );

		UnregisterTraceThread();

		void *osDependData = sParams->osDependData;
		delete sParams;

		// поток закончил работу сигналим об этом
		SignalLocalSemaphore( (LONG)osDependData );
		// закрываем поток
		ExitThread( TSR_THREAD, 0 );
	}

	class ThreadNLMImp : public Thread
	{
	public:

		ThreadNLMImp()
		{
			tHandle = NULL;			
		}

		~ThreadNLMImp()
		{			
		}
		
		//!\brief Функция создания и запуска потока.
		void Start( const wchar_t *name, ThreadFunctionPtr func, void *argp = NULL )
		{
          const unsigned thr_stack_size = 131072;
			ThreadStubParams *stubParams = new ThreadStubParams;
			stubParams->threadArgp = argp;
			stubParams->threadFunc = func;
			stubParams->threadName = name;
			threadStopSem = OpenLocalSemaphore(0);
			stubParams->osDependData = (void *)threadStopSem;

			tHandle = BeginThread( ThreadStubFunction, 0, thr_stack_size, stubParams );
			if ( tHandle==NULL )
				KLSTD_THROW2( STDE_CREATE_THREAD, name, tHandle );
		}
		
		//!\brief Функция ожидания завершения потока.
		bool Join( int timeOut )
		{
			if ( tHandle==NULL )
				KLSTD_NOINIT( L"KLSTD::Thread" );

			int dontWaitRes = 1;
			if ( timeOut<0 ) 
			{
              WaitOnLocalSemaphore( threadStopSem );
              dontWaitRes = 0;
			}
			else
			{
              dontWaitRes = TimedWaitOnLocalSemaphore( threadStopSem, timeOut );
			}

			CloseLocalSemaphore( threadStopSem );
			
			if ( !dontWaitRes ) return true;
			else return false;
		}

	private:
		int		tHandle;				// handle потока	

		std::wstring threadName;		// имя потока				
		LONG		 threadStopSem;
	};

#endif

} // namespace KLSTD

//!\brief Функция создания объекта типа Thread
KLCSC_DECL bool KLSTD_CreateThread(KLSTD::Thread** pThread)
{
#if defined(_WIN32) || defined (__unix)
	*pThread = new KLSTD::ThreadWinImp;
	KLSTD_CHKMEM(*pThread);

	return true;
#endif
#ifdef N_PLAT_NLM 		// Novell Netware
	*pThread = new KLSTD::ThreadNLMImp;
	KLSTD_CHKMEM(*pThread);

	return true;
#endif
#if !(defined(_WIN32) || defined(__unix) || defined(N_PLAT_NLM))
	KLSTD_NOTIMP();

	return false;
#endif
}

//!\brief Функция возвращает идентификатор текущего потока
KLCSC_DECL int KLSTD_GetCurrentThreadId()
{
#ifdef _WIN32
	return GetCurrentThreadId();
#elif defined(__unix)
	return (int)pthread_self();
#elif defined(N_PLAT_NLM)
	return GetThreadID();
#else
	#error "Unsupported platform, fix me!"
#endif
}

KLCSC_DECL int KLSTD_GetCurrentProcessId()
{
#ifndef N_PLAT_NLM
	return getpid();
#else 
	return GetNLMID();
#endif
}

//!\brief Изменяет приоритет текущего потока
KLCSC_DECL void KLSTD_SetCurrentThreadPriority( KLSTD::Thread::ThreadPriority priority )
{
#if defined(_WIN32)
	if ( priority==KLSTD::Thread::PriorityNormal )
		::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_NORMAL );
	else
		if ( priority==KLSTD::Thread::PriorityHigh )
			::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_HIGHEST );
		else
			if ( priority==KLSTD::Thread::PriorityLow )
				::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_LOWEST );
#endif
}


#ifdef CREATE_THREADS_WITH_PROCESS_TOKEN

CAutoPtr<MemoryChunk> GetTokenInformationInternal(
    HANDLE TokenHandle, 
    TOKEN_INFORMATION_CLASS TokenInformationClass )
{
    DWORD dwBufLen = 0;
    BOOL bResult =
        ::GetTokenInformation(
            TokenHandle,
            TokenInformationClass,
            NULL,
            0,
            & dwBufLen );

    DWORD dwLastError = ::GetLastError();
    if( bResult || ( dwLastError != ERROR_INSUFFICIENT_BUFFER ) )
    {
        KLSTD_TRACE1(
            1,
            L"First call to GetTokenInformation failed for TokenInformationClass=%d.\n",
            int(TokenInformationClass) );
        KLSTD_THROW_LASTERROR_CODE2( dwLastError );
    }
    
    CAutoPtr<MemoryChunk> pResult;
    KLSTD_AllocMemoryChunk( dwBufLen, & pResult );

    bResult =
        ::GetTokenInformation(
            TokenHandle,
            TokenInformationClass,
            pResult->GetDataPtr(),
            dwBufLen,
            & dwBufLen );

    if( ! bResult )
    {
        dwLastError = ::GetLastError();
        KLSTD_TRACE2(
            1,
            L"Second call to GetTokenInformation failed for TokenInformationClass=%d. Buffer len=%d\n",
            int(TokenInformationClass),
            dwBufLen );
        KLSTD_THROW_LASTERROR_CODE2( dwLastError );
    }

    return pResult;
}

#endif // CREATE_THREADS_WITH_PROCESS_TOKEN

void KLSTD_InitializeThreads()
{
#ifdef CREATE_THREADS_WITH_PROCESS_TOKEN
    HANDLE hPrimaryAccessToken = NULL;

    KLERR_TRY
        KLSTD_TRACE0(
            1,
            L"Initializing default security descriptor for newly "
            L"created threads (based on primary access token)...\n" );

        DWORD dwLastError = 0;

        KLSTD_AllocMemoryChunk( SECURITY_DESCRIPTOR_MIN_LENGTH, & g_securityAttributesForNewThreads.pSD );
        PSECURITY_DESCRIPTOR pSD = 
            ( PSECURITY_DESCRIPTOR )( g_securityAttributesForNewThreads.pSD->GetDataPtr() );

        BOOL bResult = ::InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION );
        if( ! bResult )
        {  
            KLSTD_THROW_LASTERROR();
        }

        bResult = ::OpenProcessToken( GetCurrentProcess(), TOKEN_READ, & hPrimaryAccessToken );
        if( ! bResult )
        {
            KLSTD_THROW_LASTERROR();
        }

        
        g_securityAttributesForNewThreads.pTokenDefaultDacl =
            GetTokenInformationInternal( hPrimaryAccessToken, TokenDefaultDacl );
        PTOKEN_DEFAULT_DACL pTokenDefaultDacl = 
            ( PTOKEN_DEFAULT_DACL ) ( g_securityAttributesForNewThreads.pTokenDefaultDacl->GetDataPtr() );

        bResult =
            ::SetSecurityDescriptorDacl(
                pSD, 
                TRUE,     // fDaclPresent flag   
                pTokenDefaultDacl->DefaultDacl, 
                TRUE
            );
        if( ! bResult )
        {  
            KLSTD_THROW_LASTERROR();
        }
        

        g_securityAttributesForNewThreads.pTokenPrimaryGroup =
            GetTokenInformationInternal( hPrimaryAccessToken, TokenPrimaryGroup );
        PTOKEN_PRIMARY_GROUP pTokenPrimaryGroup =
            ( PTOKEN_PRIMARY_GROUP ) ( g_securityAttributesForNewThreads.pTokenPrimaryGroup->GetDataPtr() );

        bResult =
            :: SetSecurityDescriptorGroup(
                pSD,
                pTokenPrimaryGroup->PrimaryGroup,
                TRUE
            );

        if( ! bResult )
        {  
            KLSTD_THROW_LASTERROR();
        }

        KLSTD_TRACE0(
            1,
            L"Default security descriptor for newly created threads initialized.\n" );
    KLERR_CATCH(pError)
        g_securityAttributesForNewThreads.Clear();
        KLERR_SAY_FAILURE( 1, pError );
        KLSTD_TRACE0(
            1,
            L"Cannot initialize default security descriptor for newly "
            L"created threads. Will still continue - new thread "
            L"will accept security descriptors provided by system (either from primary "
            L"or impersonation token).\n" );
    KLERR_ENDTRY

    if( hPrimaryAccessToken )
    {
        ::CloseHandle( hPrimaryAccessToken );
    }
#endif // CREATE_THREADS_WITH_PROCESS_TOKEN
}

void KLSTD_DeinitializeThreads()
{
#ifdef CREATE_THREADS_WITH_PROCESS_TOKEN
    g_securityAttributesForNewThreads.Clear();
#endif // CREATE_THREADS_WITH_PROCESS_TOKEN
}
