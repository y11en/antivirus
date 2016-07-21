/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	srvhrch\srvhrch_soapsrv.h
 * \author	Andrew Lashenkov
 * \date	11.01.2005 16:31:00
 * \brief	Server-side soap-related methods need interface to server 
 *				implementation of Srvhrch functionality. This file contain
 *				declaration to let this soap methods know which interface to use
 * 
 */

#ifndef __KLRPT_SRVHRCHSOAPSRV_H__
#define __KLRPT_SRVHRCHSOAPSRV_H__

#include <server/srvhrch/srvhrch_srv.h>

KLCSSRV_DECL void KLSRVH_SetServerSideSrvHierarchyImpl(KLSRVH::SrvHierarchySrv* pSrvHierarchyImpl);

#endif //__KLRPT_SRVHRCHSOAPSRV_H__
