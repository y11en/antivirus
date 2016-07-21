/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TasksIterator.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением абстрактного класса-итератора для
 *        просмотра списка задач. 
 *
 */

#ifndef KLSCH_TASKSITERATOR_H
#define KLSCH_TASKSITERATOR_H

#include "std/base/klbase.h"

#include "std/sch/errors.h"

namespace KLSCH {


   
    /*!
    * \brief Класс - итератор для просмотра списка задач.
    *
    */ 
	class Scheduler;


	class TasksIterator
	{

        friend class SchedulerImp;

    public:
		
		/*!
			\brief Виртуальный деструктор.
		*/
		virtual ~TasksIterator() {}
        /*!
            \brief Устанавливает итератор на начало списка.

        */
        virtual Errors Reset() = 0;

        /*!
            \brief Возвращает константный указатель на следующую задачу в списке.

            \param t [out] Указатель на задачу в списке.
            \param hasMoreTasks [out] Истина, если в списке имеются еще задачи.
            \return код ошибки

        */
        virtual Errors Next( const Task ** t, bool& hasMoreTasks ) = 0;
    };

}

/*
	\brief Метод для создания объекта Scheduler
*/
KLCSC_DECL KLSCH::TasksIterator *KLSCH_CreateTasksIterator();

#endif // KLSCH_TASKSITERATOR_H
