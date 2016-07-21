/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file ThreadsPool.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Интерфейс пула потоков
 * 
 */

#ifndef KLTP_THREADSPOOL_H
#define KLTP_THREADSPOOL_H

#include "std/base/klbase.h"

namespace KLTP {	

    const int c_DefaultNumberOfPoolsThreads = 8;

/*!
 * \brief Интерфейс пула потоков
 * 
 */	
	class ThreadsPool  
	{
	public:

		typedef int WorkerId;

		/*\brief Класса "рабочего" пула потоков */
		class Worker : public KLSTD::KLBase
		{
		public:
			virtual int RunWorker( WorkerId wId ) = 0;
		};

		/*\brief Тип*/
		enum WorkerType {
			RunOnce,			//! Метод RunWorker будет вызван только один раз 
			RunNormal			//! Нормальный запуск. Функция будет вызываться до тех пор
								// пока не будет вызван метод DeleteWorker. Функция 
								// может быть одновременно вызванна из различных потоков
		};
		
		/*\brief Тип*/
		enum WorkerPriority {						
			PriorityLow,				//! Низкий приоритет выполнения 									
					//! ( для выполнения выделяется не более 20% всех потоков пула и сумарно не более 10 потоков )
			PriorityNormal			//! Нормальные приоритет выполнения воркера 
		};

		/*\brief Статус*/
		enum WorkerStatus {
			WaitStatus,
			RunStatus,
			RunInWaitingMode
		};

		virtual ~ThreadsPool() {}
		
		/*!
			\brief Функция инициализации пула потоков.
	
			\param maxNumberOfThreads [in] Максимальное количество потоков.
					Если данный параметра равен 0 используется single thread модуль.
		*/
        virtual void Init( int maxNumberOfThreads = c_DefaultNumberOfPoolsThreads ) = 0;

		/*!
			\brief Останавливает работу пула потоков
		*/
		virtual void Stop( ) = 0;

		/*!
			\brief Функция передачи управления пулу потоков.
				
			В рамках данной функции производиться обработка всех зарегестрированных
			функцийю. Вызов данной функции единственная возможность работы с пулом 
			потоков в однопотоковой модели.			

			Вызов данной функции приводит к однократному вызову всех 
			зарегестрированных функций.
		*/
		virtual void DoProcessing( void *param, int threadNum ) = 0;

		/*!
			\brief Функция добавления "рабочего" в пул потоков.

			Добавленный при помощи данной функции функция будет вызываться
			переодически из всех потоков пула.
			
			Вызываемеая функция ( RunWorker )  не должна производить ожидания сигнала 
			на начало выполнения необходимой операции. Если на момент вызова у 
			функции нет задачи на выполнения, функция должна немедленно вернуть управляние.

			\param workerName [in] Имя "рабочего"			
			\param worker [in] "Рабочий"
			\param serializableRun [in] Елси true то callback не будет вызван одновременно
					из разных потоков пула
			\param periodTime [in] Период времени запуска ( в милисекундах )
			\param wtype [in] Тип запуска
			\return Идентификатор новой функции в пуле потоков
		*/
		virtual void AddWorker( WorkerId *workerId, const wchar_t *workerName, 
			Worker *worker, WorkerType wtype = RunNormal,
			bool serializableRun = true, int periodTime = (-1) ) = 0;

		/*!
			\brief Функция добавления ожидающего "рабочего" в пул потоков.

			Данный тип "рабочего" ждет активности в своей подсистеме.
			Используется для добавеления базовых системных "рабочих" ( например транспортный и
			"рабочий" системы Eventing )
		*/
		virtual void AddWaitingWorker( WorkerId *workerId, const wchar_t *workerName, 
			Worker *worker, bool serializableRun = true, bool createThreadForWorker = false,
			bool runWorkerOnlyInCreatedThread = true ) = 0;

		/*!
			\brief Приостанавливает работу добавленной функции.

			После вызова функция, идентифицируемая переданным идентификатором,
			вызываться не будет до вызова ResumeWorker

			\param workerId [in] Идентификатор функции
			\return Результат операции. flase - с таким идентификатором функции нет
		*/
		virtual bool SuspendWorker( WorkerId workerId ) = 0;

		/*!
			\brief Востанавливает работу приостанавленной функции.

			\param workerId [in] Идентификатор функции
			\return Результат операции. flase - с таким идентификатором функции нет
		*/
		virtual bool ResumeWorker( WorkerId workerId ) = 0;

		/*!
			\brief Удаляет рабочую функцию из пула поток

			Данная функция удаляет ранее добавелнную функцию из списка и освобождает 
			параметр функции - context.
		
			\param workerId [in] Идентификатор функции
			\return Результат операции. flase - с таким идентификатором функции нет
		*/
		virtual bool DeleteWorker( WorkerId workerId ) = 0;

		/*!
			\brief Возвращает текущий статус
		*/
		virtual bool GetWorkerInfo( WorkerId workerId, int &poolsThreadsNumber,
			WorkerStatus &status ) = 0;

		/*!
			\brief Функция перевода рабочего потока пула в состояния 'in use'

			\param workerId [in] Идентификатор функции
		*/
		virtual void SetWorkerState( WorkerId workerId, bool inUse ) = 0;

		/*
			\brief Возвращает кол-во потоков в пуле потоков
		*/
		virtual int GetThreadsNumber() = 0;
		
		/*!
			\brief Функция добавления "рабочего" в пул потоков.
		*/
		virtual void AddWorker2( WorkerId *workerId, const wchar_t *workerName, 
			Worker *worker, WorkerType wtype = RunNormal,
			bool serializableRun = true, int periodTime = (-1),
			WorkerPriority priority = PriorityNormal ) = 0;
		
	};

} // end namespace KLTP


/*
	\brief Функция возвращает указатель на глобальный ThreadsPool
*/
KLCSC_DECL KLTP::ThreadsPool *KLTP_GetThreadsPool();


#endif // !defined(KLTP_THREADSPOOL_H)
