#ifndef _FILTER_H
#define _FILTER_H

/*!
*		
*		
*		(C) 2001-3 Kaspersky Lab 
*		
*		\file	filter.h
*		\brief	Система фильтраци событий
*		
*		\author Sergey Belov, Andrew Sobko
*		\date	11.09.2003 18:22:12
*		
*		Типичная схема работы - 
*		1. При формировании события выполнить проверку на невидимый поток (IsInvisibleThread) (не обязательно, 
*			проверка так же будет выполнена на шаге 2)
*		2. Проверить, будет ли событие фильтроваться (быстрая проверка IsNeedFilterEvent...)
*		3. Сформировать данные в сруктуру EVENT_TRANSMIT (указать уровень стабильности системы)
*		4. Отдать на фильтрацию (FilterEvent). Если при фильтрации будет выполнен запрос к клиенту то поток будет 
*			остановлен до ответа или окончания таймаута
*		5. Обработать вердикт
*		
*/		

#include "mtypes.h"
#include "osspec.h"
#include "structures.h"
#include "debug.h"

#include "evqueue.h"

#include "../hook/avpgcom.h"
#include "../hook/hookspec.h"

typedef struct _QFLT
{
    LIST_ENTRY		m_List;
	ULONG			m_ClientId;
	ULONG			m_HookID;
	ULONG			m_Mj;
	ULONG			m_Mi;
	ULONG			m_FilterID;
	PROCESSING_TYPE	m_ProcessingType;
	CHAR			m_pProcName[PROCNAMELEN];
	ULONG			m_Flags;
	ULONG			m_Timeout;
	__int64			m_Expiration;
	ULONG			m_ParamsCount;
	BYTE			m_Params[0];		// FILTER_PARAM 
}QFLT, *PQFLT;

//+ ------------------------------------------------------------------------------------------
//! \fn				: GlobalFiltersInit
//! \brief			: инициализация системы фильтрации
//! \return			: 
//! \param          : void
BOOLEAN
GlobalFiltersInit(void);

//! \fn				: GlobalFiltersDone
//! \brief			: освобождение внутренних данных
//! \return			: 
//! \param          : void
void
GlobalFiltersDone(void);

//! \fn				: IsNeedFilterEvent
//! \brief			: проверка на необходимость фильтрации событий (быстрая проверка) для текущего потока
//! \return			: TRUE - передеать данные на фильтрации / FALSE - событие не будет фильтроваться
//! \param          : ULONG HookID - идентификатор перехватчика
//! \param          : ULONG FuncMj - идентификатор функции
//! \param          : ULONG FuncMi - идентификатор подфункции
BOOLEAN __stdcall
IsNeedFilterEvent(ULONG HookID, ULONG FuncMj, ULONG FuncMi);

//! \fn				: IsNeedFilterEventEx
//! \brief			: проверка на необходимость фильтрации событий (быстрая проверка) для указанного потока
//! \return			: TRUE - передеать данные на фильтрации / FALSE - событие не будет фильтроваться
//! \param          : ULONG HookID - идентификатор перехватчика
//! \param          : ULONG FuncMj - идентификатор функции
//! \param          : ULONG FuncMi - идентификатор подфункции
//! \param          : ThreadID - идентификатор потока
BOOLEAN __stdcall
IsNeedFilterEventEx(ULONG HookID, ULONG FuncMj, ULONG FuncMi, HANDLE ThreadID);

#ifdef _HOOK_NT_
//! \fn				: IsNeedFilterEventAsyncChk
//! \brief			: проверка на необходимость фильтрации событий (быстрая проверка) идентификатор потока будет взят из IRP
//! \return			: TRUE - передеать данные на фильтрации / FALSE - событие не будет фильтроваться
//! \param          : ULONG HookID - идентификатор перехватчика
//! \param          : ULONG FuncMj - идентификатор функции
//! \param          : ULONG FuncMi - идентификатор подфункции
//! \param          : PIRP Irp - системный запрос
//! \param			: результат проверки на невидимость потока
BOOLEAN
IsNeedFilterEventAsyncChk(ULONG HookID, ULONG FuncMj, ULONG FuncMi, PIRP Irp, BOOLEAN* pInvisible);
#endif

//! \fn				: FilterEvent
//! \brief			: фильтровать событие
//! \return			: tefVerdict - результат фильтрации (пропустить, запретить, etc...)
//! \param          : PFILTER_EVENT_PARAM pEventParam - данные о событии
//! \param          : PEVENT_OBJ pEventObj - ассоциированные с событием данные (опционально)
//! \param          : ULONG ClientId - точный идентификатор клиента кому предназначено событие (0 - если всем)
tefVerdict
FilterEventImp (
	PFILTER_EVENT_PARAM pEventParam,
	PEVENT_OBJ pEventObj,
	ULONG ClientId
	);

