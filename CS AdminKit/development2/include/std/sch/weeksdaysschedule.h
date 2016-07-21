/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file weeksdaysschedule.h
 * \author Дамир Шияфетдинов
 * \date 2007
 * \brief Файл с определением класса расписания задачи 
 *        для запуска модулем-планировщиком в определенные дни недели.
 *
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef KLSCH_WEEKSDAYSSCHEDULE_H
#define KLSCH_WEEKSDAYSSCHEDULE_H

#include "std/sch/errors.h"
#include "schedule.h"

#include "std/base/klbase.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком
    *        в определенные дни недели.
    *
    */ 


    class WeeksDaysSchedule : public Schedule
	{

    public:

		/*!
			\brief Виртуальный деструктор.
		*/
		virtual ~WeeksDaysSchedule() {}

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const = 0;

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
								int daysBitMask = 0 ) = 0;


        /*!
            \brief Возвращает время для ежедневного выполнения задачи.

            \param hours [out] Час для запуска задачи (0-23).
            \param min [out] Минута для запуска задачи (0-59).
            \param sec [out] Секунда для запуска задачи (0-59).
			\param daysBitMask [out] Список дней недели (биты 1-7). Воскресенье - бит 1.
            \return код ошибки
				\retval <ERR_SCHEDULE> внутренние значения класса не инициализированы

        */
        virtual Errors GetExecutionTime( int& hours, int& min, int& sec,
			int &daysBitMask ) const = 0;
		
    };

}

/*
	\brief Метод для создания объекта WeeksDaysSchedule
*/
KLCSC_DECL KLSCH::WeeksDaysSchedule *KLSCH_CreateWeeksDaysSchedule();

#endif // KLSCH_WEEKSDAYSSCHEDULE_H
