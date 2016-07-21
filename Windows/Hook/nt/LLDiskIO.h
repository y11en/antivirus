#ifndef _LLDISK_IO_H
#define _LLDISK_IO_H

/*!
*		
*		
*		(C) 2002-3 Kaspersky Lab 
*		
*		\file	LLDiskIO.h
*		\brief	низкоуровневые операции с накопителями (диски, флоппи, cd, etc..) - конфигурация, чтение, запись, etc...
*		
*		\author Andrew Sobko
*		\date	12.09.2003 11:33:06
*		
*		
*		
*/		

#include "../osspec.h"

//! \fn				: DoDiskRequest
//! \brief			: запрос к кидску
//! \return			: 
//! \param          : PEXTERNAL_DRV_REQUEST pInRequest - данные запроса
//! \param          : PVOID pOutRequest - выходные данные
//! \param          : ULONG OutRequestSize - размер выходного буфера для данных
//! \param          : ULONG *pRetSize - размер полученных при обработке запроса данных
NTSTATUS
DoDiskRequest(PEXTERNAL_DRV_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, ULONG *pRetSize);

#endif