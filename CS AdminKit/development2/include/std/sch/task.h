/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Task.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением абстрактного интерфейса задачи для запуска модулем-планировщиком.
 *
 */
/*KLCSAK_PUBLIC_HEADER*/


#ifndef KLSCH_TASK_H
#define KLSCH_TASK_H

#include <ctime>

#include "std/base/klbase.h"

#include "std/sch/common.h"
#include "std/sch/errors.h"

namespace KLSCH {


    class Schedule;
    class TaskResults;
    class TaskParams;

	struct TaskOptions
	{	
		TaskOptions()
		{
			PreparativeStart = false;
		}

		TaskOptions( bool prepStart, unsigned long prepTimeout )
		{
			PreparativeStart = prepStart;
			PreparativeTimeout = prepTimeout;
		}		

		bool			PreparativeStart;
		unsigned long	PreparativeTimeout;
	};
    
    /*!
		\brief Callback - функция для вызова задачи

		\param tParams [in]  Параметры задачи
		\param tResult [out] Результат выполенения задачи. Callback функция должна завести 
							 объект TaskResults и заполнив его вернуть через этот параметр
	*/
    typedef Errors (*TaskCallback)(const TaskParams* tParams, TaskResults **tResult );

	/*!
		\brief Callback - функция для вызова задачи

		\param tParams [in]  Параметры задачи
		\param tOptions [in] Настройки задачи
		\param tResult [out] Результат выполенения задачи. Callback функция должна завести 
							 объект TaskResults и заполнив его вернуть через этот параметр
							  \param tParams [in]  Параметры задачи		
	*/
    typedef Errors (*TaskCallback2)(const TaskParams* tParams, 
		const TaskOptions *tOptions, TaskResults **tResult );

   /*!
    * \brief Задача для запуска модулем-планировщиком.
    *
    */ 


	class Task
	{
    public:

		//! \enum - Состояние задачи
		enum State {
			Created,			///< Задача только что создана и еще не попала в Scheduler
			Scheduled,			///< Задача находиться в состоянии ожидания вызова
			WaitForRunning,		///< Данная задача ожидает запуска
			Running,			///< Задача выполняется
			Suspended,			///< Задача времено приостановлена ( см. Scheduler::SuspendTask )
			Completed,			///< Задача уже выполнелась и больше зупускаться не будет ( на основе ее расписания )
			Deleted				///< Задача удалена ( физически из списка будет удалена позже )
		};

		//! \enum - Приоритет потока задачи
		enum TaskThreadPriority {
			TP_Low,
			TP_Normal,
			TP_Hight
		};

		/*!
			\brief Виртуальный деструктор.
		*/
		virtual ~Task() {}

		/*!
            \brief Устанавливает идентификатор задачн.

            \param id [in] Идентификатор задачи.
            \return код ошибки

        */
        virtual Errors SetTaskId( TaskId id ) = 0;


        /*!
            \brief Возвращает идентификатор задачн.

            \param id [out] Идентификатор задачи.
            \return код ошибки

        */
        virtual Errors GetTaskId( TaskId& id ) const = 0;


        /*!
            \brief Устанавливает идентификатор класса задачн.

            \param id [in] Идентификатор класса задачи.
            \return код ошибки

        */
        virtual Errors SetClassId( TaskClassId cid ) = 0;


        /*!
            \brief Возвращает идентификатор класса задачн.

            \param id [out] Идентификатор класса задачи.
            \return код ошибки

        */
        virtual Errors GetClassId( TaskClassId& cid ) const = 0;


		/*!
            \brief Возвращает текущие состояние задачи.

            \param s [out] Текущие состояние задачи.
            \return код ошибки

        */

        virtual Errors GetCurrentState( State &s ) const = 0;

        /*!
            \brief Устанавливает расписание для запуска задачи.

            \param s [in] Расписание для запуска задачи.
            \return код ошибки

        */


        virtual Errors SetSchedule( Schedule* s ) = 0;

        /*!
            \brief Возвращает указатель на расписание для запуска задачи.

            \param s [out] Расписание для запуска задачи. 0 если расписание не
                            определено.
            \return код ошибки

        */

        virtual Errors GetSchedule( const Schedule ** s) const = 0;

        /*!
            \brief Устанавливает callback для запуска задачи.

            \param cb [in] Указатель на функцию для запуска задачи.
            \return код ошибки

        */
        virtual Errors SetCallback( TaskCallback cb) = 0;

		/*!
            \brief Устанавливает расширенную callback для запуска задачи.

            \param cb [in] Указатель на функцию для запуска задачи.
            \return код ошибки

        */
        virtual Errors SetCallback2( TaskCallback2 cb) = 0;
		

        /*!
            \brief Возвращает установленный callback для запуска задачи.

            \param cb [out] Указатель на функцию для запуска задачи.  0 если callback
                            не определен.
            \return код ошибки

        */
        virtual Errors GetCallback( TaskCallback& cb ) const = 0;

        /*!
            \brief Устанавливает параметры для запуска задачи.

            \param par [in] Ссылка на параметры для запуска задачи.
            \return код ошибки

        */
        virtual Errors SetTaskParams( TaskParams * par ) = 0;

        /*!
            \brief Возвращает копию параметров для запуска задачи.

            \param par [in] Указатель на параметры для запуска задачи.
            \return код ошибки

        */
        virtual Errors GetTaskParams( const TaskParams** par ) const = 0;

