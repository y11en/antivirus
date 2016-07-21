/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	cmp_helpers.cpp
 * \author	Andrew Kazachkov
 * \date	10.05.2003 12:37:23
 * \brief	
 * 
 */


#include "std/base/klstd.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "soapH.h"
#include <std/base/klbase.h>
#include <std/par/paramsi.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prss/errors.h>
#include <kca/prci/componentinstance.h>
#include <kca/prcp/proxybase.h>
#include <kca/prci/runtimeinfo.h>

#include <kca/prci/cmp_helpers.h>

#define KLCS_MODULENAME L"KLPRCI"


using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;
using namespace std;


using namespace KLPRCI;

static int GetInstancePortI(
                    KLPRSS::SettingsStorage*	pStorages,
                    const ComponentId&          id)
{
	KLSTD_TRACE1(3, L"Getting instance port number for '%ls'...\n", id.instanceId.c_str());
	CAutoPtr<Params>			pParams;
	CAutoPtr<IntValue>			pPortValue;
	pStorages->Read(
		id.productName,
		id.version,
		KLPRSS_SECTION_COMMON_SETTINGS,
		&pParams);
	const wchar_t* path[]=
	{
		id.componentName.c_str(),
		KLPRSS_VAL_INSTANCES,
		id.instanceId.c_str(),
		NULL
	};
	KLERR_TRY
		KLPAR_GetValue(
				pParams,
				path,
				KLPRSS_VAL_INSTANCEPORT,
				(Value**)&pPortValue);
	KLERR_CATCH(pError)
        if(!KLERR_IfIgnore(pError->GetId(), KLPAR::NOT_EXIST, KLPRSS::NOT_EXIST, 0))
			KLERR_RETHROW();				
	KLERR_ENDTRY
	if(pPortValue)
	{
		KLPAR_CHKTYPE(pPortValue, INT_T, KLPRSS_VAL_INSTANCEPORT);
		KLSTD_TRACE1(3, L"...OK getting instance port number for '%ls'\n", id.instanceId.c_str());
		return pPortValue->GetValue();
	};
	return 0;
}


KLCSKCA_DECL int KLPRCI_GetInstancePort(
                const std::wstring&         storage,                    
                const ComponentId&          id,
                KLPRSS::SettingsStorage*	pBase)
{
    CAutoPtr<SettingsStorage>	pStorages;
    if(pBase)
        pBase->CreateSettingsStorage(storage, CF_OPEN_ALWAYS, AF_READ, &pStorages);
    else
        ::KLPRSS_OpenSettingsStorageR(storage, &pStorages);
    pStorages->SetTimeout(KLPRCI_DEFAULT_SS_TIMEOUT);
    return GetInstancePortI(pStorages, id);
}

KLCSKCA_DECL std::wstring KLPRCI_MakeInstance(
                    const KLPRCI::ComponentId&  id,
                    KLPAR::Params*              pInfo)
{
    InstanceListenerType type=ILT_UNKNOWN;
    if(pInfo->DoesExist(KLPRSS_VAL_CONNTYPE))
        type=(InstanceListenerType)GetIntValue(pInfo, KLPRSS_VAL_CONNTYPE);
    int nPort=GetIntValue(pInfo, KLPRSS_VAL_INSTANCEPORT);
	//int nPort=KLPRCI_GetInstancePort(wstrRtLocation,id,pBase);

    if(type == ILT_UNKNOWN)
    {
        type=ILT_RPC;
    };

    switch(type)
    {
    case ILT_SOCKET:
        return KLPRCP_MakeAddress2(L"localhost", nPort);
    case ILT_RPC:
        return KLPRCP_MakeAddress2(L"127.0.0.1", nPort);
    };
	return L"";
}

KLCSKCA_DECL std::wstring KLPRCI_FindInstance(
                        const ComponentId& id,
                        KLPRSS::SettingsStorage*	pBase)
{    
    CAutoPtr<Params> pInfo;
    const std::wstring& wstrRtLocation=
                            KLPRSS_GetSettingsStorageLocation(SS_RUNTIME);
    KLERR_TRY
        KLPRCI_GetRuntimeInfo(wstrRtLocation, id, &pInfo);
    KLERR_CATCH(pError)
        if(KLERR_IfIgnore(pError->GetId(), KLSTD::STDE_NOTFOUND, KLPAR::NOT_EXIST, KLPRSS::NOT_EXIST, 0))
            KLERR_MYTHROW1(KLPRCP::ERR_CANT_CONNECT, id.instanceId.c_str());
        else
        {
            KLERR_RETHROW();
        }
    KLERR_ENDTRY
    KLSTD_ASSERT_THROW(pInfo!=NULL);
    return KLPRCI_MakeInstance(id, pInfo);
};
