/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file ThreadsPool.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Реализация пула потоков
 * 
 */
#include "std/base/kldefs.h"
#include "std/trc/trace.h"
#include "std/err/error.h"
#include "std/err/klerrors.h"
#include "std/time/kltime.h"

#include <list>
#include <vector>
#include <algorithm>

#include "std/thr/sync.h"

#include "std/tp/threadspool.h"
#include "threadspoolmanager.h"

//#include "../wat/windowsauthentication.h"

#define USE_ThreadsPoolAutoUnlocker KLSTD::AutoCriticalSection autoUnlocker( m_pCricSec );

namespace KLTP {	

	const wchar_t *KLCS_MODULENAME = L"KLTP";

	ThreadsPool *globalThreadsPool = NULL;

/*!
 * \brief Интерфейс пула потоков
 * 
 */	
	class ThreadsPoolImp : public ThreadsPool
	{
	public:
		
		enum WorkerState {
			WorkerWaiting,			
			WorkerSuspending
		};

		ThreadsPoolImp()
		{
			m_pCricSec = NULL;
			KLSTD_CreateCriticalSection( &m_pCricSec );
			internalCounter = 1;
			m_threadsManager = NULL;
			m_threadInUseCounter = 0;
			m_numThreads = 0;
			m_runOnlySystemWorkers = false;
			m_threadPoolGrowthTime = 0;
			m_numLowPriorityUseThreads = 0;
		}

		~ThreadsPoolImp()
		{
			Stop();
			if ( m_pCricSec ) m_pCricSec->Release();
		}
		
		//!\brief Функция инициализации пула потоков.
        virtual void Init( int maxNumberOfThreads )
		{
			if ( m_threadsManager!=NULL ) return;

			if ( maxNumberOfThreads<=0 ) return;
			if ( maxNumberOfThreads>c_MaxAllowablePoolsThreads ) 
				maxNumberOfThreads = c_MaxAllowablePoolsThreads;

			m_pTMRelSem = 0;
			KLSTD_CreateSemaphore( &m_pTMRelSem, 1 );

			KLTP_CreateThreadsPoolManager( &m_threadsManager, 
				maxNumberOfThreads, this );

			for( int tc = 0; tc < maxNumberOfThreads; tc++ )
				m_threadsManager->AddThread( NULL ); // добавляем задачи для запуска метода DoProcessing

			m_numThreads = maxNumberOfThreads;
			m_currentMaxLowPriorityThreads = m_numThreads>>2;
		}

		virtual void Stop( )
		{
			KLTP::ThreadsPoolManager *saveThreadsManager = NULL;;

			{
				USE_ThreadsPoolAutoUnlocker;

				if ( m_threadsManager==NULL ) return;
				saveThreadsManager = m_threadsManager;
				m_threadsManager = NULL;
			}

			m_pTMRelSem->Wait();

			saveThreadsManager->Release();
		}

