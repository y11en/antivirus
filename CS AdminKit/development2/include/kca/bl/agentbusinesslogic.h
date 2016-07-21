/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	AgentBusinessLogic.h
 * \author	Andrew Kazachkov
 * \date	06.02.2003 13:36:41
 * \brief	
 * 
 */

#ifndef __AGENTBUSINESSLOGIC_H__bR49iSQI7uKcEdBS3DcmQ3
#define __AGENTBUSINESSLOGIC_H__bR49iSQI7uKcEdBS3DcmQ3

#include <kca/prcp/componentproxy.h>

#define KLBLAG_ERROR_BUSY       L"KLBLAG_ERROR_BUSY"
#define KLBLAG_ERROR_INFO		L"KLBLAG_ERROR_INFO"
#define KLBLAG_ERROR_MODULE		L"KLBLAG_ERROR_MODULE";
#define KLBLAG_ERROR_CODE		L"KLBLAG_ERROR_CODE";
#define KLBLAG_ERROR_SUBCODE	L"KLBLAG_ERROR_SUBCODE";
#define KLBLAG_ERROR_MSG		L"KLBLAG_ERROR_MSG";
#define KLBLAG_ERROR_FNAME		L"KLBLAG_ERROR_FNAME";
#define KLBLAG_ERROR_LNUMBER	L"KLBLAG_ERROR_LNUMBER";

#define KLBLAG_ERROR_LOCDATA    L"KLBLAG_ERROR_LOCDATA";    //PARAMS_T
#define KLBLAG_ERROR_FORMATID   L"KLBLAG_ERROR_FORMATID";   //INT_T
#define KLBLAG_ERROR_FORMAT     L"KLBLAG_ERROR_FORMAT";     //STRING_T
#define KLBLAG_ERROR_FORMATARGS L"KLBLAG_ERROR_FORMATARGS"; //ARRAY_T|STRING_T

namespace KLBLAG
{
    typedef std::vector<KLSTD::KLAdapt<KLSTD::CAutoPtr<KLPRCP::ComponentProxy> > > proxies_t;

	/*!
		\brief	Имя компонента бизнес-логики. Данную переменную необходимо
				определить в хранилище SS_PRODINFO в настройках продукта
                (следует воспользоваться функцией KLPRSS_RegisterProduct).
    */
    const wchar_t c_szwBusinessLogicName[]=L"KLBLAG_BL_COMPONENT";//STRING_T

	/*!
		\brief	Имя компонента бизнеслогики, через который следует производить
                запуск задач из хранилища задач.
                Чтобы агент использовал данную функциональность, следует
                определить данную переименную в хранилище SS_PRODINFO в
                настройках продукта (следует воспользоваться функцией
                KLPRSS_RegisterProduct). 
	*/
    const wchar_t c_szwStartTasksThroushBL[]=L"KLBLAG_USE_BL";//STRING_T

	/*!
		\brief	Местонахождение динамической библиотеки прокси для БЛ
                Данная переменная определяется в секции
                KLPRSS_COMPONENT_PRODUCT (Хранилище SS_PRODINFO). Следует
                воспользоваться функцией KLPRSS_RegisterProduct.
    */
    const wchar_t c_szwBusinessLogicProxyLocation[]=L"KLBLAG_BL_PROXY_LOCATION";//STRING_T

