/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EveryMonthScheduleImp.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска модулем-планировщиком ежемесячно.
 *
 */


#ifndef KLSCH_EVERYMONTHSCHEDULEIMP_H
#define KLSCH_EVERYMONTHSCHEDULEIMP_H

#include "std/sch/errors.h"
#include "std/sch/schedule.h"

#include "std/sch/everymonthschedule.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком
    *        ежемесячно.
    *
    */ 


    class EveryMonthScheduleImp : public EveryMonthSchedule
	{

    public:

		/*!
            \brief Конструктор.
		*/
		EveryMonthScheduleImp();

		/*!
            \brief Конструтор копирования.
		*/
		EveryMonthScheduleImp( const EveryMonthScheduleImp &schedule );

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const { return SCH_TYPE_EVERYMONTH; }

        /*!
            \brief Устанавливает время для ежемесячного выполнения задачи.

            \param day [in] День месяца для запуска задачи (1-31).
            \param hours [in] Час для запуска задачи (0-23).
            \param min [in] Минута для запуска задачи (0-59).
            \param sec [in] Секунда для запуска задачи (0-59).
            \return код ошибки

        */
        Errors SetExecutionTime( int day = 0,
                                 int hours = 0, 
                                 int min = 0,
                                 int sec = 0);


        /*!
            \brief Возвращает время для ежемесячного выполнения задачи.

            \param day [out] День месяца для запуска задачи (1-31).
            \param hours [out] Час для запуска задачи (0-23).
            \param min [out] Минута для запуска задачи (0-59).
            \param sec [out] Секунда для запуска задачи (0-59).
            \return код ошибки

        */
        Errors GetExecutionTime( int& day, int& hours, int& min, int& sec) const;
		
		/*!
            \brief Устанавливает период выполнения задачи в месяцах
				( по умолчанию период равен 1 месяц )

            \param months [in] Период выполнения задачи в месяцах
            \return код ошибки

        */
        Errors SetPeriod( int months = 1 );

        /*!
            \brief Возвращает период выполнения задачи в месяцах

            \param months [out] Период выполнения задачи в месяцах
            \return код ошибки

        */
        Errors GetPeriod( int& months ) const;

		/*!
            \brief Содает копию объекта.

            \param newCopy [out] Указатель на скопированный объект.
            \return код ошибки

        */
		virtual Errors Clone( Schedule **newCopy );

	private:
		/** Внутренние переменные */

		int		  day;		//!< День месяца запуска задачи ( 1 - 31 )
		int		hours;		//!< Час запуска задача ( 0 - 23 )
		int		  min;		//!< Минута запуска задача ( 0 - 59 )
		int		  sec;		//!< Секунда запуска задача ( 0 - 59 )

		int		monthsPeriod;	//!< Период расписания в месяцах

	protected:
		/** class helper functions */

		/*!
            \brief Фунция расчета времени следущего запуска. 
		*/
		virtual Errors CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
			time_t lastExeSec, int lastExeMsec, int startDelta = 0 );
		
		int GetDaysInMonth( struct tm &monthTm );
    };

}

#endif // KLSCH_EVERYMONTHSCHEDULEIMP_H
