/*!
* (C) 2002 "Kaspersky Lab"
*
* \file WorkingThread.h
* \author Шияфетдинов Дамир
* \date 2002
* \brief Интерфейс рабочего потока транспорта ( используется при работе с I/O completion port )
*
*/

#ifndef KLTP_WORKINGTHREAD_H
#define KLTP_WORKINGTHREAD_H

#include "std/thr/sync.h"
#include "std/base/klbase.h"

#include "std/tp/threadspool.h"

namespace KLTP {

	/*!
	* \brief Рабочий поток транспорта 
	*/	

	class ThreadsPoolManager;

	class WorkingThread : public KLSTD::KLBase {
	public:

		enum ActionState {	//!< Состояние потока
			Free, 
			Working,
			ThreadNotStarted
		};

		/*
			\brief Функция производит запуск потока
		*/
		virtual void StartThread( ThreadsPoolManager *manager ) = 0;

		/*
			\brief Функция производит останов потока ( без ожидания окончания работы потока )
		*/
		virtual void StopThread() = 0;

		/*
			\brief Функция определяет остановился ли рабочий поток
		*/
		virtual bool IsStopped() = 0;
		
		/*
			\brief Запускает рабочий поток на выполнения задачи

			\params params [in] Параметры задачи
		*/
		virtual void NewTask( void *params ) = 0;

		/*
			\brief Возвращает текущее состояние потока
		*/
		virtual ActionState GetCurrentState() = 0;

		/*
			\brief Задает номер для данного потока ( по умолчанию равен (-1) )
		*/
		virtual void SetWorkingThreadNumber( int wNum ) = 0;

		/*
			\brief Возвращает параметры потока
		*/
		virtual void *GetParams() = 0;

		/*
			\brief Проверяет является ли текущий поток, потоком WorkingThread
		*/
		virtual bool IsCurrentThread() = 0;

	};

} // end namespace KLTP

/*
	\breif Функция создает объект класса WorkingThread
*/
void KLTP_CreateWorkingThread( KLTP::WorkingThread **workingThread, int threadNum,
								KLTP::ThreadsPool *threadPool );

#endif // KLTP_WORKINGTHREAD_H