		//!\brief Функция передачи управления пулу потоков.
		virtual void DoProcessing( void *params, int threadNum )
		{
			// пробигаем по всем задачям и вызываем каждую по одному разу
			std::vector<WorkerId> workerIdsList;
			WorkerId saveMaxWorkerId;
			WorkerId waitingWorkerId = (WorkerId)params;
			if ( waitingWorkerId!=0 ) params = NULL;

			{
				USE_ThreadsPoolAutoUnlocker;

				WorkersList::iterator it = m_workersList.begin();
				for(;it!=m_workersList.end();++it)				
					workerIdsList.push_back((*it)->wId);							

				saveMaxWorkerId = internalCounter;
			}

			if ( workerIdsList.size()==0 )
			{
				KLSTD_Sleep( 10 );
				return;
			}

			bool someWorkerRun = false;
			// список новых задач добавленных в процессе работы старых задач
			std::vector<WorkerId> newWorkerIdsList;
			do
			{	
				std::vector<WorkerId>::iterator it = workerIdsList.begin();
				for( ; it!=workerIdsList.end(); ++it )
				{
					WorkerDesc *fwDesc = NULL;
					bool bRunLowPriorityWorker = false;
					{
						USE_ThreadsPoolAutoUnlocker;

						bool bCanCallLowPriorityWorker = true;
						if ( m_numLowPriorityUseThreads>=m_currentMaxLowPriorityThreads ) bCanCallLowPriorityWorker = false;
						
						WorkersList::iterator wit = FindWorkerIteratorById( (*it) );
						if ( wit!=m_workersList.end() )
						{
							fwDesc = (*wit);

							if ( m_runOnlySystemWorkers && fwDesc->m_waitingWorker==false ) 
								continue;	// на данный момент запускаються только системные worker'ы

							if ( fwDesc->m_priority==PriorityLow && !bCanCallLowPriorityWorker ) continue;
							
							if ( !fwDesc->IsNeedToRun( waitingWorkerId ) ) continue;	// берем следующего "рабочего"
							fwDesc->PrepareToRun( threadNum );
							if ( fwDesc->m_priority==PriorityLow ) 
							{
								bRunLowPriorityWorker = true;
								m_numLowPriorityUseThreads++;
							}
						}					
					}
					
					if ( fwDesc ) {

						someWorkerRun = true;
						bool needSetState = (fwDesc->m_waitingWorker==false);
						WorkerId saveWorkerId = fwDesc->wId;

						if ( needSetState ) SetWorkerState( saveWorkerId, true );

						fwDesc->RunWorker();		
						
#ifdef _DEBUG
//						KLSTD::CAutoPtr<KLWAT::AuthServer> pAuthServer;
//						KLWAT_GetGlobalAuthServer( &pAuthServer );
//						KLSTD_CHKMEM(pAuthServer);
//						KLASSERT( pAuthServer->IsThreadImpersonated()==false );
#endif
						WorkerType saveWType = fwDesc->wtype;

						fwDesc->AfterRun();

						if ( saveWType==RunOnce )
						{
							// удаляем рабочего из списка
							DeleteWorker( saveWorkerId );
						}

						if ( needSetState ) SetWorkerState( saveWorkerId, false );
					}

					{
						USE_ThreadsPoolAutoUnlocker;

						if ( bRunLowPriorityWorker ) m_numLowPriorityUseThreads--;
					}
				}

				newWorkerIdsList.clear();

				{
					// проверяем не появились ли новые задачи в списке
					USE_ThreadsPoolAutoUnlocker;
	
					WorkersList::iterator it = m_workersList.begin();
					for(;it!=m_workersList.end();++it)	{
						if ( (unsigned)(*it)->wId>=(unsigned)saveMaxWorkerId &&
							std::find(workerIdsList.begin(), workerIdsList.end(), (*it)->wId)==
							workerIdsList.end() ) 
						{							
							newWorkerIdsList.push_back( (*it)->wId );
						}
					}
					saveMaxWorkerId = internalCounter;
				}

				workerIdsList = newWorkerIdsList;

			} while( workerIdsList.size()!=0 );

			if ( !someWorkerRun ) KLSTD_Sleep( 10 );
		}

		//!\brief Функция добавления callback функции в пул потоков.
		virtual void AddWorker2( WorkerId *workerId, const wchar_t *workerName, 
			Worker *worker, WorkerType wtype = RunNormal,
			bool serializableRun = true, int periodTime = (-1),
			WorkerPriority priority = PriorityNormal )
		{
			USE_ThreadsPoolAutoUnlocker;

			if ( internalCounter==(-1) ) internalCounter++;
			WorkerId newWorkerId = internalCounter++;

			WorkerDesc *wDesc = new WorkerDescImp( newWorkerId, worker, serializableRun,
				periodTime, wtype, priority );

			KLTRACE3( L"TP", L"ThreadsPool::AddWorker wId - %d WorkerName - '%ls' Type - %d\n", 
				newWorkerId, workerName, RunNormal );

			if ( wDesc ) m_workersList.push_back( wDesc );

			*workerId = newWorkerId;
		}


		//!\brief Функция добавления callback функции в пул потоков.
		virtual void AddWorker( WorkerId *workerId, const wchar_t *workerName, 
			Worker *worker, WorkerType wtype = RunNormal,
			bool serializableRun = true, int periodTime = (-1) )
		{
			AddWorker2( workerId, workerName, worker, wtype, serializableRun, periodTime );
		}


