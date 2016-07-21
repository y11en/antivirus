#ifndef _GLOBAL_ENVIRONMENT_H
#define _GLOBAL_ENVIRONMENT_H

/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	glbenvir.h
*		\brief	Инифиализация глобальных переменных для функционирования драйвера
*		
*		\author Andrew Sobko
*		\date	12.09.2003 11:02:21
*		
*		
*		
*/		

#include <ntifs.h>

extern ULONG	gDrvFlags;
// билд операционной системы
extern PSHORT NtBuildNumber;

// указатели на системные модули
extern PVOID BaseOfNtDllDll;
extern PVOID BaseOfNtOsKrnl;
extern PVOID BaseOfHal;

extern int gFSNeedRescan;

// смещение имени в структуре процесса
extern ULONG ProcessNameOffset;

//! \fn				: InitGlobalEnvironment
//! \brief			: проинициализировать данные
//! \return			: TRUE - все данные успешно проинициализированны / FALSE - при сбое инициализации любого параметра
BOOLEAN
InitGlobalEnvironment();


VOID
LoadDriverFlags(VOID);

#endif //_GLOBAL_ENVIRONMENT_H