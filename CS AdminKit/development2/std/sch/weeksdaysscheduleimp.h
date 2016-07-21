/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file weeksdaysscheduleimp.h
 * \author Дамир Шияфетдинов
 * \date 2007
 * \brief Файл с определением класса расписания задачи 
 *        для запуска модулем-планировщиком в определенные дни недели.
 *
 */

#ifndef KLSCH_WEEKSDAYSSCHEDULEIMP_H
#define KLSCH_WEEKSDAYSSCHEDULEIMP_H

#include "std/sch/errors.h"
#include "std/sch/weeksdaysschedule.h"

#include "std/base/klbase.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком
    *        в определенные дни недели.
    *
    */ 


    class WeeksDaysScheduleImp : public WeeksDaysSchedule
	{

    public:

		/*!
            \brief Конструктор.
		*/
		WeeksDaysScheduleImp();			

		/*!
            \brief Конструтор копирования.
		*/
		WeeksDaysScheduleImp( const WeeksDaysScheduleImp &schedule );

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const { return SCH_TYPE_WEEKSDAYS; }

		/*!
            \brief Устанавливает время.

            \param hours [in] Час для запуска задачи (0-23).
            \param min [in] Минута для запуска задачи (0-59).
            \param sec [in] Секунда для запуска задачи (0-59).
			\param daysBitMask [in] Битовая маска Список дней недели (биты 1-7). Воскресенье - бит 1.
            \return код ошибки
				\retval <ERR_TIME_INTERVAL>

        */
        virtual Errors SetExecutionTime( 
                                 int hours = 0, 
                                 int min = 0,
                                 int sec = 0,
								 int daysBitMask = 0 );



        /*!
            \brief Возвращает время для ежедневного выполнения задачи.

            \param day [out] День для запуска задачи (0-6). Воскресенье - 0.
            \param hours [out] Час для запуска задачи (0-23).
            \param min [out] Минута для запуска задачи (0-59).
            \param sec [out] Секунда для запуска задачи (0-59).
			\param daysBitMask [out] Список дней недели (биты 1-7). Воскресенье - бит 1.
            \return код ошибки
				\retval <ERR_SCHEDULE> внутренние значения класса не инициализированы

        */
        virtual Errors GetExecutionTime( int& hours, int& min, int& sec,
			int &daysBitMask ) const;

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

		std::vector<int>	days;		//!< Дни недели запуска задачи ( 0 - 7 )
		int					daysBitMask;
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

#endif // KLSCH_WEEKSDAYSSCHEDULEIMP_H
