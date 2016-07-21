
/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file stdsoap.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief SOAP header для библиотеки STD
 */


/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file common.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Общие типы
 */

class xsd__anyType {
public:
	virtual int getType();
}; 

class xsd__anySimpleType: public xsd__anyType {
public:
	virtual int getType();
}; 

typedef char *xsd__anyURI; 
class xsd__anyURI_: public xsd__anySimpleType {
public:	
	xsd__anyURI __item;
	virtual int getType();
}; 

typedef bool xsd__boolean; 
class xsd__boolean_: public xsd__anySimpleType {
public:
	xsd__boolean __item;
	virtual int getType();
}; 

typedef char *xsd__date; 
class xsd__date_: public xsd__anySimpleType {
public:
	xsd__date __item;
	virtual int getType();
}; 

typedef char* xsd__dateTime; 
class xsd__dateTime_: public xsd__anySimpleType {
public:	
	xsd__dateTime __item;
	virtual int getType();
}; 

typedef double xsd__double; 
class xsd__double_: public xsd__anySimpleType{
public:
	xsd__double __item;
	virtual int getType();
}; 

typedef char *xsd__duration; 
class xsd__duration_: public xsd__anySimpleType {
public:
	xsd__duration __item;
	virtual int getType();
}; 

typedef float xsd__float; 
class xsd__float_: public xsd__anySimpleType {
public:
	xsd__float __item;
	virtual int getType();
}; 

typedef char *xsd__time; 
class xsd__time_: public xsd__anySimpleType {
public:
	xsd__time __item;
	virtual int getType();
}; 

typedef char *xsd__decimal; 
class xsd__decimal_: public xsd__anySimpleType {
public:
	xsd__decimal __item;
	virtual int getType();
}; 

typedef char *xsd__integer; 
class xsd__integer_: public xsd__decimal_ {
public:
	xsd__integer __item;
	virtual int getType();
}; 

typedef LONG64 xsd__long; 
class xsd__long_: public xsd__integer_ {
public:
	xsd__long __item;
	virtual int getType();
}; 

typedef long xsd__int; 
class xsd__int_: public xsd__long_ {
public:
	xsd__int __item;
	virtual int getType();
}; 

typedef short xsd__short; 
class xsd__short_: public xsd__int_ {
public:
	xsd__short __item;
	virtual int getType();
}; 

typedef char xsd__byte; 
class xsd__byte_: public xsd__short_{
public:
	xsd__byte __item;
	virtual int getType();
}; 

typedef char *xsd__nonPositiveInteger; 
class xsd__nonPositiveInteger_: public xsd__integer_ {
public:
	xsd__nonPositiveInteger __item;
	virtual int getType();
}; 

typedef char *xsd__negativeInteger; 
class xsd__negativeInteger_: public xsd__nonPositiveInteger_ {
public:
	xsd__negativeInteger __item;
	virtual int getType();
}; 

typedef char *xsd__nonNegativeInteger; 
class xsd__nonNegativeInteger_: public xsd__integer_ {
public:
	xsd__nonNegativeInteger __item;
	virtual int getType();
}; 

typedef char *xsd__positiveInteger; 
class xsd__positiveInteger_: public xsd__nonNegativeInteger_ {
public:
	xsd__positiveInteger __item;
	virtual int getType();
}; 

typedef ULONG64 xsd__unsignedLong; 
class xsd__unsignedLong_: public xsd__nonNegativeInteger_{
public:
	xsd__unsignedLong __item;
	virtual int getType();
}; 

typedef unsigned long xsd__unsignedInt; 
class xsd__unsignedInt_: public xsd__unsignedLong_ {
public:
	xsd__unsignedInt __item;
	virtual int getType();
}; 

typedef unsigned short xsd__unsignedShort; 
class xsd__unsignedShort_: public xsd__unsignedInt_ {
public:
	xsd__unsignedShort __item;
	virtual int getType();
}; 

typedef unsigned char xsd__unsignedByte; 
class xsd__unsignedByte_: public xsd__unsignedShort_ {
public:
	xsd__unsignedByte __item;
	virtual int getType();
}; 

typedef wchar_t *xsd__wstring; 
class xsd__wstring_: public xsd__anySimpleType {
public:
	xsd__wstring __item;
	virtual int getType();
}; 

typedef char *xsd__string; 
class xsd__string_: public xsd__anySimpleType {
public:
	xsd__string __item;
	virtual int getType();
}; 

typedef char *xsd__normalizedString; 
class xsd__normalizedString_: public xsd__string_ {
public:
	xsd__normalizedString __item;
	virtual int getType();
}; 

typedef char *xsd__token; 
class xsd__token_: public xsd__normalizedString_ {
public:
	xsd__token __item;
	virtual int getType();
}; 

class xsd__base64Binary: public xsd__anySimpleType{
public:
	unsigned char *__ptr;
	int __size;
	virtual int getType();
}; 

class xsd__hexBinary: public xsd__anySimpleType {
public:
	unsigned char *__ptr;
	int __size;
	virtual int getType();
}; 

class SOAPComponentId {
public:
	SOAPComponentId();
	xsd__wstring productName;
	xsd__wstring version;
	xsd__wstring componentName;
	xsd__wstring instanceId;
};

struct SOAPComponentIdsList
{
	SOAPComponentId* 		__ptr;
	int						__size;
};

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file sertypes.h
 * \author Андрей Казачков
 * \date 2002
 * \brief 
 */


class param_error_loc_args
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

class param_error_loc
{
public:
    xsd__int                format_id;
    xsd__wstring            format;
    xsd__wstring            locmodule;
    param_error_loc_args    args;
};

//struct param_error;

struct param_error{
	xsd__int            code;
	xsd__wstring        module;
	xsd__string         file;
	xsd__int            line;
	xsd__wstring        message;
    param_error_loc*    locdata;
    //param_error*        prev;
};

typedef xsd__boolean param__null;


//Value
class param__value{
public:
	xsd__anyType* data;
};

class param__entry{
public:
	xsd__wstring	name;
	param__value	value;
};

//Params
class param__node{
public:
	param__entry*	    __ptr;
	int				    __size;
};

//ParamsValue
class param__params : public xsd__anyType{
public:
	param__node *node;
	virtual int getType();
    xsd__int            checksum;
    xsd__base64Binary   binfmt;
};

//ArrayValue
class param__arrayvalue: public xsd__anyType{
public:
	param__value*	__ptr;
	int				__size;
	virtual int getType();
};

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TransportSOAPFuncs.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Опеределение системных функций транспорта. Данные функции вызываются транспортом
 *			для корректного установления и поддержания соединения. Все функции вызываются
 *			по протоколу SOAP.
 */

extern typedef struct message_desc_t{
	long requestFlag;
	long messageId;
} message_desc;

struct SOAP_ENV__Header 
{ 
	message_desc messDesc;	
}; 

class SOAPTransportLoc {
public:	
	xsd__wstring remoteName;
	xsd__wstring location;
	xsd__boolean unicastLocFlag;
	xsd__boolean useSSLUnicast;
	xsd__boolean compressTraffic;
};


struct TransportLocList
{
	SOAPTransportLoc	*__ptr;
	int					__size;
};


class SOAPPermissions
{
public:
	xsd__unsignedLong	Allow;
	xsd__unsignedLong	Deny;
	xsd__wstring		UserAccountName;
};

class SOAPConnectionOptions
{
public:
	xsd__boolean		CompressTrafic;
	xsd__boolean		FlagNewClientVersion;
	xsd__unsignedLong	TransportVersion;
	xsd__boolean		DontWaitCleaningEventQueue;
};

//!\brief Функция инициации соединения
int tr_InitiateRemoteConnection( char *remoteName, char *localName, int *result );


//!\brief Функция создания обратного соединения
int tr_CreateReverseConnection( char *remoteClientConnName, char *connName,
			char *remoteClientLoc, char *authClientName, int gatewayConnFlag,
			int connPingTimeout, int connPriority, 
			SOAPPermissions *permissions, SOAPConnectionOptions *options,
			int *result );

//!\brief Функция создания получения результата создания обратного получения
int tr_ReverseConnectionReply( char *remoteServerConnName, 
			char *clientConnName, int pingTimeout, 
			SOAPPermissions *permissions, SOAPConnectionOptions *options,
			int *result );

//!\brief Функция получения Ping сообщений
int tr_PingMethod( char *remoteClientConnName, int *result );


//!\brief Функция для осуществления authntication handshake
int tr_AuthenticationHandshake( xsd__wstring inClientIdentity, xsd__hexBinary inString, 
							   int connPriority,
							   struct tr_AuthHandshakeResult{ 
									xsd__hexBinary outString;
									xsd__wstring   outClientIdentity;
								} &r );


//!\brief Функция для установления gateway соединения
int tr_CreateGatewayConnection( xsd__wstring localName, struct TransportLocList *processNamesList, 
							   int nextProcessPos, int connectTimeout, int *result );

//!\brief Функция для тестирования библиотеки
int tr_TestMethod(int num, char *str, int *result);


//!\brief Функция для тестирования библиотеки
int tr_TestMethod1(int num, char *str, struct tr_TestMethod1Result{ 
									xsd__string outString;
								} &r);

//!\brief Системная функция для организации контроля трафика
int tr_TrafficControlNotify(int error, int *result);

// Local Variables:
// mode: C++
// End:


/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EvSOAPFuncs.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Определение системных функций Eventing'а для вызова через gSOAP. 
 *
 */

struct SOAPSubsIdsList
{
	long *__ptr;
	int  __size;
};

//!\brief Функция удаленной подписки
int ev_RemoteSubscribe( xsd__int subsId, xsd__wstring localCompName, 
		SOAPComponentId *subscriber, SOAPComponentId *publisher, 
		xsd__wstring eventType, param__params eventBodyFilter, int sendToAgentSubs,
		int useOnlyLocalConnsFlag,
		int *result );

//!\brief Функция удаленной отписки
int ev_RemoteUnsubscribe( xsd__int remoteSubsId, int *result );


//!\breif Функция публикации события
int ev_PublishEvent( SOAPComponentId *eventPublisher, xsd__wstring	eventType, 
					param__params eventBody, time_t eventBirthTime, int eventLifeTime,
					struct SOAPComponentIdsList *processedCompsList, 
					struct SOAPSubsIdsList *remoteSubsIdsList,
					int *result );

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file spl/soapapi.h
 * \author Шияфетдинов Дамир
 * \date 2005
 * \brief SOAP-интерфейс SecurityPolicy
 */

struct klspl_permission_unit_t
{	
	xsd__wstring		sid;
	xsd__wstring		userOrUsergroup;
	xsd__boolean		groupFlag;
	xsd__boolean		readonly;
	xsd__boolean		uneraseable;
	xsd__unsignedLong	allowMask;
	xsd__unsignedLong	denyMask;	
};

struct klspl_permission_list_t
{
	struct klspl_permission_unit_t*	__ptr;
	int								__size;
};

struct klspl_permissions_t
{
	struct klspl_permission_list_t	PermissionsList;
	xsd__boolean					InheritedFlag;
	xsd__unsignedLong				VisibilityMask;
};


int klspl_GetPermissions(
						xsd__long		groupId,
						struct klspl_GetPermissionsResponse
						{
							struct klspl_permissions_t	perms;
							struct param_error	error;
						}	&r );

int klspl_SetPermissions(
						xsd__long		groupId,
						struct klspl_permissions_t	perms,
						xsd__boolean	bCheckUserRights,
						struct klspl_SetPermissionsResponse
						{						
							struct param_error	error;
						}	&r );
/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRCI/soapapi.h
 * \author Андрей Казачков
 * \date 2002
 * \brief SOAP-интерфейс Компонента
 *
 */

struct klprci_tasks_t
{
	xsd__int*	__ptr;
	int			__size;
};

int klprci_GetTasksParams(
						xsd__wstring	ID,
						xsd__int		idtask,
						param__params	filter,
						struct klprci_GetTasksParamsResponse
						{
							param__params		pars;
							struct param_error	error;
						}	&r);

int klprci_GetPingTimeout	(xsd__wstring ID,
							 struct klprci_GetPingTimeoutResponse{xsd__int timeout; struct param_error error;} &r);

int klprci_GetState			(xsd__wstring ID,
							 struct klprci_GetStateResponse{xsd__int state; struct param_error error;} &r);

int klprci_GetStatistics	(xsd__wstring ID,
							 param__params	statisticsIn,
							 struct klprci_GetStatisticsResponse{param__params statisticsOut; struct param_error error;} &r);

int klprci_Stop				(xsd__wstring ID,
							 struct klprci_StopResponse{struct param_error error;} &r);

int klprci_Suspend			(xsd__wstring ID,
							 struct klprci_SuspendResponse{struct param_error error;} &r);

int klprci_Resume			(xsd__wstring ID,
							 struct klprci_ResumeResponse{struct param_error error;} &r);

int klprci_RunTask			(xsd__wstring ID,
							 xsd__wstring name,
							 param__params settings,
							 xsd__wstring asyncID,
							 xsd__int timeout,
							 struct klprci_RunTaskResponse{
										xsd__int	taskId;
										struct param_error error;} &r);
                                        
int klprci_RunMethod(        xsd__wstring ID,
                             xsd__wstring name,
							 param__params settings,
							 xsd__int timeout,
							 struct klprci_RunMethodResponse{
                                        param__params res;
										struct param_error error;} &r);

int klprci_GetTasksList		(xsd__wstring ID,
							 struct klprci_GetTasksListResponse{
										struct klprci_tasks_t	taskslist;
										struct param_error		error;} &r);

int klprci_GetTaskName		(xsd__wstring ID,
							 xsd__int idTask,
							struct klprci_GetTaskNameResponse
							{
								struct param_error error;
								xsd__wstring name;
							} &r);


int klprci_TaskStop			(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_TaskStopResponse{struct param_error error;} &r);

int klprci_TaskSuspend		(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_TaskSuspendResponse{struct param_error error;} &r);

int klprci_TaskResume		(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_TaskResumeResponse{struct param_error error;} &r);

int klprci_TaskGetState		(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_GetTaskStateResponse{xsd__int state; struct param_error error;}& r);

int klprci_TaskGetCompletion(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_GetTaskCompletionResponse{xsd__int percent; struct param_error error;}& r);

int klprci_TaskReload(
                        xsd__wstring    ID,
						xsd__int        idTask,
                        param__params   settings,
						struct klprci_TaskReloadResponse{
                            struct param_error  error;
                        }& r);

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRTS/pressoapapi.h
 * \author Андрей Брыксин
 * \date 2002
 * \brief SOAP-интерфейс EventsStorage
 *
 */

int KLPRES_ResetSubscriptionsIterator(xsd__wstring sServerObjectID,
									  SOAPComponentId cidFilter,
                                      xsd__int  nTimeout,
									  struct KLPRES_RESET_SUBSCRIPTIONS_ITERATOR_RESPONSE{ 
											xsd__wstring sSubscrIteratorId; 
											struct param_error rcError; } &rc);

int KLPRES_ReleaseSubscriptionsIterator(xsd__wstring sServerObjectID,
									  xsd__wstring sSubscrIteratorId,
									  struct KLPRES_RELEASE_SUBSCRIPTIONS_ITERATOR_RESPONSE{ 
											struct param_error rcError; } &rc);

int KLPRES_GetNextSubscription( xsd__wstring sServerObjectID,
							    xsd__wstring sSubscrIteratorId,
							    struct KLPRES_GET_NEXT_SUBSCRIPTION_RESPONSE{ 
                                    param__params parSubscription;
                                    xsd__wstring sSubscrId;
                                    xsd__boolean bOk;
								    struct param_error rcError; } &rc);

int KLPRES_AddSubscription(xsd__wstring sServerObjectID,
						   xsd__wstring sSubscriptionID,
						   bool bPersistent, 
						   param__params parSubscription,
						   struct KLPRES_ADD_SUBSCRIPTION_RESPONSE{ 
								xsd__wstring sSubscriptionID; 
								struct param_error rcError; } &rc);

int KLPRES_UpdateSubscription(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID,
							  param__params parSubscription,
							  struct param_error& rc);

int KLPRES_PauseSubscription(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID,
							  struct param_error& rc);
								
