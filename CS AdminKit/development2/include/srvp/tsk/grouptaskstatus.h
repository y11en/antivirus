/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file tsk/grouptaskstatus.h
 * \author Михаил Кармазин
 * \date 2003
 * \brief Интерфейс для получения детального статуса одной групповой задачи, 
 *        выполняющейся на многих машинах.
 */

#ifndef __TSK_GROUP_TASK_STATUS_H__
#define __TSK_GROUP_TASK_STATUS_H__

#include <srvp/csp/klcsp.h>
#include <srvp/tsk/grouptaskbase.h>

namespace KLTSK
{
    /*!
    * \brief Интерфейс для получения детального статуса одной групповой задачи, 
        выполняющейся на многих машинах.
    */
    class GroupTaskStatus : public KLSTD::KLBase
    {
    public:

		/*
			\brief Устанавливает на начало итератор получения списка хостов.

			\param fields [in] список полей, которые надо выдавать.
            \size [in] размер списка
            \param hsHostStateMask [in] хосты с таким статусом нужно выдавать. Значения статуса
                можно комбинировать побитово.
			\param lifetime [in] Время жизни курсора на сервере (сек).
		*/
		virtual void ResetHostIterator(
			const wchar_t**		fields,
			int					size,
            HostState   hsHostStateMask,
			long        nLifetime = 20)  = 0;

		/*
			\brief Получает информацию о следующем компьютере в группе.

			\param nCount	[in, out] in - количество записей, которое необходимо считать,
                    out - кол-во реально считанных записей.
			\param ppParHostStatus     [out] результат - содержит значение c именем c_host_state_array,
                типом ARRAY_T. Массив содержит nCount[out] элементов, каждый элемент в массиве -
                 контейнер Params c значениями с именам, описанными выше (константы c_hs*).
			\return	false, если достигнут конец списка компьютеров. Если false, то также 
                в nCount возвращаеться 0.

		*/
        virtual bool GetNext( long& nCount, KLPAR::Params** ppParHostStatus ) = 0;

		/*
			\brief Устанавливает на начало итератор получения списка хостов.

			\param vectFields [in] список полей, которые надо выдавать.
			\param vect_fields_to_order [in] список полей, по которым надо осуществлять сортировку
            \param hsHostStateMask [in] хосты с таким статусом нужно выдавать. Значения статуса
                можно комбинировать побитово.
			\param lifetime [in] Время жизни курсора на сервере (сек).
		*/
		virtual void ResetHostIteratorEx(
			const std::vector<std::wstring> vectFields,
			const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
            HostState   hsHostStateMask,
			long        nLifetime = 20)  = 0;

        /*!
          \brief	GetRecordCount

          \return	long возвращает количество записей в списке
        */
        virtual long GetRecordCount() = 0;
		
		/*
			\brief Возвращает заданный интервал записей.

            \param	long nStart [in] - порядковый номер начальной записи
            \param	long nEnd [in] - порядковый номер конечной записи
			\param ppParHostStatus [out] - результат - содержит значение c именем c_host_state_array,
                типом ARRAY_T. Массив содержит (nEnd - nStart + 1) элементов, каждый элемент в массиве -
                 контейнер Params c значениями с именам, описанными выше (константы c_hs*).
			\return	int - реальное число реально возвращаемых записей.

		*/
        virtual int GetRecordRange(long nStart, long nEnd, KLPAR::Params** ppParHostStatus ) = 0;
    };
}

#endif // __TSK_GROUP_TASK_STATUS_H__

