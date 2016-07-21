/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Trace.h
 * \author Шияфетдинов Дамир
 * \date 2002
 * \brief Функции вывода трассировачной информации.
 *
 */

#include "std/base/kldefs.h"
#include "std/conv/klconv.h"
/*
#ifdef _WIN32
#pragma warning ( disable : 4786 ) // disable 'identifier was truncated to '255' characters in the debug information' warrning
#endif
*/
#include <list>
#include <map>
#include <string>

#include <stdlib.h>

#ifdef N_PLAT_NLM 		// Novell Netware
#  include "wcharcrt.h"
#  include <nwthread.h>
#endif

#include <cwchar>

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "std/trc/trace.h"
// #include "sync.h"

#ifdef _WIN32
    #include <atlbase.h>
    #include <crtdbg.h>
    #include <process.h>
#else	
	#include <iostream>
	#include <sstream>
	#include <fstream>
#endif

#ifdef USE_BOOST_THREADS
	#include <boost/thread/thread.hpp>
	#include <boost/thread/mutex.hpp>
	#include <boost/thread/recursive_mutex.hpp>
	#include <boost/thread/condition.hpp>
	#include <boost/thread/xtime.hpp>

	typedef boost::detail::thread::lock_ops<boost::recursive_mutex> recursive_mutex_lock_ops;	
#endif

#include <sstream>

#if defined(__unix) || defined(N_PLAT_NLM)

#include <iostream>
#include <iomanip>

#include "../conv/_print.h"


#if defined(__unix)
    #include <sys/utsname.h>
#endif // __unix

#endif // __unix || N_PLAT_NLM

#if defined(_WIN32)// && defined(_DEBUG)
	static void outputDebugStringV(
								const wchar_t*	module,
								const wchar_t*	format,
								va_list			args)
	{
		USES_CONVERSION;
		wchar_t		szBuffer[2048]=L"";
		const int	nBuffer=sizeof(szBuffer)/sizeof(szBuffer[0]);
		int result=0;

		if(0)//if(module)
		{
			const int nCharsToStore=nBuffer/2;
			result=_snwprintf(szBuffer, nCharsToStore, L"%ls: ", module);
			szBuffer[nCharsToStore-1]=0;
			if(result < 0)
				wcscat(szBuffer, L"...\n");
			OutputDebugString(W2CT(szBuffer));
		};
		const int nCharsToStore=nBuffer/2;
		result=_vsnwprintf(szBuffer, nCharsToStore, format, args);
		szBuffer[nCharsToStore-1]=0;
		if(result < 0)
			wcscat(szBuffer, L"...\n");
		OutputDebugString(W2CT(szBuffer));
	};

	static void outputDebugString(
								const wchar_t*	module,
								const wchar_t*	format,
								...)
	{
		va_list args;
		va_start(args, format);
		outputDebugStringV(module, format, args );
		va_end(args);
	};

#endif