	/*!
		\brief	Бизнес-логика может заявлять о своей готовности выполнять
                задачи с помощью события, имя которого прописывается в секции
                KLPRSS_COMPONENT_PRODUCT (Хранилище SS_PRODINFO) в переменной
                c_szwEventReady, значение переменной -- имя события.
                Если переменная не определена, то бизнес-логика считается
                готовой выполнять задачи, если у нё стоит состояние RUNNING.
	*/
    const wchar_t c_szwEventReady[]=L"KLBLAG_EVENT_READY";//STRING_T


/*!
    Публикация данного события означает старт продукта.
*/
const wchar_t c_szwEv_OnAppStart[]=L"KLBLAG_EV_ONAPPSTART";

	
	//! Запустить означенную задачу
const wchar_t c_szwStartScheduledTask[]=L"KLBLAG_START_SCHEDULED_TASK";	
	/*! Параметры имеют следующую структуру.			
			c_szwServerProduct
			c_szwServerVersion
			c_szwServerComponent
			c_szwServerInstance
			c_szwClientProduct
			c_szwClientVersion
			c_szwClientComponent
			c_szwClientInstance
			c_szwServerTaskName
			c_szwServerTaskParams
			c_szwServerTaskTimeout
			c_szwUser
			c_szwDomain
			c_szwPassword
	*/

/*!
	Результаты выполнения задачи имеют следующую структуру:
		- в случае успеха
			c_szwServerProduct		- Имя продукта 
			c_szwServerVersion		- Версия продукта 
			c_szwServerComponent	- Имя компонента
			c_szwServerInstance		- Имя экземпляра компонента, на котором была запущена задача
            c_szwServerTaskName     - Имя типа запущенной задачи
			с_szwTaskId				- Идентификатор запущенной задачи
		- в случае неудачи
			c_szwErrorInfo			- 
				c_szwErrorModule	- Имя модуля, в котором произошла ошибка
				c_szwErrorCode		- Код ошибки
				c_szwErrorMsg		- Сообщение об ошибке
				c_szwErrorFileName	- Имя файла, в котором проиошла ошибка
				c_szwErrorLineNumber	- Строка, на которой произошла ошибка
*/
	
		//!Информация о компоненте-сервере
const wchar_t c_szwServerProduct[]=L"KLBLAG_SERVER_PRODUCT";//< Имя продукта
const wchar_t c_szwServerVersion[]=L"KLBLAG_SERVER_VERSION";//< Версия продукта
const wchar_t c_szwServerComponent[]=L"KLBLAG_SERVER_COMPONENT";//< Имя компонента
const wchar_t c_szwServerInstance[]=L"KLBLAG_SERVER_INSTANCE";//< Идентификатор копии

		//!Информация о компоненте-клиенте
const wchar_t c_szwClientProduct[]=L"KLBLAG_CLIENT_PRODUCT";//< Имя продукта
const wchar_t c_szwClientVersion[]=L"KLBLAG_CLIENT_VERSION";//< Версия продукта
const wchar_t c_szwClientComponent[]=L"KLBLAG_CLIENT_COMPONENT";//< Имя компонента
const wchar_t c_szwClientInstance[]=L"KLBLAG_CLIENT_INSTANCE";//< Идентификатор копии

		//!Информация о задаче
const wchar_t c_szwServerTaskName[]=L"KLBLAG_SERVER_TASKNAME";//< Имя задачи
const wchar_t c_szwServerTaskParams[]=L"KLBLAG_SERVER_TASKPARAMS";//< Парамеры задачи
const wchar_t c_szwServerTaskTimeout[]=L"KLBLAG_SERVER_TASKTIMEOUT";//< Таймаут задачи
const wchar_t c_szwTaskId[]=L"KLBLAG_TASK_ID";			//< INT_T 

		//!Информация о пользователе, под коорым должна работать задача
const wchar_t c_szwUser[]=L"KLBLAG_TASK_USER";			//< STRING_T
const wchar_t c_szwDomain[]=L"KLBLAG_TASK_DOMAIN";		//< STRING_T
const wchar_t c_szwPassword[]=L"KLBLAG_TASK_PASSWORD";	//< STRING_T

		//!Информация об ошибке
const wchar_t c_szwErrorInfo[]=KLBLAG_ERROR_INFO;
const wchar_t c_szwErrorModule[]=KLBLAG_ERROR_MODULE;
const wchar_t c_szwErrorCode[]=KLBLAG_ERROR_CODE;
const wchar_t c_szwErrorMsg[]=KLBLAG_ERROR_MSG;
const wchar_t c_szwErrorFileName[]=KLBLAG_ERROR_FNAME;
const wchar_t c_szwErrorLineNumber[]=KLBLAG_ERROR_LNUMBER;

} // namespace KLBLAG

#endif //__AGENTBUSINESSLOGIC_H__bR49iSQI7uKcEdBS3DcmQ3

// Local Variables:
// mode: C++
// End:
