/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	conn_chg_settings.h
 * \author	Andrew Kazachkov
 * \date	13.08.2007 15:47:34
 * \brief	Интерфейс, отвечающий за модификацию с помощью политики 
 *        настроек и локальных задач приложения
 */

#ifndef __KLCONN_CHG_SETTINGS_H__
#define __KLCONN_CHG_SETTINGS_H__
/*KLCSAK_PUBLIC_HEADER*/

#include "./conn_types.h"

#include "./settings_synchronizer_base.h"

namespace KLCONN
{
    /*!
     \brief Интерфейс, отвечающий за модификацию с помощью политики 
        настроек и локальных задач приложения

        Интерфейс реализуется библиотекой интеграции.
        Интерфейс используется системой администрирования.

        Вызовы методов интерфейса могут производится на разных нитях, но при
        этом сериализуются вызывающей стороной.

        Данный интерфейс используется системой администрирования только в случае, 
        если при установке коннектора был выставлен флаг CIF_PRIVATE_POLICY_FORMAT.
        В противном случае сетевой агент полагает, что формат политики полностью 
        соответствует формату настроек и модифицирует настройки самостоятельно. 
    */

    class KLSTD_NOVTABLE SettingsModifier
        :   public KLSTD::KLBaseQI
    {
    public:

        /*!
          \brief	Коннектор должен модифицировать настройки и локальные 
                    задачи, которые соответстуют секции политики с именем szwName. 
                    Если модифицировать не удалось (например, продукт 
                    начали останавливать), следует кинуть исключение. 
                    Метод ModifyLocalTasksAndSettingsWithPolicySection будет 
                    вызван поднее. 

          \param	szwName IN имя секции в политики
        */
        virtual void ModifyLocalTasksAndSettingsWithPolicySection(
                        const wchar_t* szwName) = 0;
    };
};

#endif //__KLCONN_CHG_SETTINGS_H__
