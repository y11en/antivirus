/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file auditsource.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Главный интерфейс модуля ауирования
 * 
 */

#ifndef KLAUD_AUDITSOURCE_H
#define KLAUD_AUDITSOURCE_H

#include <string>

#include <std/base/kldefs.h>
#include <std/base/klbase.h>
#include <std/thr/sync.h>

#include <transport/wat/common.h>

namespace KLAUD
{
	/*
		\brief Тип модифицируемого объекта
	*/
	enum AuditObjectType
	{
		AuditObjectTask,
		AuditObjectGroupTask,
		AuditObjectGlobalTask,
		AuditObjectPolicy,			
		AuditObjectProduct,
		AuditObjectServer,
		AuditObjectHost,
		AuditObjectAdministrationGroup,
		AuditObjectIPDiapason,
		AuditObjectReport,
		AuditObjectNetworkLst,
		AuditObjectSecurityPolicy
	};

	enum ActionType
	{
		ActionConnect,			// Подключение к серверу администрирования
		ActionAdded,			// Добавление объекта ( задачи, политики, хоста в гурппу и т.п. )
		ActionDeleted,			// Удаление объекта ( -//- )			
		ActionModified,			// Модификация свойств объекта ( -//- )
		ActionRun,				// Запуск задачи или конпоненты
		ActionStop,				// Остановка задачи или конпоненты
		ActionSuspend,			// Приостановка выполнения задачи или работы конпонента
		ActionResume,			// Возобновления выполнения задачи или работы конпонента
		ActionHostAdded,		// Добавление хоста в группу администрирования
		ActionHostDeleted,		// Удаление хоста из группы администрирования
		ActionGroupAdded, 		// Добавление группы в группу администрирования
		ActionGroupDeleted,		// Удаление группы из группы администрирования
		ActionNetworkListTask,	// Добавленна задача для элемента сетевого списка
		ActionNetworkListHostTask	// Добавленна задача для всех элемента сетевого списка указанного хоста
	};

	/*
			\brief Callback класс для получения локализаций
	*/
	class ObjectLocalization : public KLSTD::KLBaseQI
	{
	public:
		virtual std::wstring LocalizeProductName( const std::wstring &productName ) = 0;
	};



	class AuditSource : public KLSTD::KLBase
	{
	public:		

		/*
			\brief Определяет активен ли аудит для данного типа объектов				
			
		*/
		virtual bool IsAuditActive( AuditObjectType objectType ) = 0;

		/*
			\brief Функция нотификации об произошедшем действии
				Данная функция публикует события аудита
				
			Информация о текущей пользователе функция берет из ThreadStore в ячейке 
			  KLWAT_SEC_CONTEXT. Если информация о пользователся отсутствует, то событие 
			  не публикуется

		*/
		virtual void NotifyAction( AuditObjectType objectType, ActionType actionType,
			const std::wstring &objectName, 
			const std::wstring param1 =	L"", const std::wstring param2 = L"" ) = 0;

		/*
			\brief Функция нотификации об произошедшем действии
				Данная функция публикует события аудита
				
			Информация о текущей пользователе функция берет из ThreadStore в ячейке 
			  KLWAT_SEC_CONTEXT. Если информация о пользователся отсутствует, то событие 
			  не публикуется

		*/
		virtual void NotifyAction( AuditObjectType objectType, ActionType actionType,
			const int &objectNameLocId, const std::wstring &defObjectName, 
			const int param1LocId =	(-1), const std::wstring defParam1 =	L"",
			const int param2LocId = (-1), const std::wstring defParam2 =	L"" ) = 0;

		/*
			\brief Функция нотификации об подключении нового пользователя
				Данная функция публикует события аудита
		*/
		virtual void NotifyUserConnectAction( struct soap* soap ) = 0;

		/*
			\brief Функция нотификации об произошедшем действии
				Данная функция публикует события аудита
		*/
		virtual void NotifyAction( KLWAT::ClientContext *pClientContext, AuditObjectType objectType, ActionType actionType,
			const std::wstring &objectName, 
			const std::wstring param1 =	L"", const std::wstring param2 = L"" ) = 0;

		/*
			\brief Функция нотификации об изменении секции SetingsStorage
		*/
		virtual void NotifySSChanged( 
			const std::wstring &szwProduct, 
			const std::wstring &szwVersion,
            const std::wstring &szwSection  ) = 0;

		/*
			\brief Устанавливает callback класс для локализаций

			\param pLoc [in] Указатель на класс
		*/
		virtual void SetLocalization( ObjectLocalization *pLoc ) = 0;
	};

