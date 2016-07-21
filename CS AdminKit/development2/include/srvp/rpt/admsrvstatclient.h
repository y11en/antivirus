/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	srvp/rpt/admsrvstatclient.h
 * \author	Andrew Lashchenkov	
 * \date	23.08.2005 12:31:38
 * \brief	Этот файл должны подключать клинеты модуля KLRPT
 * 
 */

#ifndef __KLRPT_ADMSRVSTATCLIENT_H__
#define __KLRPT_ADMSRVSTATCLIENT_H__

#include <srvp/rpt/admsrvstat.h>

KLCSC_DECL void KLRPT_CreateAdmSrvStatisticsProxy(
                        const wchar_t*  szwAddress,
                        KLRPT::AdmSrvStatistics**           pp,
                        const unsigned short* pPorts = NULL,
                        bool            bUseSsl = true);
                                         
KLCSC_DECL void KLRPT_CreateAdmSrvStatistics2Proxy(
                        const wchar_t*  szwAddress,
                        KLRPT::AdmSrvStatistics2**           pp,
                        const unsigned short* pPorts = NULL,
                        bool            bUseSsl = true);
                                         
#endif //__KLRPT_ADMSRVSTATCLIENT_H__
