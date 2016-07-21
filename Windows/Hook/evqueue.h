#ifndef _EVQUEUE_H
#define _EVQUEUE_H

/*!
*		
*		
*		(C) 2001-3 Kaspersky Lab 
*		
*		\file	evqueue.h
*		\brief	обработка событий для клиентов (очереди событий, отслеживание времени обработки, etc...)
*		
*		\author Sergey Belov, Andrew Sobko
*		\date	11.09.2003 17:06:58
*		
*		Типичная схема обработки события
*		1. добавление нового события в очередь (QueueAdd)
*		2. нотификаци клиенту о новом событии (PulseEventToSubClientSafe)
*		3. маркирование события и забор данных (GetEventForClient)
*		4. результат обработки события клиентом (EventSetVerdict)
*/		


#include "osspec.h"
#include "klsecur.h"

#include "hook/avpgcom.h"

//+ ------------------------------------------------------------------------------------------
//+ внутренние счётчики для системы обработки событий
extern ULONG gDeadLocks;
extern ULONG gReplyWaiterEnters;
//+ ------------------------------------------------------------------------------------------

//! \fn				: InitEventList
//! \brief			: инициализация очереди событий
//! \return			: TRUE - успешно / FALSE - сбой
//! \param          : PEVENT_LIST pEvList - очередь
BOOLEAN
InitEventList(PEVENT_LIST pEvList);

//! \fn				: DoneEventList
//! \brief			: освобождение очереди (для событий будут приняты решения по умолчанию)
//! \return			: 
//! \param          : PEVENT_LIST pEvList - очередь
void
DoneEventList(PEVENT_LIST pEvList);

//! \fn				: ReleaseMarkedEvents
//! \brief			: освобождение маркированных подклиентом событий (из-за отключения...)
//! \return			: 
//! \param          : ULONG AppID - идентификатор клиента
//! \param          : PEVENT_LIST pEvList - список
//! \param          : ULONG ProcessID - убиваемый процесс
void
ReleaseMarkedEvents(ULONG AppID, PEVENT_LIST pEvList, ULONG ProcessID);

//! \fn				: ReleaseEvents
//! \brief			: удаление всех событий в очереди (для событий будут приняты решения по умолчанию)
//! \return			: 
//! \param          : ULONG AppID - идентификатор клиента
//! \param          : PEVENT_LIST pEvList - очередь
void
ReleaseEvents(ULONG AppID, PEVENT_LIST pEvList);

void
ReleasePopupEventsByProcess(PEVENT_LIST pEvList, ULONG ProcessId, ULONG HookId);

//! \fn				: SkipEventForClient
//! \brief			: пропустить первое немаркированное событие в очереди (принять решение по умолчанию)
//! \return			: STATUS_SUCCESS - событие обработано / STATUS_NOT_FOUND - нет событий в очереди
//! \param          : ULONG AppID - идентификатор клиента
NTSTATUS
SkipEventForClient(ULONG AppID);

//! \fn				: GetEventForClient
//! \brief			: маркирование и возвращение контента события
//! \return			: TRUE - событие помаркировано / FALSE сбой при обработке
//! \param          : PGET_EVENT pGetEvent - даные для маркирования
//! \param          : PEVENT_TRANSMIT pEventTransmit - событие
//! \param          : DWORD* pOutputBufferLength - на входе размер буфера, на выходе размер данных события
BOOLEAN
GetEventForClient(PGET_EVENT pGetEvent, PEVENT_TRANSMIT pEventTransmit, DWORD* pOutputBufferLength);

//! \fn				: EventSetVerdict
//! \brief			: установка результата обработки события (будет удалено из очереди, результат -> перехватчик)
//! \return			: STATUS_SUCCESS - успешно / STATUS_UNSUCCESSFUL - не обработано (уже удалено, etc..)
//! \param          : PSET_VERDICT pSetVerdict - указатель на вердикт
NTSTATUS
EventSetVerdict(PSET_VERDICT pSetVerdict);

//! \fn				: YeldEventForClient
//! \brief			: команда на удержание события в очереди на определённый таймаут (возможность обработки данных на 
//! остановленном потоке
//! \return			: STATUS_SUCCESS - успешно / STATUS_NOT_FOUND - событие не найдено, прочие ошибки
//! \param          : PYELD_EVENT pYeldEvent - указатель на данные запроса
//! \param          : ULONG InYeldSize - размер входных данных
//! \param          : PCHAR OutputBuffer - указатель на результирующие данные
//! \param          : DWORD* pOutDataLen - на входе размер выходного буфера, на выходе размер выходных данных
NTSTATUS
YeldEventForClient(PYELD_EVENT pYeldEvent, ULONG InYeldSize, PCHAR OutputBuffer, DWORD* pOutDataLen);

