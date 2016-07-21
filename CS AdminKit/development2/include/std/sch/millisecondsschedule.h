/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file MillisecondsSchedule.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска задачи модулем-планировщиком каждые N миллисекунд
 *
 */
/*KLCSAK_PUBLIC_HEADER*/


#ifndef KLSCH_MILLISECONDSSCHEDULE_H
#define KLSCH_MILLISECONDSSCHEDULE_H

#include "std/sch/errors.h"
#include "schedule.h"

#include "std/base/klbase.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком каждые
    *        N миллисекунд
    *
    */ 


    class MillisecondsSchedule : public Schedule
	{

    public:
		
		/*!
			\brief Виртуальный деструктор.
		*/
		virtual ~MillisecondsSchedule() {}

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const = 0;

        /*!
            \brief Устанавливает период выполнения задачи.

            \param msec [in] Период выполнения задачи в msec.
            \return код ошибки

        */
        virtual Errors SetPeriod( int msec = 0 ) = 0;

        /*!
            \brief Возвращает период выполнения задачи.

            \param msec [out] Период выполнения задачи в msec.
            \return код ошибки

        */
        virtual Errors GetPeriod( int& msec ) const = 0;		
    };

}

/*
	\brief Метод для создания объекта MillisecondsSchedule
*/
KLCSC_DECL KLSCH::MillisecondsSchedule *KLSCH_CreateMillisecondsSchedule();

#endif // KLSCH_MILLISECONDSSCHEDULE_H