		//!\brief Функция добавления ожидающего "рабочего" в пул потоков.
		virtual void AddWaitingWorker( WorkerId *workerId, const wchar_t *workerName, 
			Worker *worker, bool serializableRun = true, bool createThreadForWorker = false,
			bool runWorkerOnlyInCreatedThread = false )
		{
			USE_ThreadsPoolAutoUnlocker;

			if ( internalCounter==(-1) ) internalCounter++;
			WorkerId newWorkerId = internalCounter++;

			WorkerDesc *wDesc = new WorkerDescImp( newWorkerId, worker, serializableRun,
				(-1), RunNormal, PriorityNormal );

			wDesc->m_waitingWorker = true;
			wDesc->m_useOneThread = createThreadForWorker;
			wDesc->m_runOnlyInCreatedThread = runWorkerOnlyInCreatedThread;

			KLTRACE3( L"TP", L"ThreadsPool::AddWorker wId - %d WorkerName - '%ls' Type - %d\n", 
				newWorkerId, workerName, RunNormal);

			if ( wDesc ) m_workersList.push_back( wDesc );

			if ( createThreadForWorker && m_threadsManager!=NULL )
			{
				KLTRACE1( L"TP", L"ThreadsPool. Add new thread in ThreadsPool for useOneThread worker. Current number of threads - %d\n",
					m_numThreads );

				m_threadsManager->AddThread( (void*)newWorkerId ); // добавляем поток
			}

			*workerId = newWorkerId;
		}

		//!\brief Приостанавливает работу добавленной функции.
		virtual bool SuspendWorker( WorkerId workerId )
		{
			WorkerDesc *wDesc = NULL;
			{
				USE_ThreadsPoolAutoUnlocker;

				wDesc = FindWorkerById( workerId );
				if ( wDesc==NULL ) return false;
				
				if ( wDesc->wstate == WorkerSuspending ) return true;
				if ( wDesc->wtype==ThreadsPool::RunOnce ) return false;

				//!< Ожидаем окончание работы всех запущенных в данный момент функций
				wDesc->wstate = WorkerSuspending;
			}

			// ждем заверщения текущей обработки
			if ( wDesc->serializableRun )
			{
				wDesc->pRunCricSec->Enter();
				wDesc->pRunCricSec->Leave();

			}
			else
			{
				wDesc->pRunSem->Wait();
				wDesc->pRunSem->Post();
			}

			return true;
		}

		//!\brief Востанавливает работу приостанавленной функции.
		virtual bool ResumeWorker( WorkerId workerId )
		{
			USE_ThreadsPoolAutoUnlocker;

			WorkerDesc *wDesc = FindWorkerById( workerId );
			if ( wDesc==NULL ) return false;

			if ( wDesc->wstate == WorkerSuspending ) 			
				wDesc->wstate = WorkerWaiting;
			else return false;

			return true;
		}

		//!\brief Удаляет рабочую функцию из пула поток
		virtual bool DeleteWorker( WorkerId workerId )
		{
			WorkerDesc *wDesc = NULL;
			{
				USE_ThreadsPoolAutoUnlocker;

				WorkersList::iterator it = FindWorkerIteratorById( workerId );
				if ( it==m_workersList.end() ) return false;
				else
				{	
					wDesc = (*it);
					if ( wDesc->wstate==WorkerSuspending ) return false; 
					if ( wDesc->wtype==RunOnce ) 
					{
						if ( wDesc->descDeleting ) return false;	// даннай worker уже удалется
						wDesc->descDeleting = true;
					}
					else m_workersList.erase( it );
				}
			}

			if ( wDesc!=NULL )
			{
				if ( wDesc->wtype==RunOnce ) 
				{
					// ждем момента запуска Worker'а
					wDesc->pRunSem->Wait();

					// удаляем описатель из списка
					USE_ThreadsPoolAutoUnlocker;
					
					WorkersList::iterator it = FindWorkerIteratorById( workerId );
					if ( it==m_workersList.end() ) return false;
					else m_workersList.erase( it );
				}

				if ( wDesc->serializableRun ) 
				{
					wDesc->pRunCricSec->Enter();
					wDesc->pRunCricSec->Leave();
                } 
				else 
				{
					wDesc->pRunSem->Wait();
                }

				// ждем завершение текущих операций над WorkerDesc
				wDesc->WaitOperationEnd();

				wDesc->Release();
				return true;
			}

			return false;
		}

