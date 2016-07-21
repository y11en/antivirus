/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRCI/ProductSettings.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Интерфейс класса, предоставляющего функциональность по доступу к настройкам
 *          продукта.
 *
 */

#ifndef KLPRCI_PRODUCTSETTINGS_H
#define KLPRCI_PRODUCTSETTINGS_H

#include <string>

#include "prci_const.h"
#include "kca/prss/settingsstorage.h"
#include "std/err/error.h"
#include "std/par/value.h"

namespace KLPRCI
{	

    /*!
		\brief Callback - функция для получения оповещения о изменении настроек. 
         Реализация этой функции должна быть потокобезопасной.

        \param productName      [in]  Имя продукта для которого изменились настройки.
        \param version          [in]  Версия продукта.
		\param section          [in]  Имя секции продукта с измененным значением.
		\param parameterName    [in]  Имя измененного параметра продукта.
        \param oldValue         [in]  Старое значение параметра.
        \param newValue         [in]  Новое значение параметра.
	*/
    typedef void (*SettingsChangeCallback)(
		void*				context,
        const std::wstring&	productName, 
        const std::wstring&	version,
        const std::wstring&	section, 
        const std::wstring&	parameterName,
        const KLPAR::Value*	oldValue,
        const KLPAR::Value*	newValue );

    /*!
    * \brief Интерфейс модуля Product Settings.
    *  
    *  Данный модуль используется для получения адреса хранилища настроек
    *  продукта и для получения/отправки нотификаций о изменении настроек.
    *
    */
    
    class KLSTD_NOVTABLE ProductSettings: public KLSTD::KLBaseQI
    {
    public:
    /*!
      \brief Возвращает расположение хранилища настроек, в котором хранятся параметры компонентов.

    */
        virtual std::wstring GetSettingsStorageLocation(
                  KLPRSS::SS_OPEN_TYPE nOpenType=KLPRSS::SSOT_SMART) const = 0;


    /*!
      \brief Подписывает компонент продукта на получение оповещений о 
            изменении настроек и устанавливает callback для получения оповещений.
       
        Если клиент не задает один из параметров подписки (имя продукта, версия,
        секция, имя параметра), то фильтрация событий по этому параметру не производится.
        Клиент может подписаться на несколько типов событий.  В этом случае он получит
        отдельные нотификации для каждого из подходящих по условиям подписки событий.

        \param productName      [in]  Имя продукта для которого изменились настройки.
        \param version          [in]  Версия продукта.
		\param section          [in]  Имя секции продукта с измененным значением.
		\param parameterName    [in]  Имя измененного параметра продукта.
        \param callback         [in]  Callback - функция для получения оповещения о 
                                        изменении настроек.
		\param hSubscription	[out] Идентификатор подписки

    */

       virtual void SubscribeOnSettingsChange( 
                        const std::wstring&		productName, 
                        const std::wstring&		version,
                        const std::wstring&		section, 
                        const std::wstring&		parameterName,
						void*					context,
                        SettingsChangeCallback	callback,
						KLPRCI::HSUBSCRIBE&		hSubscription)  = 0;

    /*!
      \brief Отменяет подписку на получение оповещений о изменении настроек.       

        \param hSubscription    [in]  Идентификатор подписки
    */

       virtual void CancelSettingsChangeSubscription(
						KLPRCI::HSUBSCRIBE		hSubscription)  = 0;



    /*!
		\brief Функция для нотификации о изменении настроек.

        \param productName      [in]  Имя продукта для которого изменились настройки.
        \param version          [in]  Версия продукта.
		\param section          [in]  Имя секции продукта с измененным значением.
		\param parameterName    [in]  Имя измененного параметра продукта.
        \param oldValue         [in]  Старое значение параметра.
        \param newValue         [in]  Новое значение параметра.
        \param type             [in]  Указывает, какое именно хранилище было
                                      изменено.
        
        Например:
        Получаем хранилище
        wstrLocation=pInstance->GetSettingsStorageLocation(SSOT_CURRENT_USER);
        CAutoPtr<SettingsStorage> pSS;        
        KLPRSS::KLPRSS_OpenSettingsStorageRW(wstrLocation, &pSS);
        ... Пишем в хранилище...
        pInstance->NotifyAboutSettingsChange(
                                        MY_PRODUCT,
                                        MY_VERSION,
                                        MY_SECTION,
                                        L"",
                                        NULL,
                                        NULL,
                                        SSOT_CURRENT_USER);// тип хранилища
                              
	*/
    virtual void NotifyAboutSettingsChange(
        const std::wstring & productName, 
        const std::wstring & version,
        const std::wstring & section, 
        const std::wstring & parameterName,
        const KLPAR::Value * oldValue,
        const KLPAR::Value * newValue,
        KLPRSS::SS_OPEN_TYPE type = KLPRSS::SSOT_SMART) = 0;
    
    };



}


#endif // KLPRCI_PRODUCTSETTINGS_H