namespace
{
    void GetSystemVersionInfo(
                std::wstring& wstrOsName, 
                std::wstring& wstrOsVer)
    {
    #if defined(_WIN32)
        OSVERSIONINFO osv;
        KLSTD_ZEROSTRUCT(osv);
        osv.dwOSVersionInfoSize  = sizeof(osv);
        if(GetVersionEx(&osv))
        {
            wstrOsName = (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
                            ?   L"Microsoft Windows NT"
                            :   L"Microsoft Windows 9x";
            std::wostringstream os;
            os << osv.dwMajorVersion 
                            << L"." 
                            << osv.dwMinorVersion 
                            << L", " 
                            << osv.dwBuildNumber;
            if(osv.szCSDVersion && osv.szCSDVersion[0])
                os << L", " << (const wchar_t*)KLSTD_T2CW2(osv.szCSDVersion);
            wstrOsVer = os.str();
        };
    #elif defined(__unix__)
		    struct utsname osname;
		    int res = uname( &osname );
		    if ( res == 0 )
            {
                wstrOsName = (const wchar_t*)KLSTD_A2CW2(osname.sysname);
                std::wostringstream os;
                if(osname.release[0])
                    os << (const wchar_t*)KLSTD_A2CW2(osname.release);
                if(osname.version[0])
                    os << L", " << (const wchar_t*)KLSTD_A2CW2(osname.version);
                wstrOsVer = os.str();
            };                
    #endif		
    };

};


namespace KLSTD{

//!\brief Возращает текущее количество милисекунд
// int GetCurrentMillisecondsString()

// temporary solution: reflection of the same function in sync
// Should be fixed... via std.dll, that include std/*, yes?
static unsigned long GetCurrentMilliseconds()
{
#ifdef _WIN32
  SYSTEMTIME tempSysTime;
  GetLocalTime( &tempSysTime );
  return tempSysTime.wMilliseconds;
#endif
#ifdef __unix
#ifdef USE_BOOST_THREADS
		std::wstringstream s;
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		return xt.nsec / 1000000;
#else
		timespec t;
		__impl::Thread::gettime( &t );
		return t.tv_nsec / 1000000;
#endif
#endif
#ifdef N_PLAT_NLM
  return 0;//time(0) * 1000;
#endif
}

#if defined(_WIN32) || defined (__unix) 
class TraceCriticalSection
{
#ifdef USE_BOOST_THREADS
		boost::recursive_mutex m_cs;
#else
	#ifdef _WIN32	
		CRITICAL_SECTION m_cs;
	#else
		__impl::MutexSDS _lock;
	#endif
#endif

  public:
    TraceCriticalSection()
      {
#ifdef USE_BOOST_THREADS	
#else
	#ifdef _WIN32
        ::InitializeCriticalSection(&m_cs);
	#endif
#endif
      }
		
    virtual ~TraceCriticalSection()
      {
#ifdef USE_BOOST_THREADS	
#else
	#ifdef _WIN32
        DeleteCriticalSection(&m_cs);
	#endif
#endif
      }

    void Enter()
      {
#ifdef USE_BOOST_THREADS
		recursive_mutex_lock_ops::lock( m_cs );
#else
	#ifdef _WIN32
        ::EnterCriticalSection(&m_cs);
	#else
        _lock.lock();
	#endif
#endif
      }

    void Leave()
      {
#ifdef USE_BOOST_THREADS
		recursive_mutex_lock_ops::unlock( m_cs );
#else
	#ifdef _WIN32
        ::LeaveCriticalSection(&m_cs);
	#else
        _lock.unlock();
	#endif
#endif
      }
};
#elif defined(N_PLAT_NLM)
#  include <nwsemaph.h>

	class TraceCriticalSection
	{
		LONG m_sem;
	public:
		TraceCriticalSection()
		{
			m_sem = OpenLocalSemaphore(1);
		};
		
		virtual ~TraceCriticalSection()
		{
			CloseLocalSemaphore( m_sem );
		};

		void Enter()
		{
			WaitOnLocalSemaphore( m_sem );
		};

		void Leave()
		{
			SignalLocalSemaphore( m_sem );
		};
	};
#endif
//#endif

	class TraceAutoUnlock
	{
	public:
		TraceAutoUnlock()
		{
			m_cs = NULL;
		}
		TraceAutoUnlock( TraceCriticalSection *cs )
		{
			Init( cs );
		};
		~TraceAutoUnlock( )
		{
			if ( m_cs!=NULL ) m_cs->Leave();
		};
		void Init( TraceCriticalSection *cs )
		{
			m_cs = cs;
			m_cs->Enter();
		};
	private:
		TraceCriticalSection *m_cs;
	};

	//!< Строка открытия log файла
	const wchar_t c_START_LOG_FILE_STR[] = 
		L"============================================================================\n"
		L"========== Trace Log File. Pid=%u. Started - %hs ===========\n"
		L"============================================================================\n";

