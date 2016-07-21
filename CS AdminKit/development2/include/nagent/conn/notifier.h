#ifndef KLCONN_NOTIFIER_H
#define KLCONN_NOTIFIER_H

#include "./conn_types.h"
#include "./app_synchronizer.h"
#include "./tasks_synchronizer.h"
/*KLCSAK_PUBLIC_HEADER*/

namespace KLCONN
{
    /*!
     \brief Интерфейс, отвечающий за нотификацию системы администрирования
        о изменениях в состоянии приложения.

        Интерфейс реализуется системой администрирования.
        Интерфейс используется библиотекой интеграции.

        Вызовы методов интерфейса могут производится на разных нитях.
    */


    class KLSTD_NOVTABLE  Notifier : public KLSTD::KLBaseQI
    {
    public:
    /*!
      \brief	Уведомляет об изменениях в настройках приложения. Данный
                метод следует вызывать при создании/удалении/изменении
                разделов настроек.

      \param	pSectionNames [in] массив указателей на имена секций.
      \param	nSectionNames [in] длина массива pSectionNames.

     */
        virtual void SettingsChange(
                    const wchar_t** pSectionNames,
                    size_t          nSectionNames) = 0;

    /*!
      \brief Уведомляет об изменениях в настройках задач приложений. Данный
                метод следует вызывать при создании/удалении задач или 
                изменении их настроек.

      \param	pTaskIds [in] массив указателей на идентификаторы задач.
      \param	nTaskIds [in] длина массива pTaskIds.

    */
        virtual void TasksSettingsChange(
                    const wchar_t** pTaskIds,
                    size_t          nTaskIds) = 0;


	/*!
      \brief Уведомляет об изменениях в состоянии задачи.

      \param	szwTaskId       [in] идентификатор задачи.
      \param	nNewState       [in] новое состояние задачи.
      \param	pTaskResults    [in] дополнительные параметры.
      \param    tmRiseTime      [in] время публкации события в UTC.

      \remarks  Имеется возможность задать в событии о смене состояния 
                задачи строку описания (например, в случае перехода в 
                состояние TSK_STATE_FAILED имеет смысл добавить описание 
                проблемы). Для этого необходимо поместить в pTaskResults 
                следующие атрибуты.
                KLEVP::c_er_locid (INT_T)- идентификатор форматной строки -- 
                        для локализации плагином 
                KLEVP::c_er_par1...c_er_par9 (STRING_T) - параметры форматной 
                        строки c_er_locid (строки)
                KLEVP::c_er_descr (STRING_T) - описание события (по сути 
                        форматная строка с подставленными c_er_par1...
                        c_er_par9) для корректной локализации событий 
                        при записи в EventLog или отправке по почте. 

                Для состояния TSK_STATE_RUNNING может иметь смысл 
                добавление описания текущего действия (см. выше) и/или 
                процента выполнения задачи с помощью атрибута 
                c_er_completedPercent.
                KLEVP::c_er_completedPercent (INT_T) - процент выполнения.

                Кроме того, можно задать отличное от значения по умолчанию
                значение важности, для этого следует поместить в pTaskResults 
                атрибут c_er_severity. По умолчанию, состояние TSK_STATE_FAILED
                имеет важность KLEVP_EVENT_SEVERITY_ERROR, прочие состояния
                имеют важность KLEVP_EVENT_SEVERITY_INFO.
                KLEVP::c_er_severity (INT_T) - важность, одна из 
                        констант KLEVP_EVENT_SEVERITY_*:
                            KLEVP_EVENT_SEVERITY_INFO, 
                            KLEVP_EVENT_SEVERITY_WARNING, 
                            KLEVP_EVENT_SEVERITY_ERROR, 
                            KLEVP_EVENT_SEVERITY_CRITICAL. 
    */
        virtual void TaskStateChange(
                    const wchar_t*      szwTaskId,
                    KLCONN::TskState    nNewState,
                    KLPAR::Params*      pTaskResults,
                    const raise_time_t& tmRiseTime = raise_time_t::Now()) = 0;


	/*!
      \brief Уведомляет об изменениях в состоянии задачи.

      \param	szwTaskId       [in] идентификатор задачи.
      \param	nCompletion     [in] новое значение процента выполнения.
      \param    tmRiseTime   [in] время публкации события в UTC.

    */
        virtual void TaskCompletionChange(
                    const wchar_t*      szwTaskId,
                    int                 nCompletion,
                    const raise_time_t& tmRiseTime = raise_time_t::Now()) = 0;


    /*!
      \brief Уведомляет о изменениях в состоянии приложения

      \param	nNewState [in] новое состояние приложения.
      \param	pData     [in] дополнительные параметры.
      \param    tmRiseTime   [in] время публкации события в UTC.

    */
        virtual void ApplicationStateChange(
                    KLCONN::AppState    nNewState,
                    KLPAR::Params*      pData,
                    const raise_time_t& tmRiseTime = raise_time_t::Now()) = 0;


    /*!
      \brief	Уведомляет о событии в работе приложения.

      \param	szwEventType [in] имя (тип) события
      \param	pEventBody   [in] тело события.
      \param    tmRiseTime   [in] время публкации события в UTC.

      \remarks  Для последующей корректной обработки и локализации 
                в тело события следует поместить следующие атрибуты:
                KLEVP::c_er_locid (INT_T)- идентификатор форматной строки -- 
                        для локализации плагином 
                KLEVP::c_er_par1...c_er_par9 (STRING_T) - параметры форматной 
                        строки c_er_locid (строки)
                KLEVP::c_er_severity (INT_T) - важность, одна из 
                        констант KLEVP_EVENT_SEVERITY_*:
                            KLEVP_EVENT_SEVERITY_INFO, 
                            KLEVP_EVENT_SEVERITY_WARNING, 
                            KLEVP_EVENT_SEVERITY_ERROR, 
                            KLEVP_EVENT_SEVERITY_CRITICAL. 

                KLEVP::c_er_task_display_name (STRING_T) - 
                        Отображаемое имя задачи, в результате работы 
                        которой было опубликовано событие. 

                Кроме того, следующие поля необходимы для корректной 
                локализации событий при записи в EventLog или отправке по
                почте. 
                KLEVP::c_er_descr (STRING_T) - описание события (по сути 
                        форматная строка с подставленными c_er_par1...c_er_par9). 
                KLEVP::c_er_event_type_display_name (STRING_T) 
                        Локализованое имя типа события.

    */
        virtual void Event(
                        const wchar_t*      szwEventType,
                        KLPAR::Params*      pEventBody,
                        const raise_time_t& tmRiseTime = raise_time_t::Now()) = 0;

        /*!
          \brief	Уведомляет об изменении свойств приложения.
                    см. AppSynchronizer::GetApplicationProperties

          \param    tmRiseTime   [in] время публкации события в UTC.

        */
        virtual void ApplicationPropertiesChange(
                    const raise_time_t& tmRiseTime = raise_time_t::Now()) = 0;

        /*!
          \brief	Уведомляет об изменении состояния защиты реального 
                    времени. см. AppSynchronizer::GetRtpState

          \param    nRtpState   [in] состояние защиты реального времени
          \param    nErrorCode  [in] код ошибки ( для состояния of APP_RTP_STATE_FAILURE)

        */
        virtual void RtpStateChanges(
                        AppRtpState    nRtpState,
                        int            nErrorCode) = 0;
    };    
}

#endif // KLCONN_NOTIFIER_H


