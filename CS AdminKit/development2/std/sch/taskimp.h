/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TaskImp.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением интерфейса задачи для запуска модулем-планировщиком.
 *
 */


#ifndef KLSCH_TASKIMP_H
#define KLSCH_TASKIMP_H

#include <ctime>

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "std/sch/task.h"

namespace KLSCH {

   
        
   /*!
    * \brief Задача для запуска модулем-планировщиком.
    *
    */ 


	class TaskImp : public Task
	{

        friend class TaskList;
		friend class TasksIteratorImp;
		friend class SchedulerImp;

    public:
		
		/*!
            \brief Конструкторы.

            \param id [in]  Идентификатор задачи.
			\param cId [in] Идентификатор класса задачи.
        */
		TaskImp( );
		TaskImp( TaskId id, TaskClassId cId );
		/*!
			\brief Оператор приравнивания.
		*/
		const TaskImp &operator = ( const TaskImp &t );

		/*!
            \brief Деструктор. 
				   Удаляет объект Schedule.
				   Удаляет TaskParams.
        */
        ~TaskImp( );

        /*!
            \brief Устанавливает идентификатор задачн.

            \param id [in] Идентификатор задачи.
            \return код ошибки

        */
        Errors SetTaskId( TaskId id );


        /*!
            \brief Возвращает идентификатор задачн.

            \param id [out] Идентификатор задачи.
            \return код ошибки

        */
        Errors GetTaskId( TaskId& id ) const;


        /*!
            \brief Устанавливает идентификатор класса задачн.

            \param id [in] Идентификатор класса задачи.
            \return код ошибки

        */
        Errors SetClassId( TaskClassId cid );


        /*!
            \brief Возвращает идентификатор класса задачн.

            \param id [out] Идентификатор класса задачи.
            \return код ошибки

        */
        Errors GetClassId( TaskClassId& cid ) const;


		/*!
            \brief Возвращает текущие состояние задачи.

            \param s [out] Текущие состояние задачи.
            \return код ошибки

        */

        Errors GetCurrentState( State &s ) const;

        /*!
            \brief Устанавливает расписание для запуска задачи.

            \param s [in] Расписание для запуска задачи.
            \return код ошибки

        */


        Errors SetSchedule( Schedule* s );

        /*!
            \brief Возвращает указатель на расписание для запуска задачи.

            \param s [out] Расписание для запуска задачи. 0 если расписание не
                            определено.
            \return код ошибки

        */

        Errors GetSchedule( const Schedule ** s) const;

        /*!
            \brief Устанавливает callback для запуска задачи.

            \param cb [in] Указатель на функцию для запуска задачи.
            \return код ошибки

        */
        Errors SetCallback( TaskCallback cb);

		/*!
            \brief Устанавливает расширенную callback для запуска задачи.

            \param cb [in] Указатель на функцию для запуска задачи.
            \return код ошибки

        */
        Errors SetCallback2( TaskCallback2 cb);

        /*!
            \brief Возвращает установленный callback для запуска задачи.

            \param cb [out] Указатель на функцию для запуска задачи.  0 если callback
                            не определен.
            \return код ошибки

        */
        Errors GetCallback( TaskCallback& cb ) const;

        /*!
            \brief Устанавливает параметры для запуска задачи.

            \param par [in] Ссылка на параметры для запуска задачи.
            \return код ошибки

        */
        Errors SetTaskParams( TaskParams * par );

        /*!
            \brief Возвращает копию параметров для запуска задачи.

            \param par [in] Указатель на параметры для запуска задачи.
            \return код ошибки

        */
        Errors GetTaskParams( const TaskParams** par ) const;

		/*!
            \brief Устанавливает диапазона в котором может колибаться время запуска задачи (msec).

			При расчете времени запуска задачи из диапазона от 0 до startDeltaTimeout выбирается
			случайное число, которая прибавляется к моменту запуска. Используется для
			сглаживания нагрузки для задач с одинаковым временем запуска.

            \param startDeltaTimeout [in] Период (msec).
            \return код ошибки
        */
        Errors SetStartDeltaTimeout( int startDeltaTimeout );

        /*!
            \brief Возвращает диапазона в котором может колибаться время запуска задачи (msec).

            \param startDeltaTimeout [out] Максимальное время выполнения задачи (msec).
            \return код ошибки
        */
        Errors GetStartDeltaTimeout(int& startDeltaTimeout) const;

		/*!
            \brief Устанавливает timeout для подготовительного запуска

			Если данный timeout установлен, то задача сначала будет запушенна в 
			подготовительном режиме, а затем в точное время расписания запуститься
			в нормальном.

			\param timeout [in] Timeout (msec).            
            \return код ошибки

        */
        Errors SetPreparativeStartTimeout( int timeout );

		/*!
			\brief Возвращяет timeout для подготовительного запуска
            
            \param timeout [out] Timeout (msec).
            \return код ошибки

        */
        Errors GetPreparativeStartTimeout( int &timeout ) const;

        /*!
            \brief Устанавливает максимальное время выполнения задачи (msec).

            \param maxTime [in] Максимальное время выполнения задачи (msec).
             \return код ошибки

        */
        Errors SetMaxExecutionTime( int maxTime = 10000 );

        /*!
            \brief Возвращает максимальное время выполнения задачи (msec).

            \param maxTime [out] Максимальное время выполнения задачи (msec).
            \return код ошибки ( ERR_TASK_STATE, ERR_NONE )

        */
        Errors GetMaxExecutionTime(int& maxTime) const;