	//!< Строка зкарытия log файла
	const wchar_t c_STOP_LOG_FILE_STR[] = 
		L"============================================================================\n"
		L"========== Trace Log File. Pid=%u. Closed - %hs ============\n"
		L"============================================================================\n";

	//!< Функция возвращает строковое представления текущего времени
	inline std::string GetCurrentTimeString( int traceFlags )
	{
		struct tm nowtime;
		time_t now;

		time( &now );		
		KL_LOCALTIME( now, nowtime );

		char timeStr[128];
		memset(timeStr,0,128);
		if ( traceFlags & TF_PRINT_MILLISECONDS )
		{
			char milisecondsString[64];
			int milliseconds = GetCurrentMilliseconds();

			sprintf( milisecondsString, ".%03d", milliseconds );			

			strftime( timeStr, sizeof(timeStr), "%a %b %d %Y %H:%M:%S", &nowtime );
			strcat( timeStr, milisecondsString );
		}
		else
			strftime( timeStr, sizeof(timeStr), "%a %b %d %Y %H:%M:%S", &nowtime );
		return timeStr;
	}

	long KLGetCurrentThreadId()
	{
		long threadId = 0;
#ifdef WIN32
		threadId = GetCurrentThreadId( );
#endif
#ifdef __unix
		threadId = (long)pthread_self();
#endif
#ifdef N_PLAT_NLM
        threadId = GetThreadID();
#endif
		return threadId;
	}

    //<-- Changed by andkaz 08.04.2004 10:10:35
    //! Check whether Output debug string is enabled
    static bool IsDebugOutEnabled();
    // -->