	/*
		\brief Функции позволяет временно пропускать публикацию событий аудита
		на текущем потоке. Используется для выполнения служеюных действий 
	*/
	KLCSKCA_DECL void StopThreadAudit();
	KLCSKCA_DECL void StartThreadAudit();

	//\brief Класс автоматического включение/выключения аудита для текущего потока
	class AuditThreadAutoStopper
	{
	public:
		AuditThreadAutoStopper()
		{
			StopThreadAudit();
		}

		~AuditThreadAutoStopper()
		{
			StartThreadAudit();
		}
	};


} // end namespace KLAUD

//!\brief Вспомогательные макросы
#define KLAUD_NOTIFY_SERVER_CONNECT(soap) KLAUD_GetAuditSource()->NotifyUserConnectAction( soap );
//#define KLAUD_NOTIFY_SERVER_CONNECT() KLAUD_GetAuditSource()->NotifyAction(\
//	KLAUD::AuditObjectServer, KLAUD::ActionConnect, L"" );

#define KLAUD_NOTIFY_TASK_MODIFIED( action, taskName, product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectTask, action, taskName, product, version );\
}

#define KLAUD_NOTIFY_GROUPTASK_MODIFIED( action, groupTaskName, product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectGroupTask, action, groupTaskName, product, version );\
}

#define KLAUD_NOTIFY_GLOBALTASK_MODIFIED( action, globalTaskName, product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectGlobalTask, action, globalTaskName, product, version );\
}

#define KLAUD_NOTIFY_POLICY_MODIFIED( action, policyName, product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectPolicy, action, policyName, product, version );\
}

#define KLAUD_NOTIFY_POLICY_STATE_CHANGED( action, policyName, product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectPolicy, action, policyName, product, version );\
}

#define KLAUD_NOTIFY_PRODUCT_MODIFIED( product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectProduct, KLAUD::ActionModified, L"", product, version );\
}

#define KLAUD_NOTIFY_HOST_MODIFIED( action, hostName )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectHost, action, hostName );\
}

#define KLAUD_NOTIFY_SERVER_MODIFIED()\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectServer, KLAUD::ActionModified, L"" );\
}

#define KLAUD_NOTIFY_TASK_STATE_CHANGED( action, taskName, product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectTask, action, taskName, product, version );\
}

#define KLAUD_NOTIFY_GROUPTASK_STATE_CHANGED( action, groupTaskName, product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectGroupTask, action, groupTaskName, product, version );\
}

#define KLAUD_NOTIFY_GLOBALTASK_STATE_CHANGED( action, globalTaskName, product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectGlobalTask, action, globalTaskName, product, version );\
}

#define KLAUD_NOTIFY_PRODUCT_STATE_CHANGED( action, product, version )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectProduct, action, product, version );\
}

#define KLAUD_NOTIFY_ADMGROUP_CHANGED( groupName, action, hostOrGroupName )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectAdministrationGroup, action, groupName, hostOrGroupName );\
}

#define KLAUD_NOTIFY_IPDIAPASON_CHANGED( diapasonName, action )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectIPDiapason, action, diapasonName );\
}

#define KLAUD_NOTIFY_REPORT_CHANGED( reportOrStencilName, action )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectReport, action, reportOrStencilName );\
}

#define KLAUD_NOTIFY_NETWORKLIST_ADD_TASK( networkListLocId, networkListName, taskNameLocId, taskName, hostName )\
{\
	KLAUD_GetAuditSource()->NotifyAction	(\
		KLAUD::AuditObjectNetworkLst, KLAUD::ActionRun, networkListLocId, networkListName, taskNameLocId, taskName, (-1), hostName );\
}

#define KLAUD_NOTIFY_SPL_CHANGED( szwGroupName )\
{\
	KLAUD_GetAuditSource()->NotifyAction(\
		KLAUD::AuditObjectSecurityPolicy, KLAUD::ActionModified, szwGroupName );\
}

#define KLAUD_NOTIFY_SS_CHANGED( szwProduct, szwVersion, szwSection )\
{\
	KLAUD_GetAuditSource()->NotifySSChanged(\
		szwProduct, szwVersion, szwSection );\
}


KLCSKCA_DECL KLAUD::AuditSource *KLAUD_GetAuditSource();

/*
	\brief Функции инициализаци/деинициализации библиотеки
*/
DECLARE_MODULE_INIT_DEINIT2( KLCSKCA_DECL, KLAUD );

#endif // KLAUD_AUDITSOURCE_H