int KLPRES_ResumeSubscription(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID,
							  struct param_error& rc);

int KLPRES_RemoveSubscription(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID, 
							  struct param_error& rc);
// Iterators

int KLPRES_ResetEventsIterator(xsd__wstring sServerObjectID,
							   xsd__wstring sSubscriptionID,
							   struct param_error &rc);

int KLPRES_SetEventsIterator(xsd__wstring sServerObjectID,
							 xsd__wstring sSubscriptionID,
							 long eventID,
							 struct param_error &rc);

int KLPRES_ResetEventsIteratorEx(xsd__wstring sServerObjectID,
								 xsd__wstring sSubscriptionID,
								 struct KLPRES_RESET_EVENTS_ITERATOR_EX_RESPONSE{
									xsd__wstring sJournalID;
									struct param_error rcError; } &rc);

int KLPRES_SetEventsIteratorEx(xsd__wstring sServerObjectID,
							   xsd__wstring sSubscriptionID,
							   long eventID,
							   struct KLPRES_SET_EVENTS_ITERATOR_EX_RESPONSE{
									xsd__wstring sJournalID;
									struct param_error rcError; } &rc);

int KLPRES_MoveToEventWithIndex(xsd__wstring sServerObjectID,
							    xsd__wstring sSubscriptionID,
							    long nIndex,
								struct param_error &rc);

// end of iterators

int KLPRES_GetEventsCount(xsd__wstring sServerObjectID,
						  xsd__wstring sSubscriptionID,
						  struct KLPRES_GET_EVENTS_COUNT_RESPONSE{ 
								xsd__long nCount; 
								struct param_error rcError; } &rc);

int KLPRES_GetNextEventsChunk(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID,
							  struct KLPRES_GET_NEXT_EVENTS_CHUNK_RESPONSE{ 
									param__params pparEventsParamsOut; 
									struct param_error rcError; } &rc);

int KLPRES_ClearEventsList(xsd__wstring sServerObjectID,
						   xsd__wstring sSubscriptionID, 
						   struct param_error& rc);

int KLPRES_DeleteEvents(xsd__wstring sServerObjectID,
						xsd__wstring sSubscriptionID, 
						long nStopEventID, 
						struct param_error& rc);

int KLPRES_DeleteOneEventByID(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID, 
							  long nEventID, 
						      struct param_error& rc);

int KLPRES_DeleteRanges(xsd__wstring sServerObjectID,
						xsd__wstring sSubscriptionID, 
						param__params parRanges, 
                        struct param_error& rc);

int KLPRES_RemoveExpiredEvents(xsd__wstring sServerObjectID,
							   xsd__wstring sSubscriptionID, 
							   struct param_error& rc);

int KLPRES_GetEventStorageFileSize(xsd__wstring sServerObjectID,
									xsd__wstring sSubscriptionID, 
									struct KLPRES_GET_EVENT_STORAGE_FILE_SIZE_RESPONSE{
										xsd__long llStorageSize;
										struct param_error rcError;	}& rc);
/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRTS/prtssoapapi.h
 * \author Андрей Брыксин
 * \date 2002
 * \brief SOAP-интерфейс TaskStorage
 *
 */

int KLPRTS_ResetTasksIterator(xsd__wstring sServerObjectID,
							  SOAPComponentId cidFilter, 
							  xsd__wstring sTaskNameFilter,
							  struct KLPRTS_RESET_TASKS_ITERATOR_RESPONSE{ 
									param__params parTaskListParamsOut; 
									struct param_error rcError; } &rc);

int KLPRTS_GetTaskByID(xsd__wstring sServerObjectID,
					   xsd__wstring TaskID, 
					   struct KLPRTS_GET_TASK_BY_ID_RESPONSE{ 
							param__params parTaskParamsOut; 
							struct param_error rcError; } &rc);

int KLPRTS_GetTaskByIDWithPolicyApplied(xsd__wstring sServerObjectID,
					   xsd__wstring TaskID, 
					   struct KLPRTS_GET_TASK_BY_ID_WPA_RESPONSE{ 
							param__params parTaskParamsOut; 
							struct param_error rcError; } &rc);

int KLPRTS_AddTask(xsd__wstring sServerObjectID,
				   param__params parTaskParams, 
				   struct KLPRTS_ADD_TASK_RESPONSE{ 
						xsd__wstring TaskID; 
						struct param_error rcError; } &rc);

int KLPRTS_UpdateTask(xsd__wstring sServerObjectID,
					  xsd__wstring TaskID, 
					  param__params parTaskParams, 
					  struct param_error& rc);

int KLPRTS_DeleteTask(xsd__wstring sServerObjectID, 
					  xsd__wstring TaskID, 
					  struct param_error& rc);

int KLPRTS_SetTaskStartEvent(xsd__wstring sServerObjectID,
							 xsd__wstring TaskID, 
							 SOAPComponentId cidEventFilter, 
							 xsd__wstring sEventType, 
							 param__params parBodyFilter, 
							 struct param_error& rc);

int KLPRTS_GetTaskStartEvent(xsd__wstring sServerObjectID,
							 xsd__wstring TaskID, 
							 struct KLPRTS_GET_TASK_START_EVENT_RESPONSE {
								 SOAPComponentId cidEventFilter;
								 xsd__wstring sEventType;
								 param__params parBodyFilterOut;
								 struct param_error rcError;
							 } &rc);

int KLPRTS_UpdateTask2(xsd__wstring sServerObjectID,
					   xsd__wstring TaskID, 
					   param__params parTaskParams, 
					   struct param_error& rc);

int KLPRTS_ReplaceTask(xsd__wstring sServerObjectID,
					   xsd__wstring TaskID, 
					   param__params parTaskParams, 
					   struct param_error& rc);

/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sssrv_soapapi.h
 * \author	Andrew Kazachkov
 * \date	05.05.2003 11:52:40
 * \brief	
 * 
 */

class klsssrv_wstrings
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

int klsssrv_Close(
                    xsd__wstring    wstrIdSSS,
                    xsd__wstring    wstrProxyId,
                    struct klsssrv_CloseResponse
                    {
                        struct param_error	error;
                    }& r);

int klsssrv_CreateSettingsStorage(
                    xsd__wstring    wstrIdSSS,
                    xsd__wstring    wstrLocation,
					xsd__int        nCreationFlags,
					xsd__int        nAccessFlags,
                    param__params   options,
                    struct klsssrv_CreateSettingsStorageResponse
                    {
                        xsd__wstring        wstrProxyId;
					    struct param_error	error;
                    }& r);


int klsssrv_Read(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    param__params  extra,
                    struct klsssrv_ReadResponse
                    {
                        param__params       result;
                        struct param_error	error;                 
                    }& r);

int klsssrv_Write(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    xsd__int       nFlags,
                    param__params  data,
                    struct klsssrv_WriteResponse
                    {
                        struct param_error	error;
                    }& r);


int klsssrv_CreateSection(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    struct klsssrv_CreateSectionResponse
                    {
                        struct param_error	error;
                    }& r);

int klsssrv_DeleteSection(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    struct klsssrv_DeleteSectionResponse
                    {
                        struct param_error	error;
                    }& r);

int klsssrv_SetTimeout(
                    xsd__wstring    wstrIdSSS,
                    xsd__wstring    wstrProxyId,
                    xsd__int        lTimeout,
                    struct klsssrv_SetTimeoutResponse
                    {
                        struct param_error	error;
                    }& r);

int klsssrv_GetNames(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    struct klsssrv_GetNamesResponse
                    {
                        klsssrv_wstrings names;
                        struct param_error	error;
                    }& r);

int klsssrv_AttrRead(
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    xsd__wstring   wstrAttr,
                    struct klsssrv_AttrReadResponse
                    {
                        param__value        value;
                        struct param_error	error;
                    }& r);

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file AgentSOAPFuncs.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Опеределение функций AgentInstance. Все функции вызываются
 *			по протоколу SOAP.
 */

// Функции от AgentInstance
class klagent_buffer
{
public:
	xsd__unsignedByte*	__ptr;
	int			__size;
};

int klagent_StartTaskStorageTask(
							xsd__wstring	idTask,
							xsd__wstring	luid,
							xsd__int		lTimeout,
							struct klagent_StartTaskStorageTaskResponse
							{
								struct param_error	error;
							} &r);

//If agent was unable to open clients' process it returns	
int klagent_StartComponentEx(
				SOAPComponentId		componentId,
				param__params		startParams,
				xsd__wstring		asyncId,
				xsd__int			flags,
				xsd__int			asyncTimeout,
				xsd__int			timeoutStop,
				xsd__int			logonType,
				xsd__wstring		user,
				xsd__wstring		domain,
				xsd__wstring		password,
				param__params		extraInParams,
				struct klagent_StartComponentExResponse{
					param__params		extraOutParams;
					struct param_error	error;
				} &r);

int klagent_GetStartedComponentsList( SOAPComponentId filter, 
							struct klagent_GetStartedComponentsListResponse{ struct SOAPComponentIdsList componentsListOut; struct param_error error;} &r );

int klagent_IsComponentStarted( SOAPComponentId componentId, 
						  struct klagent_IsComponentStartedResponse{ xsd__int res; struct param_error error;} &r );

int klagent_GetComponentStartParameters( SOAPComponentId componentId, 
						struct klagent_GetComponentStartParametersResponse{ param__params componentStartParams; struct param_error error;} &r );

int klagent_GetComponentPort(	SOAPComponentId componentId, 
									struct klagent_GetComponentPortResponse{
										xsd__int			nPort;
										struct param_error	error;} &r );

/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	evpsoapapi.h
 * \author	Mikhail Karmazine
 * \date	15:14 26.11.2002
 * \brief	заголовочный файл с описанием SOAP-методов сервера, относящихся к
 *				модулю EVP
 */


// EventProperties -------------------------------

struct klevprop_field_order
{
	xsd__wstring	wstrFieldName;
	xsd__boolean	bOrder;
};

class klevprop_field_order_array
{
public:
	struct klevprop_field_order*	__ptr;
	int								__size;
};

class klevprop_wstring_array
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

class klevprop_int_array
{
public:
	xsd__int*	__ptr;
	int			__size;
};

int klevprop_ResetIterator(
        xsd__int		            type,
        param__params		        paramIteratorDescr,
		klevprop_wstring_array		fields,
		klevprop_field_order_array	fields_to_order,
		xsd__int					lifetime,
		struct klevprop_ResetIteratorResponse{
			xsd__wstring		iteratorId;
			struct param_error	error;
		}&r);

int klevprop_GetRecordCount(
		xsd__wstring	iteratorId,
		struct klevprop_GetRecordCountResponse{
			xsd__int			nCount;
			struct param_error	error;
		}&r);

int klevprop_GetRecordRange(
		xsd__wstring	iteratorId,
		xsd__int		nStart,
        xsd__int		nEnd,
		struct klevprop_GetRecordRangeResponse{
			param__params		paramProperties;
			struct param_error	error;
		}&r);

int klevprop_FindRecordByFirstChars(
		xsd__wstring	iteratorId,
        xsd__wstring	wstrFieldToSearch,
        xsd__wstring	wstrFirstChars,
		struct klevprop_FindRecordByFirstCharsResponse
        {
            xsd__int            nStart;
			struct param_error	error;
		}&r);

int klevprop_DeleteProperties(
		klevprop_int_array	id_array,
		struct klevprop_DeletePropertiesResponse{
			struct param_error	error;
		}&r);

int klevprop_InitiateDelete(
		param__params	paramSettings,
		struct klevprop_InitiateDeleteResponse{
            param__params	paramResponse;
			struct param_error	error;
		}&r);

int klevprop_CancelDelete(
		param__params	paramSettings,
		struct klevprop_CancelDeleteResponse{
            param__params	paramResponse;
			struct param_error	error;
		}&r);

int klevprop_ReleaseIterator(
		xsd__wstring	iteratorId,
		struct klevprop_ReleaseIteratorResponse{
			struct param_error	error;
		}&r);




// EventProcessor -------------------------------

int klevproc_Process(
		param__params		parEventInfoArray,
		xsd__wstring		deliveryId,
		xsd__wstring		domain,
		xsd__wstring		hostName,
		xsd__wstring		agentId,
		struct klevproc_ProcessResponse{
			struct param_error	error;
		}&r);




// NotificationProperties -------------------------------

struct klevpnp_notification_descr_t
{
	xsd__wstring	wstrProduct;
	xsd__wstring	wstrEventType;
    xsd__wstring	wstrSMTPServer;
    xsd__int		nSMTPPort;
	xsd__wstring	wstrEMail;
    xsd__boolean    bUseEmail;
	xsd__wstring	wstrNetSendParameter;
    xsd__boolean    bUseNetSend;
	xsd__wstring	wstrScript;
	xsd__wstring	wstrScriptFolder;
    xsd__boolean    bUseScript;
	xsd__int		lDaysToStoreEvent;
    xsd__wstring	wstrMessageTemplate;
    xsd__boolean    bStoreAtSeverEventLog;
    xsd__boolean    bStoreAtClientEventLog;
    param__params   *pParamsBodyFilter;
    xsd__wstring	wstrEmailSubject;
    xsd__wstring	wstrEmailFrom;
	xsd__boolean    bDenyDefaults;
};

class klevpnp_notification_descr_array
{
public:
	struct klevpnp_notification_descr_t*	__ptr;
	int										__size;
};

int klevpnp_GetDefaultSettings(
		void* _,
		struct klevpnp_GetDefaultSettingsResponse{
			param__params	    paramDefaultSettings;
			struct param_error	error;
		}&r);

int klevpnp_SetDefaultSettings(
		param__params	    paramDefaultSettings,
		struct klevpnp_SetDefaultSettingsResponse{
			struct param_error	error;
		}&r);

int klevpnp_TestNotification(
    param__params    paramSettings,
	struct klevpnp_TestNotificationResponse{
        param__params	    paramResponse;
		struct param_error	error;
	}&r);

int klevpnp_SetNotificationLimits(
    param__params    paramSettings,
	struct klevpnp_SetNotificationLimitsResponse{
		struct param_error	error;
	}&r);

int klevpnp_GetNotificationLimits(
    void* _,
	struct klevpnp_GetNotificationLimitsResponse{
        param__params	    paramResponse;
		struct param_error	error;
	}&r);

int klevpnp_GetCommonProperties(
		void* _,
		struct klevpnp_GetCommonPropertiesResponse{
			klevpnp_notification_descr_array	notificationDescrArray;
			struct param_error					error;
		}&r);

int klevpnp_GetGroupProperties(
		xsd__int			nGroupId,
		struct klevpnp_GetGroupPropertiesResponse{
			klevpnp_notification_descr_array	notificationDescrArray;
			struct param_error					error;
		}&r);

int klevpnp_GetTaskProperties(
		xsd__wstring			wstrTaskId,
		struct klevpnp_GetTaskPropertiesResponse{
			klevpnp_notification_descr_array	notificationDescrArray;
			struct param_error					error;
		}&r);

int klevpnp_SetCommonProperties(
		klevpnp_notification_descr_array	notificationDescrArray,
		struct klevpnp_SetCommonPropertiesResponse{
			struct param_error	error;
		}&r);

int klevpnp_SetGroupProperties(
		xsd__int							nGroupId,
		klevpnp_notification_descr_array	notificationDescrArray,
		struct klevpnp_SetGroupPropertiesResponse{
			struct param_error	error;
		}&r);

int klevpnp_SetTaskProperties(
		xsd__wstring			wstrTaskId,
		klevpnp_notification_descr_array	notificationDescrArray,
		struct klevpnp_SetTaskPropertiesResponse{
			struct param_error	error;
		}&r);

// Local Variables:
// mode: C++
// End:


/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	nag_sapi.h
 * \author	Andrew Kazachkov
 * \date	21.02.2003 16:14:48
 * \brief	
 * 
 */

struct klnag_StoresList
{
	xsd__wstring*   __ptr;
	int				__size;
};

struct klnag_IndexesList
{
	xsd__int*   __ptr;
	int			__size;
};

int klnag_RemoveStores(
                    struct klnag_StoresList stores,
					struct klnag_RemoveStoresResponse{
                        struct klnag_IndexesList   il;
						struct param_error	error;
					}&r);