		 /*!
            \brief Устанавливает диапазона в котором может колибаться время запуска задачи (msec).

			При расчете времени запуска задачи из диапазона от 0 до startDeltaTimeout выбирается
			случайное число, которая прибавляется к моменту запуска. Используется для
			сглаживания нагрузки для задач с одинаковым временем запуска.

            \param startDeltaTimeout [in] Период (msec).
            \return код ошибки
        */
        virtual Errors SetStartDeltaTimeout( int startDeltaTimeout ) = 0;

        /*!
            \brief Возвращает диапазона в котором может колибаться время запуска задачи (msec).

            \param startDeltaTimeout [out] Максимальное время выполнения задачи (msec).
            \return код ошибки
        */
        virtual Errors GetStartDeltaTimeout(int& startDeltaTimeout) const = 0;
		
		/*!
            \brief Устанавливает timeout для подготовительного запуска

			Если данный timeout установлен, то задача сначала будет запушенна в 
			подготовительном режиме, а затем в точное время расписания запуститься
			в нормальном.

			\param timeout [in] Timeout (msec).            
            \return код ошибки

        */
        virtual Errors SetPreparativeStartTimeout( int timeout ) = 0;

		/*!
			\brief Возвращяет timeout для подготовительного запуска
            
            \param timeout [out] Timeout (msec).
            \return код ошибки

        */
        virtual Errors GetPreparativeStartTimeout( int &timeout ) const = 0;

        /*!
            \brief Устанавливает максимальное время выполнения задачи (msec).

            \param maxTime [in] Максимальное время выполнения задачи (msec).
             \return код ошибки

        */
        virtual Errors SetMaxExecutionTime( int maxTime = 10000 ) = 0;

        /*!
            \brief Возвращает максимальное время выполнения задачи (msec).

            \param maxTime [out] Максимальное время выполнения задачи (msec).
            \return код ошибки ( ERR_TASK_STATE, ERR_NONE )

        */
        virtual Errors GetMaxExecutionTime(int& maxTime) const = 0;

		/*!
            \brief Возвращает время выполенения текущей задачи.
            
            \param msec [in] Время выполнения задачи в msec. 
            \return код ошибки

        */
        virtual Errors GetRunningTime( int &msec ) = 0;	

        /*!
            \brief Устанавливает последнее время выполнения задачи.

            \param sec [in] Последнее время выполнения задачи в формате time_t.
            \param msec [in] Последнее время выполнения задачи в msec. Значения
                             параметров sec и msec складываются.
            \return код ошибки

        */
        virtual Errors SetLastExecutionTime( time_t sec = 0,  int msec = 0 ) = 0;

        /*!
            \brief Возвращает последнее время выполнения задачи.

            \param sec [out] Последнее время выполнения задачи в формате time_t.
            \param msec [out] Последнее время выполнения задачи в msec. Значения
                             параметров sec и msec складываются.
            \return код ошибки

        */
        virtual Errors GetLastExecutionTime( time_t& sec, int& msec ) const = 0;

        /*!
            \brief Возвращает следующее запланированное время выполнения задачи.

            \param sec [out] Запланированное время выполнения задачи в формате time_t.
            \param msec [out] Запланорованное время выполнения задачи в msec. Значения
                             параметров sec и msec складываются.
            \return код ошибки

        */
        virtual Errors GetNextExecutionTime( time_t& sec, int& msec ) = 0;

		/*!
            \brief Устанавливает флаг удаления задачи сразу после ее выполнения.

            \param delAfterRun [in] Если true, то задача удалиться после выполнения.
            \return код ошибки

        */
        virtual Errors SetDelAfterRunFlag( bool delAfterRun ) = 0;

		/*!
            \brief Возвращяет флаг удаления задачи сразу после ее выполнения.

            \param delAfterRun [out] Если true, то задача удалиться после выполнения.
            \return код ошибки

        */
        virtual Errors GetDelAfterRunFlag( bool &delAfterRun ) const = 0;

		/*!
			\brief Устанавливате приоритет потока в котором должна выполняться задача.
					По умолчанию равен TP_Normal.
			
			\param ttPriority [in] Новый приоритет выполнения задачи
		*/
		virtual Errors SetTaskThreadPriority( TaskThreadPriority ttPriority ) = 0;

		/*!
			\brief Возвращает приоритет потока в котором должна выполняться задача.
								
			\param ttPriority [out] Приоритет выполнения задачи
		*/
		virtual Errors GetTaskThreadPriority( TaskThreadPriority &ttPriority ) const = 0;

		/*!
            \brief Возвращает контекст потока, в котором производиться выполнение задачи.

            \param threadCntx [out] Контекст потока ( не равен NULL только когда задача 
									находиться в состоянии Running.            
            \return код ошибки

        */
        virtual Errors GetThreadContext( void **threadCntx ) const = 0;

		/*!
			\brief Функция создания копии объекта

			\return указатель на созданную копию задачи
		*/
		virtual void Clone( Task **ppTask ) const = 0;
		
		/*!
            \brief Возвращает указатель на расписание для запуска задачи для
			 дальнейшей модификации.

            \param s [out] Расписание для запуска задачи. 0 если расписание не
                            определено.
            \return код ошибки

        */
        virtual Errors GetWritableSchedule( Schedule ** s) = 0;
    };

}	// end namespace KLSCH

/*
	\brief Метод для создания объекта Task
*/
KLCSC_DECL KLSCH::Task *KLSCH_CreateTask();

#endif // KLSCH_TASK_H
