/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Schedule.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением базового класса расписания задачи 
 *        для запуска модулем-планировщиком.
 *
 */


#ifndef KLSCH_SCHEDULE_H
#define KLSCH_SCHEDULE_H

#include <ctime>
#include "std/base/klbase.h"

#include "std/sch/errors.h"

namespace KLSCH {

    //! Перечисление возможных типов расписания задач
    enum ScheduleType {
        SCH_TYPE_NONE,			///< Тип расписания не определен.
        SCH_TYPE_MILLISECONS,	///< Расписание с периодом, задаваемым числом msec.
        SCH_TYPE_EVERYDAY,		///< Задача запускается каждый день.
        SCH_TYPE_EVERYWEEK,		///< Задача запускается каждую неделю.
        SCH_TYPE_EVERYMONTH,	///< Задача запускается каждый месяц.
		SCH_TYPE_SECONDS,		///< Расписание с периодом, задаваемым числом секунд.
		SCH_TYPE_WEEKSDAYS		///< Расписание с запуском в опередленные дни недели
    };  


   
    /*!
    * \brief Расписание задачи для запуска модулем-планировщиком.
    *
    */ 


	class Schedule
	{

		friend class TaskImp;

    public:

		
		Schedule();
		virtual ~Schedule();


        virtual ScheduleType GetType() const { return SCH_TYPE_NONE; }


        /*!
            \brief Устанавливает флаг запуска пропущенных задач.

            \param runMissed [in] Если true, то пропущенная задача будет выполнена.
            \return код ошибки

        */
        virtual Errors SetRunMissedFlag( bool runMissed = false );

        /*!
            \brief Возвращает флаг запуска пропущенных задач.

            \param runMissed [out] Если true, то пропущенная задача будет выполнена.
            \return код ошибки

        */
        virtual Errors GetRunMissedFlag( bool& runMissed ) const;

        /*!
            \brief Устанавливает период актуальности расписания. Задача с
                   просроченным периодом актуальности не будет выполнена.

            \param lifetime [in] Момент времени в формате time_t, до которого
                                 задача является актуальной.
            \return код ошибки

        */
        virtual Errors SetLifetime( time_t lifetime );


        /*!
            \brief Возвращает период актуальности расписания. Задача с
                   просроченным периодом актуальности не будет выполнена.

            \param lifetime [out] Момент времени в формате time_t, до которого
                                 задача является актуальной.
            \return код ошибки

        */
        virtual Errors GetLifetime( time_t& lifetime ) const;


        /*!
            \brief Устанавливает время для выполнения задачи первый раз.

            \param sec [in] Время выполнения задачи в формате time_t.
            \param msec [in] Время выполнения задачи в msec. Значения
                             параметров sec и msec складываются.
            \return код ошибки

        */
        virtual Errors SetFirstExecutionTime( time_t sec = 0, int msec = 0 );

        /*!
            \brief Возвращает время для выполнения задачи первый раз.

            \param sec [out] Время выполнения задачи в формате time_t.
            \param msec [out] Время выполнения задачи в msec. Значения
                             параметров sec и msec складываются.
            \return код ошибки

        */
        virtual Errors GetFirstExecutionTime( time_t& sec, int& msec) const;

		/*!
            \brief Содает копию объекта.

            \param newCopy [out] Указатель на скопированный объект.
            \return код ошибки

        */
		virtual Errors Clone( Schedule **newCopy ) = 0;
		
		/*!
            \brief Устанавливает период выполнения задачи.

            \param period [in] Период выполнения задачи в соотвествующих расписанию единицах
            \return код ошибки

        */
        virtual Errors SetPeriod( int period = 0 ) = 0;

        /*!
            \brief Возвращает период выполнения задачи.

            param period [in] Период выполнения задачи в соотвествующих расписанию единицах
            \return код ошибки
        */
        virtual Errors GetPeriod( int& period ) const = 0;		

		/*!
			\brief Оператор копирования	
		*/
		void CopyFrom( const Schedule *from );

	protected: // protected для доступа к пременным из наследуюмых классов
		/** Внутренние переменные */

		bool  runMissed;		//!< Флаг запуска пропущенных задач
		time_t lifeTime;		//!< Время актуальности задачи
		
		time_t firstExeTime;	//!< Первое время выполнения задачи
		time_t firstExemsec;

	protected:
		/** class helper functions */

		/*!
            \brief Фунция расчета времени следущего запуска. 
		*/
		virtual Errors CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
			time_t lastExeSec, int lastExeMsec, int startDelta = 0 ) = 0;

    };

}

/*
	\brief Метод для создания объекта EmptySchedule
*/
KLCSC_DECL KLSCH::Schedule *KLSCH_CreateEmptySchedule();

#endif // KLSCH_SCHEDULE_H