		//!\brief Функция перевода рабочего потока пула в состояния 'in use'
		virtual void SetWorkerState( WorkerId workerId, bool inUse )
		{
			KLERR_BEGIN
				
			bool needAddNewWorkerThread = false;
			bool needDelNonusedWorkerThread = false;
			KLTP::ThreadsPoolManager *pTPM = NULL;			
			{
				USE_ThreadsPoolAutoUnlocker;

				if ( inUse ) m_threadInUseCounter++;
				else m_threadInUseCounter--;

				if ( (m_numThreads - m_threadInUseCounter) <= c_ThreadReserveNumber &&
					m_numThreads < c_MaxAllowablePoolsThreads ) 
				{
					needAddNewWorkerThread = true;
					m_numThreads++;					// прибавляем один поток					
					m_threadPoolGrowthTime = KLSTD::GetSysTickCount();
					m_currentMaxLowPriorityThreads = m_numThreads>>2;
				}

				if ( (m_numThreads - m_threadInUseCounter) >= (c_ThreadReserveNumber * 2) + 1 &&
					m_numThreads > c_DefaultNumberOfPoolsThreads )
				{
					if ( KLSTD::DiffTickCount(KLSTD::GetSysTickCount(), m_threadPoolGrowthTime) > c_ThreadPoolDeleteThreadTimeout )
					{
						needDelNonusedWorkerThread = true;
						m_numThreads--;
						m_currentMaxLowPriorityThreads = m_numThreads>>2;
					}
				}

				if ( m_numThreads >= c_MaxAllowablePoolsThreads &&
					(m_numThreads - m_threadInUseCounter) <= ( c_ThreadReserveNumber + 1 ) )				
					m_runOnlySystemWorkers = true;
				else 
					m_runOnlySystemWorkers = false;

				if ( m_numThreads >= c_MaxAllowablePoolsThreads && needAddNewWorkerThread )
				{
					KLTRACE1( L"TP", L"ERROR in ThreadsPool. Max number of threads was exceeded\n" );
				}

				if ( m_threadsManager!=NULL ) 
				{
					pTPM = m_threadsManager;
					m_pTMRelSem->Wait();
				}
			}
	
			if ( needAddNewWorkerThread && pTPM!=NULL ) // прибавляем один поток
			{
				KLTRACE1( L"TP", L"ThreadsPool. Add new thread in ThreadsPool. Current number of threads - %d\n",
					m_numThreads );

				KLERR_BEGIN
					pTPM->AddThread( NULL ); // добавляем поток
				KLERR_ENDT( 3 )
			}

			if ( needDelNonusedWorkerThread && pTPM!=NULL )
			{
				KLTRACE1( L"TP", L"ThreadsPool. Delete thread in ThreadsPool. Current number of threads - %d\n",
					m_numThreads );

				KLERR_BEGIN
					pTPM->DeleteNonusedThread(); 
				KLERR_ENDT( 3 )
			}

			if ( pTPM!=NULL ) m_pTMRelSem->Post();
			
			KLERR_ENDT( 1 )
		}

		//\brief Возвращает кол-во потоков в пуле потоков
		virtual int GetThreadsNumber()
		{
			USE_ThreadsPoolAutoUnlocker;

			return m_numThreads;			
		}
		
	protected:
		// Внутренние переменные класса
		

