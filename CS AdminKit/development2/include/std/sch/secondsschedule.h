/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file SecondsSchedule.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска задачи модулем-планировщиком каждые N секунд
 *
 */
/*KLCSAK_PUBLIC_HEADER*/


#ifndef KLSCH_SECONDSSCHEDULE_H
#define KLSCH_SECONDSSCHEDULE_H

#include "std/sch/errors.h"
#include "schedule.h"

#include "std/base/klbase.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком каждые
    *        N миллисекунд
    *
    */ 


    class SecondsSchedule : public Schedule
	{

    public:
		
		/*!
			\brief Виртуальный деструктор.
		*/
		virtual ~SecondsSchedule() {}

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const = 0;

        /*!
            \brief Устанавливает период выполнения задачи.

            \param sec [in] Период выполнения задачи в sec.
            \return код ошибки

        */
        virtual Errors SetPeriod( int sec = 0 ) = 0;

        /*!
            \brief Возвращает период выполнения задачи.

            \param sec [out] Период выполнения задачи в sec.
            \return код ошибки

        */

        virtual Errors GetPeriod( int& sec ) const = 0;		
    };

}

/*
	\brief Метод для создания объекта SecondsSchedule
*/
KLCSC_DECL KLSCH::SecondsSchedule *KLSCH_CreateSecondsSchedule();

#endif // KLSCH_SECONDSSCHEDULE_H
