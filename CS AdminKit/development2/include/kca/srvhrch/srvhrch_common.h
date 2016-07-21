/*!
 * (C) 2004 "Kaspersky Lab"
 * 
 * \file	srvhrch/srvhrch_common.h
 * \author	Andrew Lashenkov
 * \date	01.03.2005 14:11:00
 * \brief	ѕолучение свойств соединени€ с мастер-сервером
 * 
 */

#ifndef __KLSRVH_SRVHRCHCOMMON_H__
#define __KLSRVH_SRVHRCHCOMMON_H__

#include <std/par/params.h>
#include <kca/prcp/proxy_attrib.h>
#include <kca/prci/componentid.h>

KLCSKCA_DECL bool KLSRVH_GetMasterSrvConnectionProp(
		KLSTD::CAutoPtr<KLPAR::Params> pMasterSrvData,
		std::wstring& wstrMasterSrvAddress,
		std::vector<int>& vecMasterSrvPorts,
		KLPRCI::ComponentId* pidMasterSrv,
		KLPAR::BinaryValue** ppMasterSrvCert,
		KLPRCP::proxy_auth_t& proxyHost,
		bool& bCompressTraffic);
                                         
#endif //__KLSRVH_SRVHRCHCOMMON_H__
