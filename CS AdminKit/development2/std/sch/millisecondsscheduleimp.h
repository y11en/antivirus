/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file MillisecondsScheduleImp.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска задачи модулем-планировщиком каждые N миллисекунд
 *
 */


#ifndef KLSCH_MILLISECONDSSCHEDULEIMP_H
#define KLSCH_MILLISECONDSSCHEDULEIMP_H

#include "std/sch/errors.h"
#include "std/sch/schedule.h"
#include "std/sch/millisecondsschedule.h"

namespace KLSCH {
   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком каждые
    *        N миллисекунд
    *
    */ 

    class MillisecondsScheduleImp : public MillisecondsSchedule
	{

    public:
	
		/*!
            \brief Конструтор.
		*/
		MillisecondsScheduleImp();

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const { return SCH_TYPE_MILLISECONS; }

		/*!
            \brief Конструтор копирования.
		*/
		MillisecondsScheduleImp( const MillisecondsScheduleImp &schedule );

        /*!
            \brief Устанавливает период выполнения задачи.

            \param msec [in] Период выполнения задачи в msec.
            \return код ошибки

        */
        Errors SetPeriod( int msec = 0 );

        /*!
            \brief Возвращает период выполнения задачи.

            \param msec [out] Период выполнения задачи в msec.
            \return код ошибки

        */

        Errors GetPeriod( int& msec ) const;

		/*!
            \brief Содает копию объекта.

            \param newCopy [out] Указатель на скопированный объект.
            \return код ошибки

        */
		virtual Errors Clone( Schedule **newCopy );

	private:
		/** Внутренние переменные */

		int		period;			//!< Период запуска ( в милисикундах )		
		time_t	firstStart;		//!< Время первого запуска
		int     periodCounter;	//!< Подсчет периода запуска ( используется если period<1000 )		

	protected:
		/** class helper functions */

		/*!
            \brief Фунция расчета времени следущего запуска. 
		*/
		virtual Errors CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
			time_t lastExeSec, int lastExeMsec, int startDelta = 0 );

		void GetExecutionTimes( time_t currTime, time_t &executionTimeBeforeCurrentSec,
			int &beforeMsec, 
			time_t &executionTimeAfterCurrentSec,
			int &AfterMsec,
			time_t lastExeSec, int lastExeMsec );
    };

}

#endif // KLSCH_MILLISECONDSSCHEDULEIMP_H
