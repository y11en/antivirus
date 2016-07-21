#ifndef __KL_SETTINGS_SYNCHRONIZER_BASE_H__
#define __KL_SETTINGS_SYNCHRONIZER_BASE_H__

#include "./conn_types.h"

namespace KLCONN
{
    typedef enum
    {
        SSC_SUPPORT_LIST_LOCK   = 0x1,  //! Хранилище поддерживает "блокировку списка"
        SSC_SUPPORT_ITEM_LOCK   = 0x2   //! Хранилище поддерживает блокировку элементов
    }SS_CAPABILITIES;

    /*!
     \brief Родительский интерфейс для интерфейсов, отвечающих за
            синхронизацию настроек.

        Вызовы методов интерфейса могут производится на разных нитях, но при
        этом сериализуются вызывающей стороной.
    */

    class KLSTD_NOVTABLE SettingsSynchronizerBase : public KLSTD::KLBaseQI
    {
    public:
        /*!
        \brief Блокирует хранилище - предотвращает удаление/добавление
            элементов хранилища другими потоками, но позволяет изменение
            отдельных элементов. Каждому успешному вызову LockList
            соответствует вызов UnlockList.
            Если блокировка хранилища поддерживается, то значение,
            возвращаемое методом GetCapabilities, должно содержать флаг
            SSC_SUPPORT_LIST_LOCK. 
            В случае ошибки выбрасывается исключение типа KLERR::Error*

        \param lMaxWaitTime - максимальное время ожидания блокировки, мс

        \exception STDE_NOFUNC - блокировка не поддерживается
        \exception STDE_TIMEOUT - не удалось заблокировать хранилище за
                время lMaxWaitTime.
        */

        virtual void LockList(long lMaxWaitTime) = 0;

        /*!
          \brief	Снимает блокировку, установленную методом LockList.
            В случае ошибки выбрасывается исключение типа KLERR::Error*

          \exception STDE_NOFUNC - блокировка не поддерживается
        */
        virtual void UnlockList() = 0;

        /*!
        \brief Блокирует указанный элемент хранилища - предотвращает
            его изменение другими потоками. В хранилище настроек роль
            элементов играют секции, в хранилище настроек задач - задачи.
            Каждому успешному вызову LockItem соответствует вызов
            UnlockItem. Если блокировка элементов хранилища
            поддерживается, то значение, возвращаемое методом
            GetCapabilities, должно содержать флаг
            SSC_SUPPORT_ITEM_LOCK. 
            В случае ошибки выбрасывается исключение типа KLERR::Error*

        \param lMaxWaitTime - максимальное время ожидания блокировки, мс

        \exception STDE_NOFUNC - блокировка не поддерживается
        \exception STDE_TIMEOUT - не удалось заблокировать элемент за
                время lMaxWaitTime.
        \exception STDE_NOTFOUND - элемент с указанным именем не найден
        */

        virtual void LockItem(
                        const wchar_t*  szwName,
                        long            lMaxWaitTime) = 0;

        /*!
          \brief	Снимает блокировку, установленную методом LockItem.
            В случае ошибки выбрасывается исключение типа KLERR::Error*

          \exception STDE_NOFUNC - блокировка не поддерживается
          \exception STDE_NOTFOUND - элемент с указанным именем не найден
        */
        virtual void UnlockItem(const wchar_t* szwName) = 0;

        /*!
          \brief Возвращает набор флагов, описывающих возможности
            хранилища

          \return флаги SSC_SUPPORT_* объединенные с помощью '|'
        */
        virtual unsigned long   GetCapabilities() = 0;
    };
};

#endif //__KL_SETTINGS_SYNCHRONIZER_BASE_H__
