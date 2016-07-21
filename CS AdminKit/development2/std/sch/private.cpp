 /*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Private.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Реализация внутренних классов для модуля Scheduler.
 *
 */

#ifdef _WIN32
#  include <windows.h>
#  include <tchar.h>
#endif

#include "std/trc/trace.h"

#include <limits.h>
#include <stdio.h>

#ifdef N_PLAT_NLM
	#include <stdarg.h>
	#include <nwthread.h>
	#include <nwsemaph.h>
	#include <nwconio.h>
	#include <stdio.h>
	#include <time.h>
	#include <nwdebug.h>
	#include "std/conv/wcharcrt.h"
#endif

#ifdef __unix
#  include <stdarg.h>
#endif

#include "private.h"
#include "std/sch/common.h"
#include "std/sch/errors.h"

namespace KLSCH {

	const int DEFAULT_TRACE_LEVEL = 6;

/*!
    * \brief Класс LightMutex.
    *
    */ 
	
		//!\ Класс автоматической заблокировки mutex
		LightMutex::AutoUnlock::AutoUnlock( LightMutex *m )
		{			
			mtx = m;
//            XTRACE( "X1 " << std::hex << (unsigned)m << std::dec
//                    << ", " << pthread_self() );
			mtx->Lock();
//            XTRACE( "X2 " << std::hex << (unsigned)m << std::dec );
		}
		LightMutex::AutoUnlock::~AutoUnlock()
		{
//            XTRACE( "~X2 " << std::hex << (unsigned)mtx << std::dec );
			mtx->Unlock();
//            XTRACE( "~X1 " << std::hex << (unsigned)mtx << std::dec );
		}

	LightMutex::LightMutex()
	{
		criticalSection = NULL;
		KLSTD_CreateCriticalSection( &criticalSection );
	}

	LightMutex::~LightMutex()
	{
		criticalSection = NULL;
	}

	//!\brief Операция блокировки mutex
	void LightMutex::Lock()
	{
		criticalSection->Enter();
	}

	//!\brief Операция разблокировки mutex
	void LightMutex::Unlock()
	{
		criticalSection->Leave();
	}

	/*!
    * \brief Класс Semaphore.
    *
    */ 

	Semaphore::Semaphore( int initCount )
	{
		semHandle = NULL;
		KLSTD_CreateSemaphore( &semHandle, initCount );
	}
	Semaphore::~Semaphore()
	{
		semHandle = NULL;
	}


	/*!
	\brief Уменьшает значение семафора.
	
	  \param msec [in] Время ожидания в милисекундах. Для бесконечного ожидания c_Infinite.
	  \return true  - значение семафора бело успешно уменьшено
	  false - истекло время ожидания
	  
	*/
	bool Semaphore::Wait( int msec )
	{
		return semHandle->Wait( msec );
	}
	
	/*!
	\brief Увеличивает значение семафора.
	*/
	void Semaphore::Post()
	{
		semHandle->Post();
	}

	void Trace( int traceLevel, const char* format , ...)
	{
		int nBuf;
		char szBuffer[10240];
		va_list args;

		//if ( traceLevel>DEFAULT_TRACE_LEVEL ) return;
		traceLevel = DEFAULT_TRACE_LEVEL;
		
		va_start(args, format);

		//NASTraceTimeW();
/*#ifdef N_PLAT_NLM
		vsprintf(szBuffer, format, args);
		szBuffer[10240-1]=0;
//		ConsolePrintf("%s\n",szBuffer);
#else	*/	
#ifdef _WIN32
		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), format, args);		
#else
 # ifdef N_PLAT_NLM
		nBuf = vsprintf(szBuffer, format, args);
		szBuffer[10240-1]=0;		
 #else
		nBuf = vsnprintf(szBuffer, sizeof(szBuffer), format, args);		
 #endif 
#endif

		//NASASSERT(nBuf < sizeof(szBuffer)); //Output truncated as it was > sizeof(szBuffer)
		szBuffer[10240-1]=0;

		KLSTD::Trace( traceLevel, L"SCH", L"%hs", szBuffer );
		//fprintf(stdout, szBuffer);

		//fflush(stdout); 
//#endif // N_PLAT_NLM		
		va_end(args);
		
	}

} // namespace KLSCH
