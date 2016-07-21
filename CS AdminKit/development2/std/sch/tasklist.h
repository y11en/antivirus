/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TaskList.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением интерфейса списка задач ( используется как вспомогательный класс ).
 *
 */


#ifndef KL_TASK_LIST_H
#define KL_TASK_LIST_H

#include <ctime>

#include "std/sch/common.h"
#include "std/sch/errors.h"

#include "taskimp.h"

namespace KLSCH {

   /*!
    * \brief Список задач планировщика.
    *
    */ 

	class TaskList
	{
    public:
		/*! \struct ListUnit
			\brief Используется для формирования списка задач
		*/
		struct ListUnit {
			ListUnit( const TaskImp &t )
			{				
				task = t;				
				next = NULL;				
			}
			~ListUnit( )
			{				
			}
			TaskImp				task;		//!< Задача		

			struct ListUnit *next;		//!< Поле для организации списка
		};
		
		/*!
            \brief Конструктор.
        */
		TaskList( );

		/*!
            \brief Деструктор. 
				   Освобождает список задач.				   
        */
        ~TaskList( );

        /*!
            \brief Устанавливает идентификатор задачн.

            \param t [in] Задача для добавления.
            \return Указатель на добавленную структуру

        */
		ListUnit *AddTask( const Task *t );	      


        /*!
            \brief Производить поиск задачи в списке.

			\param taskUnit [out] Указатель на найденный элемент списка
            \param tId [in] Идентификатор задачи.
            \return результат поиска

        */
        bool FindTask( ListUnit **taskUnit, const TaskId tId );


        /*!
            \brief Удаляет задачу из списка.

            \param tId [in] Идентификатор задачи.
            \return результат операции

        */
        bool DeleteTask( const TaskId tId ); 

		/*!
            \brief Возвращает следущую задачу для выполенения.

            \param сtId [in] Идентификатор класса задачи.
            \return Указатель на найденную структуру

        */
        ListUnit *FindNextTask( const TaskClassId ctId ); 


        /*!
            \brief Вызывает задачу на выполенение.

			\param tId [in] Идентификатор задачи.
			\param threadContext [in] Контекст клиентского потока.
            \param res [out] Результат работы задачи.
            
        */
		bool CallTaskById( const TaskId tId, void *threadContext, TaskResults** res );

		/*!
            \brief Функция преднозначена для перебора описателей задач.

			\param tId [in] Идентификатор предыдуще задачи в списке или (c_Undefined)
				для получение описателся первой задачи.			
			\param tCid [in] Указывает класс задач для поиска
            \return Указатель на найденную структуру или NULL
            
        */
		ListUnit *GetNextUnit( const TaskId prevtId, TaskClassId tCid );

		/*!
            \brief Удаляет все задачи, у которых текущий статус Deleted, из списка.			
            \return 
            
        */
		void EraseAllDeletedTasks();
        
	private:
		/** Внутренние переменные */

		ListUnit *first;			//!< Указатель на начало динамического списка

	protected:
		/** class helper functions */

    };

}	// end namespace KLSCH

#endif // KL_TASK_LIST_H