		class WorkerDesc : public KLSTD::KLBase
		{
		public:
			void Init( WorkerId id, Worker	*wrk, bool sRun, int pTime, 
				WorkerType type, WorkerPriority p )
			{
				wId = id, worker = wrk, wtype = type, wstate = WorkerWaiting;
				serializableRun = sRun;
				periodTime = pTime;
				firstStartFlag = true;
				dontRunWorker = false;
				lastRunTime = 0;
				if ( worker ) worker->AddRef();
				runNumberNow = 0, runCounter = 0;
				m_waitingWorker = false;
				m_useOneThread = false;
				m_runOnlyInCreatedThread = false;
				descDeleting = false;
				pRunCricSec = NULL;
				m_priority = p;
				KLSTD_CreateCriticalSection( &pRunCricSec );
				pRunSem = NULL;
				if ( type==RunOnce )
				{
					serializableRun = true;
					KLSTD_CreateSemaphore( &pRunSem, 0 );
				}
				else
					KLSTD_CreateSemaphore( &pRunSem, 1 );

				pSerCricSec = NULL;
				KLSTD_CreateCriticalSection( &pSerCricSec );

				m_threadNum = (-1);
			}
			~WorkerDesc()
			{
				if ( worker ) worker->Release();

				if ( pRunCricSec ) pRunCricSec->Release();
				if ( pSerCricSec ) pSerCricSec->Release();

				if ( pRunSem ) pRunSem->Release();
			}
			bool IsNeedToRun( WorkerId waitingWorkerId )
			{
				KLSTD::AutoCriticalSection autoUnlocker( pSerCricSec );

				if ( dontRunWorker ) return false;
				if ( serializableRun && runNumberNow!=0 ) return false;

				if ( waitingWorkerId!=0 && /*m_waitingWorker && На данном потоке может запускаться только waiter поток ( без данного комментария есть deadlock) */ 
					wId!=waitingWorkerId ) 
					return false;	// на данном потоке должен запускаться только опеределенный ждущий воркер

				if ( m_useOneThread && 
					( !m_runOnlyInCreatedThread || waitingWorkerId!=wId ) ) 
					return false;	// должен запускать только в своем потоке

				if ( periodTime>0 ) 
				{
					if ( firstStartFlag || 
						 KLSTD::DiffTickCount(KLSTD::GetSysTickCount(), lastRunTime) >= (unsigned)periodTime )
					{
						firstStartFlag = false;
						lastRunTime = KLSTD::GetSysTickCount();
						return true;
					}
					else return false;
				}
				
				return true;
			}
			void PrepareToRun( int threadNum )
			{
				KLSTD::AutoCriticalSection autoUnlocker( pSerCricSec );

				if ( runNumberNow==0 ) {
					if ( serializableRun )
						pRunCricSec->Enter();
					else
						pRunSem->Wait();
                }
				AddRef();
				runNumberNow++;
				m_threadNum = threadNum;
				if ( wtype==RunOnce ) 
				{
					pRunSem->Post();		// Выставляем признак того, что RunOnce worker запушен
					dontRunWorker = true;
				}
			}
			int RunWorker()
			{
				int res = 0;
				if ( worker )
				{
					KLERR_BEGIN

						res = worker->RunWorker( wId );

					KLERR_ENDT( 3 )

					runCounter++;
				}
				return res;
			}
			void AfterRun()
			{
				KLSTD::AutoCriticalSection autoUnlocker( pSerCricSec );

				m_threadNum = (-1);
				runNumberNow--;
				if ( runNumberNow==0 ) {
					if ( serializableRun )
						pRunCricSec->Leave();
					else
						pRunSem->Post();
                }
				this->Release();
			}
			void WaitOperationEnd()
			{
				KLSTD::AutoCriticalSection autoUnlocker( pSerCricSec );
			}
			
			WorkerId	wId;
			Worker		*worker;
			WorkerType	wtype;
			WorkerState	wstate;			
			unsigned int volatile runNumberNow;		//!< Кол-во одновременно запущенных функций
			unsigned long volatile runCounter;		//!< Общее кол-во запусков
			KLSTD::Semaphore	   *pRunSem;		//!< Используется для контроля состояния
			KLSTD::CriticalSection *pRunCricSec;
			KLSTD::CriticalSection *pSerCricSec;	//!< Используется для контроля состояния
			bool		serializableRun;			//!< Последовательный вызов callback'ов 
			int			periodTime;					//!< Время периода запуска (в милисекундах)


			unsigned long		lastRunTime;				//!< Последнее время запуска "рабочего" в тиках
			bool				firstStartFlag;			//!< Признак первого запуска

			bool				m_waitingWorker;	//!< Признак ждущего воркера
			bool				m_useOneThread;		//!< Признак использования отдельного потока 
								// ( в данном потоке не будут вызываться другие ждущие воркеры )
			bool				m_runOnlyInCreatedThread;
			WorkerPriority		m_priority;			//!< Приоритет worker'а
			int					m_threadNum;
			bool				dontRunWorker;			//!< Больше данный воркер запускать не надо
			bool				descDeleting;			//!< Признак удаляемого описания ( используется только для RunOnce )
		};

