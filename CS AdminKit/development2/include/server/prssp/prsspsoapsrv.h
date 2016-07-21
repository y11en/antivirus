/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prssp/prsspsoapsrv.h
 * \author	Mikhail Karmazine
 * \date	15.04.2003 12:35:08
 * \brief	Server-side soap-related methods need interface to server 
 *				implementation of PRSSP functionality. This file contains
 *				declaration to let this soap methods know which interface to use
 * 
 */


#ifndef __PRSSP_SOAPSRV_H__
#define __PRSSP_SOAPSRV_H__

#include "ssproxycontrolsrv.h"

void KLPRSSP_SetSSProxyControlSrv( KLPRSSP::SSProxyControlSrv* pSSProxyControlSrv );

#endif //__PRSSP_SOAPSRV_H__