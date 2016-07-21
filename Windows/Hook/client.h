#ifndef __CLIENT_H
#define __CLIENT_H

/*!
*		
*		
*		(C) 2001-3 Kaspersky Lab 
*		
*		\file	client.h
*		\brief	Обеспечение подключения клиентов к драйверу, создание данных, необходимых для транстляции запросов 
*		Ring0 - Ring3
*
*		\author Andrew Sobko
*		\date	11.09.2003 14:23:15
*		
*		Типичная схема работы клиента - 
*		1. проверка версии интерфейса (IOCTLHOOK_GetVersion)
*		2. регистрация клиента(IOCTLHOOK_RegistrApp)
*		3. надиктовка фильтров (см. filter.h)
*		4. перевод подключения в активный режим (IOCTLHOOK_Activity -> _AS_GoActive)
*		5. ожидание событий на объекте APP_REGISTRATION.m_WhistleUp
*		6. обработка события (см. evqueu.h)
*		7. перевод подключения в спящий режим (IOCTLHOOK_Activity -> _AS_GoSleep)
*		8. удаление фильтров
*		9. отключение от драйвера (IOCTLHOOK_Activity -> _AS_Unload)
*
*		каждый клиент может иметь несколько подклиентов для распределения нагрузки. Общее для подклиентов:
*		1. очередь фильтров			
*		2. очередь немаркированных событий
*		3. кеш
*
*		Особенности реализации:
*		1. создаётся служебный поток для освобождения неиспользуемых ресурсов
*
*/		

#include "osspec.h"
#include "structures.h"
#include "debug.h"

//+ ------------------------------------------------------------------------------------------

//! \fn				: ProcessDelayedQueue
//! \brief			: внешний метод, используется для передачи управления для отложенных операций
//! \return			: 
extern VOID ProcessDelayedQueue();

//! \fn				: FS_CheckNewDevices
//! \brief			: внешний метод для передачи на отложенном потоке (для проверки загруженных драйверов)
//! \return			: 
extern void FS_CheckNewDevices();

//+ ------------------------------------------------------------------------------------------
//+ флаги состояния клиента

#define _INTERNAL_APP_STATE_NONE			0x000

// client connection status. if this bit setted mean that client connected
#define _INTERNAL_APP_STATE_CONNECTED		0x001

// client activity status. if this bit setted mean that cleint active (otherwise in SleepMode)
#define _INTERNAL_APP_STATE_ACTIVE			0x002

// client functional status. if this bit setted mean that client was connected but not responding
#define _INTERNAL_APP_STATE_DEAD			0x004

// client activity status. if this bit setted mean that client lifetime timeout expired
#define _INTERNAL_APP_STATE_WATCHDOGPAUSED	0x008

//+ ------------------------------------------------------------------------------------------
//+ результаты обращения в кеш

#define _CL_CACHE_NOTCHECKED	-1
#define _CL_CACHE_NOTCACHED		0 
#define _CL_CACHE_CACHED_DIRTY	1
#define _CL_CACHE_CACHED		2

//+ ------------------------------------------------------------------------------------------
//+ внутренние счётчики/состояния для системы обработки клиентов
extern ULONG gActiveClientCounter;
extern BOOLEAN bClientsMutexInited;

//+ ------------------------------------------------------------------------------------------
//+ служебный поток
#ifdef _HOOK_NT_ 
extern PVOID ActivityWatchDogThread;
#else
extern TIMEOUTHANDLE hActivityWatchDog;
#endif //_HOOK_NT_

//+ ------------------------------------------------------------------------------------------
//! \fn				: GlobalClientsInit
//! \brief			: Инициализация внтуренних данных, обеспечивающих работоспособность системы клиентов
//! \return			: STATUS_UNSUCCESSFUL если инициализация не выполнена / STATUS_SUCCESS - успешно
//! \param          : void
NTSTATUS
GlobalClientsInit(void);

//! \fn				: GlobalClientsDone
//! \brief			: Освобождение внутренних данных. Должна быть вызвана при любом результате работы GlobalClientsInit
//! \return			: 
//! \param          : void
void
GlobalClientsDone(void);

//! \fn				: AcquireClient
//! \brief			: захват данных о клиенте (данные не будут удалены до освобождения)
//! \return			: PQCLIENT - указатель на данные по клиенту / NULL если клиент не найден
//! \param          : ULONG AppID - идентификатор клиента
PQCLIENT
AcquireClient(ULONG AppID);

//! \fn				: ReleaseClient
//! \brief			: освобождение клиентсуой структуры
//! \return			: 
//! \param          : PQCLIENT pClient - указатель, полученный через AcquireClient
void
ReleaseClient(PQCLIENT pClient);

void
ReleaseClientAndSubClient(PQCLIENT pClient, PQSUBCLIENT pSubClient);

