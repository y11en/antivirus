/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TSK/GroupTaskControl.h
 * \author Михаил Кармазин
 * \date 2002
 * \brief Интерфейс для работы с групповыми задачами. Использует TaskStorage.
 *
 */

#ifndef __TSK_GROUPTASKCONTROL_H__
#define __TSK_GROUPTASKCONTROL_H__

#include <kca/prts/tasksstorage.h>
#include <kca/prci/producttaskscontrol.h>
#include "./grouptaskstatus.h"
#include <srvp/evp/eventproperties.h>
#include <server/tsk/common.h>

namespace KLTSK
{
	const wchar_t c_need_reboot_cnt[] = L"KLTSK_NEED_RBT_CNT"; // INT_T - cм. GroupTaskControl::GetTaskStatistics
	const wchar_t c_error_reasons_cnt[] = L"KLTSK_ERR_REASONS_CNT"; // PARAMS_T - см. GroupTaskControl::GetTaskStatistics
	const wchar_t c_virtual_progress_active[] = L"KLTSK_VIRT_PRGS"; // BOOL_T - см. GroupTaskControl::GetTaskStatistics

    /*!
    * \brief Интерфейс для работы с групповыми задачами.
    */
    class GroupTaskControl : public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief Возвращает TasksStorage для групповых задач (т.е. инициированных с сервера).
                    ID каждой задачи для любого TasksStorage, возвращенного этой функцией, 
                    уникально среди всех групповых задач, т.е. для задач из всех TaskStorage, 
                    полученных этой функцией и функцией GetCommonTasks.

          \param nGroupId   ID группы

          \return интерфейс TasksStorage
        */
        virtual KLSTD::CAutoPtr<KLPRTS::TasksStorage> GetGroupTasks( int nGroupId ) = 0;

        virtual void ForbidTaskForGroup( int nGroupId, const std::wstring& wstrTaskId ) = 0;

        /*!
          \brief Возвращает TasksStorage для групповых задач, инициированных с сервера, не имеющих
                    отношение. к определенной группе. ID каждой задачи для любого TasksStorage,
                    возвращенного этой функцией, уникально среди всех групповых задач.

          \return интерфейс TasksStorage
        */
        virtual KLSTD::CAutoPtr<KLPRTS::TasksStorage> GetCommonTasks() = 0;

        /*!
          \brief Возвращает TasksStorage для локальных задач, т.е. инициированных с 
                    локальной машины. ID каждой задачи для TasksStorage, 
                    возвращенного этой функцией, уникально ТОЛЬКО среди задач этой 
                    локальной машины.
          \param domain   имя доиена, в котором находится хост.
          \param name     имя хоста

          \return интерфейс TasksStorage
        */
        virtual KLSTD::CAutoPtr<KLPRTS::TasksStorage>
        GetHostTasks( const std::wstring & domain, const std::wstring & name ) = 0;

        /*
          \brief Возвращает GroupTaskStatus для групповых задач, т.е. задач, инициированных с 
                    сервера.

          \param wstrTaskId [in]  ID задачи, полученной от TaskStorage, возвращенного
            либо GetCommonTasks(), либо GetGroupTasks()
        */
        virtual KLSTD::CAutoPtr<GroupTaskStatus> GetGroupTaskStatus( const std::wstring& wstrTaskId ) = 0;

        /*
          \brief Возвращает GroupTaskHistory для групповых задач, т.е. задач, инициированных с 
                    сервера.

          \param wstrTaskId [in]  ID задачи, полученной от TaskStorage, возвращенного
            либо GetCommonTasks(), либо GetGroupTasks()
        */
        virtual KLSTD::CAutoPtr<KLEVP::EventProperties> GetGroupTaskHistory(
            const std::wstring& wstrTaskId,
	        const std::vector<std::wstring>& vect_fields,
	        const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
            long  lifetime,
            const std::wstring& wstrDomainName = L"" ,
            const std::wstring& wstrHostName = L"",
			const KLPAR::Params* pParamsIteratorDescr = NULL /*! Параметры фильтрации событий*/) = 0;

        /*!
          \brief Запуск задачи "прямо сейчас"

            \param wstrTaskId  ID групповой задачи, полученный от TaskStorage,
                возвращенного либо GetGroupTasks, либо GetCommonTasks.
            \wstrHostName Имя хоста, на котором запускать задачу. Может быть пустой строкой - тогда
                    запускается на всех хостах, на которых должна работать эта задача.
                    Если не пустая строка, то указанный хост должен входить в спсиок таких хостов.
            \bEvenIfAlreadySucceeded если true, то запускаеться для каждого хоста, даже если на 
                    этом хосте, задача уже успешно отработала. Если false, то запускаеся лишь на хостах,
                    где задача успешно не завершалась. Параметр имеет смысл и в случае, когда указан 
                    один хост.
        */
        virtual void RunTaskNow(
            const std::wstring& wstrTaskId,
            const std::wstring& wstrDomainName = L"",
            const std::wstring& wstrHostName = L"",
            bool bEvenIfAlreadySucceeded = false
        ) = 0;

        /*!
          \brief Приостановка задачи

            \param wstrTaskId  ID задачи из TaskStorage, возвращаемого функцией GetServerInitiatedTasks
            \param domain   имя доиена, в котором находится хост. Если пустая строка, 
                                то TaskId - это ID групповой задачи, иначе 
                                это ID локальной задачи на хосте.
            \param name     имя хоста
        */
        virtual void SuspendTask( const std::wstring& wstrTaskId,
                                  const std::wstring & domain,
                                  const std::wstring & name ) = 0;


