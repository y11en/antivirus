/*!
 * (C) 2004 "Kaspersky Lab"
 * 
 * \file	srvhrch/srvhrch_client.h
 * \author	Mikhail Karmazine
 * \date	11.01.2005 18:29:00
 * \brief	Этот файл должны подключать клинеты модуля KLSRVH
 * 
 */

#ifndef __KLSRVH_SRVHRCHCLIENT_H__
#define __KLSRVH_SRVHRCHCLIENT_H__

#include <srvp/srvhrch/srvhrch_lst.h>

KLCSSRVP_DECL void KLSRVH_CreateChildServersProxy(
                        const wchar_t*  szwAddress,
                        KLSRVH::ChildServers**           pp,
                        const unsigned short* pPorts = NULL,
                        bool            bUseSsl = true);
                                         
#endif //__KLSRVH_SRVHRCHCLIENT_H__