VERDICT
__stdcall
FilterEvent (
	PFILTER_EVENT_PARAM pEventParam,
	PEVENT_OBJ pEventObj
	);

//! \fn				: FilterTransmit
//! \brief			: общая управляющая фильтрами функция (добавить, удалить, etc..)
//! \return			: STATUS_SUCCESS - успешно / коды ошибок
//! \param          : PFILTER_TRANSMIT pFilterTransmitIn - входной запро
//! \param          : PFILTER_TRANSMIT pFilterTransmitOut - результат обработки запроса
//! \param          : ULONG OutputBufferLength - размер выходного буфера
NTSTATUS
FilterTransmit(PFILTER_TRANSMIT pFilterTransmitIn, PFILTER_TRANSMIT pFilterTransmitOut, ULONG OutputBufferLength);

//! \fn				: SaveFilters
//! \brief			: сохранить фильтры для указанного клиента в реестре (востановливаются данные при старте драйвера)
//! \return			: TRUE - успешно / FALSE - сбой при сохранении
//! \param          : PQCLIENT pClient - клиентская структура
BOOLEAN
SaveFilters(PQCLIENT pClient);

NTSTATUS
SaveFiltersForApp (
	ULONG ClientID
	);

//! \fn				: LoadFilters
//! \brief			: восстановить фильтры из реестре для клиента
//! \return			: STATUS_SUCCESS - успешно / коды ошибки
NTSTATUS
LoadFilters();

//! \fn				: _FreeAppFilters
//! \brief			: Удалить фильтры для клиента
//! \return			: 
//! \param          : PQCLIENT pClient - клиентская структура
void
Filters_FreeAppFilters (
	ULONG ClientId
	);

//+ ------------------------------------------------------------------------------------------

//! \fn				: GetSingleParamPtr
//! \brief			: поиск параметра события по идентификатору
//! \return			: NULL - параметр не найдет / указатель на данные
//! \param          : PFILTER_EVENT_PARAM pEventParam - указатель на событие
//! \param          : ULONG ParamID - идентификатор параметра
PSINGLE_PARAM
GetSingleParamPtr(PFILTER_EVENT_PARAM pEventParam, ULONG ParamID);

//! \fn				: InitStartFiltering
//! \brief			: инициировать данные для принятия событий на фильтрацию со стороны
//! \return			: 
//! \param          : PEXT_START_FILTERING pStartFiltering - заполняемые данные
void
InitStartFiltering(PEXT_START_FILTERING pStartFiltering);

//! \fn				: MatchWithPatternW
//! \brief			: сравнить строку по маске (юникод)
//! \return			: TRUE - совпадает / FALSE - не совпадает :)
//! \param          : PWCHAR Pattern - паттерн (к примеру '*string*')
//! \param          : PWCHAR Name - сравниваемая строка
//! \param          : BOOLEAN CaseSensivity - выполнять ли проверку с учётом регистра
__inline BOOLEAN
MatchWithPatternW(PWCHAR Pattern, PWCHAR Name, BOOLEAN CaseSensivity);


ULONG CalcFilterTransmitSize ( 
	PFILTER_TRANSMIT pFltIn
	);

BOOLEAN
CheckEventParams (
	PFILTER_EVENT_PARAM pEvent,
	ULONG EventSize
	);

VOID
DrvRestartHook (
	);

VOID
Di_PauseClient (
	ULONG AppID,
	BOOLEAN bGoSleep
	);

NTSTATUS
Filters_GetCountFor (
	ULONG ClientID,
	PULONG pFiltersCount
	);

ULONG
Filters_CalcParamSize (
	PQFLT pQFlt,
	ULONG* pParamSize
	);

VOID
Filters_FillOutData (
	PFILTER_TRANSMIT pFilterOut,
	PQFLT pQFlt,
	ULONG FilterParamsSize
	);

PQFLT
Filters_Find (
	ULONG ClientId,
	ULONG FilterId
	);

PQFLT
Filters_FindNext (
	ULONG AppID,
	ULONG StartFilterId
	);

NTSTATUS
Filters_ChangeParams (
	ULONG ClientId,
	ULONG FilterId,
	ULONG ParamsCount,
	PFILTER_PARAM pParams
	);

void
Filters_Release (
	PQFLT pQFlt
	);

#endif // _FILTER_H