		/*!
            \brief Возвращает время выполенения текущей задачи.
            
            \param msec [in] Время выполнения задачи в msec. 
            \return код ошибки

        */
        Errors GetRunningTime( int &msec );	

        /*!
            \brief Устанавливает последнее время выполнения задачи.

            \param sec [in] Последнее время выполнения задачи в формате time_t.
            \param msec [in] Последнее время выполнения задачи в msec. Значения
                             параметров sec и msec складываются.
            \return код ошибки

        */
        Errors SetLastExecutionTime( time_t sec = 0,  int msec = 0 );

        /*!
            \brief Возвращает последнее время выполнения задачи.

            \param sec [out] Последнее время выполнения задачи в формате time_t.
            \param msec [out] Последнее время выполнения задачи в msec. Значения
                             параметров sec и msec складываются.
            \return код ошибки

        */
        Errors GetLastExecutionTime( time_t& sec, int& msec ) const;

        /*!
            \brief Возвращает следующее запланированное время выполнения задачи.

            \param sec [out] Запланированное время выполнения задачи в формате time_t.
            \param msec [out] Запланорованное время выполнения задачи в msec. Значения
                             параметров sec и msec складываются.
            \return код ошибки

        */
        Errors GetNextExecutionTime( time_t& sec, int& msec );

		/*!
            \brief Устанавливает флаг удаления задачи сразу после ее выполнения.

            \param delAfterRun [in] Если true, то задача удалиться после выполнения.
            \return код ошибки

        */
        Errors SetDelAfterRunFlag( bool delAfterRun );

		/*!
            \brief Возвращяет флаг удаления задачи сразу после ее выполнения.

            \param delAfterRun [out] Если true, то задача удалиться после выполнения.
            \return код ошибки

        */
        Errors GetDelAfterRunFlag( bool &delAfterRun ) const;
		

		/*!
			\brief Устанавливате приоритет потока в котором должна выполняться задача.
					По умолчанию равен TP_Normal.
			
			\param ttPriority [in] Новый приоритет выполнения задачи
		*/
		Errors SetTaskThreadPriority( TaskThreadPriority ttPriority );

		/*!
			\brief Возвращает приоритет потока в котором должна выполняться задача.
								
			\param ttPriority [out] Приоритет выполнения задачи
		*/
		Errors GetTaskThreadPriority( TaskThreadPriority &ttPriority ) const;

		/*!
            \brief Возвращает контекст потока, в котором производиться выполнение задачи.

            \param threadCntx [out] Контекст потока ( не равен NULL только когда задача 
									находиться в состоянии Running.            
            \return код ошибки

        */
        Errors GetThreadContext( void **threadCntx ) const;

		/*!
			\brief Функция создания копии объекта

			\return указатель на созданную копию задачи
		*/
		virtual void Clone( Task **ppTask ) const; 

		/*!
            \brief Возвращает указатель на расписание для запуска задачи для
			 дальнейшей модификации.

            \param s [out] Расписание для запуска задачи. 0 если расписание не
                            определено.
            \return код ошибки

        */
        virtual Errors GetWritableSchedule( Schedule ** s);

	private:
		/** Внутренние переменные */

		TaskId		tId;		//!< Идентификатор задачи
		TaskClassId cId;		//!< Идентификатор класса задачи

		State		state;		//!< Текущие состояние задачи

		TaskCallback	func;	//!< Адрес callback функции задачи
		TaskCallback2	func2;	//!< Адрес расширенной callback функции задачи
		TaskParams  *params;	//!< Параметры задачи

		int	 maxExecuteTime;	//!< Маскимально возможное время выполенения задачи ( в милисекундах )

		void	*threadCntx;	/*!< Контекст потока в котором производиться выполения callback 
									функции. Заполнен только на момент выполения. */

		Schedule *schedule;		//!< Расписание задачи

		Schedule *accessorySchedule;	//!< Вспомогательное расписание задачи ( используется для предварительного запуска задачи )

		time_t lastExeTime;			//!< Последнее время выполнения задачи
		int    lastExemsec;

		time_t nextExeTime;			//!< Запланированное время выполнения задачи
		int    nextExemsec;

		int  startSysTick;		//!< Время запуска задачи в милисекундах ( в тиках )

		int  startDeltaTimeout;

		int  preparativeTimeout;
		bool preparativeStart;	//!< Признак подготовительного запуска

		TaskThreadPriority taskThreadPriority;	//!< Приоритет потока задачи

		bool delAfterRun;		//!< Флаг удаления задачи сразу полсе ее первого выполнения

	protected:
		/** class helper functions */

		void InitializeVars();					//!< Инициализирует внутренние переменные

		void SetCurrState( State newState );	//!< Устанавливает текущее состояние задачи

		/** Расчитывает следующие время запуска */
		void CalculateNextExecutionTime( time_t currTime = 0 ); 

		/** Расчитывает следующие время запуска после Suspend состояния */
		void CalculateNextTimeAfterSuspend( );

		void SetThreadContext( void	*tCntx );	//!< Устанавливает контекст потока для задачи

		void Call( TaskResults** result, bool &delTask );	//!< Вызывает callback задачи

		bool IsTaskHanging() const;				//!< Определяет является ли данная задача завишей

    };

}	// end namespace KLSCH

#endif // KLSCH_TASKIMP_H
