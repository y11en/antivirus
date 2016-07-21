/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file conn/settings_synchronizer.h
 * \author Андрей Казачков
 * \date 2004
 * \brief Интерфейс, отвечающий за синхронизацию настроек
 *        приложения с настройками системы администрирования. 
 *
 */

#ifndef KLCONN_SETTINGS_SYNCHRONIZER_H
#define KLCONN_SETTINGS_SYNCHRONIZER_H
/*KLCSAK_PUBLIC_HEADER*/

#include "./conn_types.h"

#include "./settings_synchronizer_base.h"

namespace KLCONN
{
    /*!
     \brief Интерфейс, отвечающий за синхронизацию настроек приложения
	        с настройками системы администрирования.

        Интерфейс реализуется библиотекой интеграции.
        Интерфейс используется системой администрирования.

        Вызовы методов интерфейса могут производится на разных нитях, но при
        этом сериализуются вызывающей стороной.

        Настройки состоят из одной нескольких секций. Каждая секция 
        представлена именем и содержимым -- контейнером Params.
        Имя секции  чувствительно к регистру символов, имеет длину не 
        большую чем 31 символ, не может содержать символы \ / : * ? \ " < > 
        и не должно начинаться на точку.
    */

    class KLSTD_NOVTABLE SettingsSynchronizer : public SettingsSynchronizerBase
    {
    public:

    /*!
      \brief Возвращает список имен секций настроек приложения.
        В случае ошибки выбрасывается исключение типа KLERR::Error*

	  \param vecSections [out] Список имеющихся у приложений секций настроек
                    Возвращаемую память освобождает вызывающая сторона.

    */
        virtual void GetSectionsList(
                    KLSTD::AKWSTRARR& vecSections)  = 0;

    /*!
      \brief Возвращает текущее значение настроек для заданной секции.
        В случае ошибки выбрасывается исключение типа KLERR::Error*

		\param szwSection [in]	Имя секции для чтения настроек.
        \param pFilter    [in]  Фильтр; возвращаются, по крайней мере, те
                переменные, которые присутствуют в фильтре; если равен
                NULL, возвращается все содержимое секции.
		\param ppSettings [out]	Контейнер Params со содержимым данной
                секции. Библиотека интеграции не должна хранить ссылки на
                данный контейнер и его содержимое.
		
		\exception KLSTD::STDE_NOTFOUND - секции с указанным именем не
            существует
    */
        virtual void GetSection (
			        const wchar_t*  szwSection,
                    KLPAR::Params*  pFilter,
			        KLPAR::Params** ppSettings) = 0;


    /*!
      \brief Записывает секцию в хранилище. Если секции не существует, она
        создается; если секция уже существует, то ее старое содержимое
        заменяется на pData.
        В случае ошибки выбрасывается исключение типа KLERR::Error*
        
		\param szwSection   [in] Имя секции для обновления.
		\param pData	    [in] содержимое для данной секции.
        \param ppUnsupported[out] неподдерживаемые переменные, т.е. 
                неизвестные коннектору переменные из pData, которые не
                могут быть им коннектором сохранены.

    */
        virtual void PutSection (
			        const wchar_t*  szwSection,
			        KLPAR::Params*  pData,
                    KLPAR::Params** ppUnsupported)  = 0;

    /*!
      \brief Удаляет секцию с заданным именем.
        В случае ошибки выбрасывается исключение типа KLERR::Error*

		\param szwSection [in] Имя секции для удаления.

		\return false - если секции не существует
        \return true - если секции была успешно удалена

    */
        virtual bool DeleteSection (
			        const wchar_t*  szwSection)  = 0;

    };
    
}


#endif // KLCONN_SETTINGS_SYNCHRONIZER_H

