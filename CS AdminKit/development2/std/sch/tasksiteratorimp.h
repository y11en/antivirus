/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TasksIteratorImp.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса-итератора для
 *        просмотра списка задач. 
 *
 */


#ifndef KLSCH_TASKSITERATORIMP_H
#define KLSCH_TASKSITERATORIMP_H

#include "std/sch/errors.h"
#include "std/sch/tasksiterator.h"

namespace KLSCH {

   
    /*!
    * \brief Класс - итератор для просмотра списка задач.
    *
    */ 

	class TasksIteratorImp : public TasksIterator
	{

        friend class SchedulerImp;

    public:

		/*!
            \brief Конструктор.
        */
		TasksIteratorImp();

		/*!
            \brief Деструктор.
        */
		~TasksIteratorImp();

        /*!
            \brief Устанавливает итератор на начало списка.

        */
        Errors Reset();

        /*!
            \brief Возвращает константный указатель на следующую задачу в списке.

            \param t [out] Указатель на задачу в списке.
            \param hasMoreTasks [out] Истина, если в списке имеются еще задачи.
            \return код ошибки

        */
        Errors Next( const Task ** t, bool& hasMoreTasks );

	protected:

		/* внутренние пременные класса */
		SchedulerImp *scheduler;	//!< указатель на scheduler
		TaskList	  *taskList;	//!< указатель на список задач
		TaskId		 currentTid;	//!< идентификатор текущей задачи

		TaskClassId filterCid;	//!< класс задач для выборки
		bool	  onlyHanging;	//!< выбирать только завишие задачи

		/* helper class functions */

		/** Функция инициализации итератора  */
		void Init( SchedulerImp *sch, TaskList *lst, TaskClassId fCid,
			bool hanging );
    };

}

#endif // KLSCH_TASKSITERATORIMP_H
