/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TaskResults.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением результатов выполнения задачи 
 *        для запуска модулем-планировщиком.
 *
 */
/*KLCSAK_PUBLIC_HEADER*/


#ifndef KLSCH_TASKRESULTS_H
#define KLSCH_TASKRESULTS_H

namespace KLSCH {

/*!
 * \brief Результаты выполнения задачи.
 *
 * Данный класс используется для типизации и содержит результаты выполнения задачи.
 * Для определения своих собственных результатов выполнения задачи 
 * клиент должен унаследовать свой класс от класса TaskResults.
 */ 
    

    class TaskResults 
	{ 

	public:
		virtual ~TaskResults() {};

		/*
			\brief Возвращает копию объекта

			Данный метод должен быть реализован в наследуемомо классе.

			\return Возвращает указатель на вновь созданный объект
		*/
		virtual TaskResults *Clone() = 0;

	};

}

#endif // KLSCH_TASKRESULTS_H
