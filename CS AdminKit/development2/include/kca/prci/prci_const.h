/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prci_const.h
 * \author	Andrew Kazachkov
 * \date	21.04.2003 11:42:52
 * \brief	Константы модуля prci
 * 
 */
/*KLCSAK_PUBLIC_HEADER*/
/*KLCSAK_PUBLIC_HEADER*/


#ifndef __KLPRCI_CONST_H__
#define __KLPRCI_CONST_H__

#define KLPRCI_ASYNCID          L"KLPRCI_EVP_ASYNCID"
#define KLPRCI_PRODUCT_NAME     L"KLPRCI_EVP_PRODUCT_NAME"
#define KLPRCI_PRODUCT_VERSION  L"KLPRCI_EVP_PRODUCT_VERSION"
#define KLPRCI_COMPONENT_NAME   L"KLPRCI_EVP_COMPONENT_NAME"
#define KLPRCI_INSTANCE_ID      L"KLPRCI_EVP_INSTANCE_ID"
#define KLPRCI_SECTION_NAME     L"KLPRCI_EVP_SECTION_NAME"


namespace KLPRCI
{   
    
    const wchar_t c_szAddrLocal[]=L"";
    const wchar_t c_szAddrAny[]=L"*";
    const wchar_t c_szAddrAnySSL[]=L"*SSL";
    /*
        Следующие атрибуты добавляются в контейнер параметров экземпляра компонента
    */
	const wchar_t c_ComponentAsyncId[]=KLPRCI_ASYNCID;              // STRING_T
	const wchar_t c_WasStartedByAgent[]=L"KLPRCI_STARTED_BY_AGENT"; // BOOL_T
	const wchar_t c_AutoStopAllowed[]=L"KLPRCI_AUTOSTOP_ALLOWED";	// BOOL_T
	const wchar_t c_TimeoutStopAllowed[]=L"KLPRCI_TIMEOUT_STOP";	//INT_T

	/*!
		Следующие атрибуты добавляются в контейнер параметров задачи.
	*/	
	const wchar_t c_szwTaskAsyncId[]=KLPRCI_ASYNCID;                //STRING_T
	const wchar_t c_szwTaskIsInvisible[]=L"KLPRCI_TASK_INVISIBLE";  //BOOL_T
    

    /*! This parameter can be queried via ComponentProxy::GetTaskParams 
        call and contains params specified in SetTaskState. 
    */
    const wchar_t c_szwTaskStateExtra[]=L"KLPRCI_TASK_STATE_EXTRA";  //PARAMS_T

    /*! Данное событие c_EventInstanceState  публикуется при изменении
        состояния экземпляра
		Параметры собыия:
            c_evpInstanceNewState,	INT_T  - Старое состояние.
            c_evpInstanceOldState,	INT_T - Новое состояние.
    */
	const wchar_t c_EventInstanceState[]=L"KLPRCI_EV_INSTANCE_STATE";
    const wchar_t c_evpInstanceNewState[]=L"KLPRCI_newState";
    const wchar_t c_evpInstanceOldState[]=L"KLPRCI_oldState";

    /*!
        Событие об изменении в хранилище настроек
        Параметры собыия:
            c_evpProductName
            c_evpVersion
            c_evpSection
            c_evpSsType тип хранилища
            c_evpParameterName   -   Необязательный параметр            
            c_evpOldValue   -   Необязательный параметр
            c_evpNewValue   -   Необязательный параметр
            c_evpUserName
    */
	const wchar_t c_EventSettingsChange[]=L"KLPRCI_EV_SS_CHANGE";        
		const wchar_t c_evpProductName[]=KLPRCI_PRODUCT_NAME;
		const wchar_t c_evpVersion[]=KLPRCI_PRODUCT_VERSION;
		const wchar_t c_evpSection[]=KLPRCI_SECTION_NAME;
        const wchar_t c_evpSsType[]=L"KLPRCI_SS_TYPE";//STRING_T
        const wchar_t c_evpIs_SETTINGS[]=L"KLPRCI_IS_SETTINGS";// BOOL_T
        const wchar_t c_evpChangeType[]=L"KLPRCI_CHANGE_TYPE";//INT_T PRSS::DataChangeType
		const wchar_t c_evpParameterName[]=L"KLPRCI_EVP_PARAMETER";//Необязательный параметр
		const wchar_t c_evpOldValue[]=L"KLPRCI_EVP_OLDVALUE";//Необязательный параметр
		const wchar_t c_evpNewValue[]=L"KLPRCI_EVP_NEWVALUE";//Необязательный параметр
        const wchar_t c_evpUserName[]=L"KLPRCI_EVP_USERNAME"; //sid юзера или пустая строка.