//! \fn				: IsExistClient
//! \brief			: проверка наличия клиента с указанным идентификаторов	
//! \return			: TRUE если найден / FALSE если не найден
//! \param          : ULONG AppID - идентификатор клиента
BOOLEAN
IsExistClient(ULONG AppID);

//! \fn				: IsSubClientConnected
//! \brief			: проверка что указанный подклиент имеет активное подключение	
//! \return			: TRUE - подключение существует / FALSE - подключение не существует
//! \param          : PQSUBCLIENT pSubClient - указатель на подклиента
BOOLEAN
IsSubClientConnected(PQSUBCLIENT pSubClient);

//! \fn				: ClientActivity
//! \brief			: Управляющие команды для клиента (см. APPSTATE_REQUEST)
//! \return			: TRUE - запрос обработан до конца / FALSE в ходе обработки были ошибки (невалидный запрос)
//! \param          : PHDSTATE In - управляющий блок
//! \param          : PHDSTATE Out - результат обработки запроса (зависит от типа входной команды)
BOOLEAN
ClientActivity(PHDSTATE In, PHDSTATE Out);

//! \fn				: RegisterAppFromSav
//! \brief			: восстановление клиентского блока из реестра (выполняется на старте драйвера)
//! \return			: TRUE - успешно / FALSE - клиент не восстановлен
//! \param          : ULONG AppID - идентификатор клиента
//! \param          : PCLIENT_SAV pClientSav - данные из реестра
BOOLEAN
RegisterAppFromSav(ULONG AppID, PCLIENT_SAV pClientSav);

//! \fn				: RegisterApplication
//! \brief			: функция подключения клиента из Ring3
//! \return			: TRUE - подключение выполнено / FALSE - клиент не подключен
//! \param          : PAPP_REGISTRATION pReg - данные на регистрацию
//! \param          : PCLIENT_INFO pClientInfo - выходной блок (выданный идентификатор, etc...)
BOOLEAN
RegisterApplication(PAPP_REGISTRATION pReg, PCLIENT_INFO pClientInfo);

//! \fn				: DisconnectClientByProcID
//! \brief			: Отключение клиента при убивании процесса (подчистка данных)
//! \return			: 
//! \param          : ULONG ProcID - идентификатор убиваемого процесса
void
DisconnectClientByProcID(ULONG ProcID);

//! \fn				: DisconnectClientByThreadID
//! \brief			: подчистка данных при убивании потока (служебных)
//! \return			: 
//! \param          : ULONG ThreadID - идентификатор убиваемого потока
void
DisconnectClientByThreadID(ULONG ThreadID);

//! \fn				: FreeUnusedClients
//! \brief			: Удаление освобождённых данных по клиентав	(вызывается по таймауту из служебного потока)
//! \return			: TRUE - если был удалён хоть один ресурс / FALSE - не было удалёния ресурсов
//! \param          : void
BOOLEAN
FreeUnusedClients(void);

//! \fn				: GetClientPriority
//! \brief			: получение приоритета клиента (0 если клиент не найдет)
//! \return			: приоритет
//! \param          : ULONG AppID - идентификатор клиента
ULONG
GetClientPriority(ULONG AppID);

//! \fn				: GetFreeSubClient
//! \brief			: найти активынй подклиент с минимальной нагрузкой
//! \return			: указатель на данные по подклиенту
//! \param          : PQCLIENT pClient - клиент
PQSUBCLIENT
GetFreeSubClient(PQCLIENT pClient);

void
ReleaseSubClient(PQSUBCLIENT pSubClient);

//! \fn				: GetCurrentSubClient
//! \brief			: получение клиента для зарегистрированного на данном процессе/потоке
//! \return			: указатель на подклиента
//! \param          : PQSUBCLIENT pSubClient - начало списка подклиентов
PQSUBCLIENT
GetCurrentSubClient(PQSUBCLIENT pSubClient);

//! \fn				: ClientSetFlowTimeout - не реализовано (зарезервирован)
//! \brief			: установка плавающего таймаута
//! \return			: 
//! \param          : PFLOWTIMEOUT pFlow
void
ClientSetFlowTimeout(PFLOWTIMEOUT pFlow);

//! \fn				: ClientResetCache
//! \brief			: очистка данных в кеше
//! \return			: STATUS_SUCCESS - данные в кеше очищены / STATUS_UNSUCCESSFUL - ошибка при обработке (
//! не найден клиент, клиент не имеет кеша, etc...
//! \param          : ULONG AppID - идентификатор клиента
NTSTATUS
ClientResetCache(ULONG AppID);

//+ ------------------------------------------------------------------------------------------
//! \fn				: AcquireClientResource
//! \brief			: блокирование операций с очередью клиентов
//! \return			: 
void
AcquireClientResource(BOOLEAN bExclusive);

