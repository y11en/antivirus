/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	srvhrch\srvhrch_common_impl.cpp
 * \author	Andrew Lashenkov
 * \date	12.01.2005 10:35:00
 * \brief	
 * 
 */

//#include "stdafx.h"

#include <std/base/kldefs.h>
#include <std/conv/klconv.h>
#include <std/err/klerrors.h>
#include <kca/srvhrch/srvhrch_common.h>
#include <server/srvinst/server_constants.h>
#include <kca/prts/prxsmacros.h>
#include <std/par/helpers.h>
#include <common/normaddress.h>
#include <avp/klids.h>
#include <std/klcspwd/klcspwd.h>
#include <std/trc/trace.h>
#include <server/srvinst/srvinst_utils.h>

#define KLCS_MODULENAME L"KLSRVH"

KLCSKCA_DECL bool KLSRVH_GetMasterSrvConnectionProp(
	KLSTD::CAutoPtr<KLPAR::Params> pMasterSrvData,
	std::wstring& wstrMasterSrvAddress,
	std::vector<int>& vecMasterSrvPorts,
	KLPRCI::ComponentId* pidMasterSrv,
	KLPAR::BinaryValue** ppMasterSrvCert,
	KLPRCP::proxy_auth_t& proxyHost,
	bool& bCompressTraffic)
{
	if( !KLPAR::GetBoolValue(pMasterSrvData, KLSRV_MASTER_SRV_USE) )
	{
		return false;
	}

	bCompressTraffic = false;

	wstrMasterSrvAddress = KLPAR::GetStringValue(pMasterSrvData, KLSRV_MASTER_SRV_ADDR);

	if( pidMasterSrv )
	{
		*pidMasterSrv = KLPRCI::ComponentId(
			KLCS_PRODUCT_ADMSERVER,
			KLCS_VERSION_ADMSERVER,
			KLCS_COMPONENT_SERVER,
			KLCS_WELLKNOWN_SERVER);
		if( pMasterSrvData->DoesExist(KLSRV_MASTER_SRV_VER) )
		{
			pidMasterSrv->version = KLPAR::GetStringValue(pMasterSrvData, KLSRV_MASTER_SRV_VER);
		}
		if( pMasterSrvData->DoesExist(KLSRV_MASTER_SRV_INST_ID) )
		{
			pidMasterSrv->instanceId = KLPAR::GetStringValue(pMasterSrvData, KLSRV_MASTER_SRV_INST_ID);
		}
	}

	KLSTD::CAutoPtr<KLPAR::BinaryValue> pMasterSrvCert;
	pMasterSrvCert = (KLPAR::BinaryValue*)
		pMasterSrvData->GetValue2(KLSRV_MASTER_SRV_SSL_CERT, true);
	*ppMasterSrvCert = pMasterSrvCert.Detach();
	
	GET_ARRAY_PARAMS_VALUE_NO_THROW(pMasterSrvData, KLSRV_MASTER_SRV_PORTS, IntValue, vecMasterSrvPorts);
	vecMasterSrvPorts.push_back(0);

	bCompressTraffic = true;
	GET_PARAMS_VALUE_NO_THROW(pMasterSrvData, L"KLSRV_MASTER_SRV_USE_COMPRESSION", BoolValue, BOOL_T, bCompressTraffic);

	if( pMasterSrvData->DoesExist(KLSRV_MASTER_SRV_PROXYHOST_USE) &&
		KLPAR::GetBoolValue(pMasterSrvData, KLSRV_MASTER_SRV_PROXYHOST_USE))
	{
	KLERR_TRY
		proxyHost.location = KLADDR::NormalizeAddress(
						KLPAR::GetStringValue(
							pMasterSrvData,
							KLSRV_MASTER_SRV_PROXYHOST_LOCATION).c_str());
		if( pMasterSrvData->DoesExist(KLSRV_MASTER_SRV_PROXYHOST_LOGIN) )
		{
			proxyHost.authUserName = KLPAR::GetStringValue(pMasterSrvData, KLSRV_MASTER_SRV_PROXYHOST_LOGIN);
			if( pMasterSrvData->DoesExist(KLSRV_MASTER_SRV_PROXYHOST_PASSWORD) )
			{
				const KLPAR::Value *accountPasswordValue = NULL;					
				accountPasswordValue = pMasterSrvData->GetValue2(KLSRV_MASTER_SRV_PROXYHOST_PASSWORD, false);
				if( accountPasswordValue != NULL && accountPasswordValue->GetType() == KLPAR::Value::BINARY_T )
				{
					KLPAR::BinaryValue *pBinValue = (KLPAR::BinaryValue *)accountPasswordValue;
					if ( pBinValue->GetSize()!=0 )
					{
						KLSTD_USES_CONVERSION;
						std::wstring wstrPswdTmp;
						void *ppswdData = NULL;
						size_t pswdDataSize = 0;
						int res = KLCSPWD::UnprotectData( pBinValue->GetValue(), pBinValue->GetSize(), ppswdData, pswdDataSize );							
						if( res != 0 )
						{
						    KLSTD_TRACE1(3, L"KLCSPWD::UnprotectData returned 0x%X\n", res);
							KLSTD_THROW(KLSTD::STDE_GENERAL);
						}
						wstrPswdTmp.append( (wchar_t *)ppswdData, pswdDataSize / sizeof( wchar_t ) );
						proxyHost.authUserPswd = KLSTD_W2A2(wstrPswdTmp.c_str());
					}
				}
			};
		};
	KLERR_CATCH(pError)
		KLERR_SAY_FAILURE(1, pError);
		proxyHost.location = L"";
		proxyHost.authUserName = L"";
		proxyHost.authUserPswd = "";
	KLERR_ENDTRY
	};

	return true;
}