	void WriteStartLogString( FILE *traceOut )
	{
#if defined(_WIN32)// && defined(_DEBUG)
        if(IsDebugOutEnabled())
		    outputDebugString(
                    NULL,
                    c_START_LOG_FILE_STR, 
                    getpid(),
			        GetCurrentTimeString( TF_PRINT_MILLISECONDS ).c_str());
#endif
		// записываем строчку открытия log файла
#ifndef __unix__
		fwprintf( traceOut, c_START_LOG_FILE_STR, 
#else
                KLSTD_USES_CONVERSION; 
		fprintf( traceOut,KLSTD_W2CA(c_START_LOG_FILE_STR), 
#endif
#ifdef N_PLAT_NLM
                        0,  
#else
			getpid(),
#endif
			GetCurrentTimeString( TF_PRINT_MILLISECONDS ).c_str() );	



    std::wstring wstrOsName, wstrOsVer;

    GetSystemVersionInfo(wstrOsName, wstrOsVer);

    fprintf(traceOut, "System:\t %ls\nVersion:\t%ls\n\n", wstrOsName.c_str(), wstrOsVer.c_str());





		fflush( traceOut );
	}

	void WriteStopLogString( FILE *traceOut )
	{
#if defined(_WIN32)// && defined(_DEBUG)
        if(IsDebugOutEnabled())
		    outputDebugString(
                NULL,
                c_STOP_LOG_FILE_STR, 
                getpid(),
			    GetCurrentTimeString( TF_PRINT_MILLISECONDS ).c_str());
#endif
		// записываем строчку открытия log файла
#ifndef __unix__
		fwprintf( traceOut, c_STOP_LOG_FILE_STR, 
#else
		KLSTD_USES_CONVERSION;
		fprintf( traceOut, KLSTD_W2CA(c_STOP_LOG_FILE_STR), 
#endif
#ifdef N_PLAT_NLM
                        0,  
#else
			getpid(),
#endif
            GetCurrentTimeString( TF_PRINT_MILLISECONDS ).c_str() );	
		fflush( traceOut );
	}

	struct TraceModuleDesc	//!< Структура описания трассировачного модуля
	{
		std::wstring moduleName;
		int			 traceLevel;
		std::string	 traceLogFile;
		FILE		 *traceOut;
	};	
	typedef std::list<TraceModuleDesc*> TracedModulesList;

	struct TraceThreadDesc	//!< Струткутра описания трассировачного потока
	{	
		std::wstring threadName;
		int			 traceLevel;
		std::string	 traceLogFile;
		FILE		 *traceOut;
	};
	typedef std::list<TraceThreadDesc*> TracedThreadsList;

	struct ThreadDesc //!< Описатель зарегестрированного в trace подсистеме потока
	{		
		std::wstring threadName;
		long		 threadId;
	};
	typedef std::map<long, ThreadDesc*> RegistredThreadsMap;

	
	struct TraceInfo;

	TraceInfo             *klTraceInfo = NULL;
	TraceCriticalSection  *klTraceCricSec = NULL;
	bool				  firstStartTraceCall = true;

	//!< Структура описания параметров и текущего состояния подсистемы трассировки
	struct TraceInfo
	{
		TraceInfo()
		{
			traceLevel = 0;
			traceOut = stdout;
			traceFlags = 0;
			criticalSec = NULL;
			criticalSec = new TraceCriticalSection;
			tracedModulesMinTraceLevel = 100;
		}

		~TraceInfo()
		{
			TracedModulesList::iterator tmIt = tracedModulesList.begin();
			while( tmIt!=tracedModulesList.end() )
			{				
				if ( (*tmIt)->traceOut!=stdout ) fclose( (*tmIt)->traceOut );
				delete (*tmIt);
				tmIt = tracedModulesList.erase( tmIt );
			}

			TracedThreadsList::iterator ttIt = tracedThreadsList.begin();
			while( ttIt!=tracedThreadsList.end() )
			{				
				if ( (*ttIt)->traceOut!=stdout ) fclose( (*ttIt)->traceOut );
				delete (*ttIt);
				ttIt = tracedThreadsList.erase( ttIt );
			}

			RegistredThreadsMap::iterator rtIt = registeredThreadsMap.begin();
			while( rtIt!=registeredThreadsMap.end() )
			{
				delete (*rtIt).second;
				registeredThreadsMap.erase( rtIt );
				rtIt = registeredThreadsMap.begin();
			}

			delete criticalSec;
		}

		TraceModuleDesc *GetTracedModuleDesc( const wchar_t *moduleName )
		{
			TraceModuleDesc *fDecs = NULL;
			criticalSec->Enter();

			TracedModulesList::iterator it = tracedModulesList.begin();
			for( ; it != tracedModulesList.end(); ++it )
			{
				if ( (*it)->moduleName == moduleName ) {
					fDecs = (*it);
					break;
				}
			}
			criticalSec->Leave();
			return fDecs;
		}

		TraceThreadDesc *GetTracedThreadDesc( const wchar_t *threadName )
		{
			TraceThreadDesc *fDecs = NULL;
			criticalSec->Enter();

			TracedThreadsList::iterator it = tracedThreadsList.begin();
			for( ; it != tracedThreadsList.end(); ++it )
			{
				if ( (*it)->threadName == threadName ) {
					fDecs = (*it);
					break;
				}
			}
			criticalSec->Leave();
			return fDecs;
		}

		TraceThreadDesc *GetTracedThreadDesc( long threadId )
		{
			TraceThreadDesc *fDecs = NULL;
			criticalSec->Enter();

			RegistredThreadsMap::iterator fIt = registeredThreadsMap.find( threadId );
			if ( fIt==registeredThreadsMap.end() ) 
			{
				criticalSec->Leave();
				return NULL;
			}

			TracedThreadsList::iterator it = tracedThreadsList.begin();
			for( ; it != tracedThreadsList.end(); ++it )
			{
				if ( (*it)->threadName == (*fIt).second->threadName ) {
					fDecs = (*it);
					break;
				}
			}
			criticalSec->Leave();
			return fDecs;
		}

		bool IsModulesFiltered( int tl )
		{
			//if ( tl<tracedModulesMinTraceLevel ) return false;

			return tracedModulesList.empty() ? false : true;
		}

		bool IsThreadsFiltered( )
		{
			return tracedThreadsList.empty() ? false : true;
		}

		void AddTracedThread( const wchar_t *threadName, int trLevel,
			char *logFName )
		{
			criticalSec->Enter();

			if ( GetTracedThreadDesc( threadName )!=NULL )
			{
				criticalSec->Leave();
				return; 
			}

			TraceThreadDesc *threadDesc = new TraceThreadDesc;
			
			threadDesc->threadName = threadName;
			if ( trLevel==0 )
				threadDesc->traceLevel = traceLevel;
			else
				threadDesc->traceLevel = trLevel;
			threadDesc->traceOut = stdout;
			
			if ( logFName ) threadDesc->traceLogFile = logFName;
						
			if ( threadDesc->traceLogFile.empty() )
				threadDesc->traceLogFile = traceLogFile;

			if ( !threadDesc->traceLogFile.empty() )
			{
				threadDesc->traceOut = fopen( threadDesc->traceLogFile.c_str(), "a" );
				if ( threadDesc->traceOut==0 ) //не смогли открыть файл
				{
					threadDesc->traceLogFile.erase();
					threadDesc->traceOut = stdout;
				}
			}

			tracedThreadsList.push_back( threadDesc );
			
			criticalSec->Leave();

			if ( !threadDesc->traceLogFile.empty() )			
				WriteStartLogString( threadDesc->traceOut );
		}

		void AddTracedModule( const wchar_t *moduleName, int trLevel,
			char *logFName )
		{
			criticalSec->Enter();

			if ( GetTracedModuleDesc( moduleName )!=NULL )
			{
				criticalSec->Leave();
				return; 
			}

			TraceModuleDesc *moduleDesc = new TraceModuleDesc;
			
			moduleDesc->moduleName = moduleName;
			if ( trLevel==0 )
				moduleDesc->traceLevel = traceLevel;	// устанавливаем общий trace level
			else
				moduleDesc->traceLevel = trLevel;

			moduleDesc->traceOut = stdout;
			
			if ( logFName ) moduleDesc->traceLogFile = logFName;
						
			if ( moduleDesc->traceLogFile.empty() )
				moduleDesc->traceLogFile = traceLogFile;

			if ( !moduleDesc->traceLogFile.empty() )
			{
				moduleDesc->traceOut = fopen( moduleDesc->traceLogFile.c_str(), "a" );
				if ( moduleDesc->traceOut==0 ) //не смогли открыть файл
				{
					moduleDesc->traceLogFile.erase();
					moduleDesc->traceOut = stdout;
				}
				
			}			

			if ( moduleDesc->traceLevel<tracedModulesMinTraceLevel )
				tracedModulesMinTraceLevel = moduleDesc->traceLevel;

			tracedModulesList.push_back( moduleDesc );
			
			criticalSec->Leave();

			if ( !moduleDesc->traceLogFile.empty() &&
				moduleDesc->traceLogFile!=traceLogFile)
				WriteStartLogString( moduleDesc->traceOut );
		}

		void RegisterThread( const wchar_t *threadName )
		{
			ThreadDesc *threadDesc = new ThreadDesc;
			
			threadDesc->threadName = threadName;
			threadDesc->threadId = KLGetCurrentThreadId();
			
			criticalSec->Enter();
			
			if ( registeredThreadsMap.find( threadDesc->threadId )==
				registeredThreadsMap.end() )
			{
				registeredThreadsMap[threadDesc->threadId] = threadDesc;
			}
			else
				delete threadDesc;
			
			criticalSec->Leave();
		}

		void UnregisterThread( int threadId )
		{
			criticalSec->Enter();
			
			RegistredThreadsMap::iterator fIt = registeredThreadsMap.find( threadId );
			if ( fIt!=registeredThreadsMap.end() )
			{
				delete (*fIt).second;
				registeredThreadsMap.erase( fIt );
			}

			criticalSec->Leave();
		}
		

		int				traceLevel;		//!< Текущий уровень вывода ( по умолчанию 1 )
		FILE*			traceOut;		//!< Файл для вывода ( по умолчанию stdout )
		std::string		traceLogFile;	//!< Имя файла для вывода
		int				traceFlags;		//!< Флаги трассировки

		TraceCriticalSection  *criticalSec;

		TracedModulesList		tracedModulesList;	//!< Список модулей для трасировки
		int						tracedModulesMinTraceLevel; //!< Минимальный уровень трассировки для модлуей
		TracedThreadsList		tracedThreadsList;	//!< Список имен потоков для трасировки

		RegistredThreadsMap registeredThreadsMap;	//!< Список зарегестрированных потоков
	};

    //<-- Changed by andkaz 08.04.2004 10:10:35
    //! Check whether Output debug string is enabled
    static bool IsDebugOutEnabled()
    {		
        bool bResult = false;
        if(klTraceCricSec)
        {
            TraceAutoUnlock au(klTraceCricSec);
            bResult = klTraceInfo && ((klTraceInfo->traceFlags & TF_ENABLE_DEBUGOUT) != 0);
        };		
        return bResult;
    }
    // -->

	//!\brief Функция инициализации системы Trace. 
	void InitTrace()
	{
		if ( klTraceCricSec!=NULL ) return;
		
		klTraceCricSec = new TraceCriticalSection;
		klTraceInfo = new TraceInfo;
	}

	//!\brief Функция запуска системы Trace. 
	void StartTrace( int traceLevel, char *logFileName, int traceFlags )
	{
		if ( klTraceCricSec==NULL ) return;

		klTraceCricSec->Enter();

		if ( traceLevel>=1 ) klTraceInfo->traceLevel = traceLevel;

		klTraceInfo->traceFlags = traceFlags;

		if ( logFileName==NULL || logFileName[0]==0 ) 
		{
			if ( klTraceInfo->traceOut!=stdout &&
				klTraceInfo->traceOut!=NULL ) 
			{
				// записываем строчку закрытия log файла и закрываем сам файл
				WriteStopLogString( klTraceInfo->traceOut );
				fclose( klTraceInfo->traceOut );
			}
			klTraceInfo->traceOut = stdout;
			klTraceInfo->traceLogFile.erase();
			if ( firstStartTraceCall )
			{
				firstStartTraceCall = false;
				WriteStartLogString( klTraceInfo->traceOut );
			}
		}
		else
		{
			klTraceInfo->traceOut = fopen( logFileName, "a" );
			if ( klTraceInfo->traceOut==0 ) //не смогли открыть файл
				klTraceInfo->traceOut = stdout;
			else
				klTraceInfo->traceLogFile = logFileName;

			// записываем строчку открытия log файла
			WriteStartLogString( klTraceInfo->traceOut );
		}

		klTraceCricSec->Leave();
	}

	//!\brief Функция производит остановку системы трассировки
	void StopTrace()
	{
		if ( klTraceCricSec==NULL ) return;

		klTraceCricSec->Enter();

		klTraceInfo->traceLevel = 0;

		if ( klTraceInfo->traceOut!=stdout &&
			klTraceInfo->traceOut != NULL ) 
		{
			// записываем строчку закрытия log файла и закрываем сам файл
			WriteStopLogString( klTraceInfo->traceOut );
			fclose( klTraceInfo->traceOut );
			klTraceInfo->traceOut = NULL;
		}

		klTraceCricSec->Leave();
	}

	//!\brief Функция возвращает текущий статус трассировки
	bool IsTraceStarted( int *curTraceLevel )
	{
		if ( klTraceCricSec==NULL ) return false;

		bool res = false;
		klTraceCricSec->Enter();

		if ( klTraceInfo->traceLevel>0 ) {
			res = true;
		}
		if ( curTraceLevel!=NULL) *curTraceLevel = klTraceInfo->traceLevel;

		klTraceCricSec->Leave();

		return res;
	}

	//!\brief Функция деинициализация подсистемы трассировки
	void DeinitTrace()
	{
		if ( klTraceInfo==NULL ) return;

		// записываем строчку закрытия log файла
		if ( klTraceInfo->traceOut!=NULL)
		{
			WriteStopLogString( klTraceInfo->traceOut );	
			if ( klTraceInfo->traceOut!=stdout ) fclose( klTraceInfo->traceOut );
		}

		delete klTraceInfo;
		delete klTraceCricSec;
		klTraceInfo = NULL;
		klTraceCricSec = NULL;
	}

	//!\brief Функция спецификации модулей участвующих в трассировке
	void StartTraceModule( const wchar_t *moduleName, int traceLevel,
		char *logFileName )
	{
		if ( klTraceInfo==NULL ) return;
	
		klTraceInfo->AddTracedModule( moduleName, traceLevel, logFileName );
	}

	//!\brief Функия определения потоков участвующих в трассировке.
	void StartTraceThread( const wchar_t *threadName, int traceLevel,
		char *logFileName )
	{
		if ( klTraceInfo==NULL ) return;

		klTraceInfo->AddTracedThread( threadName, traceLevel, logFileName );
	}

	//!\brief Функция производит соотвествие между текущим потоком и указанным именем
	void SetTraceThreadName( const wchar_t *threadName )
	{
		if ( klTraceInfo==NULL ) return;

		klTraceInfo->RegisterThread( threadName );
	}

	/*!
		\brief Функия удаляет текущий поток из внутреннего списка ( см. SetTraceThreadName )
	*/
	void UnregisterTraceThread()
	{
		if ( klTraceInfo==NULL ) return;

		klTraceInfo->UnregisterThread( KLGetCurrentThreadId() );
	}


	//!\brief Функция возвращает файловый указатель для вывода Trace сообщений. 
	void GetTraceLogFileName( char *logFileName, int *strSizeInChar )
	{
		if ( klTraceInfo==NULL ) return;

		TraceAutoUnlock unlocker( klTraceCricSec );

		*strSizeInChar = klTraceInfo->traceLogFile.length() + 1;
		if ( logFileName!=NULL ) 
			strcpy( logFileName, klTraceInfo->traceLogFile.c_str() );
	}

	//!\brief Общая функция вывода Trace информации
	void Trace(	int traceLevel, const wchar_t *module, const wchar_t *format, ... )
	{
		if ( klTraceInfo && traceLevel <= klTraceInfo->traceLevel )
		{
			va_list args;
			va_start(args, format);
			
			TraceCommon( traceLevel, module, format, args );
			
			va_end(args);
		}
#ifdef N_PLAT_NLM
		else {
			ThreadSwitch();		
		}
#endif

	}

	/** @name Функции вывода трассировачной информации с определенными уровнями вывода*/
	/** Вывод с traceLevel равным 1 */
	void Trace1( const wchar_t *module, const wchar_t *format, ... )
	{
		va_list args;
		va_start(args, format);
		
		TraceCommon( 1, module, format, args );
		
		va_end(args);
	}

	/** Вывод с traceLevel равным 2 */
	void Trace2( const wchar_t *module, const wchar_t *format, ... )
	{
		va_list args;
		va_start(args, format);
		
		TraceCommon( 2, module, format, args );
		
		va_end(args);
	}

	/** Вывод с traceLevel равным 3 */
	void Trace3( const wchar_t *module, const wchar_t *format, ... )
	{
		va_list args;
		va_start(args, format);
		
		TraceCommon( 3, module, format, args );
		
		va_end(args);
	}	

	/** Вывод с traceLevel равным 4 */
	void Trace4( const wchar_t *module, const wchar_t *format, ... )
	{
		va_list args;
		va_start(args, format);
		
		TraceCommon( 4, module, format, args );
		
		va_end(args);
	}	

	/** Вывод с traceLevel равным 5 */
	void Trace5( const wchar_t *module, const wchar_t *format, ... )
	{
		va_list args;
		va_start(args, format);
		
		TraceCommon( 5, module, format, args );
		
		va_end(args);
	}	

	//!\brief Общая функция трассировачного вывода 
	void TraceCommon( int traceLevel, const wchar_t *module, const wchar_t *format,
		va_list args )
	{
		if ( klTraceInfo==NULL ) return;
		TraceAutoUnlock unlocker;

		if ( !(klTraceInfo->traceFlags & TF_NOT_SYNCHRONIZE) )
			unlocker.Init( klTraceCricSec );

		FILE *out = klTraceInfo->traceOut;
		int level = klTraceInfo->traceLevel;
		
		if ( klTraceInfo->IsModulesFiltered( traceLevel ) )
		{
			TraceModuleDesc *fDesc = klTraceInfo->GetTracedModuleDesc( module );
			if ( fDesc!=NULL )
				out = fDesc->traceOut, level = fDesc->traceLevel;
			else return;
		}

		if ( klTraceInfo->IsThreadsFiltered() )
		{
			TraceThreadDesc *fDesc = klTraceInfo->GetTracedThreadDesc( KLGetCurrentThreadId() );
			if ( fDesc!=NULL )
				out = fDesc->traceOut, level = fDesc->traceLevel;
			else return;
		}

		if ( traceLevel <= level && traceLevel!=0 )
		{
			if ( klTraceInfo->traceFlags==0 ){
				vfwprintf( out, format, args );
			}
			else
			{
#if defined (__unix) //|| defined (N_PLAT_NLM) 
                std::wstringstream s_out;
				if ( klTraceInfo->traceFlags & TF_PRINT_DATETIME )
                  s_out << GetCurrentTimeString( klTraceInfo->traceFlags ).data() << " ";
#else
				char	dateTimeString[128];		
				wchar_t additionalInfoStr[256]; 
				wchar_t moduleFormatedString[256];

				additionalInfoStr[0]=0;
				dateTimeString[0]=0;
#endif

#if defined (_WIN32) || defined (N_PLAT_NLM)
				if ( klTraceInfo->traceFlags & TF_PRINT_DATETIME ) {
					strcat( dateTimeString, 
						GetCurrentTimeString( klTraceInfo->traceFlags ).c_str() );
				}
#endif
				if ( klTraceInfo->traceFlags & TF_PRINT_THREAD_ID )
				{
#if defined(_WIN32)  || defined(N_PLAT_NLM)
					swprintf( additionalInfoStr, L" %08X", KLGetCurrentThreadId() );
#else
                    s_out << std::hex
                          << std::setiosflags(std::ios_base::right)
                          << std::setfill('0')
                          << std::setw(8)
                          << KLGetCurrentThreadId();
#endif
				}

				if ( klTraceInfo->traceFlags & TF_PRINT_MODULE )
				{
#if defined(_WIN32)  || defined(N_PLAT_NLM) 
					swprintf( moduleFormatedString, L" %6ls", module );
					wcscat( additionalInfoStr, moduleFormatedString );
#else
                    s_out << ' ' << module;
#endif
				}
				
#if defined(__unix)
				std::wofstream fs( fileno(out) );
				fs << s_out.str() << " ";
                _safe_printer( fs, format, args );
#else
 				fwprintf( out, L"%hs %ls: ", dateTimeString, additionalInfoStr );
				vfwprintf( out, format, args );
#endif
#if defined(_WIN32)// && defined(_DEBUG)
                if(IsDebugOutEnabled())
                {
                    wchar_t additionalBuff[512];
				    KLSTD_SWPRINTF(additionalBuff, KLSTD_COUNTOF(additionalBuff), L"%hs %ls: ", dateTimeString, additionalInfoStr );
                    additionalBuff[KLSTD_COUNTOF(additionalBuff)-1]=0;                
                    outputDebugStringV(module, additionalBuff, args);
				    outputDebugStringV(module, format, args);
                };
#endif

			}

			fflush( out );
		}
#ifdef N_PLAT_NLM
		else	ThreadSwitch();		
#endif

	}


} // end namespace KLSTD
