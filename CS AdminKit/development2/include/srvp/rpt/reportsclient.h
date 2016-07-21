/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	rpt/rpticiesclient.h
 * \author	Mikhail Karmazine
 * \date	11.06.2003 16:54:38
 * \brief	Этот файл должны подключать клинеты модуля KLRPT
 * 
 */

#ifndef __KLRPT_RPTICIESCLIENT_H__
#define __KLRPT_RPTICIESCLIENT_H__

#include "reports.h"

KLCSC_DECL void KLRPT_CreateReportsProxy(
                        const wchar_t*  szwAddress,
                        KLRPT::Reports**           pp,
                        const unsigned short* pPorts = NULL,
                        bool            bUseSsl = true);
                                         
#endif //__KLRPT_RPTICIESCLIENT_H__
