/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	pol/policiesclient.h
 * \author	Mikhail Karmazine
 * \date	11.04.2003 16:54:38
 * \brief	Этот файл должны подключать клинеты модуля KLPOL
 * 
 */

#ifndef __KLPOL_POLICIESCLIENT_H__
#define __KLPOL_POLICIESCLIENT_H__

#include "policies.h"

KLCSSRVP_DECL void KLPOL_CreatePoliciesProxy(
                        const wchar_t*  szwAddress,
                        KLPOL::Policies**           pp,
                        const unsigned short* pPorts = NULL,
                        bool            bUseSsl = true);

KLCSSRVP_DECL void KLPOL_CreatePoliciesLocalProxy(
                        KLPOL::Policies**       ppPolicies );

#endif //__KLPOL_POLICIESCLIENT_H__