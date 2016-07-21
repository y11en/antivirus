/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EveryWeekScheduleImp.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска модулем-планировщиком еженедельно.
 *
 */


#ifndef KLSCH_EVERYWEEKSCHEDULEIMP_H
#define KLSCH_EVERYWEEKSCHEDULEIMP_H

#include "std/sch/errors.h"
#include "std/sch/schedule.h"
#include "std/sch/everyweekschedule.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком
    *        еженедельно.
    *
    */ 


    class EveryWeekScheduleImp : public EveryWeekSchedule
	{

    public:

		/*!
            \brief Конструктор.
		*/
		EveryWeekScheduleImp();			

		/*!
            \brief Конструтор копирования.
		*/
		EveryWeekScheduleImp( const EveryWeekScheduleImp &schedule );

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const { return SCH_TYPE_EVERYWEEK; }
			
        /*!
            \brief Устанавливает время для еженедельного выполнения задачи.

            \param day [in] День для запуска задачи (0-6). Воскресенье - 0.
            \param hours [in] Час для запуска задачи (0-23).
            \param min [in] Минута для запуска задачи (0-59).
            \param sec [in] Секунда для запуска задачи (0-59).
            \return код ошибки
				\retval <ERR_TIME_INTERVAL>

        */
        Errors SetExecutionTime( int day = 0,
                                 int hours = 0, 
                                 int min = 0,
                                 int sec = 0);


        /*!
            \brief Возвращает время для ежедневного выполнения задачи.

            \param day [out] День для запуска задачи (0-6). Воскресенье - 0.
            \param hours [out] Час для запуска задачи (0-23).
            \param min [out] Минута для запуска задачи (0-59).
            \param sec [out] Секунда для запуска задачи (0-59).
            \return код ошибки
				\retval <ERR_SCHEDULE> внутренние значения класса не инициализированы

        */
        Errors GetExecutionTime( int& day, int& hours, int& min, int& sec) const;
		
		/*!
            \brief Устанавливает период выполнения задачи в неделях
				( по умолчанию период равен 1 недели )

            \param weeks [in] Период выполнения задачи в неделях
            \return код ошибки

        */
        Errors SetPeriod( int weeks = 1 );

        /*!
            \brief Возвращает период выполнения задачи в неделях

            \param weeks [out] Период выполнения задачи в неделях
            \return код ошибки

        */
        Errors GetPeriod( int& weeks ) const;

		/*!
            \brief Содает копию объекта.

            \param newCopy [out] Указатель на скопированный объект.
            \return код ошибки

        */
		virtual Errors Clone( Schedule **newCopy );

	private:
		/** Внутренние переменные */

		int		  day;		//!< День недели запуска задачи ( 0 - 7 )
		int		hours;		//!< Час запуска задача ( 0 - 23 )
		int		  min;		//!< Минута запуска задача ( 0 - 59 )
		int		  sec;		//!< Секунда запуска задача ( 0 - 59 )

		int		weeksPeriod;	//!< Период запуска

	protected:
		/** class helper functions */

		/*!
            \brief Фунция расчета времени следущего запуска. 
		*/
		virtual Errors CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
			time_t lastExeSec, int lastExeMsec, int startDelta = 0 );

    };

}

#endif // KLSCH_EVERYWEEKSCHEDULEIMP_H