    //! KLPRCI::HSUBSCRIBE - дескриптор подписки
    KLSTD_DECLARE_HANDLE(HSUBSCRIBE); // KLPRCI::HSUBSCRIBE   


	/*!
        Событие c_EventTaskState публикуется при изменении состояния задачи
        Параметры:
            c_evpTaskId,        INT_T - Идентификатор задачи.
            c_evpTaskOldState,  INT_T - Старое состояние задачи.
            c_evpTaskNewState,  INT_T - Новое состояние задачи.
		    c_evpTaskResults,   PARAMS_T - Необязательный параметр - результат.
		    c_evpTaskAsyncID,   STRING_T - 
            c_evpTaskTsId,      STRING_T - Идентификатор задачи в TasksStorage.
                                Если задача не из TasksStorage, атрибут
                                отсутствует или равен пустой строке.
    */
	const wchar_t c_EventTaskState[]=L"KLPRCI_TaskState";

    /*!
        Событие c_EventTaskCompletion публикуется когда изменяется процент
        выполнения задачи.
            c_evpTaskId,        INT_T - Идентификатор задачи.
            c_evpTaskPercent,   INT_T  - Процент выполнения задачи.
		    c_evpTaskAsyncID,   STRING_T
            c_evpTaskTsId,      STRING_T - Идентификатор задачи в TasksStorage.
                                Если задача не из TasksStorage, атрибут
                                отсутствует или равен пустой строке.
    */
	const wchar_t c_EventTaskCompletion[]=L"KLPRCI_TaskCompletion";

    /*!
        Событие c_EventTaskStarted публикуется при запуске задачи
		    c_evpTaskId, STRING_T
		    c_evpTaskAsyncID, INT_T - Идентификатор задачи.        
    */
	const wchar_t c_EventTaskStarted[]=L"KLPRCI_TaskStarted";

		/*!
			Идентификатор задачи
		*/
		const wchar_t c_evpTaskId[]=L"KLPRCI_taskId"; //INT_T

		/*!
			Старое состояние (см. enum TaskState)
		*/
		const wchar_t c_evpTaskOldState[]=L"KLPRCI_oldState"; //INT_T

		/*!
			Новое состояние (см. enum TaskState)
		*/
		const wchar_t c_evpTaskNewState[]=L"KLPRCI_newState"; //INT_T

		/*!
			Параметры (задаются в методе ProductTasksControl::SetState).
		*/
		const wchar_t c_evpTaskResults[]=L"KLPRCI_TASK_RESULTS"; //PARAMS_T

		/*!
			AsyncID задачи (задаётся в методе RunTask).
		*/
		const wchar_t c_evpTaskAsyncID[]=L"KLPRCI_TASKASYNCID";//STRING_T

		/*!
			Прогресс выполнения (задаётся в методе SetCompletion).
		*/
		const wchar_t c_evpTaskPercent[]=L"KLPRCI_COMPLETION"; //INT_T

		/*!
			Идентификатор задачи в TasksStorage. Если задача не из
			TasksStorage, атрибут отсутствует или равен пустой строке.
		*/
		const wchar_t c_evpTaskTsId[]=L"KLPRCI_TASK_TS_ID"; //STRING_T

		/*!
			Идентификатор ошибки уровня KCA. Используется для локализации
			ошибок задач, возникших при старте задачи на уровне KCA.
		*/
	    const wchar_t c_evp_kca_locid[]=L"KLPRCI_kca_loc_id"; // INT_T, из EKCALocId

		/*!
			Имя типа задачи
		*/
		const wchar_t c_evpTaskType[]=L"KLPRCI_taskType";	//STRING_T
		
		enum EKCALocId{
			KLPRCI_LOC_ERR_TASK_ALREADY_RUNNING = 0x00800001
		};
		
		/*!
			
		*/
        const wchar_t c_evpTaskInvisible[]=L"KLPRCI_IS_TASK_INVISIBLE";  //BOOL_T
        
        const wchar_t c_szwTskConnectionInfo[] = L"KLPRCI_TSK_CONN_INFO";

#define KLPRCI_DEFAULT_PING_TIMEOUT  (1000*60*2) // 2 minues
#define KLPRCI_DEFAULT_SS_TIMEOUT    KLSTD_INFINITE

} // namespace KLPRCI

#endif //__KLPRCI_CONST_H__

// Local Variables:
// mode: C++
// End:
