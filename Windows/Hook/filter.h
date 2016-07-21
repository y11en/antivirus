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


#include "osspec.h"
#include "evqueue.h"

#include "hook/avpgcom.h"
#include "hook/hookspec.h"

extern BOOLEAN DrvRequestHook(ULONG HookID);
extern BOOLEAN DrvRequestUnhook(ULONG HookID);

//+ ------------------------------------------------------------------------------------------
//+ признак выгрузки системы
extern BOOLEAN UnloadInProgress;

//+ ------------------------------------------------------------------------------------------
//+ внутренние счётчики для системы фильтрации
extern ULONG gFilterEventEntersCount;

//! \fn				: GlobalFiltersInit
//! \brief			: инициализация системы фильтрации
//! \return			: 
//! \param          : void
void
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
//! \param          : ULONG ThreadID - идентификатор потока
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
IsNeedFilterEventAsyncChk(ULONG HookID, ULONG FuncMj, ULONG FuncMi,PIRP Irp, BOOLEAN* pInvisible);
#endif

#ifdef CHECK_WHAT_AVPM_DO
// служебная функция (для отладки)
//! \fn				: IsNeedFilterEventNoInvCheck
//! \brief			: проверка на необходимость фильтрации событий (быстрая проверка) без проверки на невидимость потока
//! \return			: TRUE - передеать данные на фильтрации / FALSE - событие не будет фильтроваться
//! \param          : ULONG HookID - идентификатор перехватчика
//! \param          : ULONG FuncMj - идентификатор функции
//! \param          : ULONG FuncMi - идентификатор подфункции
BOOLEAN
IsNeedFilterEventNoInvCheck(ULONG HookID, ULONG FuncMj, ULONG FuncMi);
#endif

//! \fn				: FilterEvent
//! \brief			: фильтровать событие
//! \return			: VERDICT - результат фильтрации (пропустить, запретить, etc...)
//! \param          : PFILTER_EVENT_PARAM pEventParam - данные о событии
//! \param          : PEVENT_OBJECT_INFO pEventObject - ассоциированные с событием данные (опционально)
VERDICT __stdcall
FilterEvent(PFILTER_EVENT_PARAM pEventParam, PEVENT_OBJECT_INFO pEventObject);

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
_FreeAppFilters(PQCLIENT pClient);
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

#ifdef _HOOK_NT_
// dispatch filters
	void
	Di_PauseClient(ULONG AppID, BOOLEAN bGoSleep);
#else
	#define Di_PauseClient
#endif

#endif // _FILTER_H