//! \fn				: ReleaseClientResource
//! \brief			: разблокирование операций с очередью клиентов
//! \return			: 
void
ReleaseClientResource();

//+ ------------------------------------------------------------------------------------------
//+ функции работы с кешом
//+ для всех функций: ExpirationTimeShift - сдвиг от текущего момента времени в милисекундах

//! \fn				: Obj_CacheReset
//! \brief			: очистка кеша
//! \return			: TRUE - кеш очищен / FALSE - ошибка при отработке команды
//! \param          : PQCLIENT pClient
BOOLEAN
Obj_CacheReset(PQCLIENT pClient);

//! \fn				: Obj_IsCached
//! \brief			: проверка данных об объекте в кеше
//! \return			: см CL_CACHE_??? константы
//! \param          : PQCLIENT pClient - указатель на клиентские данные
//! \param          : DWORD dwVal - идентификатор объекта
//! \param          : BOOLEAN bNeedProtect - для внешних запросов TRUE
//! \param          : PVERDICT pVerdict - если объект найден в кеше то вердикт будет заполнен закешированными данными
DWORD
Obj_IsCached(PQCLIENT pClient, DWORD dwVal, BOOLEAN bNeedProtect, PVERDICT pVerdict);

//! \fn				: Obj_CacheAdd
//! \brief			: добавление объекта в кеш
//! \return			: TRUE - объект добавлен / FALSE - ошибка при отработке команды
//! \param          : PQCLIENT pClient - указатель на клиентские данные
//! \param          : DWORD dwVal - идентификатор объекта
//! \param          : BOOLEAN bDirty - признак что добавляемые данные в кеш являются временно не валидными (Obj_CacheUpdate)
//! \param          : VERDICT Verdict - ассоциированные данные
//! \param          : ULONG ExpirationTimeShift - время жизни данных в кеше (0 - не ограничено)
BOOLEAN
Obj_CacheAdd(PQCLIENT pClient, DWORD dwVal, BOOLEAN bDirty, VERDICT Verdict, ULONG ExpirationTimeShift);

//! \fn				: Obj_CacheUpdate
//! \brief			: обновить ассоциированные данные в кеше
//! \return			: 
//! \param          : PQCLIENT pClient - указатель на клиентские данные
//! \param          : DWORD dwVal - идентификатор объекта
//! \param          : BOOLEAN bDirty - валидность данных
//! \param          : VERDICT Verdict - ассоциированные данные
//! \param          : ULONG ExpirationTimeShift - время жизни данных в кеше (0 - не ограничено)
void
Obj_CacheUpdate(PQCLIENT pClient, DWORD dwVal, VERDICT Verdict, ULONG ExpirationTimeShift);

//! \fn				: Obj_CacheDel
//! \brief			: удалить объект из кеша
//! \return			: 
//! \param          : PQCLIENT pClient - указатель на клиентские данные
//! \param          : DWORD dwVal - идентификатор объекта
BOOLEAN
Obj_CacheDel(PQCLIENT pClient, DWORD dwVal);

//+ ------------------------------------------------------------------------------------------
//! \fn				: ActivityWatchDogCheck
//! \brief			: поточная функция (служебный поток)
//! \return			: 
//! \param          : VOID
VOID
ActivityWatchDogCheck(VOID);

//! \fn				: StartAWDog
//! \brief			: запустить служебный поток
//! \return			: STATUS_SUCCESS - поток запущен / STATUS_UNSUCCESSFUL - сбой при запуске
//! \param          : VOID
NTSTATUS
StartAWDog(VOID);

//! \fn				: StopAWDog
//! \brief			: остановить служебный поток
//! \return			: 
//! \param          : VOID
VOID
StopAWDog(VOID);


//+ ------------------------------------------------------------------------------------------
//+ прочие функции

//! \fn				: CalcCacheIDUnSens
//! \brief			: расчет хеша для строки (без учёта регистра - upperchar)
//! \return			: хеш
//! \param          : ULONG size - длина строки
//! \param          : BYTE* ptr - указатель на строку
//! \param          : const DWORD dwInitValue - инициализирующее значение
//! \param          : const BOOLEAN IsUnicode - признак юникода
DWORD
CalcCacheIDUnSens(ULONG size, BYTE* ptr, const DWORD dwInitValue, const BOOLEAN IsUnicode);

//! \fn				: CalcCacheID
//! \brief			: расчет хеша для блока данных
//! \return			: хеш
//! \param          : ULONG size - длина блока
//! \param          : BYTE* ptr - указатель на данные
//! \param          : DWORD dwInitValue - инициализирующее значение
DWORD
CalcCacheID(ULONG size, BYTE* ptr, DWORD dwInitValue);


void SuspendSubClient(ULONG pkThread);
void ResumeSubClient(ULONG pkThread);

void ReleasePopupEventsForProcess(ULONG ProcessId, ULONG HookId);

#endif // __CLIENT_H