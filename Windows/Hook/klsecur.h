#ifndef _SECURITY_H_
#define _SECURITY_H_

/*!
*		
*		
*		(C) 2001-3 Kaspersky Lab 
*		
*		\file	klsecur.h
*		\brief	security - sid, имперсонализация...
*		
*		\author Sergey Belov, Andrew Sobko
*		\date	12.09.2003 11:18:11
*		
*		
*/		


#include "osspec.h"
#ifdef _HOOK_NT_

//! \fn				: InitImpersonate
//! \brief			: инициализция врутренних данных
//! \return			: TRUE - успешно / FALSE - сбой инициализации
//! \param          : VOID
BOOLEAN
InitImpersonate(VOID);

//! \fn				: Security_CaptureContext
//! \brief			: захватить контекст для последующей работы
//! \return			: STATUS_SUCCESS - контекст захвачен / коды ошибок
//! \param          : PETHREAD OrigTh - поток
//! \param          : SECURITY_CLIENT_CONTEXT *pClientContext - буфер для контекста
NTSTATUS
Security_CaptureContext(PETHREAD OrigTh, SECURITY_CLIENT_CONTEXT *pClientContext);

//! \fn				: Security_ReleaseContext
//! \brief			: освободить security контекст
//! \return			: 
//! \param          : SECURITY_CLIENT_CONTEXT *pClientContext - контекст
void
Security_ReleaseContext(SECURITY_CLIENT_CONTEXT *pClientContext);

//! \fn				: MyImpersonateClient
//! \brief			: имперсонализировать поток
//! \return			: 
//! \param          : IN PETHREAD  ServerThread  OPTIONAL - поток для имперсонализации (если не указан то текущий)
//! \param          : IN PSECURITY_CLIENT_CONTEXT  ClientContext - ssecurity контекст
NTSTATUS
MyImpersonateClient(IN PETHREAD  ServerThread, IN PSECURITY_CLIENT_CONTEXT  ClientContext);

BOOLEAN
SeGetLuid(PLUID pLuid);

//! \fn				: SeGetSID
//! \brief			: получить SID текущего пользователя
//! \return			: 
//! \param          : PSID pSid - указатель на буфер для SID-а
//! \param          : ULONG* pSidLength - на входе длина блока, на выходе - длина SID-а
BOOLEAN
SeGetSID(PSID pSid, ULONG* pSidLength);

/*NTSTATUS
SA_GetData(HANDLE hFile, PVOID psa_info, ULONG* sa_info_len);

NTSTATUS
SA_SetData(HANDLE hFile, PVOID psa_info, ULONG sa_info_len);


NTSTATUS
SA_ClearValidFlag(PWCHAR pwchFile, ULONG namelen);

VOID
SA_PatchSecrDescr(PISECURITY_DESCRIPTOR pidescr);*/

#endif // _HOOK_NT_

#endif // _SECURITY_H_