//! \fn				: GetEventSizeForClient
//! \brief			: получение размера блока памяти для первого немаркированного события
//! \return			: 
//! \param          : ULONG AppID - идентификатор клиента
//! \param          : DWORD *pSize - указатель на буфер, куда будет помещён результат (0 - нт события)
void
GetEventSizeForClient(ULONG AppID, DWORD *pSize);

//! \fn				: QueueAdd
//! \brief			: добавить новое событие в очередь для обработки
//! \return			: указатель на блок данных события
//! \param          : PFILTER_EVENT_PARAM pEventParam - данные из перехватчика
//! \param          : PEVENT_LIST pEvList - текущая очередь событий
//! \param          : PFILTER_SUB pFilterSub - данные из фильтра
//! \param          : ULONG YeldTimeout - максимальное время обработки события
//! \param          : DWORD EventFlags - данные, полученные при фильтрации
//! \param          : DWORD dwClientFalgs - данные о клиенте
//! \param          : PEVENT_OBJECT_INFO pEventObject - ассоциированные с событием данные
PQELEM
QueueAdd(PFILTER_EVENT_PARAM pEventParam, PEVENT_LIST pEvList, PFILTER_SUB pFilterSub, ULONG YeldTimeout, 
		 DWORD EventFlags, DWORD dwClientFalgs, PEVENT_OBJECT_INFO pEventObject);

//! \fn				: DoEventRequest
//! \brief			: дополнительные команды для обработки событий (+ групповая обработка событий в очереди)
//! \return			: 
//! \param          : PEVENT_OBJECT_REQUEST pInRequest - запрос на обработку
//! \param          : PVOID pOutRequest - результат обработки
//! \param          : ULONG OutRequestSize - размер выходного буфера
//! \param          : DWORD* pOutDataLen - размер выходных данных
NTSTATUS
DoEventRequest(PEVENT_OBJECT_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, DWORD* pOutDataLen);

//+ ------------------------------------------------------------------------------------------

//! \fn				: ReplyWaiter
//! \brief			: функция удержание текущего потока до окончания обработки события
//! \return			: 
//! \param          : PQSUBCLIENT pSubClient - подклиент, захвативший событие
//! \param          : PEVENT_LIST pEvList - очередь событий
//! \param          : PQELEM pEventElem - событие
//! \param          : PCHAR pUrl - доп. инф.:  имя события
//! \param          : ULONG AppID - доп. инф.: идентификатор клиента
//! \param          : peReplyWaiter peReply - указатель на результат удержания события
VERDICT
ReplyWaiter(PQSUBCLIENT pSubClient, PEVENT_LIST pEvList, PQELEM pEventElem, PCHAR pUrl, ULONG AppID, 
			peReplyWaiter peReply);

//! \fn				: PulseEventToSubClientSafe
//! \brief			: нотификация клиенту о фильтрации события (событие может быть не помещено в очередь)
//! \return			: 
//! \param          : PQSUBCLIENT pSubClient - выбранный подклиент
//! \param          : BOOLEAN bWhistlerUp - тип объекта синхронизации (WhistlerUp / WhistlerChanged)
//! \param          : DWORD DeadLockWatchDog - время на передачу события
//! \param          : ULONG AppID - идентификатор приложения
BOOLEAN
PulseEventToSubClientSafe(PQSUBCLIENT pSubClient, BOOLEAN bWhistlerUp, DWORD DeadLockWatchDog, ULONG AppID);

//! \fn				: BlueScreenReq
//! \brief			: запрос к клиенту в Ring0
//! \return			: результат запроса
//! \param          : PFILTER_EVENT_PARAM pEventParam - данные события
//! \param          : PCHAR pUrl - строковое описание события
VERDICT
BlueScreenReq(PFILTER_EVENT_PARAM pEventParam, PCHAR pUrl);

#ifdef __DBG__
//! \fn				: ShowEventsInfo
//! \brief			: служебная функция, описать события в очереди в DebugOutput
//! \return			: 
//! \param          : PEVENT_LIST pEvList - очередь событий
void
ShowEventsInfo(PEVENT_LIST pEvList);
#endif

//+ ------------------------------------------------------------------------------------------
//! \fn				: ImpersonateThreadForClient
//! \brief			: имперонифицировать текущий поток под поток для указанного события
//! \return			: 
//! \param          : PIMPERSONATE_REQUEST pImperonate - запрос на имперсонализацию
NTSTATUS
ImpersonateThreadForClient(PIMPERSONATE_REQUEST pImperonate);

#endif // _EVQUEUE_H