int klnag_StartNewIntegration(
                    xsd__wstring    hostid,
					struct klnag_StartNewIntegrationResponse{
						struct param_error	error;
					}&r);

int klnag_NotifySSChange(
					xsd__wstring	product,
					xsd__wstring	version,
                    xsd__wstring	section,
                    xsd__int        ss_type,
					struct klnag_NotifySSChangeResponse{
						struct param_error	error;
					}&r);

int klnag_OnPing(
					param__params info,
					struct klnag_OnPingResponse{
                        param__params       response;
						struct param_error	error;
					}&r);

int klnag_WatchdogPing(
				  struct klnag_WatchdogPingResponse{
					struct param_error	error;
				}&r);


int klnag_GetRunTimeInfo(
					param__params filter,
					struct klnag_GetRunTimeInfoResponse{
						param__params info;
						struct param_error	error;
					}&r);

int klnag_GetStaticInfo(
					param__params filter,
					struct klnag_GetStaticInfoResponse{
						param__params info;
						struct param_error	error;
					}&r);


int klnag_GetStatistics(
					param__params filter,
					struct klnag_GetStatisticsResponse{
						param__params info;
						struct param_error	error;
					}&r);

int klnag_GetInstancePort(
					xsd__wstring	product,
					xsd__wstring	version,
					xsd__wstring	component,
					xsd__wstring	instance,
					struct klnag_GetInstancePortResponse{
                        xsd__wstring	    location;
						xsd__int			port;
                        xsd__int			type;
						struct param_error	error;
					}&r);

int klnag_StartProduct(
                        xsd__wstring productname,
                        xsd__wstring productversion,
                        param__params extraparams,
                        struct klnag_StartProductResponse{
                            param__params       results;
                            struct param_error	error;
                        } &r);

int klnag_StopProduct(
                        xsd__wstring productname,
                        xsd__wstring productversion,
                        param__params extraparams,
                        struct klnag_StopProductResponse{
                            param__params       results;
                            struct param_error	error;
                        } &r);

int klnag_OnStartRemoteInstallTask(
						xsd__int		taskId,
                        struct klnag_OnStartRemoteInstallTaskResponse{
                            param__params       results;
                            struct param_error	error;
                        } &r);

int klnag_OnInstallerNotification(
                        param__params info, 
                        struct klnag_OnInstallerNotificationResponse{ 
                            param__params       results; 
                            struct param_error	error; 
                        }& r);

int klnag_GetHostPublicKey(
                        struct klnag_GetHostPublicKeyResponse{ 
                            xsd__base64Binary   key; 
                            xsd__wstring        hash;
                            struct param_error	error; 
                        }& r);

int klnag_PutGlobalKeys(
                        xsd__base64Binary   key1,
                        xsd__base64Binary   key2, 
                        struct klnag_PutGlobalKeysResponse{ 
                            struct param_error	error; 
                        }& r);

int klnag_OnUserForceSyncRequest(
                        param__params info,
                        struct klnag_OnUserForceSyncRequestResponse{ 
                            struct param_error	error; 
                        }& r);

/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file AF/filereceiverbridgesoapapi.h
 * \author Шияфетдинов Дамир
 * \date 2005
 * \brief SOAP-интерфейс FileReceiver'а 
 */


struct xsd__klft_file_chunk_buff
{
	xsd__unsignedByte*	__ptr;
	int					__size;
};

struct klft_file_chunk_t
{
	xsd__wstring		fileName;
	xsd__wstring		fileId;
	
	xsd__unsignedLong	CRC;
	xsd__unsignedLong	startPosInFile;
	xsd__unsignedLong	fullFileSize;

	struct xsd__klft_file_chunk_buff buff;
};

struct klft_file_info_t
{
	xsd__wstring		fileName;
	xsd__boolean		isDir;
	xsd__boolean		readPermission;
	xsd__unsignedLong	createTime;
	xsd__unsignedLong	fullFileSize;
};

int klftbridge_CreateReceiver( xsd__boolean		useUpdaterFolder,
						struct klftbridge_CreateReceiverResponse
						{
							xsd__wstring				receiverId;							
							xsd__long					res;
						}	&r );

int klftbridge_Connect( xsd__wstring			receiverId,
					    xsd__boolean			toMasterServer,
						struct klftbridge_ConnectResponse
						{
							xsd__long					res;
						}	&r );

int klftbridge_GetFileInfo(
						xsd__wstring	receiverId,
						xsd__wstring	fileName,
						struct klftbridge_GetFileInfoResponse
						{
							struct klft_file_info_t	info;
							xsd__long				res;
						}	&r );

int klftbridge_GetNextFileChunk(
						xsd__wstring	receiverId,
						xsd__wstring	fileName,						
						xsd__unsignedLong	startFilePos,
						xsd__unsignedLong	neededSize,						
						struct klftbridge_GetNextFileChunkResponse
						{
							struct klft_file_chunk_t	chunk;
							xsd__long					res;
						}	&r );

int klftbridge_ReleaseReceiver( xsd__wstring			receiverId,
						struct klftbridge_ReleaseReceiverResponse
						{
							xsd__long					res;
						}	&r );



/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file AF/filetransfersoapapi.h
 * \author Шияфетдинов Дамир
 * \date 2004
 * \brief SOAP-интерфейс FileTransfer'а 
 */

class SOAPUpdateAgentInfo {
public:	
	xsd__wstring connectionName;
	xsd__wstring location;
	xsd__wstring sslLocation;	
	xsd__wstring multicastAddr;
};

struct SOAPUpdateAgentInfosList
{
	SOAPUpdateAgentInfo* 	__ptr;
	int						__size;
};


int klft_InitiateFileUpload(
						xsd__wstring	fileName,
						xsd__wstring	fileId,
						xsd__wstring	description,
						xsd__unsignedLong	dirFlag,
						xsd__unsignedLong	fullFileSize,						
						xsd__wstring	serverComponent,
						xsd__wstring	remoteClientName,
						xsd__unsignedLong	uploadDelay,
						xsd__unsignedLong	nextChunkDelay,
						xsd__int			fileType,
						xsd__unsignedLong	forUpdateAgentFlag,
						struct SOAPUpdateAgentInfosList *updatesAgents,
						struct klft_InitiateFileUploadResponse
						{
							struct param_error	error;
						}	&r );

int klft_RegisterUpdateAgent(
						xsd__wstring	strAddress,
						xsd__wstring	strNoSSLAddress,
						xsd__wstring	strMulticastIp,
						param__params	pParOptions,
						struct klft_RegisterUpdateAgentResponse
						{
							struct param_error	error;
						}	&r );

int klft_InitiateServerFileUpload(						
						xsd__wstring	fileId,
						xsd__unsignedLong	dirFlag,
						xsd__unsignedLong	lifeTime,
						xsd__unsignedLong	fullFileSize,
						struct klft_InitiateServerFileUploadResponse
						{							
							struct param_error	error;
						}	&r );

int klft_CancelServerFileUpload(						
						xsd__wstring	fileId,
						struct klft_CancelServerFileUploadResponse
						{							
							struct param_error	error;
						}	&r );

int klft_PutNextFileChunk(						
						xsd__wstring	fileId,						
						struct klft_file_chunk_t	chunk,
						struct klft_PutNextFileChunkResponse
						{							
							struct param_error	error;
						}	&r );

class SOAPFolderFileInfo {
public:	
	xsd__unsignedInt	CRC;
	xsd__string			FileNameHash;
};

struct SOAPFilesInfoList
{
	SOAPFolderFileInfo* 	__ptr;
	int						__size;
};

struct SOAPSyncFolderSubfoldersList
{
	xsd__wstring*			__ptr;
	int						__size;
};

struct SOAPSyncFolderOptions
{
	struct SOAPSyncFolderSubfoldersList subfoldersList;
};

struct SOAPFolderHashList {	
	xsd__string*			__ptr;
	int						__size;
};


int klft_GetSyncFolderDiff(						
						xsd__wstring	fileDescription,
						struct SOAPFilesInfoList *clientFilesList,
						struct SOAPSyncFolderOptions *subfoldersOptions,
						struct klft_GetSyncFolderDiffResponse
						{							
							struct param_error	error;
							xsd__wstring		syncFolderFileId;
							xsd__boolean		folderIsEqual;
							struct SOAPFilesInfoList *diffFilesList;
							struct SOAPFolderHashList *folderHashList;
						}	&r );

int klft_GetSyncFolderFileInfo(
						xsd__wstring	syncFolderFileId,
						xsd__wstring	folderFileName,						
						struct klft_GetSyncFolderFileInfoResponse
						{
							struct klft_file_info_t	info;
							struct param_error	error;
						}	&r );

int klft_GetSyncFolderFileChunk(
						xsd__wstring	syncFolderFileId,
						xsd__wstring	folderFileName,						
						xsd__unsignedLong	startFilePos,
						xsd__unsignedLong	neededSize,
						struct klft_GetSyncFolderFileChunkResponse
						{							
							struct klft_file_chunk_t	chunk;
							struct param_error			error;							
						}	&r );



/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	sync/syncapi.h
 * \author	Andrew Kazachkov
 * \date	29.10.2002 16:03:11
 * \brief	Интерфейсы для синхронизации
 * 
 */


int sync_putData(
				xsd__wstring wstrID,
				param__params data,
				struct sync_putDataResponse{
					struct param_error	error;
				} &r);

int sync_Call(
				xsd__wstring wstrIDTo,
				xsd__wstring wstrIDFrom,
				param__params parInputData,
				struct sync_CallResponse{
					struct param_error	error;
					param__params parOutputData;
				} &r);

int sync_start(
				xsd__wstring wstrType,
				xsd__wstring wstrIDFrom,
				param__params parOptions,
				struct sync_startResponse{
					xsd__wstring		wstrIDTo;
					param__params		parOutputData;
					struct param_error	error;
				} &r);


/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	soapapi.h
 * \author	Andrew Kazachkov
 * \date	29.10.2002 16:52:45
 * \brief	
 * 
 */


struct klhst_domain_info
{
	xsd__wstring	wstrName;
	xsd__int		nType;
};

class klhst_domains_info
{
public:
	struct klhst_domain_info*	__ptr;
	int					        __size;
};

struct klhst_host_info
{
	xsd__wstring	wstrName;
    xsd__wstring	wstrWinName;
	xsd__int		nType;
};

class klhst_hosts_info
{
public:
	struct klhst_host_info*	__ptr;
	int					    __size;
};


class klhst_wstrings
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

struct klhst_field_order
{
	xsd__wstring	wstrFieldName;
	xsd__boolean	bOrder;
};

class klhst_field_order_array
{
public:
	struct klhst_field_order*	__ptr;
	int							__size;
};

int klhst_UseKeepConnection(
					xsd__wstring		host,
					struct klhst_UseKeepConnectionResponse{
						struct param_error	error;
					}&r);

int klhst_AddHosts(
					param__params	info,
					struct klhst_AddHostsResponse{
                        param__params	    info;
						struct param_error	error;
					}&r);


int klhst_GetHostTasks(
					xsd__wstring		host,
					xsd__wstring		product,
					xsd__wstring		version,
					xsd__int			lifetime,
					struct klhst_GetHostTasksResponse{
						struct param_error	error;
					}&r);
					  

int klhst_GetDomains(
					void* _,
					struct klhst_GetDomainsResponse{
						klhst_domains_info  units;
						struct param_error	error;
					}&r);

int klhst_GetDomainHosts(
					xsd__wstring domain,
					struct klhst_GetDomainHostsResponse{
						klhst_hosts_info    units;
						struct param_error	error;
					}&r);

int klhst_GetHostInfo(
					  xsd__wstring		name,
					  klhst_wstrings	fields,
					  struct klhst_GetHostInfoResponse{
						param__params	info;
						struct param_error	error;
					}&r);

int klhst_AddDomain(
					  xsd__wstring		domain,
					  xsd__int			type,
					  struct klhst_AddDomainResponse{
						struct param_error	error;
					}&r);

int klhst_DelDomain(
					  xsd__wstring		domain,
					  struct klhst_DelDomainResponse{
						struct param_error	error;
					}&r);

int klhst_AddHost(
					  param__params		info,
					  struct klhst_AddHostResponse{
                        xsd__wstring		name;
						struct param_error	error;                        
					}&r);

int klhst_DelHost(
					  xsd__wstring		name,
					  struct klhst_DelHostResponse{
						struct param_error	error;
					}&r);


int klhst_UpdateHost(
					  xsd__wstring		name,
					  param__params		info,
					  struct klhst_UpdateHostResponse{
						struct param_error	error;
					}&r);


struct klhst_product
{
	  xsd__wstring		wstrName;
	  xsd__wstring		wstrVersion;
};

class klhst_products
{
public:
	struct klhst_product*	__ptr;
	int						__size;
};

int klhst_GetHostProducts(
					  xsd__wstring		name,
					  struct klhst_GetHostProductsResponse{
						param__params       products;
						struct param_error	error;
					}&r);

int klhst_SS_Read(
				  xsd__wstring	name,
				  xsd__wstring	type,
				  xsd__wstring	product,
				  xsd__wstring	version,
				  xsd__wstring	section,
				  xsd__int		timeout,
				  struct klhst_SS_ReadResponse{
					param__params		data;
					struct param_error	error;
				}&r);

int klhst_SS_Write(
					xsd__wstring	name,
					xsd__wstring	type,
					xsd__wstring	product,
					xsd__wstring	version,
					xsd__wstring	section,
					xsd__int		flags,
					param__params	data,
					xsd__int		timeout,
					struct klhst_SS_WriteResponse{
						struct param_error	error;
					}&r);

int klhst_SS_CreateSection(
					xsd__wstring	name,
					xsd__wstring	type,
					xsd__wstring	product,
					xsd__wstring	version,
					xsd__wstring	section,
					xsd__int		timeout,
					struct klhst_SS_CreateSectionResponse{
						struct param_error	error;
					}&r);

int klhst_SS_DeleteSection(
					xsd__wstring	name,
					xsd__wstring	type,
					xsd__wstring	product,
					xsd__wstring	version,
					xsd__wstring	section,
					xsd__int		timeout,
					struct klhst_SS_DeleteSectionResponse{
						struct param_error	error;
					}&r);

int klhst_SS_GetNames(
					xsd__wstring	name,
					xsd__wstring	type,
					xsd__wstring	product,
					xsd__wstring	version,
					xsd__int		timeout,
					struct klhst_SS_GetNamesResponse{
						klhst_wstrings		names;
						struct param_error	error;
					}&r);


int klhst_CallGetRunTimeInfo(
					xsd__wstring	name,
					param__params filter,
					struct klhst_CallGetRunTimeInfoResponse{
						param__params info;
						struct param_error	error;
					}&r);

int klhst_CallGetStaticInfo(
					xsd__wstring	name,
					param__params filter,
					struct klhst_CallGetStaticInfoResponse{
						param__params info;
						struct param_error	error;
					}&r);


int klhst_CallGetStatistics(
					xsd__wstring	name,
					param__params filter,
					struct klhst_CallGetStatisticsResponse{
						param__params info;
						struct param_error	error;
					}&r);

int klhst_CallGetInstancePort(
					xsd__wstring	name,
					xsd__wstring	product,
					xsd__wstring	version,
					xsd__wstring	component,
					xsd__wstring	instance,
					struct klhst_CallGetInstancePortResponse{
						xsd__int			port;
                        xsd__wstring        location;
						struct param_error	error;
					}&r);

int klhst_NotifyTask(
                        xsd__wstring	name,
                        struct klhst_NotifyTaskResponse{
                            struct param_error	error;
                        }& r);

int klupdt_GetUpdatesInfo(
                    param__params filter,
					struct klupdt_GetUpdatesInfoResponse{
                        param__params       data;
						struct param_error	error;
					}&r);

int klupdt_SetBundleOptions(
                    param__params paramOptions,
					struct klupdt_SetBundleOptionsResponse{
                        param__params       data;
						struct param_error	error;
					}&r);

int klupdt_SetGeneralOptions(
                    param__params paramOptions,
					struct klupdt_SetGeneralOptionsResponse{
                        param__params       data;
						struct param_error	error;
					}&r);

int klupdt_GetGeneralOptions(
                    param__params data,
					struct klupdt_GetGeneralOptionsResponse{
                        param__params       paramOptions;
						struct param_error	error;
					}&r);

int klupdt_GetAvailableUpdatesInfo(
					xsd__wstring wstrLocalization,
					struct klupdt_GetAvailableUpdatesInfoResponse{
                        param__params       parAvailableUpdateComps;
                        param__params       parAvailableUpdateApps;
						struct param_error	error;
					}&r);

int klupdt_GetRequiedUpdatesInfo(
					int nDummy,
					struct klupdt_GetRequiedUpdatesInfoResponse{
                        param__params       parInstalledComps;
                        param__params       parInstalledApps;
						struct param_error	error;
					}&r);

int kllicsrv_GetKeysInfo(
                    klhst_wstrings              fields,
                    xsd__wstring	            filter,
                    xsd__int                    lTimeout,
                    param__params	            options,
                    struct kllicsrv_GetKeysInfoResponse{
                        xsd__wstring        it;
                        struct param_error	error;
                    }&r);

int kllicsrv_GetKeyInfo(
                    klhst_wstrings          fields,
					klhst_field_order_array	fieldsToOrder,
                    xsd__wstring	        serial,
                    xsd__int                lTimeout,
                    struct kllicsrv_GetKeyInfoResponse{
                        xsd__wstring        it;
						long				nCount;
                        struct param_error	error;
                    }&r);

int kllicsrv_DeleteKeyInfo(
                    xsd__wstring	        serial,
                    struct kllicsrv_DeleteKeyInfoResponse{
                        struct param_error	error;
                    }&r);

int kllicsrv_InstallKey(
                    param__params	        keyinfo,
                    struct kllicsrv_InstallKeyResponse{
                        struct param_error	error;
                    }&r);

int kllicsrv_UninstallKey(
                    param__params	        keyinfo,
                    struct kllicsrv_UninstallKeyResponse{
                        struct param_error	error;
                    }&r);

int kllicsrv_GetKeyData(
                    param__params	        indata,
                    struct kllicsrv_GetKeyDataResponse{
                        param__params	    keydata;
                        struct param_error	error;
                    }&r);

int kllicsrv_AdjustFlexibleKey(
                    param__params	        keyinfo,
                    struct kllicsrv_AdjustFlexibleKeyResponse{
                        struct param_error	error;
                    }&r);

int kllicsrv_EnumKeys(
                    klhst_wstrings          fields,
					klhst_field_order_array	fieldsToOrder,
                    param__params	        options,
                    xsd__int                lTimeout,
                    struct kllicsrv_EnumKeysResponse{
                        xsd__wstring        it;
						long				nCount;
                        struct param_error	error;
                    }&r);

int klhst_MoveRulesAddRule(
                    param__params ruleinfo,
                    struct klhst_MoveRulesAddRuleResponse{
                        xsd__int            nRule;
                        struct param_error	error;
                    }&r);

int klhst_MoveRulesUpdateRule(
                    xsd__int            nRule,
                    param__params       ruleinfo,
                    struct klhst_MoveRulesUpdateRuleResponse{
                        struct param_error	error;
                    }&r);

int klhst_MoveRulesDeleteRule(
                    xsd__int    nRule,
                    struct klhst_MoveRulesDeleteRuleResponse{
                        struct param_error	error;
                    }&r);

class klhst_rule_order_array
{
public:
	xsd__int*	__ptr;
	int		    __size;
};

int klhst_MoveRulesSetRulesOrder(
                    klhst_rule_order_array  arr,
                    struct klhst_MoveRulesSetRulesOrderResponse{
                        struct param_error	error;
                    }&r);
       
int klhst_MoveRulesGetRule(
                    xsd__int    nRule,
                    struct klhst_MoveRulesGetRuleResponse{
                        param__params       ruleinfo;
                        struct param_error	error;
                    }&r);
                    
int klhst_MoveRulesGetRules(
                    klhst_wstrings  fields,
                    struct klhst_MoveRulesGetRulesResponse{
                        param__params       rulesinfo;
                        struct param_error	error;
                    }&r);

/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	dpns_soapapi.h
 * \author	Andrew Kazachkov
 * \date	06.09.2005 13:38:07
 * \brief	
 * 
 */


int kldpns_AddDiapason(
                    param__params data,
                    struct kldpns_AddDiapasonResponse{
                        param__params	    InvalidIntervals;
                        xsd__int            id;
                        struct param_error	error;
                    }&r);

int kldpns_UpdateDiapason(
                    xsd__int        id,
                    param__params data,
                    struct kldpns_UpdateDiapasonResponse{
                        param__params	    InvalidIntervals;
                        xsd__boolean        bResult;
                        struct param_error	error;
                    }&r);


int kldpns_RemoveDiapason(
                    xsd__int        id,
                    struct kldpns_RemoveDiapasonResponse{
                        struct param_error	error;
                    }&r);


int kldpns_GetDiapasons(
                    klhst_wstrings	fields,
                    xsd__int        lTimeout,
					struct kldpns_GetDiapasonsResponse{
                        xsd__wstring        it;
						struct param_error	error;
					}&r);

int kldpns_GetDiapason(
                    xsd__int        id,
                    klhst_wstrings	fields,
					struct kldpns_GetDiapasonResponse{
                        param__params	    result;
						struct param_error	error;
					}&r);

int kldpns_NotifyDpnsTask(
					struct kldpns_NotifyDpnsTaskResponse{
						struct param_error	error;
					}&r);


/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	grp_soapapi.h
 * \author	Andrew Kazachkov
 * \date	18.11.2002 13:31:41
 * \brief	
 * 
 */

class klgrp_wstrings
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

struct klgrp_field_order
{
	xsd__wstring	wstrFieldName;
	xsd__boolean	bOrder;
};

class klgrp_field_order_array
{
public:
	struct klgrp_field_order*	__ptr;
	int							__size;
};


int klgrp_GetGroups(
					xsd__int	parent,
					xsd__int	depth, 
					struct klgrp_GetGroupsResponse{
						param__params		groups;
						struct param_error	error;
					}&r);

int klgrp_GetUnassignedGroupId(
					void* _,
					struct klgrp_GetUnassignedGroupIdResponse{
						xsd__int			id;
						struct param_error	error;
					}&r);

int klgrp_GetGroupInfo(
					xsd__int			id,
                    param__params		info,
					struct klgrp_GetGroupInfoResponse{
						param__params		info;
						struct param_error	error;
					}&r);

int klgrp_AddGroup(
					param__params		info,
					struct klgrp_AddGroupResponse{
						long				id;
						struct param_error	error;
					}&r);

int klgrp_DelGroup(
					xsd__int id,
					struct klgrp_DelGroupResponse{
						struct param_error	error;
					}&r);

int klgrp_UpdateGroup(
					xsd__int			id,
					param__params		info,
					struct klgrp_UpdateGroupResponse{
						struct param_error	error;
					}&r);

int klgrp_SetGroupHosts(
					xsd__int			id,
					param__params		hosts,
					struct klgrp_SetGroupHostsResponse{
						struct param_error	error;
					}&r);

int klgrp_RemoveHosts(
    				klgrp_wstrings	hosts,
					struct klgrp_RemoveHostsResponse{
						struct param_error	error;
					}&r);

int klgrp_ResetGroupHostsIterator(
					xsd__int			id,
					klgrp_wstrings		fields,
					xsd__int			lifetime,
					struct klgrp_ResetGroupHostsIteratorResponse{
						xsd__wstring		it;
						struct param_error	error;
					}&r);

int klgrp_ReleaseGroupHostsIterator(
					xsd__wstring		it,
					struct klgrp_ReleaseGroupHostsIteratorResponse{
						struct param_error	error;
					}&r);

int klgrp_GetNextGroupHost(
					xsd__wstring	it,
					xsd__int		nCount,
					struct klgrp_GetNextGroupHostResponse{
						param__params		hosts;
						xsd__int			nCount;
						xsd__boolean		bEOF;
						struct param_error	error;
					}&r);

int klgrp_FindHosts(
                    xsd__wstring	            wstrFilter,
					klgrp_wstrings	            arrFields,
					klgrp_field_order_array     arrFieldsToOrder,
					param__params				params,
					xsd__int		            lTimeout,
                    xsd__wstring	            wstrRequestId,
					struct klgrp_FindHostsResponse{
						xsd__int		    nCount;
						xsd__wstring		wstrIterator;
						struct param_error	error;
					}& r);

int klgrp_FindHostsCancel(
					xsd__wstring	wstrRequestId,
					struct klgrp_FindHostsCancelResponse{
						struct param_error	error;
					}&r );

int klgrp_FindHostsGetAccessor(
					xsd__wstring	wstrRequestId,
					struct klgrp_FindHostsGetAccessorResponse{
						xsd__int		    nCount;
						xsd__wstring		wstrAccessor;
						param__params		parFailedSlaves;
						struct param_error	error;
					}&r );

int klgrp_GetSuperGroupId(
					void* _,
					struct klgrp_GetSuperGroupIdResponse{
						xsd__int			id;
						struct param_error	error;
					}&r);

int klgrp_FindGroups(
                    xsd__wstring	            wstrFilter,
					klgrp_wstrings	            arrFields,
					klgrp_field_order_array     arrFieldsToOrder,
					param__params				params,
					xsd__int		            lTimeout,
                    xsd__wstring	            wstrRequestId,
					struct klgrp_FindGroupsResponse{
						xsd__int		    nCount;
						xsd__wstring		wstrIterator;
						struct param_error	error;
					}& r);

/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	pol/polsoapapi.h
 * \author	Mikhail Karmazine
 * \date	10.04.2003 13:04:41
 * \brief	заголовочный файл с описанием SOAP-методов, относящихся к
 *				модулю POL	
 * 
 */

struct klpol_policy_common_data
{    
    xsd__int        policyId;
    xsd__wstring    wstrDisplayName;
    xsd__wstring    wstrProductName;
    xsd__wstring    wstrProductVersion;
    xsd__int        tCreated;
    xsd__int        tModified;
    xsd__boolean    bInherited;
    xsd__int        nGroupSyncId;
    xsd__boolean    bActive = true;
    xsd__int        dwFlags = -1;
    xsd__boolean    bRoaming = false;
    xsd__boolean    bAcceptParent = true; // policy may be changed by parent policy
    xsd__boolean    bForceDistrib2Children = false; //sets bAcceptParent for all child policies
    xsd__boolean    bForced = false; //readonly property -- bAcceptParent cannot be changed
    xsd__int        nGroup = -1;

};

class klpol_policies
{
public:
	struct klpol_policy_common_data*	__ptr;
	int								    __size;
};

class klpol_polici_ids
{
public:
	xsd__int*   __ptr;
	int			__size;
};


int klpol_GetPolicyCommonData(
    xsd__int nGroupId,
    xsd__int nExpectedServerVersion,
	struct klpol_GetPolicyCommonDataResponse{
		klpol_policies		policies;
		struct param_error	error;
	}&r );


int klpol_GetAllAcivePoliciesForGroup(
    xsd__int nGroupId,
	struct klpol_GetAllAcivePoliciesForGroupResponse{
		klpol_policies		policies;
		struct param_error	error;
	}&r );

int klpol_GetSinglePolicyCommonData(
    xsd__int policyId,
	struct klpol_GetSinglePolicyCommonDataResponse{
		struct  klpol_policy_common_data    policy;
		struct param_error	        error;
	}&r );

int klpol_AddPolicy(
    xsd__int nGroupId,
    struct  klpol_policy_common_data    policy,
	struct klpol_AddPolicyResponse{
		xsd__int            policyId;
        struct klpol_policy_common_data    policy;
		struct param_error	error;
	}&r );

int klpol_UpdatePolicyCommonData(
    struct  klpol_policy_common_data    policy,
	struct klpol_UpdatePolicyCommonDataResponse{
		struct param_error	error;
	}&r );

int klpol_DeletePolicy(
    xsd__int policyId,
	struct klpol_DeletePolicyResponse{
		struct param_error	error;
	}&r );

int klpol_CopyPolicyEx(
    xsd__int        policyId,
    xsd__int        nNewGroupId,
    xsd__boolean    bDeleteOrigin,
	struct klpol_CopyPolicyExResponse{
        xsd__int            policyId;
		struct param_error	error;
	}&r );

int klpol_GetSSProxyID(
    xsd__int policyId,
    xsd__int nTimeout,
	struct klpol_GetSSProxyIDResponse{
        xsd__wstring        storage_descr;
		struct param_error	error;
	}&r );

int klpol_ModifySettings(
        xsd__int policyId,
        struct klpol_ModifySettingsResponse{
            struct param_error	error;
	    }&r );

int klpol_MakePolicyActive(
        xsd__int policyId,
        xsd__boolean bActive,
        struct klpol_MakePolicyActiveResponse{
            xsd__boolean        bResult;
            struct param_error	error;
	    }&r );

int klpol_MakePolicyRoaming(
        xsd__int policyId,
        struct klpol_MakePolicyRoamingResponse{
            xsd__boolean        bResult;
            struct param_error	error;
	    }&r );

int klpol_GetPoliciesForGroup( 
    xsd__int nGroupId,
    xsd__int nExpectedServerVersion,
	struct klpol_GetPoliciesForGroupResponse{
		klpol_policies		policies;
		struct param_error	error;
	}&r );

int klpol_CopyOrMovePolicy(
    xsd__int                            policyId,
    xsd__int                            nNewGroupId,
    xsd__boolean                        bDeleteOrigin,
    struct  klpol_policy_common_data    data,
	struct klpol_CopyOrMovePolicyResponse{
        xsd__int            policyId;
		struct param_error	error;
	}&r );


int klpol_SetOutbreakPolicy(
    klpol_polici_ids    vec_policies,
	struct klpol_SetOutbreakPolicyResponse{
		struct param_error	error;
	}&r );

int klpol_GetOutbreakPolicy(
	struct klpol_GetOutbreakPolicyResponse{
        klpol_polici_ids    vec_policies;
		struct param_error	error;
	}&r );

int klpol_SetOutbreakPolicy2(
    param__params           data,
	struct klpol_SetOutbreakPolicy2Response{
		struct param_error	error;
	}&r );

int klpol_GetOutbreakPolicy2(
	struct klpol_GetOutbreakPolicy2Response{
        param__params       data;
		struct param_error	error;
	}&r );

/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prsspsoapapi.h
 * \author	Mikhail Karmazine
 * \date	15.04.2003 12:23:47
 * \brief	
 * 
 */

class klprssp_wstrings
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

int klprssp_SS_Read(
    xsd__wstring	storage_descr,
    xsd__wstring	name,
    xsd__wstring	version,
    xsd__wstring	section,
    struct klprssp_SS_ReadResponse{
        param__params		data;
        struct param_error	error;
    }&r );


// Update, Add, Replace, Clear and Delete methods of StorageSettings interface
// have the same parameters. So, join them into one soap method and distinguish them 
// by operation_type parameter:
int klprssp_SS_Write(
					xsd__wstring	storage_descr,
					xsd__wstring	name,
					xsd__wstring	version,
					xsd__wstring	section,
					param__params	data,
					xsd__int		operation_type,  
					struct klprssp_SS_WriteResponse{
						struct param_error	error;
					}&r);

// CreateSection and DeleteSection methods of StorageSettings interface
// have the same parameters. Again, join them into one soap method and distinguish them 
// by operation_type parameter:
int klprssp_SS_SectionOperation(
					xsd__wstring	storage_descr,
					xsd__wstring	name,
					xsd__wstring	version,
					xsd__wstring	section,
					xsd__int		operation_type,
					struct klprssp_SS_SectionOperationResponse{
						struct param_error	error;
					}&r);

int klprssp_SS_GetNames(
					xsd__wstring	storage_descr,
					xsd__wstring	name,
					xsd__wstring	version,
					struct klprssp_SS_GetNamesResponse{
						klprssp_wstrings		names;
						struct param_error	error;
					}&r);

int klprssp_SS_ApplyChanges(
					xsd__wstring	storage_descr,
					struct klprssp_SS_ApplyChangesResponse{
						struct param_error	error;
					}&r);

int klprssp_ReleaseSS(
                    xsd__wstring    storage_descr,
                    struct klprssp_ReleaseSSResponse{
                        struct param_error	error;
                    }&r );
/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	adhst_gsoap.h
 * \author	Andrew Kazachkov
 * \date	14.10.2004 15:23:01
 * \brief	
 * 
 */


int adhst_GetChildComputers(
                    xsd__int                    idOU, 
                    klhst_wstrings              fields,
                    xsd__int                    lTimeout,
                    struct adhst_GetChildComputersResponse{
                        xsd__wstring        it;
                        struct param_error	error;
                    }&r);

int adhst_GetChildOUs(
                    xsd__int                    idOU, 
                    klhst_wstrings              fields,
                    xsd__int                    lTimeout,
                    struct adhst_GetChildOUsResponse{
                        xsd__wstring        it;
                        struct param_error	error;
                    }&r);


int adhst_GetOU(
                    xsd__int                    idOU, 
                    klhst_wstrings              fields,
                    struct adhst_GetOUResponse{
                        param__params       data;
                        struct param_error	error;
                    }&r);

int adhst_UpdateOU(
                    xsd__int                    idOU, 
                    param__params		        parData,
                    struct adhst_UpdateOUResponse{
                        struct param_error	error;
                    }&r);

/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	gsyn/gsyn_api.h
 * \author	Andrew Kazachkov
 * \date	03.12.2002 10:42:53
 * \brief	
 * 
 */


class klgsyn_wstrings
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

class klgsyn_hostid
{
public:
	xsd__wstring	hostname;
	xsd__wstring	domain;
};

class klgsyn_hostids
{
public:
	klgsyn_hostid*	__ptr;
	int				__size;
};

struct klgsyn_field_order
{
	xsd__wstring	wstrFieldName;
	xsd__boolean	bOrder;
};

class klgsyn_field_order_array
{
public:
	struct klgsyn_field_order*	__ptr;
	int							__size;
};

int klgsyn_GetNextItems(
						xsd__wstring wstrIterator,
						xsd__int	nCount,
						struct klgsyn_GetNextItemsResponse{
							xsd__boolean	bEOF;
							xsd__int		nCount;
							param__params	parInfo;
							struct param_error	error;
						}& r);

int klgsyn_ReleaseIterator(
						xsd__wstring wstrIterator,
						struct klgsyn_ReleaseIteratorResponse{
							struct param_error	error;
						}& r);

int klgsyn_AddSyncObjG(
					xsd__wstring	szwName,
					xsd__wstring	wstrSyncObjPath,
					xsd__int		nDirection,
					xsd__int		lGroup,
					xsd__int		lElapse,
					xsd__boolean	bCutoffChildren,
					struct klgsyn_AddSyncObjGResponse{
						xsd__int			id;
						struct param_error	error;
					}& r) = 0;

int klgsyn_AddSyncObjH(
					xsd__wstring		szwName,
					xsd__wstring		wstrSyncObjPath,
					xsd__int			nDirection,
					klgsyn_hostids		arrHosts,
					xsd__int			lElapse,					
					struct klgsyn_AddSyncObjHResponse{
						xsd__int			id;
						struct param_error	error;
					}& r) = 0;

int klgsyn_GetSyncInfo(
					long				lSync,
					klgsyn_wstrings		arrFields,
					struct klgsyn_GetSyncInfoResponse{						
						param__params		parInfo;
						struct param_error	error;
					}& r);


int klgsyn_RemoveSyncObj(
					xsd__int	lSync,
					struct klgsyn_RemoveSyncObjResponse{
						struct param_error	error;
					}& r);

int klgsyn_GetSyncHostsInfo(
					xsd__int			lSync, 
					klgsyn_wstrings		arrFields,
					xsd__int			lTimeout,
					struct klgsyn_GetSyncHostsInfoResponse{
						xsd__wstring		wstrIterator;
						struct param_error	error;
					}& r);

int klgsyn_FindHosts(
					xsd__int			lSync, 
					xsd__wstring		wstrFilter,
					klgsyn_wstrings		arrFields,
					xsd__int			lTimeout,
					struct klgsyn_FindHostsResponse{
						xsd__wstring		wstrIterator;
						struct param_error	error;
					}& r);

/*
virtual bool LockSyncObj ( 
    const std::wstring&	syncObjPath,
    long				waitTimeout = 2000,
    long				lockTimeout = 2000) = 0;
*/

/*
        virtual void UnLockSyncObj( 
            const std::wstring&	syncObjPath ) = 0;
*/

/*
        virtual long GetVersion(
            const std::wstring & syncObjPath )  = 0;

*/

int klgsyn_GetSyncHostsInfo2(
					xsd__int			lSync, 
					klgsyn_wstrings		arrFields,
					klgsyn_field_order_array arrFieldsToOrder,
					xsd__int			lTimeout,
					struct klgsyn_GetSyncHostsInfo2Response{
						xsd__int		nCount;
						xsd__wstring		wstrIterator;
						struct param_error	error;
					}& r);

int klgsyn_FindHosts2(
					xsd__int			lSync, 
					xsd__wstring		wstrFilter,
					klgsyn_wstrings		arrFields,
					klgsyn_field_order_array arrFieldsToOrder,
					xsd__int			lTimeout,
					struct klgsyn_FindHosts2Response{
						xsd__int		nCount;
						xsd__wstring		wstrIterator;
						struct param_error	error;
					}& r);

int klgsyn_GetItemsCount(
						xsd__wstring wstrAccessor,
						struct klgsyn_GetItemsCountResponse{
							xsd__int		nCount;
							struct param_error	error;
						}& r);

int klgsyn_GetItemsChunk(
						xsd__wstring wstrAccessor,
						xsd__int	nStart,
						xsd__int	nCount,
						struct klgsyn_GetItemsChunkResponse{
							xsd__int		nCount;
							param__params	parInfo;
							struct param_error	error;
						}& r);

int klgsyn_ReleaseChunkAccessor(
						xsd__wstring wstrAccessor,
						struct klgsyn_ReleaseChunkAccessorResponse{
							struct param_error	error;
						}& r);

/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pkgsoapapi.h
 * \author	Mikhail Karmazine
 * \date	Dec 2002
 * \brief	заголовочный файл с описанием SOAP-методов, относящихся к
 *				модулю TSK
 */

class kltsk_wstring_array
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

class kltsk_int_array
{
public:
	xsd__int*	__ptr;
	int			__size;
};

struct kltsk_component_id
{
    xsd__wstring    productName;
    xsd__wstring    version;
    xsd__wstring    componentName;
    xsd__wstring    instanceId;
};

struct kltsk_field_order
{
	xsd__wstring	wstrFieldName;
	xsd__boolean	bOrder;
};

class kltsk_field_order_array
{
public:
	struct kltsk_field_order*	__ptr;
	int							__size;
};


int kltsk_ForbidTaskForGroup(
    xsd__int            nGroupId,
    xsd__int            nTaskId,
    struct kltsk_ForbidTaskForGroupResponse{
        struct param_error	error;
    }&r );

int kltsk_RunTaskNow(
    xsd__int nTaskId,
    xsd__wstring wstrDomainName,
    xsd__wstring wstrHostName,
    xsd__boolean bEvenIfAlreadySucceeded,
    struct kltsk_RunTaskNowResponse{
        struct param_error	error;
    }&r );

int kltsk_SuspendTask(
    xsd__int nTaskId,
    struct kltsk_SuspendTaskResponse{
        struct param_error	error;
    }&r );

int kltsk_ResumeTask(
    xsd__int nTaskId,
    struct kltsk_ResumeTaskResponse{
        struct param_error	error;
    }&r );

int kltsk_CancelTask(
    xsd__int nTaskId,
    struct kltsk_CancelTaskResponse{
        struct param_error	error;
    }&r );

int kltsk_ResetTasksIterator(
    xsd__int            nGroupId,
    xsd__boolean        bGroupIdSignificant,
    struct kltsk_component_id  compIdFilter,
    xsd__wstring        taskName,
    xsd__boolean        bIncludeSupergroups,
    struct kltsk_ResetTasksIteratorResponse{
        xsd__wstring		iteratorId;
        struct param_error	error;
    }&r );
    
int kltsk_GetNextTask(
    xsd__wstring	iteratorId,
    struct kltsk_GetNextTaskResponse{
        param__params   parTaskData;
        xsd__boolean    bStartOnEvent;
        xsd__boolean    bResult;
        struct param_error	error;
    }&r );

int kltsk_GetTaskByID(
    xsd__int    nTaskId,
    struct kltsk_GetTaskByIDResponse{
        param__params   parTaskData;
        xsd__boolean    bStartOnEvent;
        struct param_error	error;
    }&r );

int kltsk_GetTaskStatistics(
    xsd__int    nTaskId,
    struct kltsk_GetTaskStatisticsResponse{
        param__params   parStatistics;
        struct param_error	error;
    }&r );

int kltsk_AddTask(
    xsd__int            nGroupId,
    xsd__boolean        bGroupIdSignificant,
    param__params       parTaskData,
    struct kltsk_AddTaskResponse{
        xsd__int            nTaskId;
        struct param_error	error;
    }&r );

int kltsk_UpdateTask(
    xsd__int            nTaskId,
    param__params       parTaskData,
    struct kltsk_UpdateTaskResponse{
        struct param_error	error;
    }&r );

int kltsk_DeleteTask(
    xsd__int            nTaskId,
    struct kltsk_DeleteTaskResponse{
        struct param_error	error;
    }&r );

int kltsk_SetTaskStartEvent(
    xsd__int            nTaskId,
    struct kltsk_component_id  compIdFilter,
    xsd__wstring        eventType,
    param__params       bodyFilter,
    struct kltsk_SetTaskStartEventResponse{
        struct param_error	error; 
    }&r );

int kltsk_GetTaskStartEvent(
    xsd__int            nTaskId,
    struct kltsk_GetTaskStartEventResponse{
        struct kltsk_component_id  compIdFilter;
        xsd__wstring        eventType;
        param__params       bodyFilter;
        struct param_error	error; 
    }&r );

int kltsk_ResetHostIteratorForTaskStatus(
    xsd__wstring            wstrTaskId,
    xsd__wstring            wstrDomainName,
    xsd__wstring            wstrHostName,
    kltsk_wstring_array     arrFields,
    xsd__int                hsHostStateMask,
    xsd__int                nLifetime,
    struct kltsk_ResetHostIteratorForTaskStatusResponse{
        xsd__wstring		iteratorId;
        struct param_error	error; 
    }&r );

int kltsk_GetNextHostStatus(
	xsd__wstring	iteratorId,
	xsd__int		nCount,
    struct kltsk_GetNextHostStatusResponse{
		xsd__int			nCount;
		param__params		paramHostStatus;
		xsd__boolean		bResult;
        struct param_error	error; 
    }&r );

int kltsk_ResetHostIteratorForTaskStatusEx(
    xsd__wstring            wstrTaskId,
    xsd__wstring            wstrDomainName,
    xsd__wstring            wstrHostName,
    kltsk_wstring_array     arrFields,
	kltsk_field_order_array	arrFiledsToOrder,
    xsd__int                hsHostStateMask,
    xsd__int                nLifetime,
    struct kltsk_ResetHostIteratorForTaskStatusExResponse{
        xsd__wstring		iteratorId;
        struct param_error	error; 
    }&r );

int kltsk_GetHostStatusRecordsCount(
	xsd__wstring	iteratorId,
    struct kltsk_GetHostStatusRecordsCountResponse{
		xsd__int			nCount;
        struct param_error	error; 
    }&r );

int kltsk_GetHostStatusRecordRange(
	xsd__wstring	iteratorId,
	xsd__int		nStart,
	xsd__int		nEnd,
    struct kltsk_GetHostStatusRecordRangeResponse{
		param__params		paramHostStatus;
		xsd__int			nCount;
        struct param_error	error; 
    }&r );

int kltsk_GetAllTasksOfHost(
	xsd__wstring	wstrDomainName,
    xsd__wstring	wstrHostName,
    struct kltsk_GetAllTasksOfHostResponse{
		kltsk_int_array     vectTaskIds;
        struct param_error	error; 
    }&r );

int kltsk_GetTaskGroup(
    xsd__int    nTaskId,
    struct kltsk_GetTaskGroupResponse{
	    xsd__int    nGroupId;
        struct param_error	error; 
    }&r );

/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pkgsoapapi.h
 * \author	Mikhail Karmazine
 * \date	17:18 15.11.2002
 * \brief	заголовочный файл с описанием SOAP-методов сервера, относящихся к
 *				модулю PKG
 * 
 */


/*
klpkg_package is a representation of package_info_t:

typedef struct 
{
	std::wstring wstrName;
	std::wstring wstrProductName;
	std::wstring wstrProductVersion;
	std::wstring wstrPackagePath;
	AVP_longlong llSize;
	time_t tCreationTime;
	time_t tModificationTime;
} package_info_t;
*/

struct klpkg_package_info
{
	xsd__int		nPackageId;
	xsd__wstring	wstrName;
	xsd__wstring	wstrProductName;
	xsd__wstring	wstrProductVersion;
	xsd__wstring	wstrPackagePath;
	xsd__long		llSize;
	xsd__int		tCreationTime;
	xsd__int		tModificationTime;
	xsd__wstring	wstrSettingsStorageDescr;
	xsd__wstring	wstrProductDisplName;
	xsd__wstring	wstrProductDisplVersion;
};

// representation of vector<package_info_t>
class klpkg_package_info_array
{
public:
	struct klpkg_package_info*	__ptr;
	int							__size;
};


// void GetPackages( std::vector<package_info_t>& packages );
int klpkg_GetPackages( 
					void* _,
					struct klpkg_GetPackagesResponse{
						klpkg_package_info_array	vect;
						struct param_error			error;
					}&r);



// void RenamePackage( const std::wstring& wstrOldPackageName,
//			const std::wstring& wstrNewPackageName );

int klpkg_RenamePackage(
					  xsd__int		    nPackageId,
					  xsd__wstring		wstrNewPackageName,
					  struct klpkg_RenamePackageResponse{
						struct param_error	error;
					}&r);


/* void GetIntranetFolderForNewPackage(
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			std::wstring& wstrFolder ); */
int klpkg_GetIntranetFolderForNewPackage(
					xsd__wstring		wstrProductName,
					xsd__wstring		wstrProductVersion,
					struct klpkg_GetIntranetFolderForNewPackageResponse{
						xsd__wstring		wstrFolder;
						struct param_error	error;
					}&r);


/* void RecordNewPackage(
			const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFolder,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			package_info_t& packageInfoNew ); */
int klpkg_RecordNewPackage(
                    xsd__wstring	    wstrName,
					xsd__wstring		wstrFolder,
					xsd__wstring		wstrProductName,
					xsd__wstring		wstrProductVersion,
					xsd__wstring		wstrProductDisplName,
					xsd__wstring		wstrProductDisplVersion,
					struct klpkg_RecordNewPackageResponse{
						struct klpkg_package_info	package_info;
						struct param_error	error;
					}&r);


// void RemovePackage( const std::wstring& wstrPackageName );
int klpkg_RemovePackage(
					xsd__int		    nPackageId,
					struct klpkg_RemovePackageResponse{
					struct param_error	error;
					}&r);

//	std::wstring GetIntranetFolderForPackage(int nPackageId);
int klpkg_GetIntranetFolderForPackage(
					xsd__int		    nPackageId,
					struct klpkg_GetIntranetFolderForPackageResponse{
						xsd__wstring		wstrFolder;
						struct param_error	error;
					}&r);

class klpkg_wstrings
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};


int klpkg_SS_Read(
				  xsd__wstring	storage_descr,
				  xsd__wstring	name,
				  xsd__wstring	version,
				  xsd__wstring	section,
				  xsd__int		timeout,
				  struct klpkg_SS_ReadResponse{
					param__params		data;
					struct param_error	error;
				}&r);


// Update, Add, Replace, Clear and Delete methods of StorageSettings interface
// have the same parameters. So, join them into one soap method and distinguish them 
// by operation_type parameter:
int klpkg_SS_Write(
					xsd__wstring	storage_descr,
					xsd__wstring	name,
					xsd__wstring	version,
					xsd__wstring	section,
					param__params	data,
					xsd__int		timeout,
					xsd__int		operation_type,  
					struct klpkg_SS_WriteResponse{
						struct param_error	error;
					}&r);

// CreateSection and DeleteSection methods of StorageSettings interface
// have the same parameters. Again, join them into one soap method and distinguish them 
// by operation_type parameter:
int klpkg_SS_SectionOpearation(
					xsd__wstring	storage_descr,
					xsd__wstring	name,
					xsd__wstring	version,
					xsd__wstring	section,
					xsd__int		timeout,
					xsd__int		operation_type,
					struct klpkg_SS_SectionOpearationResponse{
						struct param_error	error;
					}&r);

int klpkg_SS_GetNames(
					xsd__wstring	storage_descr,
					xsd__wstring	name,
					xsd__wstring	version,
					xsd__int		timeout,
					struct klpkg_SS_GetNamesResponse{
						klpkg_wstrings		names;
						struct param_error	error;
					}&r);


/*
klpkg_tsk_info is a representation of task_info_t:

typedef struct 
{
	long lTaskId;
	std::wstring wstrTaskName;
	long lGroupId;
	std::wstring wstrGroupName;
} task_info_t;
*/

struct klpkg_tsk_info
{
	xsd__int		nTaskId;
	xsd__wstring	wstrTaskDisplName;
	xsd__int		nGroupId;
	xsd__wstring	wstrGroupDisplName;
};

// representation of vector<task_info_t>
class klpkg_tsk_info_array
{
public:
	struct klpkg_tsk_info*		__ptr;
	int							__size;
};


// void RemovePackage( const std::wstring& wstrPackageName );
int klpkg_RemovePackage2(
					xsd__int		    nPackageId,
					struct klpkg_RemovePackage2Response{
						xsd__boolean	bResult;
						klpkg_tsk_info_array tasks;
						struct param_error	error;
					}&r);

// std::wstring GetLoginScript(long nPackageId, const std::wstring& wstrTaskId);
int klpkg_GetLoginScript(
					xsd__int		    nPackageId,
					xsd__wstring		wstrTaskId,
					struct klpkg_GetLoginScriptResponse{
						xsd__wstring		wstrScript;
						struct param_error	error;
					}&r);

//	std::wstring SetRebootOptions();
int klpkg_SetRebootOptions(
					xsd__int		    nPackageId,
					xsd__boolean	    bRebootImmediately,
					xsd__boolean	    bAskForReboot,
					xsd__int		    nAskForRebootPeriodInMin,
					xsd__int		    nForceRebootTimeInMin,
					xsd__wstring		wstrAskRebootMsgText,
					struct klpkg_SetRebootOptionsResponse{
						struct param_error	error;
					}&r);

//	std::wstring GetRebootOptions();
int klpkg_GetRebootOptions(
					xsd__int		    nPackageId,
					struct klpkg_GetRebootOptionsResponse{
						xsd__boolean	    bRebootImmediately;
						xsd__boolean	    bAskForReboot;
						xsd__int		    nAskForRebootPeriodInMin;
						xsd__int		    nForceRebootTimeInMin;
						xsd__wstring		wstrAskRebootMsgText;
						struct param_error	error;
					}&r);

/* void RecordNewPackage2(
			const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			package_info_t& packageInfoNew ); */
int klpkg_RecordNewPackage2(
                    xsd__wstring	    wstrName,
					xsd__wstring		wstrFileId,
					xsd__wstring		wstrProductName,
					xsd__wstring		wstrProductVersion,
					xsd__wstring		wstrProductDisplName,
					xsd__wstring		wstrProductDisplVersion,
					struct klpkg_RecordNewPackage2Response{
						struct klpkg_package_info	package_info;
						struct param_error	error;
					}&r);

/* void GetLicenseKey(
	long nPackageId,
	std::wstring& wstrKeyFileName,
	KLSTD::MemoryChunk** ppMemoryChunk); */
int klpkg_GetLicenseKey(
                    xsd__int			nPackageId,
					struct klpkg_GetLicenseKeyResponse{
						xsd__wstring		wstrKeyFileName;
						xsd__hexBinary		data;
						struct param_error	error;
					}&r);

/* void SetLicenseKey(
	long nPackageId,
	const std::wstring& wstrKeyFileName,
	KLSTD::MemoryChunk* pMemoryChunk,
	bool bRemoveExisting); */
int klpkg_SetLicenseKey(
                    xsd__int			nPackageId,
					xsd__wstring		wstrKeyFileName,
					xsd__hexBinary		data,
					xsd__boolean		bRemoveExisting,
					struct klpkg_SetLicenseKeyResponse{
						struct param_error	error;
					}&r);

/* void ReadPkgCfgFile(
			long nPackageId,
			wchar_t* wszFileRelativePath,
			KLPAR::BinaryValue** ppData); */
int klpkg_ReadPkgCfgFile(
					xsd__int		    nPackageId,
					xsd__wstring		wstrFileRelativePath,
					struct klpkg_ReadPkgCfgFileResponse{
						xsd__hexBinary		data;
						struct param_error	error;
					}&r);

/* void WritePkgCfgFile(
			long nPackageId,
			wchar_t* wszFileRelativePath,
			KLPAR::BinaryValue* pData); */
int klpkg_WritePkgCfgFile(
					xsd__int		    nPackageId,
					xsd__wstring		wstrFileRelativePath,
					xsd__hexBinary		data,
					struct klpkg_WritePkgCfgFileResponse{
						struct param_error	error;
					}&r);

/* bool PackagesProxy::ReadKpdFile(
	long nPackageId,
	KLSTD::MemoryChunk** ppChunk) */
int klpkg_ReadKpdFile(
					xsd__int		    nPackageId,
					struct klpkg_ReadKpdFileResponse{
						xsd__hexBinary		data;
						struct param_error	error;
					}&r);

/* void GetKpdProfileString(
			long nPackageId,
			const std::wstring& wstrSection,
			const std::wstring& wstrKey,
			const std::wstring& wstrDefault,
			std::wstring& wstrValue); */
int klpkg_GetKpdProfileString(
					xsd__int		    nPackageId,
					xsd__wstring		wstrSection,
					xsd__wstring		wstrKey,
					xsd__wstring		wstrDefault,
					struct klpkg_GetKpdProfileStringResponse{
						xsd__wstring		wstrValue;
						struct param_error	error;
					}&r);

/* void WriteKpdProfileString(
			long nPackageId,
			const std::wstring& wstrSection,
			const std::wstring& wstrKey,
			const std::wstring& wstrValue); */
int klpkg_WriteKpdProfileString(
					xsd__int		    nPackageId,
					xsd__wstring		wstrSection,
					xsd__wstring		wstrKey,
					xsd__wstring		wstrValue,
					struct klpkg_WriteKpdProfileStringResponse{
						struct param_error	error;
					}&r);

//	std::wstring SetRebootOptionsEx();
int klpkg_SetRebootOptionsEx(
					xsd__int		    nPackageId,
					xsd__boolean	    bRebootImmediately,
					xsd__boolean	    bAskForReboot,
					xsd__int		    nAskForRebootPeriodInMin,
					xsd__int		    nForceRebootTimeInMin,
					xsd__wstring		wstrAskRebootMsgText,
					xsd__boolean		    bForceAppsClosed,
					param__params	    parParams,
					struct klpkg_SetRebootOptionsExResponse{
						struct param_error	error;
					}&r);

//	std::wstring GetRebootOptionsEx();
int klpkg_GetRebootOptionsEx(
					xsd__int		    nPackageId,
					struct klpkg_GetRebootOptionsExResponse{
						xsd__boolean	    bRebootImmediately;
						xsd__boolean	    bAskForReboot;
						xsd__int		    nAskForRebootPeriodInMin;
						xsd__int		    nForceRebootTimeInMin;
						xsd__wstring		wstrAskRebootMsgText;
						xsd__boolean	    bForceAppsClosed;
						param__params	    parParams;
						struct param_error	error;
					}&r);

/* void RecordNewPackageAsync(
			const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFolder,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			const std::wstring& wstrRequestId); */
int klpkg_RecordNewPackageAsync(
                    xsd__wstring	    wstrName,
					xsd__wstring		wstrFolder,
					xsd__wstring		wstrProductName,
					xsd__wstring		wstrProductVersion,
					xsd__wstring		wstrProductDisplName,
					xsd__wstring		wstrProductDisplVersion,
					xsd__wstring		wstrRequestId,
					struct klpkg_RecordNewPackageAsyncResponse{
						struct param_error	error;
					}&r);

/*	void GetPackageInfo(
			long nPackageId,
			package_info_t& packageInfoNew); */
int klpkg_GetPackageInfo(
                    xsd__int		    nPackageId,
					struct klpkg_GetPackageInfoResponse{
						struct klpkg_package_info	package_info;
						struct param_error	error;
					}&r);

/*	bool GetIncompatibleAppsInfo(
			long nPackageId,
			KLPAR::Params** ppData,
			bool& bCanRemoveOneself); */
int klpkg_GetIncompatibleAppsInfo(
                    xsd__int			nPackageId,
					struct klpkg_GetIncompatibleAppsInfoResponse{
						param__params		data;
						xsd__boolean		bCanRemoveByInstaller;
						xsd__boolean		bRemoveByInstaller;
						xsd__boolean		bResult;
						struct param_error	error;
					}&r);

/*	bool SetRemoveIncompatibleApps(
			long nPackageId,
			bool bRemoveIncompatibleApps); */
int klpkg_SetRemoveIncompatibleApps(
                    xsd__int		    nPackageId,
					xsd__boolean		bRemoveIncompatibleApps,
					struct klpkg_SetRemoveIncompatibleAppsResponse{
						xsd__boolean		bResult;
						struct param_error	error;
					}&r);



/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	rpt/repsoapapi.h
 * \author	Mikhail Karmazine
 * \date	10.06.2003 16:14:44
 * \brief	заголовочный файл с описанием SOAP-методов, относящихся к
 *				модулю KLRPT	
 * 
 */

struct klrpt_type_common
{
    xsd__int        reportType;
    xsd__wstring    wstrDisplayName;
};

struct klrpt_type_common_vector
{
	struct klrpt_type_common*	__ptr;
	int							__size;
};

struct klrpt_string_vector
{
	xsd__wstring*	__ptr;
	int			    __size;
};

struct klrpt_type_details
{
	xsd__wstring            wstrDescr;
    xsd__boolean            bAcceptsTimeInterval;
    xsd__boolean            bAcceptsComputerList;
    struct klrpt_string_vector     vectPossibleDetFields;
    struct klrpt_string_vector     vectPossibleSumFields;
    struct klrpt_string_vector     vectPossibleTotals;
};

struct klrpt_common
{
    xsd__int        reportId;
    xsd__wstring    wstrDisplayName;
    xsd__int        reportType;
    xsd__int        tCreated;
    xsd__int        tModified;
    xsd__int        nGroupId;
	param__params	paramExtraData;
};

struct klrpt_common_vector
{
	struct klrpt_common*	__ptr;
	int				        __size;
};

struct klrpt_id_vector
{
	xsd__int*   __ptr;
	int			__size;
};

struct klrpt_field_order
{
    xsd__wstring    wstrFieldName;
    xsd__boolean    bOrder;
};

struct klrpt_fld_order_vector
{
	struct klrpt_field_order*	__ptr;
	int				            __size;
};

struct klrpt_host
{
    xsd__wstring    wstrDomainName;
    xsd__wstring    wstrHostInternalName;
    xsd__wstring    wstrHostDisplayName;
};

struct klrpt_host_vector
{
	struct klrpt_host*	__ptr;
	int				    __size;
};

struct klrpt_details
{
    xsd__int                tStart;
    xsd__int                tEnd;
    struct klrpt_string_vector     vectDetFields;
    struct klrpt_fld_order_vector  vectDetOrder;
    struct klrpt_string_vector     vectSumFields;
    struct klrpt_fld_order_vector  vectSumOrder;
    struct klrpt_string_vector     vectTotals;
    struct klrpt_host_vector       vectHosts;
    xsd__int                nMaxDetailsRecords;
    xsd__int                nPictureType;
};

int klrpt_GetReportTypes(
    void* _,
	struct klrpt_GetReportTypesResponse{
		struct klrpt_type_common_vector		vectReportTypes;
		struct param_error	                error;
	}&r );

int klrpt_GetReportTypeDetailedData(
    xsd__int        reportType,
	struct klrpt_GetReportTypeDetailedDataResponse{
        struct klrpt_type_details   details;
		param__params				paramExtraData;
		struct param_error	        error;
	}&r );

int klrpt_GetReportIds(
    void* _,
	struct klrpt_GetReportIdsResponse{
		struct klrpt_id_vector  reportIdVector;
		struct param_error	    error;
	}&r );

int klrpt_GetReports(
    void* _,
	struct klrpt_GetReportsResponse{
		struct klrpt_common_vector  reportCommonVector;
		struct param_error	        error;
	}&r );

int klrpt_GetReportCommonData(
    xsd__int  reportId,
	struct klrpt_GetReportCommonDataResponse{
		struct klrpt_common  commonData;
		struct param_error	 error;
	}&r );

int klrpt_GetReportDetailedData(
    xsd__int        reportId,
	struct klrpt_GetReportDetailedDataResponse{
        struct klrpt_common     commonData;
        struct klrpt_details    details;
		struct param_error	    error;
	}&r );

int klrpt_AddReport(
    struct klrpt_common    commonData,
    struct klrpt_details   details,
    struct klrpt_AddReportResponse{
        struct klrpt_common     commonDataOutput;
		struct param_error	    error;
	}&r );

int klrpt_UpdateReport(
    struct klrpt_common    commonData,
    struct klrpt_details   details,
    struct klrpt_UpdateReportResponse{
        struct klrpt_common     commonDataOutput;
		struct param_error	    error;
	}&r );

int klrpt_DeleteReport(
    xsd__int                reportId,
    struct klrpt_DeleteReportResponse{
		struct param_error	    error;
	}&r );

struct xsd__klrpt_output
{
public:
	xsd__unsignedByte*	__ptr;
	int					__size;
};

struct klrpt_input
{
    xsd__int                    tStart;
    xsd__int                    tEnd;
    struct klrpt_host_vector    vectHosts;
	param__params				paramLocData;
};

int klrpt_ExecuteReport(
    xsd__int                reportId,
    struct klrpt_input      input,
    struct klrpt_ExecuteReportResponse{
        struct xsd__klrpt_output output;
	    param__params paramChartData;
		struct param_error	error;
	}&r );

int klrpt_GetConstantOutputForReportType(
    xsd__int                reportType,
    xsd__int                xmlTargetType,
    struct klrpt_GetConstantOutputForReportTypeResponse{
        struct xsd__klrpt_output output;
		struct param_error	error;
	}&r );

int klrpt_CreateDefaultReportData(
    xsd__int                reportType,
    struct klrpt_CreateDefaultReportDataResponse{
        struct klrpt_details   details;
		struct param_error	    error;
	}&r );

int klrpt_ExecuteReportAsync(
    xsd__int                reportId,
    xsd__wstring			wstrAsyncRequestId,
    struct klrpt_input      input,
    struct klrpt_ExecuteReportAsyncResponse{
		struct param_error	error;
	}&r );


int klrpt_ExecuteReportAsyncCancel(
    xsd__wstring			wstrAsyncRequestId,
    struct klrpt_ExecuteReportAsyncCancelResponse{
		struct param_error	error;
	}&r );

int klrpt_ExecuteReportAsyncGetData(
    xsd__wstring			wstrAsyncRequestId,
    xsd__int                nMaxDataChunkSize,
    struct klrpt_ExecuteReportAsyncGetDataResponse{
        struct xsd__klrpt_output output;
	    xsd__int nDataSizeRest;
	    param__params paramChartData;
		struct param_error	error;
	}&r );

int klrpt_ExecuteSlaveReportAsync(
    struct klrpt_common		commonData,
    struct klrpt_details	details,
	param__params			paramLocalData,
    xsd__wstring			wstrAsyncRequestId,
	xsd__int				nTimeoutInSec,
    struct klrpt_ExecuteSlaveReportAsyncResponse{
		struct param_error	error;
	}&r );

int klrpt_ExecuteSlaveReportAsyncCancel(
    xsd__wstring			wstrAsyncRequestId,
    struct klrpt_ExecuteSlaveReportAsyncCancelResponse{
		struct param_error	error;
	}&r );

int klrpt_ExecuteSlaveReportAsyncGetData(
    xsd__wstring			wstrAsyncRequestId,
	xsd__int				nRecordsCount,
    struct klrpt_ExecuteSlaveReportAsyncGetDataResponse{
	    param__params		paramData;
		xsd__int			nMoreDataAvailable;
		struct param_error	error;
	}&r );


int klrpt_RequestStatisticsData(
	param__params	paramRequestParams,
    xsd__wstring	wstrRequestId,
    struct klrpt_RequestStatisticsDataResponse{
		struct param_error	error;
	}&r );

int klrpt_CancelStatisticsRequest(
    xsd__wstring	wstrRequestId,
    struct klrpt_CancelStatisticsRequestResponse{
		struct param_error	error;
	}&r );

int klrpt_GetStatisticsData(
    xsd__wstring	wstrRequestId,
    struct klrpt_GetStatisticsDataResponse{
		struct param_error	error;
	    param__params paramData;
	}&r );

int klrpt_GetFilterSettings(
    xsd__int                reportType,
    struct klrpt_GetFilterSettingsResponse{
	    param__params		paramData;
		struct param_error	error;
	}&r );

int klrpt_ExecuteReportAsyncCancelWaitingForSlaves(
    xsd__wstring			wstrAsyncRequestId,
    struct klrpt_ExecuteReportAsyncCancelWaitingForSlavesResponse{
		struct param_error	error;
	}&r );


int klrpt_ResetStatisticsData(
	param__params	paramRequestParams,
    struct klrpt_ResetStatisticsDataResponse{
		struct param_error	error;
	}&r );

/*!
 * (C) 2004 Kaspersky Lab 
 * 
 * \file	srvp/srvhrch/srvhrch_soapapi.h
 * \author	Andrew Lashenkov
 * \date	29.12.2004 15:30:45
 * \brief	
 * 
 */

struct klsrvh_chld_srv_info
{
	xsd__int		nId;	
    xsd__wstring	wstrDisplName;	
	xsd__wstring	wstrNetAddress;	
    xsd__int		nGroupId;	
    xsd__wstring	wstrVersion;	
    xsd__wstring	wstrInstanceId;	
	xsd__int		nStatus;		
};

class klsrvh_chld_srv_info_vec
{
public:
	struct klsrvh_chld_srv_info*	__ptr;
	int								__size;
};

class klsrvh_wstrings
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};


int klsrvh_GetChildServers(
	xsd__int nGroupId,
	struct klsrvh_GetChildServersResponse{
        klsrvh_chld_srv_info_vec	srv_info_vec;
		struct param_error			error;
	}&r);

int klsrvh_GetServerInfo(
	xsd__int nId,
	klsrvh_wstrings	fields,
	struct klsrvh_GetServerInfoResponse{
		param__params	info;
		struct param_error	error;
	}&r);

int klsrvh_RegisterServer(
	param__params	baseInfo,
	param__params	addInfo,
	struct klsrvh_RegisterServerResponse{
		xsd__int			nId;
		struct param_error	error;                        
	}&r);

int klsrvh_DelServer(
	xsd__int	nId,
	struct klsrvh_DelServerResponse{
		struct param_error	error;
	}&r);

int klsrvh_UpdateServer(
	  xsd__int		nId,
	  param__params	info,
	  struct klsrvh_UpdateServerResponse{
		struct param_error	error;
	}&r);

int klsrvh_InitialPingMaster(
	param__params	info,
	struct klsrvh_InitialPingMasterResponse{
		param__params	info;
		struct param_error	error;
	}&r);

int klsrvh_PingMaster(
	param__params	info,
	struct klsrvh_PingMasterResponse{
		param__params	info;
		struct param_error	error;
	}&r);

int klsrvh_PingSlave(
	param__params	info,
	struct klsrvh_PingSlaveResponse{
		param__params	info;
		struct param_error	error;
	}&r);

int klsrvh_ForceRetranslateUpdates(
	param__params	info,
	struct klsrvh_ForceRetranslateUpdatesResponse{
		xsd__boolean bResult;
		struct param_error	error;
	}&r);

int klsrvh_DeliverReport(
	xsd__wstring wstrRequestId,
	xsd__int nRecipientDeliverLevel,
	xsd__int nSenderDeliverLevel,
	xsd__wstring wstrSenderInstanceId,
	param__params parDeliveryParams,
	param__params parReportResult,
	struct klsrvh_DeliverReportResponse{
		struct param_error	error;
	}&r);

int klsrvh_RecreateInstallationPackage(						
	xsd__wstring wstrPackageGUID,
	xsd__wstring wstrNewPackageName,
	xsd__wstring wstrFileId,
	xsd__wstring wstrProductName,
	xsd__wstring wstrProductVersion,
	xsd__wstring wstrProductDisplName,
	xsd__wstring wstrProductDisplVersion,
	xsd__boolean bReplaceExisting,
	struct klsrvh_RecreateInstallationPackageResponse{
		xsd__wstring wstrNewPackageName;
		xsd__boolean bResult;
		struct param_error	error;
	}&r);

int klsrvh_StartTask(
	xsd__wstring wstrTaskTsId,
	xsd__wstring wstrTaskTypeName,
	param__params	parParams,
	struct klsrvh_StartTaskResponse{
		param__params	parOutParams;
		struct param_error	error;
	}&r);

int klsrvh_StartRemoteInstallation(						
	param__params parTaskInfo,
	param__params parTaskParams,
	struct klsrvh_StartRemoteInstallationResponse{
		xsd__boolean bResult;
		xsd__boolean bAlreadyInstalledSkipped;
		struct param_error	error;
	}&r);

int klsrvh_ReportRemoteInstallationResult(						
	xsd__wstring wstrTaskTsId,
	param__params parResultTaskStateEvent,
	struct klsrvh_ReportRemoteInstallationResultResponse{
		struct param_error	error;
	}&r);

int klsrvh_NotifyOnReportResult(
    xsd__wstring	wstrRequestId,
	xsd__int		nResultCode,
	param__params	paramParams,
    struct klsrvh_NotifyOnReportResultResponse{
		struct param_error	error;
	}&r );


int klsrvh_FindSlaveServers(
                    xsd__wstring	            wstrFilter,
					klgrp_wstrings	            arrFields,
					klgrp_field_order_array     arrFieldsToOrder,
					param__params				params,
					xsd__int		            lTimeout,
                    xsd__wstring	            wstrRequestId,
					struct klsrvh_FindSlaveServersResponse{
						xsd__int		    nCount;
						xsd__wstring		wstrIterator;
						struct param_error	error;
					}& r);

/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	srvp\srvstgs\srvstgs_soapapi.h
 * \author	Andrew Lashchenkov
 * \date	08.07.2005 13:45
 * \brief	заголовочный файл с описанием SOAP-методов сервера, относящихся к модулю KLSRVS
 * 
 */

//std::wstring GetSharedFolder()
int klsrvs_GetSharedFolder( 
					void* _,
					struct klsrvs_GetSharedFolderResponse{
						xsd__wstring		wstrFolder;
						struct param_error	error;
					}&r);



//void AdmServerSettingsImpl::ChangeSharedFolder(const std::wstring&	wstrNetworkPath, KLEV::AdviseEvSink* /*pSink*/, KLADMSRV::HSINKID&	/*hSink*/)
int klsrvs_ChangeSharedFolder(
					xsd__wstring		wstrFolder,
					struct klsrvs_ChangeSharedFolderResponse{
						struct param_error	error;
					}&r);

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file nlst/soapapi.h
 * \author Шияфетдинов Дамир
 * \date 2005
 * \brief SOAP-интерфейс NetworkList
 */

struct klnlst_field_order
{
	xsd__wstring	wstrFieldName;
	xsd__boolean	bOrder;
};

class klnlst_field_order_array
{
public:
	struct klnlst_field_order*	__ptr;
	int							__size;
};

class klnlst_wstring_array
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

class klnlst_int_array
{
public:
	xsd__int*	__ptr;
	int			__size;
};

class klnlst_long_array
{
public:
	xsd__long*	__ptr;
	int			__size;
};

struct klnlst_network_list_item
{
	xsd__wstring		wstrItemId;
	xsd__unsignedLong	tmCreate;
	param__params		parItemParams;
	xsd__boolean		bDeletedFlag;
};


class klnlst_network_list_item_array
{
public:
	struct klnlst_network_list_item*	__ptr;
	int									__size;
};


int klnlst_ResetIterator(
        xsd__wstring		        listName,
        param__params		        paramIteratorDescr,
		klnlst_wstring_array		fields,
		klnlst_field_order_array	fields_to_order,
		xsd__int					lifetime,
		struct klnlst_ResetIteratorResponse{
			xsd__wstring		iteratorId;
			struct param_error	error;
		}&r);

int klnlst_GetRecordCount(
		xsd__wstring	iteratorId,
		struct klnlst_GetRecordCountResponse{
			xsd__int			nCount;
			struct param_error	error;
		}&r);

int klnlst_GetRecordRange(
		xsd__wstring	iteratorId,
		xsd__int		nStart,
        xsd__int		nEnd,
		struct klnlst_GetRecordRangeResponse{
			param__params		paramProperties;
			struct param_error	error;
		}&r);

int klnlst_ReleaseIterator(
		xsd__wstring	iteratorId,
		struct klnlst_ReleaseIteratorResponse{
			struct param_error	error;
		}&r);

int klnlst_UpdateHostNetworkList(
		xsd__wstring					nagentTransportName,
		xsd__wstring					listName,
		xsd__wstring					listComponentName,
		klnlst_network_list_item_array	listsItemsArray,
		struct klnlst_UpdateHostNetworkListResponse{
			struct param_error	error;
		}&r);

int klnlst_AddListItemTask(
		long						itemId,
		param__params				taskParams,
		xsd__wstring				taskName,
		struct klnlst_AddListItemTaskResponse{
			struct param_error	error;
		}&r);

int klnlst_AddTaskForListItems(
		klnlst_long_array			itemsIds,
		param__params				taskParams,
		xsd__wstring				taskName,
		struct klnlst_AddTaskForListItemsResponse{
			struct param_error	error;
		}&r);

int klnlst_GetItemInfo(
		long						itemId,		
		struct klnlst_GetItemInfoResponse{
			xsd__wstring				wstrListName;
			xsd__wstring				wstrItemId;
			xsd__wstring				wstrHostId;
			struct param_error			error;
		}&r);
/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file nagnlstsoapapi.h
 * \author Шияфетдинов Дамир
 * \date 2005
 * \brief SOAP-интерфейс NetworkList для сетевого агента
 */

int klnagnlst_CallNetworkListTask(
		xsd__wstring				listName,
		xsd__wstring				itemId,
		xsd__wstring				taskName,
		param__params				taskParams,
		struct klnagnlst_CallNetworkListTaskResponse{
			param__params		taskResult;
			struct param_error	error;
		}&r);

int klnagnlst_GetItemFileInfo(
		xsd__wstring				listName,
		xsd__wstring				itemId,
		struct klnagnlst_GetItemFileInfoResponse{
			xsd__unsignedLong	lTotalSize;
			xsd__wstring		wstrFileName;
			struct param_error	error;
		}&r);

struct klnagnlst_file_chunk_buff
{
	unsigned char *__ptr;
	int __size;
};

int klnagnlst_GetItemFileChunk(
		xsd__wstring				listName,
		xsd__wstring				itemId,
		xsd__unsignedLong			startPos,
		xsd__unsignedLong			neededSize,
		struct klnagnlst_GetItemFileChunkResponse{
			xsd__unsignedLong					resultCode;
			struct klnagnlst_file_chunk_buff	buff;
			struct param_error					error;
		}&r);
/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file server/fts/ftssoapapi.h
 * \author Шияфетдинов Дамир
 * \date 2006
 * \brief SOAP-интерфейс FileTransferServer'а 
 */

struct klft_GetNextFileChunkOptions
{
	xsd__unsignedLong	nMulticastRecvSize;
	xsd__wstring		wstrNagentHostId;
};

struct klft_GetUpdaterFolderFileInfoOptions
{
	xsd__wstring		wstrNagentHostId;
};

int klft_GetNextFileChunk(
						xsd__wstring	remoteClientName,
						xsd__wstring	fileName,
						xsd__wstring	fileId,
						xsd__unsignedLong	startFilePos,
						xsd__unsignedLong	neededSize,
						xsd__boolean		useUpdaterFolder,	
						xsd__boolean		forUpdateAgent,
						struct klft_GetNextFileChunkOptions *options,
						struct klft_GetNextFileChunkResponse
						{
							struct klft_file_chunk_t	chunk;
							struct param_error	error;
						}	&r );

int klft_GetServerFilesList(
						xsd__wstring clientName,
						struct klft_GetServerFilesListResponse
						{
							param__params filesIdsList;
							struct param_error	error;
						}	&r );

int klft_GetUpdaterFolderFileInfo(
						xsd__wstring	fileName,
						struct klft_GetUpdaterFolderFileInfoOptions *options,
						struct klft_GetUpdaterFolderFileInfoResponse
						{
							struct klft_file_info_t	info;
							struct param_error	error;
						}	&r );

int klft_GetFullFileInfo(
						xsd__wstring	fileId,						
						struct klft_GetFullFileInfoResponse
						{
							param__params	fileInfo;
							struct param_error	error;
						}	&r );

int klft_GetHostUpdateAgentInfo(
						xsd__wstring	hostId,
						struct klft_GetHostUpdateAgentInfoResponse
						{
							struct SOAPUpdateAgentInfosList updatesAgents;
							struct param_error				error;
						}	&r );

int klft_SendToClientFullSyncArchive(
						xsd__wstring	remoteClientName,
						xsd__wstring	syncFolderFileId,
						xsd__boolean	fileNamesHashed,
						struct SOAPFilesInfoList *filesList,
						struct SOAPSyncFolderOptions *syncFolderOptions,
						struct klft_SendToClientFullSyncArchiveResponse
						{	
							xsd__boolean				folderIsEqual;
							struct param_error			error;
							struct SOAPFilesInfoList	*deletedFilesList;
						}	&r );


class klfts_update_agent_info
{
public:	
	xsd__wstring	strAgentHostId;
	xsd__wstring	strHostDisplayName;
	
	int				nGroupId;
	
	int				nPort;
	int				nSSLPort;
	xsd__wstring	strMulticastAddr;
};

class klfts_update_agent_info_array
{
public:
	klfts_update_agent_info*		__ptr;
	int								__size;
};

int klfts_RegisterUpdateAgent(
						klfts_update_agent_info			updateAgentInfo,
						xsd__boolean					checkExisting,
						struct klfts_RegisterUpdateAgentResponse
						{
							struct param_error	error;
						}&r);

int klfts_ModifyUpdateAgent(
						klfts_update_agent_info			updateAgentInfo,
						struct klfts_ModifyUpdateAgentResponse
						{
							struct param_error	error;
						}&r);

int klfts_UnregisterUpdateAgent(
						xsd__wstring					strAgentHostId,
						struct klfts_UnregisterUpdateAgentResponse
						{
							struct param_error	error;
						}&r);

int klfts_GetDefaultUpdateAgentRegistrationInfo(
						int								nGroupId,
						struct klfts_GetDefaultUpdateAgentRegistrationInfoResponse
						{
							klfts_update_agent_info defaultUpdateAgentInfo;
							struct param_error	error;
						}&r);

int klfts_GetUpdateAgentsForGroup(
						int								nGroupId,
						struct klfts_GetUpdateAgentsForGroupResponse
						{
							klfts_update_agent_info_array updateAgentInfoList;
							struct param_error	error;
						}&r);

int klfts_GetUpdateAgentInfo(
						xsd__wstring					strAgentHostId,
						struct klfts_GetUpdateAgentInfoResponse
						{
							klfts_update_agent_info updateAgentInfo;
							struct param_error	error;
						}&r);

int klfts_UseUAForSyncFolder(
						xsd__wstring			strAgentHostId,
						xsd__wstring			strSyncFolderName,
						xsd__boolean			bUse,
						struct klfts_UseUAForSyncFolderResponse
						{
							struct param_error	error;
						}&r);

int klfts_GetUseUAForSyncFolder(
						xsd__wstring			strAgentHostId,
						xsd__wstring			strSyncFolderName,						
						struct klfts_GetUseUAForSyncFolderResponse
						{
							xsd__boolean		bUse;
							struct param_error	error;
						}&r);


/*!
 * (C) 2006 Kaspersky Lab
 * 
 * \file	cmprsftsoapapi.h
 * \author	Eugene Rogozhnikov
 * \date	24.05.2006 10:30:54
 * \brief	SOAP-интерфейс CompetitorSoftware. Сейчас используется только для поддержки	
 *			совместимости со старой консолью - KLAK_VER_6_MP1
 */		

class klcmptrsft_wstring_array
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

int klcmptrsft_GetCompetitorSoftwareList( 
	struct klcmptrsft_GetCompetitorSoftwareLisResponse{
			klcmptrsft_wstring_array	fields;
			struct param_error	error;
		}&r);

int klcmptrsft_GetHostCompetitorSoftwareList( 
	xsd__int	hostId,
	struct klcmptrsft_GetHostCompetitorSoftwareListResponse{
			klcmptrsft_wstring_array	fields;
			struct param_error	error;
		}&r);



/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	aklwngt_soap.h
 * \author	Andrew Kazachkov
 * \date	14.03.2007 11:40:59
 * \brief	Gsoap interface for mobile clients
 * 
 */


class aklwngt__anyCmd{
public:
	// virtual int getType();
};

class aklwngt__CmdError : public aklwngt__anyCmd {
public:
    struct param_error  oError;
	// virtual int getType();
};

class aklwngt__CmdHash : public aklwngt__anyCmd {
public:
	xsd__base64Binary       hash;
	// virtual int getType();
};

class aklwngt__CmdPar : public aklwngt__anyCmd {
public:
	param__params           parameters;
	// virtual int getType();
};

class aklwngt__CmdParAndHash : public aklwngt__anyCmd {
public:
	xsd__base64Binary       hash;
	param__params           parameters;
	// virtual int getType();
};

struct aklwngt__string_array
{
	xsd__string*	__ptr;
	int				__size;
};

class aklwngt__CmdSubscriptions : public aklwngt__anyCmd {
public:
	struct aklwngt__string_array    typesInfo;
	struct aklwngt__string_array    typesWarning;
	struct aklwngt__string_array    typesError;
	struct aklwngt__string_array    typesCritical;
	// virtual int getType();
};

struct aklwngt__DevInfo
{
    xsd__wstring    presumedName;
    xsd__wstring    presumedGroup;
    xsd__int        os_type;
    xsd__int        os_ver_major;
    xsd__int        os_ver_minor;

    xsd__wstring        os_manufacturer;
    xsd__wstring        os_name;
    xsd__wstring        os_ver;
    xsd__unsignedLong   memory;
    xsd__wstring        processor_family;
    xsd__wstring        system_name;
    xsd__wstring        system_type;
};

struct aklwngt__ProdInfo
{
    xsd__string     product_name;
    xsd__string     product_version;
    xsd__dateTime   installTime;
    xsd__wstring    dn;
    xsd__string     prodVersion;
};

struct aklwngt__LicKeyInfo
{
    xsd__int        keyType;
    xsd__int        keyProdSuiteID;
    xsd__int        keyAppID;
    xsd__dateTime   keyInstallDate;
    xsd__dateTime   keyExpirationDate;
    xsd__int        keyLicPeriod;
    xsd__int        keyLicCount;
    xsd__string     keySerial;
    xsd__string     keyMajVer;
    xsd__wstring    keyProdName;
};

struct aklwngt__LicenseInfo
{
    struct aklwngt__LicKeyInfo* keyCurrent;
    struct aklwngt__LicKeyInfo* keyNext;
};

class aklwngt__CmdAppInfo : public aklwngt__anyCmd {
public:
	xsd__base64Binary               hash;
	struct aklwngt__ProdInfo        prodInfo;
	struct aklwngt__LicenseInfo     licenseInfo;
    struct aklwngt__DevInfo         devInfo;
	// virtual int getType();
};

class aklwngt__CmdAppState : public aklwngt__anyCmd {
public:
	xsd__base64Binary       hash;
    xsd__int                stateApp;
    xsd__int                stateRTP;
    xsd__dateTime           lastFullScan;
    xsd__dateTime           lastUpdateTime;
    xsd__dateTime           basesDate;
    xsd__dateTime           basesInstallDate;
    xsd__int                basesRecords;

	// virtual int getType();
};

struct aklwngt__command{
    xsd__int            code;
    aklwngt__anyCmd*    data;
};

struct aklwngt__sync_info{
    xsd__base64Binary    hash_stgs;
    xsd__base64Binary    hash_pol;
    xsd__base64Binary    hash_sbsc;
    xsd__base64Binary    hash_info;
    xsd__base64Binary    hash_state;
};

struct aklwngt__event_body
{
    xsd__int                    severity;
    xsd__string                 product_name;
    xsd__string                 product_version;
    xsd__string                 event_type;
    xsd__wstring                event_type_display_name;
    xsd__wstring                descr;
    xsd__int                    locid;
    xsd__dateTime               rise_time;
    xsd__int                    rise_time_ms;
    xsd__wstring                par1;
    xsd__wstring                par2;
    xsd__wstring                par3;
    xsd__wstring                par4;
    xsd__wstring                par5;
    xsd__wstring                par6;
    xsd__wstring                par7;
    xsd__wstring                par8;
    xsd__wstring                par9;
};

struct aklwngt__event_info
{
    xsd__string                 eventId;
    struct aklwngt__event_body  body;
};

struct aklwngt__event_info_array
{
	struct aklwngt__event_info* __ptr;
	int                         __size;
};

struct aklwngt__klft_file_chunk_buff
{
	xsd__unsignedByte*	__ptr;
	int					__size;
};

struct aklwngt__klft_file_chunk_t
{
	xsd__wstring		                    fileName;
	xsd__unsignedLong	                    CRC;
	xsd__unsignedLong	                    startPosInFile;
	xsd__unsignedLong	                    fullFileSize;
	struct aklwngt__klft_file_chunk_buff    buff;
};

struct aklwngt__klft_file_info_t
{
	xsd__wstring		fileName;
	xsd__boolean		isDir;
	xsd__boolean		readPermission;
	xsd__dateTime       createTime;
	xsd__unsignedLong	fullFileSize;
};


int aklwngt__SessionBegin(
						xsd__string     szDeviceId,
                        xsd__string     szPassword,
                        int             nProtocolVersion,
                        struct aklwngt__SessionBeginResponse
                        {
                            xsd__string         szSessionId;
                            int                 nProtocolVersion;
                            struct param_error* oError;
                        }&r);

int aklwngt__SessionEnd(
                        xsd__string         szSessionId,
                        struct aklwngt__SessionEndResponse
                        {
                            struct param_error*         oError;
                        }&r);

int aklwngt__Heartbeat(
                        xsd__string     szSessionId,
                        struct aklwngt__sync_info* oMaySynchronize,
                        struct aklwngt__HeartbeatResponse{
                            struct aklwngt__command     oCommand;
                            struct param_error*         oError;
                        }&r);

int aklwngt__PutResult(
                        xsd__string     szSessionId,
                        struct aklwngt__command*    result,
                        struct aklwngt__PutResultResponse{
                            struct aklwngt__command     oNextCommand;
                            struct param_error*         oError;
                        }&r);

int aklwngt__PutEvents(
                        xsd__string     szSessionId,
                        struct aklwngt__event_info_array* oEventsInfoArray,
                        struct aklwngt__PutEventsResponse{
                            struct param_error*         oError;
                        }&r);

int aklwngt__klft_GetFileInfo(
                        xsd__string     szSessionId,
						xsd__wstring	fileName,
						struct aklwngt__klft_GetFileInfoResponse
						{
							struct aklwngt__klft_file_info_t	    info;
							struct param_error*         oError;
						}&r);

int aklwngt__klft_GetNextFileChunk(
                        xsd__string     szSessionId,
						xsd__wstring	fileName,
						xsd__unsignedLong	startFilePos,
						xsd__unsignedLong	neededSize,
						struct aklwngt__klft_GetNextFileChunkResponse
						{
							struct aklwngt__klft_file_chunk_t	chunk;
							struct param_error*                 oError;
						}&r);

/*!
 * (C) 2007 Kaspersky Lab
 * 
 * \file	ptchsoapapi.h
 * \author	Eugene Rogozhnikov
 * \date	08.05.2007 16:43:56
 * \brief	заголовочный файл с описанием SOAP-методов сервера, относящихся к
 *				модулю PTCH
 * 
 */

int klptch_get_nagent_latest_patch_info(
	param__params	nagent_data,
	struct klptch_get_nagent_latest_patch_infoResponse{
					param__params		data;
					struct param_error	error;
				}&r);


/*!
 * (C) 2007 Kaspersky Lab
 * 
 * \file	invsoapapi.h
 * \author	Eugene Rogozhnikov
 * \date	26.07.2007 18:13:19
 * \brief	SOAP-интерфейс Inventory	
 * 
 */

struct klinv_product_info
{
	xsd__wstring	wstrProductID;
	xsd__boolean	bIsMsi;
	xsd__wstring	wstrPackageCode;
	xsd__wstring	wstrDisplayName;
	xsd__wstring	wstrDisplayVersion;
	xsd__wstring	wstrPublisher;
	xsd__unsignedShort	uLangId;
	xsd__wstring	wstrInstallDate;
	xsd__wstring	wstrInstallDir;
	xsd__wstring	wstrComments;
	xsd__wstring	wstrHelpLink;
	xsd__wstring	wstrHelpTelephone;
	xsd__wstring	wstrARPRegKey;
	xsd__wstring	wstrCleanerProductName;
};

class klinv_product_info_array
{
public:
	struct klinv_product_info*	__ptr;
	int							__size;
};

struct klinv_patch_info
{
	xsd__wstring	wstrPatchID;
	xsd__boolean	bIsMsi;
	xsd__wstring	wstrParentID;
	xsd__wstring	wstrDisplayName;
	xsd__wstring	wstrDisplayVersion;
	xsd__wstring	wstrPublisher;
	xsd__wstring	wstrInstallDate;
	xsd__wstring	wstrComments;
	xsd__wstring	wstrHelpLink;
	xsd__wstring	wstrHelpTelephone;
	xsd__wstring	wstrClassification;
	xsd__wstring	wstrMoreInfoURL;
};

class klinv_patch_info_array
{
public:
	struct klinv_patch_info*	__ptr;
	int							__size;
};

struct klinv_cleaner_inifile_info
{
	xsd__wstring	wstrProductName;
	xsd__wstring	wstrType;
	xsd__wstring	wstrFileName;
	xsd__wstring	wstrDetectRegistry;
	xsd__wstring	wstrDetectMsi;
};


class klinv_cleaner_inifile_info_array
{
public:
	struct klinv_cleaner_inifile_info*	__ptr;
	int								__size;
};

int klinv_GetInvProductsList( 
	struct klinv_GetInvProductsListResponse{
			klinv_product_info_array	fields;
			struct param_error	error;
		}&r);

int klinv_GetInvPatchesList( 
	struct klinv_GetInvPatchesListResponse{
			klinv_patch_info_array	fields;
			struct param_error	error;
		}&r);

int klinv_GetHostInvProductsList( 
	xsd__int	hostId,
	struct klinv_GetHostInvProductsListResponse{
			klinv_product_info_array	fields;
			struct param_error	error;
		}&r);

int klinv_GetHostInvPatchesList( 
	xsd__int	hostId,
	struct klinv_GetHostInvPatchesListResponse{
			klinv_patch_info_array	fields;
			struct param_error	error;
		}&r);

int klinv_GetSrvCompetitorIniFileInfoList( 
		xsd__wstring	wstrType,
		struct klinv_GetSrvCompetitorIniFileInfoListResponse{
				klinv_cleaner_inifile_info_array	fields;
				struct param_error	error;
			}&r);

//////////////////////////////////////////////////////////////////
// Для inventory-списка

struct klinv_field_order
{
	xsd__wstring	wstrFieldName;
	xsd__boolean	bOrder;
};

class klinv_field_order_array
{
public:
	struct klinv_field_order*	__ptr;
	int							__size;
};

class klinv_wstring_array
{
public:
	xsd__wstring*	__ptr;
	int				__size;
};

int klinv_ResetIterator(
		xsd__wstring	wstrFilter,
		klinv_wstring_array		fields,
		klinv_field_order_array	fields_to_order,
		xsd__int					lifetime,
		struct klinv_ResetIteratorResponse{
			xsd__wstring		iteratorId;
			struct param_error	error;
		}&r);

int klinv_GetRecordCount(
		xsd__wstring	iteratorId,
		struct klinv_GetRecordCountResponse{
			xsd__int			nCount;
			struct param_error	error;
		}&r);

int klinv_GetRecordRange(
		xsd__wstring	iteratorId,
		xsd__int		nStart,
        xsd__int		nEnd,
		struct klinv_GetRecordRangeResponse{
			param__params		paramProperties;
			struct param_error	error;
		}&r);

int klinv_ReleaseIterator(
		xsd__wstring	iteratorId,
		struct klinv_ReleaseIteratorResponse{
			struct param_error	error;
		}&r);


/*!
 * (C) 2007 "Kaspersky Lab"
 *
 * \file server/trf/trfsoapapi.h
 * \author Шияфетдинов Дамир
 * \date 2007
 * \brief SOAP-интерфейс TrafficManagerServer'а 
 */

class kltrf_Restriction
{
public:	
	xsd__int	iId;

	xsd__int	iTotalLimit;

	xsd__int	iStartHour, iStartMin;
	xsd__int	iEndHour, iEndMin;

	xsd__int	iLimit;

	xsd__int	nMaskOrLo;
	xsd__int	nSubnetOrHi;
	
	xsd__boolean bIsSubnet;
};

class kltrf_Restriction_array
{
public:
	kltrf_Restriction*		__ptr;
	int						__size;
};

int kltrf_ReplaceRestriction(						
						kltrf_Restriction	restriction,
						struct kltrf_ReplaceRestrictionResponse
						{
							struct param_error	error;
							long				nRestrictionId;
						}	&r );


int kltrf_DeleteRestriction(
						xsd__long		nRestrictionId,
						struct kltrf_DeleteRestrictionResponse
						{
							struct param_error	error;
						}	&r );

int kltrf_GetRestrictions(						
						struct kltrf_GetRestrictionsResponse
						{
							kltrf_Restriction_array	restrArray;
							struct param_error	error;
						}	&r );

/*!
* (C) 2005 "Kaspersky Lab"
*
* \file wol_soapapi.cpp
* \author Шияфетдинов Дамир
* \date 2007
* \brief GSOAP Интерфейс для промежуточной рассылки WakeOnLan сигнала
*
*/



int klnag_SendWakeOnLanOnSubnetsHosts(
                        param__params macsList,
                        struct klnag_SendWakeOnLanOnSubnetsHostsResponse{ 
                            struct param_error	error; 
                        }& r);
/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file snmp/snmpagt/snmpagtsoapapi.h
 * \author Шияфетдинов Дамир
 * \date 2007
 * \brief SOAP-интерфейс SNMP Agent
 */

struct klsnmpagt_event
{
	xsd__long		lDateTime;
	xsd__int		lSeverity;

	xsd__wstring	wstrComputerName;
	xsd__wstring	wstrApplicationName;
	xsd__wstring	wstrEventCaption;
	xsd__wstring	wstrEventDescription;

	xsd__wstring	wstrEventTypeCode;
};

int klsnmpagt_PutServerEvent(
        struct klsnmpagt_event	event,
		struct klsnmpagt_PutServerEventResponse{			
			struct param_error	error;
		}&r);

int klsnmpagt_SetAgentConnection(
        xsd__wstring	wstrLocalName,
		xsd__wstring	wstrRemoteName,
		struct klsnmpagt_SetAgentConnectionResponse{			
			struct param_error	error;
		}&r);