        /*!
          \brief Рестарт задачи после операции SuspendTask

            \param wstrTaskId  ID задачи из TaskStorage, возвращаемого функцией GetServerInitiatedTasks
            \param domain   имя доиена, в котором находится хост. Если пустая строка, 
                                то TaskId - это ID групповой задачи, иначе 
                                это ID локальной задачи на хосте.
            \param name     имя хоста
        */
        virtual void ResumeTask( const std::wstring& wstrTaskId,
                                 const std::wstring & domain,
                                 const std::wstring & name ) = 0;


        /*!
          \brief Отмена задачи

            \param wstrTaskId  ID задачи из TaskStorage, возвращаемого функцией GetServerInitiatedTasks
            \param domain   имя доиена, в котором находится хост. Если пустая строка, 
                                то TaskId - это ID групповой задачи, иначе 
                                это ID локальной задачи на хосте.
            \param name     имя хоста
        */
        virtual void CancelTask( const std::wstring& wstrTaskId,
                                 const std::wstring & domain,
                                 const std::wstring & name ) = 0;



        /*!
          \brief GetAllTasksOfHost - получение всех групповых и глобальных задач данного хоста

            \param wstrDomainName - домен
            \param wstrHostName   - имя хоста
            \param vectTaskIds [out] - вектор из TsID задач
        */
        virtual void GetAllTasksOfHost(
            const std::wstring & wstrDomainName,
            const std::wstring & wstrHostName,
            std::vector<std::wstring> & vectTaskIds ) = 0;

        /*!
          \brief GetTaskByID - Возвращает описание задачи с заданным ID.
            ID задач уникальны среди всех TaskStorage'ей, возвращаемых
            методами этого интерфейса.

            \param taskId			[in]  Идентификатор задачи в Task Storage.
            \param componentId      [out]  Идентификатор компонента.
            \param taskName         [out]  Имя задачи, запланированной для выполнения.
            \param task             [out]  Описание запланированной к выполнению задачи.
		    \param params			[out]  Параметры задачи
		    \param paramsTaskInfo	[out]  Дополнительные параметры задачи
		    \param startOnEvent     [out]  Запускается ли задача при возникновении в системе определенного события.
        */
        virtual void GetTaskByID ( 
						const std::wstring &taskId,
                        KLPRCI::ComponentId & id,
                        std::wstring & taskName,
						bool& bStartOnEvent,
                        KLSCH::Task ** task,
						KLPAR::Params** params,
						KLPAR::Params** paramsTaskInfo ) = 0;

        /*!
          \brief GetTaskStatistics - Возвращает суммарные кол-ва хостов, находящихся в
            различных состояниях.
            \param wstrTaskId		[in]  Идентификатор задачи в Task Storage.
            \param wstrTaskId       [out] Params со статистикой. Содержит в себе элементы IntValue с
                    именами, равными строковыми представлениям в десяnичной форме
                    значений типа HostState. Т.е. может присутствовать если hsNone=0x01, то
                    элемент с именем "1" будет содержать в себе кол-во хостов для данной
                    задачи со статусом hsNone.
                    В ppStatistics могут отсутствовать те или иные значения, или н даже может
                    быть пустым.
					Также могут присутствовать параметры:
					 - KLEVP::c_er_completedPercent - общий % выполнения задачи;
					 - KLTSK::c_need_reboot_cnt - число компьютеров, ожидающих перезагрузки для завершения задачи;
					 - KLTSK::c_error_reasons_cnt - Params, имя - причина ошибки (KLTSK::RIFailReason), 
						значение - число компьютеров, на которых задача завершилась с такой ошибкой.
					 - KLTSK::c_virtual_progress_active - Bool, true, если общий % выполнения может меняться 
						без событий об изменении % выполнения задачи на определенном хосте.
        */
        virtual void GetTaskStatistics(
			const std::wstring & wstrTaskId,
			KLPAR::Params** ppStatistics ) = 0;
        
        /*!
          \brief Устанавливает на начало итератор для просмотра истории выполнения 
                    задач.
 
          \param nSecondsToLive - время жизни итератора в секундах.
        */
        // virtual void ResetIteratorForTaskHistory( long nSecondsToLive = 20 ) = 0;


        /*!
          \brief Возвращает следующий блок из TaskHistory (см.функцию ResetIteratorForTaskHistory)

            \param nRecCount            [in]  in - максимальное количество записей
            \param vectTaskHistoryItem  [out] вектор из TaskHistory

            \return     флаг EOF
        */
        // virtual bool GetNextTaskHistoryBlock( long& nRecCount,
         //   std::vector<TaskHistoryItem>& vectTaskHistoryItem ) = 0;
    };
	
    class GroupTaskControl2 : public GroupTaskControl
    {
    public:
        /*!
          \brief Возвращает TasksStorage для групповых задач (т.е. инициированных с сервера),
					включая групповые задач в родительских групп.
                    ID каждой задачи для любого TasksStorage, возвращенного этой функцией, 
                    уникально среди всех групповых задач, т.е. для задач из всех TaskStorage, 
                    полученных этой функцией и функцией GetCommonTasks.

          \param nGroupId   ID группы

          \return интерфейс TasksStorage
        */
        virtual KLSTD::CAutoPtr<KLPRTS::TasksStorage> GetGroupTasks2( int nGroupId, bool bIncludeSupergroups ) = 0;

        /*!
          \brief Возвращает идентификатор группы для групповой задачи.

          \param wstrTaskId   ID задачи

          \return ID группы
        */
        virtual int GetTaskGroup( const std::wstring& wstrTaskId ) = 0;
	};

}

#endif // __TSK_GROUPTASKCONTROL_H__