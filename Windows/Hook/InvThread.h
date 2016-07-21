#ifndef __INVISIBLE_THREADS_H
#define __INVISIBLE_THREADS_H

/*!
*		
*		
*		(C) 2001-3 Kaspersky Lab 
*		
*		\file	InvThread.h
*		\brief	Невидимые потоки
*		
*		\author Andrew Sobko
*		\date	12.09.2003 11:04:30
*		
*		1. Проверка потока не невидимость
*		2. регистрация потока как невидимого (разрешается множественная регистрация)
*		3. снятие признака невидимости с потока
*		
*/		




#ifdef _HOOK_NT_
#include <ntifs.h>
#endif

#include "client.h"

//! \fn				: GlobalInvThreadInit
//! \brief			: инициализация внутренних данные
//! \return			: 
VOID
GlobalInvThreadInit();

//! \fn				: GlobalInvThreadDone
//! \brief			: освобождение внутренних данных
//! \return			: 
VOID
GlobalInvThreadDone();
//+ ------------------------------------------------------------------------------------------

//! \fn				: IsInvisibleThread
//! \brief			: Проверка потока не невидимость
//! \return			: TRUE - поток зарегистрированн как невидимый / FALSE - 'чужой' поток
//! \param          : ULONG ThreadID - идентификатор потока
BOOLEAN
IsInvisibleThread(ULONG ThreadID);

//! \fn				: InvisibleReleaseAll
//! \brief			: разрегистрировать все потоки
//! \return			: 
//! \param          : VOID
VOID
InvisibleReleaseAll(VOID);

//! \fn				: AddInvisibleThread
//! \brief			: зарегистрировать поток как невидимый
//! \return			: STATUS_SUCCESS - успешно / STATUS_UNSUCCESSFUL - сбой при регистрации
//! \param          : ULONG ThreadID
NTSTATUS
AddInvisibleThread(ULONG ThreadID);

//! \fn				: DelInvisibleThread
//! \brief			: разрегистрировать поток
//! \return			: 
//! \param          : ULONG ThreadID - идентификатор потока
//! \param          : BOOLEAN bHardFlash - если TRUE то поток разрегистрируется без учета количества регистрации 
//						(принудительная разрегистрация. к примеру, при удалении потока)
NTSTATUS
DelInvisibleThread(ULONG ThreadID, BOOLEAN bHardFlash);

//! \fn				: InvisibleTransmit
//! \brief			: внешнее API для работы с невидимыми потоками (добавление, удаление, etc...)
//! \return			: STATUS_SUCCESS - успешно / коды ошибок
//! \param          : PINVISIBLE_TRANSMIT pInvTransmit - запрос
//! \param          : PINVISIBLE_TRANSMIT pInvTransmitOut - результат обработки
NTSTATUS
InvisibleTransmit(PINVISIBLE_TRANSMIT pInvTransmit, PINVISIBLE_TRANSMIT pInvTransmitOut);


VOID
InvProc_RemoveFromList (
	HANDLE ProcessId
	);

VOID
InvProc_CheckNewChild (
	HANDLE ParentProcessId,
	HANDLE ProcessId
	);


#endif // __INVISIBLE_THREADS_H