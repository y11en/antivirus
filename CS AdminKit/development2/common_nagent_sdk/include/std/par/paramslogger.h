/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	paramslogger.h
 * \author	Nikolay Lebedev, Andrew Kazachkov
 * \date	05.05.2006 18:56:25
 * \brief	
 * 
 */


#ifndef __KL_PARAMSLOGGER_H__
#define __KL_PARAMSLOGGER_H__

#include <std/par/params.h>

KLSTD_NOTHROW KLCSC_DECL void KLPARLOG_LogParams(
                        int nTraceLevel,
                        const KLPAR::Params *pParams) throw();

#endif //__KL_PARAMSLOGGER_H__
