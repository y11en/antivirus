/*!
* (C) 2002 "Kaspersky Lab"
*
* \file WorkingThread.cpp
* \author Шияфетдинов Дамир
* \date 2002
* \brief Реализация рабочего потока транспорта
*
*/

#include "std/thr/sync.h"
#include "std/thr/thread.h"
#include "std/err/klerrors.h"

#include "workingthread.h"
#include "threadspoolmanager.h"

namespace KLTP {

	/*!
	* \brief Рабочий поток транспорта 
	*/	

	const wchar_t c_WorkingThreadName[] = L"WorkingThread";

	const wchar_t KLCS_MODULENAME[] = L"KLTP";

	class WorkingThreadImp : public KLSTD::KLBaseImpl<WorkingThread>
	{
	public:

		WorkingThreadImp( int num, KLTP::ThreadsPool *threadsPool )
		{
			number = num;

			taskParams = NULL;
			taskThreadsPool = threadsPool;

			curState = WorkingThread::ThreadNotStarted;

			criticalSec = NULL;
			KLSTD_CreateCriticalSection( &criticalSec );						

			timeToDie = false;
			thread = NULL;
			manager = NULL;
			m_threadNumber = (-1);
			threadId = (-1);
			m_threadStopped = false;
		}

		~WorkingThreadImp()
		{
			timeToDie = true;			

			if ( thread!=NULL )
			{
				KLERR_BEGIN
					
					// ждем окончания потока
					thread->Join();

				KLERR_ENDT(3)
			}

			criticalSec->Release();			

			if ( thread!=NULL ) delete thread;
		}

		//\brief Функция производит запуск потока
		void StartThread( ThreadsPoolManager *man )
		{
			if ( thread!=NULL ) return;

			manager = man;

			KLSTD_CreateThread( &thread );

			thread->Start( c_WorkingThreadName, ThreadStub, this );
		}

		//\brief Функция производит останов потока ( без ожидания окончания работы потока )
		void StopThread()
		{
			timeToDie = true;
		}

		//\brief Функция определяет остановился ли рабочий поток
		bool IsStopped()
		{
			KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

			if ( thread!=NULL )
			{
				return m_threadStopped;
			}
			else return true;
		}



		//\brief Запускает рабочий поток на выполнения транспортной задачи
		void NewTask( void *params )
		{
			KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

			taskParams = params;

			curState = WorkingThread::Working;
			
		}

		//\brief Возвращает текущее состояние потока
		WorkingThread::ActionState GetCurrentState()
		{
			KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

			return curState;
		}

		//\brief Задает номер для данного потока ( по умолчанию равен (-1) )
		void SetWorkingThreadNumber( int wNum )
		{
			KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

			m_threadNumber = wNum;
		}

		//\brief Возвращает параметры потока
		void *GetParams()
		{
			KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

			return taskParams;
		}

		//\brief Проверяет является ли текущий поток, потоком WorkingThread
		bool IsCurrentThread()
		{
			KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

			if ( threadId==(-1) ) return false;
			if ( threadId==KLSTD_GetCurrentThreadId() ) return true;

			return false;
		}

		//\brief Функция потока
		void Run( )
		{
			threadId = KLSTD_GetCurrentThreadId();

			while(1)
			{
				if ( timeToDie ) break;	// заканчивает поток
				
				// обрабатываем транспортную задачу
				if ( taskThreadsPool!=NULL )
					taskThreadsPool->DoProcessing( taskParams, m_threadNumber );
				else
				{
					// изменяем текущее состояние потока
					KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

					taskParams = NULL;
					taskThreadsPool = NULL;

					curState = WorkingThread::Free;
					manager->FreeTask( this );
				}
			}

			
			m_threadStopped = true;
		}

		static unsigned long KLSTD_THREADDECL ThreadStub( void *argp )
		{
			((WorkingThreadImp *)argp)->Run();
			return 0;
		}

	private:

		KLSTD::CriticalSection *criticalSec;	//!< Для доступа к переменным класса
		
		void* taskParams;			//!< Описатель текущей задачи
		KLTP::ThreadsPool *taskThreadsPool;	//!< Пул потоков

		WorkingThread::ActionState  curState;			//!< Текущее состояние потока

		int			number;				//!< Номер потока в пуле потоков

		KLSTD::Thread *thread;			//!< Поток
		int			   threadId;

		bool		timeToDie;			//!< Признак завершения работы потока

		ThreadsPoolManager *manager;	//!< Менеджер потоков

		int			m_threadNumber;		//!< Номер потока
			
		bool		m_threadStopped;	//!< Поток остановлен

	protected:

	};

} // end namespace KLTP

/*
	\breif Функция создает объект класса WorkingThread
*/
void KLTP_CreateWorkingThread( KLTP::WorkingThread **workingThread, int threadNum,
								KLTP::ThreadsPool *threadPool )
{
	KLSTD_CHKOUTPTR(workingThread);

	*workingThread = new KLTP::WorkingThreadImp( threadNum, threadPool );

	KLSTD_CHKMEM(*workingThread);
}
