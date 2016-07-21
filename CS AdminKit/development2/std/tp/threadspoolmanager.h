/*!
* (C) 2002 "Kaspersky Lab"
*
* \file ThreadsPoolManager.h
* \author Шияфетдинов Дамир
* \date 2002
* \brief Интерфейс менеджера рабочих потоков транспорта
*
*/

#ifndef KLTP_THREADSPOOLMANAGER_H
#define KLTP_THREADSPOOLMANAGER_H

#include "std/thr/sync.h"
#include "std/base/klbase.h"

namespace KLTP {

	const int c_DefualtMaxNumberOfThreads = 5;

	class WorkingThread;
 
	class ThreadsPoolManager : public KLSTD::KLBase {
	public:

		/* 
			\brief Инициализирует менеджера потоков

			\params maxThreads [in] Максимальное количество рабочих потоков
		*/
		virtual void Init( int maxThrds, KLTP::ThreadsPool *threadPool ) = 0;

		/*
			\brief Добавляет задачу на выполнение

			Данная функция добавляет поток

			\param params [in] Параметры для выполнения транспортной задачи
			\param useTaskQueue [in] Флаг использования внутренней очереди задач. Очередь 
						используется в случае отсутсвия на данный момент свободного 
						рабочего потока
		*/
		virtual void AddThread( void *params, bool useTaskQueue = false ) = 0;

		/*
			\brief Удаляет один поток из пула потоков
		*/
		virtual void DeleteNonusedThread() = 0;

		/*
			\brief Функция освобождения рабочего потока

			Данная функция вызывается самим рабочим потоком после выполнения задачи

		*/
		virtual void FreeTask( WorkingThread *thr ) = 0;
	};

} // end namespace KLTP

/*
	\breif Функция создает объект класса ThreadsPoolManager
*/
void KLTP_CreateThreadsPoolManager( KLTP::ThreadsPoolManager **threadManager, int maxThrds,
									 KLTP::ThreadsPool *threadPool );

#endif // KLTP_THREADSPOOLMANAGER_H
