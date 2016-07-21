/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	task_settings_synchronizer2.h
 * \author	Andrew Kazachkov
 * \date	12.09.2005 10:31:20
 * \brief Интерфейс, отвечающий за синхронизацию настроек задач
 *        приложения с настройками задач приложения для системы
 *        администрирования. 
 */

#ifndef __KL_TASK_SETTINGS_SYNCHRONIZER2_H__
#define __KL_TASK_SETTINGS_SYNCHRONIZER2_H__

#include "./task_settings_synchronizer.h"
#include <std/sch/task.h>


namespace KLCONN
{
    /*!
     \brief Интерфейс, отвечающий за синхронизацию настроек задач
            приложения с настройками задач приложения для системы
            администрирования.

        Интерфейс реализуется библиотекой интеграции.
        Интерфейс используется системой администрирования.

        Вызовы методов интерфейса могут производится на разных нитях, но при
        этом сериализуются вызывающей стороной.
    */

    class KLSTD_NOVTABLE TaskSettingsSynchronizer2 : public TaskSettingsSynchronizer
    {
    public:
    /*!
      \brief Создает новый идентификатор для задачи. Возвращаемую память 
            освобождает вызывающая сторона.

        \param szwTaskType      [in]  Тип задачи.
    */
        virtual KLSTD::AKWSTR CreateNewTaskId2(
                                const wchar_t* szwTaskType) = 0;
    };
}

#endif //__KL_TASK_SETTINGS_SYNCHRONIZER2_H__
