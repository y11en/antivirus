/*!
* (C) 2002 "Kaspersky Lab"
*
* \file ThreadsPoolManager.cpp
* \author Шияфетдинов Дамир
* \date 2002
* \brief Реализация менеджера рабочих потоков транспорта
*
*/

#ifdef _WIN32
#pragma warning ( disable : 4786 ) // disable 'identifier was truncated to '255' characters in the debug information' warrning
#endif

#include <list>

#include "std/tp/threadspool.h"
#include "std/err/klerrors.h"
#include "threadspoolmanager.h"
#include "workingthread.h"

namespace KLTP {

	 class ThreadsPoolManagerImp : public KLSTD::KLBaseImpl<ThreadsPoolManager>
	 {
	 public:

		ThreadsPoolManagerImp()
		{
			criticalSec = NULL;
			KLSTD_CreateCriticalSection( &criticalSec );
			freeThrSem = NULL;
		}

		~ThreadsPoolManagerImp()
		{
			Stop();
		}


		void Stop()
		{
			ThreadsList::iterator it = threadsList.begin();

			while( it!=threadsList.end() )
			{
				// закрываем все рабочии потоки
				(*it)->Release();
				it = threadsList.erase(it);
			}

			it = stopingThreadsList.begin();
			while( it!=stopingThreadsList.end() )
			{				
				(*it)->Release();
				it = stopingThreadsList.erase(it);
			}

			freeThrSem->Release();
			criticalSec->Release();
		}

		//\brief Инициализирует менеджера потоков
		void Init( int maxThrds, KLTP::ThreadsPool *threadPool )
		{
			if ( freeThrSem != NULL ) return;

			KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

			if ( maxThrds<=0 ) maxThrds = c_DefualtMaxNumberOfThreads;

			for ( int tc = 0; tc < maxThrds; tc++ )
			{
				WorkingThread *wrkThrd = NULL;
				KLTP_CreateWorkingThread( &wrkThrd, tc, threadPool );
				threadsList.push_back( wrkThrd );
			}

			m_threadPool = threadPool;

			KLSTD_CreateSemaphore( &freeThrSem, maxThrds );
		}


		//\brief Добавляет задачу на выполнение
		void AddThread( void *params, bool useTaskQueue = false )
		{
			if ( freeThrSem == NULL ) return;

			// осовобождаем уже остановленные потоки
			{
				KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

				ThreadsList::iterator it = stopingThreadsList.begin();
				while( it!=stopingThreadsList.end() )
				{
					if ( (*it)->IsStopped() )
					{						
						(*it)->Release();
						it = stopingThreadsList.erase(it);
					}
					else ++it;
				}
			}

			bool reGet = true;
			while( reGet )
			{
				reGet = false;
				bool found = false;
				
				{
					KLSTD::AutoCriticalSection autoUnlocker( criticalSec );
					
					while( !found )
					{
						found = true;

						ThreadsList::iterator it = threadsList.begin();
						// ищем свободный раочий поток рабочий поток
						int threadCounter = 0;
						for( ; it!=threadsList.end(); ++it, threadCounter++ )
						{
							WorkingThread::ActionState thrState = (*it)->GetCurrentState();
							/*						if ( thrState==WorkingThread::Free )
							{
							(*it)->NewTask( params );
							freeThrSem->Wait();
							break;
							}
							*/
							if ( thrState==WorkingThread::ThreadNotStarted )
							{
								(*it)->SetWorkingThreadNumber( threadCounter + 1 );
								(*it)->NewTask( params );
								(*it)->StartThread( this );								
								break;
							}
						}
						
						if ( it==threadsList.end() )
							found = false;
						
						if ( !found )
						{			
							WorkingThread *wrkThrd = NULL;
							KLTP_CreateWorkingThread( &wrkThrd, threadsList.size(), m_threadPool );
							threadsList.push_back( wrkThrd );
						}
					}
				}
				
				if ( !found )
				{
					// нет свободных потоков
					if ( useTaskQueue )
					{
						// добавляем задачу в очередь
						tasksList.push_back( params );
					}
					else
					{
						// ждем освобождение потока
						freeThrSem->Wait();
						reGet = true;
					}
				}
			}
		}

		void DeleteNonusedThread()
		{
			WorkingThread *fThread = NULL;

			{
				KLSTD::AutoCriticalSection autoUnlocker( criticalSec );
				
				// ищем задачу у которой нет параметров
				ThreadsList::iterator it = threadsList.begin();
				for( ; it!=threadsList.end(); ++it )
				{
					if ( (*it)->GetParams()==NULL && !(*it)->IsCurrentThread())
					{
						fThread = (*it);
						threadsList.erase(it);


						fThread->StopThread();	// посылаем сигнал на остановку потока
						stopingThreadsList.push_back( fThread );

						break;
					}
				}
			}
		}

		//\brief Функция освобождения рабочего потока
		void FreeTask( WorkingThread *thr )
		{
			KLSTD::AutoCriticalSection autoUnlocker( criticalSec );

			// определяем статус потока
			freeThrSem->Post();

			if ( tasksList.size()!=0 )
			{
				void *params = *(tasksList.begin());

				if ( params!=NULL )
				{
					thr->NewTask( params );
					freeThrSem->Wait();
				}

				tasksList.pop_front();
			}				
		}

	 private:

		 typedef std::list<WorkingThread *> ThreadsList;		 


		 KLSTD::CriticalSection *criticalSec;	//<! Для доступа к переменным класса

		 ThreadsList			threadsList;	//!< Список рабочих потокв
		 ThreadsList			stopingThreadsList;	//!< Список остановляемых потоков
		 KLSTD::Semaphore	   *freeThrSem;		//!< Счет свободных потоков

		 typedef std::list<void *> TasksList;

		 TasksList tasksList;					//!< Список ожидающих задач

		 KLTP::ThreadsPool		*m_threadPool;
		 
	 };

} // end namespace KLTP

//\breif Функция создает объект класса ThreadsPoolManager
void KLTP_CreateThreadsPoolManager( KLTP::ThreadsPoolManager **threadManager, int maxThrds,
									 KLTP::ThreadsPool *threadPool )
{
	KLSTD_CHKOUTPTR(threadManager);

	*threadManager = new KLTP::ThreadsPoolManagerImp( );

	KLSTD_CHKMEM(*threadManager);

	(*threadManager)->Init( maxThrds, threadPool );
}