		class WorkerDescImp : public KLSTD::KLBaseImpl<WorkerDesc>
		{
		public:
			WorkerDescImp( WorkerId id, Worker	*wrk, bool serializableRun, 
				int periodTime, WorkerType type, WorkerPriority priority )
			{
				Init( id, wrk, serializableRun, periodTime, type, priority);
			}
		};

		typedef std::list<WorkerDesc *> WorkersList;

		KLSTD::CriticalSection	*m_pCricSec;			//!< Для доступа к переменным класса

		WorkerId				internalCounter;	//!< Счетчик идентификаторов
		WorkersList				m_workersList;		//!< Список задача пула потоков

		int						m_numThreads;		//!< Количество потоков в пуле 

		int						m_threadInUseCounter;	//! Счетчик потоков с флагом ThreadInUse

		bool					m_runOnlySystemWorkers;	//! Запускаются только системные потоки

		KLTP::ThreadsPoolManager		*m_threadsManager;	//!< Менеджер реальных потоков ОС
		KLSTD::CAutoPtr<KLSTD::Semaphore>	m_pTMRelSem;	

		unsigned long			m_threadPoolGrowthTime;		//! Время последнего расширения пула потоков

		int						m_numLowPriorityUseThreads;		//! Текущее кол-во потоков в которых в данный момент выполняются worker'ы с низким приоритетом
		int						m_currentMaxLowPriorityThreads;	//! Максимальное кол-во потоков доступных для низкоприоритетных worker'ов

	private:
		// Вспомогательные функции класса
		WorkerDesc *FindWorkerById( WorkerId wid )
		{
			WorkersList::iterator it = m_workersList.begin();
			for(;it!=m_workersList.end();++it)
			{
				if ( (*it)->wId==wid ) return (*it);
			}
			return NULL;
		}
		WorkersList::iterator FindWorkerIteratorById( WorkerId wid )
		{
			WorkersList::iterator it = m_workersList.begin();
			for(;it!=m_workersList.end();++it)
			{
				if ( (*it)->wId==wid ) return it;
			}
			return m_workersList.end();
		}
		bool DeleteWorkerFromList( WorkerId wid )
		{
			WorkersList::iterator it = m_workersList.begin();
			for(;it!=m_workersList.end();++it)
			{
				if ( (*it)->wId==wid ) return true;
			}

			return false;
		}

		/*!
			\brief Возвращает текущий статус
		*/
		bool GetWorkerInfo( WorkerId workerId, int &poolsThreadsNumber, 
			WorkerStatus &status )
		{
			USE_ThreadsPoolAutoUnlocker;

			WorkersList::iterator it = m_workersList.begin();
			for(;it!=m_workersList.end();++it)
			{
				if ( (*it)->wId==workerId ) 
				{
					poolsThreadsNumber = (*it)->m_threadNum;
					return true;
				}
			}

			return false;
		}
	};

} // end namespace KLTP

/*
	\breif Функция создает объект класса ThreadsPool
*/
void KLTP_CreateThreadsPool( KLTP::ThreadsPool **threadsPool )
{
	KLSTD_CHKOUTPTR(threadsPool);

	*threadsPool = new KLTP::ThreadsPoolImp();	
	(*threadsPool)->Init( KLTP::c_DefaultNumberOfPoolsThreads );

	KLSTD_CHKMEM(*threadsPool);
}

/*
	\brief Функция возвращает указатель на глобальный ThreadsPool
*/
KLCSC_DECL KLTP::ThreadsPool *KLTP_GetThreadsPool()
{
	KLASSERT( KLTP::globalThreadsPool!=NULL );

	return KLTP::globalThreadsPool;
}

IMPLEMENT_MODULE_INIT_DEINIT(KLTP)

//\brief функция инициализация библиотеки.
IMPLEMENT_MODULE_INIT_BEGIN(KLTP)

	KLTP_CreateThreadsPool( &KLTP::globalThreadsPool );

IMPLEMENT_MODULE_INIT_END()

/*\brief Функция особождает глобальный объект класса ThreadsPool*/
IMPLEMENT_MODULE_DEINIT_BEGIN(KLTP)

	KLTP::ThreadsPool *saveP = KLTP::globalThreadsPool;
	KLTP::globalThreadsPool = NULL;
	saveP->Stop();
	delete saveP;

IMPLEMENT_MODULE_DEINIT_END()
