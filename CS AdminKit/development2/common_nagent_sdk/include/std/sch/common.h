 /*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Common.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Описание общих определений для модуля Scheduler.
 *
 */

#ifndef KLSCH_COMMON_H
#define KLSCH_COMMON_H

#include <ctime>

namespace KLSCH {


    //! Идентификатор задачи. 
    typedef int TaskId;

    //! Идентификатор класса задачи.
    typedef int TaskClassId;

	//! Константа для определения бесконечного timeout
	const int c_Infinite = (-1);

	//! Неопределенное значение
	const int c_Undefined = (-1);

	//! Кооэфицент соответствия милисекунд и секунд
	const int c_MillisecondsInSecond = 1000;
	
	/*!
      \brief Функция для сравения time_t + msec.
      \return 0 - равны, < 0 - меньше, > 0 - больше
    */
	int CompareTimes( time_t t1sec, int t1msec, time_t t2sec, int t2msec );

	/*!
      \brief Функция для расчета времени ожидания ( в милисекундах ) до указанного времени.
      \return 0 - равны, < 0 - меньше, > 0 - больше
    */
	unsigned long CalculateWaitingTime( time_t tsec, int tmsec, time_t currTime,
		int currTimeMSec );

} // nasmesapce KLSCH

#endif // KLSCH_COMMON_H
