/*!
 * (C) 2004 "Kaspersky Lab"
 * 
 * \file	srvstgs/admsrvprop_client.h
 * \author Andrew Lashenkov
 * \date	01.07.2005 11:08:00
 * \brief	Этот файл должны подключать клинеты модуля KLSRVS
 * 
 */

#ifndef __KLSRVP_ADMSRVSTNGS_CLIENT_H__
#define __KLSRVP_ADMSRVSTNGS_CLIENT_H__

#include <srvp/srvstgs/admsrvstngs.h>

KLCSSRVP_DECL void KLSRVS_CreateAdmServerSettingsProxy(
                        const wchar_t*  szwAddress,
                        KLSRVS::AdmServerSettings** pp,
                        const unsigned short* pPorts = NULL,
                        bool bUseSsl = true);
                                         
#endif //__KLSRVP_ADMSRVSTNGS_CLIENT_H__