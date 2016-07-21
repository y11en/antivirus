/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sssrv_soapapi.cpp
 * \author	Andrew Kazachkov
 * \date	05.05.2003 13:48:18
 * \brief	
 * 
 */


#include <std/base/kldefs.h>
#include <std/par/paramsi.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prss/helpersi.h>
#include <transport/tr/transport.h>
#include "kca/ss_srv/ss_srv.h"
#include "kca/ss_srv/sssrv_server.h"
#include <kca/prci/componentinstance.h>
#include <common/measurer.h>

#include <map>
#include <transport/avt/accesscheck.h>

//using namespace std;
using namespace KLERR;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;

#define KLCS_MODULENAME L"KLSSS"

using namespace KLSSS;

#define HANDLER_BEGIN	\
		KLERR_TRY   \
            KLSTD_CHKINPTR(wstrIdSSS);  \
            CAutoPtr<SettingsStorageServer> pServer;    \
            GetSsServer(wstrIdSSS, &pServer);


#define HANDLER_END	\
		KLERR_CATCH(pError)	\
			ExceptionForSoap(soap, pError, r.error);	\
		KLERR_ENDTRY;   \
        return SOAP_OK;

SOAP_FMAC1 int SOAP_FMAC2 klsssrv_CloseStub(
                    struct soap*    soap,
                    xsd__wstring    wstrIdSSS,
                    xsd__wstring    wstrProxyId,
                    struct klsssrv_CloseResponse& r)
{
KL_TMEASURE_BEGIN(L"klsssrv_Close", 4);
    HANDLER_BEGIN
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_SSSRV, KLAVT_READ);

        KLSTD_CHKINPTR(wstrProxyId);
        pServer->Close(wstrProxyId);
    HANDLER_END
KL_TMEASURE_END()
};

SOAP_FMAC1 int SOAP_FMAC2 klsssrv_CreateSettingsStorageStub(
                    struct soap*    soap,
                    xsd__wstring    wstrIdSSS,
                    xsd__wstring    wstrLocation,
					xsd__int        nCreationFlags,
					xsd__int        nAccessFlags,
                    param__params   options,
                    struct klsssrv_CreateSettingsStorageResponse &r)
{
KL_TMEASURE_BEGIN(L"klsssrv_CreateSettingsStorage", 4);
    HANDLER_BEGIN
		AVP_dword dwAccess = KLAVT_READ;
		if( (nAccessFlags & AF_WRITE) == AF_WRITE )
		{
			dwAccess |= KLAVT_WRITE;
		}

		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_SSSRV, dwAccess);

        std::wstring wstrConn;
        if(!KLTR_GetTransport()->GetConnectionName(soap, wstrConn))
            KLSTD_THROW(STDE_GENERAL);
        std::wstring wstrProxyId;
        std::wstring wstrUserStorePath;
        /* uncomment this if using SSOT_SMART
        {
            CAutoPtr<KLPRCI::SecContext> pContext;
            KLPRCI_GetClientContext(&pContext);
            KLPRCI::AutoImpersonate ai(pContext);        ;
            wstrUserStorePath=KLPRSS_TypeToFileSystemPath(
                                    KLPRSS_MakeTypeG(c_szwSST_CUserSS));            
        };
        */        
        CAutoPtr<Params> pOptions;
        ParamsFromSoap(options, &pOptions);
        pServer->CreateSettingsStorage(
                            wstrConn,
                            wstrLocation,
                            nCreationFlags,
                            nAccessFlags,
                            wstrProxyId,
                            wstrUserStorePath,
                            pOptions);
        r.wstrProxyId=soap_strdup(soap, wstrProxyId.c_str());
    HANDLER_END
KL_TMEASURE_END()
};

SOAP_FMAC1 int SOAP_FMAC2 klsssrv_ReadStub(
                    struct soap*   soap,
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    param__params  extra,
                    struct klsssrv_ReadResponse& r)
{
KL_TMEASURE_BEGIN(L"klsssrv_Read", 4);
    HANDLER_BEGIN
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_SSSRV, KLAVT_READ);

        KLSTD_CHKINPTR(wstrProxyId);
        KLSTD_CHKINPTR(wstrName);
        KLSTD_CHKINPTR(wstrVersion);
        KLSTD_CHKINPTR(wstrSection);
        CAutoPtr<Params> pData, pExtra;
        ParamsFromSoap(extra, &pExtra);
        pServer->Read(wstrProxyId, wstrName, wstrVersion, wstrSection, pExtra, &pData);
        ParamsForSoap(soap, pData, r.result);
    HANDLER_END
KL_TMEASURE_END()
};

SOAP_FMAC1 int SOAP_FMAC2 klsssrv_WriteStub(
                    struct soap*   soap,
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    xsd__int       nFlags,
                    param__params  data,
                    struct klsssrv_WriteResponse& r)
{
KL_TMEASURE_BEGIN(L"klsssrv_Write", 4);
    HANDLER_BEGIN
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_SSSRV, KLAVT_WRITE);

        KLSTD_CHKINPTR(wstrProxyId);
        KLSTD_CHKINPTR(wstrName);
        KLSTD_CHKINPTR(wstrVersion);
        KLSTD_CHKINPTR(wstrSection);
        CAutoPtr<Params> pData;
        ParamsFromSoap(data, &pData);
        pServer->Write(wstrProxyId, wstrName, wstrVersion, wstrSection, nFlags, pData);
    HANDLER_END
KL_TMEASURE_END()
};

SOAP_FMAC1 int SOAP_FMAC2 klsssrv_CreateSectionStub(
                    struct soap*   soap,
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    struct klsssrv_CreateSectionResponse& r)
{
KL_TMEASURE_BEGIN(L"klsssrv_CreateSection", 4);
    HANDLER_BEGIN
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_SSSRV, KLAVT_WRITE);

        KLSTD_CHKINPTR(wstrProxyId);
        KLSTD_CHKINPTR(wstrName);
        KLSTD_CHKINPTR(wstrVersion);
        KLSTD_CHKINPTR(wstrSection);
        pServer->CreateSection(wstrProxyId, wstrName, wstrVersion, wstrSection);
    HANDLER_END
KL_TMEASURE_END()
};

SOAP_FMAC1 int SOAP_FMAC2 klsssrv_DeleteSectionStub(
                    struct soap*    soap,
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    struct klsssrv_DeleteSectionResponse& r)
{
KL_TMEASURE_BEGIN(L"klsssrv_DeleteSection", 4);
    HANDLER_BEGIN
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_SSSRV, KLAVT_DELETE /*KLAVT_WRITE*/);

        KLSTD_CHKINPTR(wstrProxyId);
        KLSTD_CHKINPTR(wstrName);
        KLSTD_CHKINPTR(wstrVersion);
        KLSTD_CHKINPTR(wstrSection);
        pServer->DeleteSection(wstrProxyId, wstrName, wstrVersion, wstrSection);
    HANDLER_END
KL_TMEASURE_END()
};

SOAP_FMAC1 int SOAP_FMAC2 klsssrv_SetTimeoutStub(
                    struct soap*    soap,
                    xsd__wstring    wstrIdSSS,
                    xsd__wstring    wstrProxyId,
                    xsd__int        lTimeout,
                    struct klsssrv_SetTimeoutResponse& r)
{
KL_TMEASURE_BEGIN(L"klsssrv_SetTimeout", 4);
    HANDLER_BEGIN
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_SSSRV, KLAVT_READ);

        KLSTD_CHKINPTR(wstrProxyId);
        pServer->SetTimeout(wstrProxyId, lTimeout);
    HANDLER_END
KL_TMEASURE_END()
};

SOAP_FMAC1 int SOAP_FMAC2 klsssrv_GetNamesStub(
                    struct soap*   soap,
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    struct klsssrv_GetNamesResponse& r)
{
KL_TMEASURE_BEGIN(L"klsssrv_GetNames", 4);
    HANDLER_BEGIN
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_SSSRV, KLAVT_READ);

        KLSTD_CHKINPTR(wstrProxyId);
        KLSTD_CHKINPTR(wstrName);
        KLSTD_CHKINPTR(wstrVersion);
        std::vector<std::wstring>	vecNames;
        pServer->GetNames(wstrProxyId, wstrName, wstrVersion, vecNames);
        if(vecNames.size())
        {
            r.names.__ptr=(wchar_t**)soap_malloc(soap, sizeof(wchar_t*)*vecNames.size());
            KLSTD_CHKMEM(r.names.__ptr);
            r.names.__size=vecNames.size();
		    for(int i=0; i < r.names.__size; ++i)
			    r.names.__ptr[i]=soap_strdup(soap, vecNames[i].c_str());
        };
    HANDLER_END
KL_TMEASURE_END()
};

SOAP_FMAC1 int SOAP_FMAC2 klsssrv_AttrReadStub(
                    struct soap*   soap,
                    xsd__wstring   wstrIdSSS,
                    xsd__wstring   wstrProxyId,
                    xsd__wstring   wstrName,
                    xsd__wstring   wstrVersion,
                    xsd__wstring   wstrSection,
                    xsd__wstring   wstrAttr,
                    struct klsssrv_AttrReadResponse& r)
{
KL_TMEASURE_BEGIN(L"klsssrv_AttrRead", 4);
    HANDLER_BEGIN
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_SSSRV, KLAVT_READ);

        KLSTD_CHKINPTR(wstrProxyId);
        KLSTD_CHKINPTR(wstrName);
        KLSTD_CHKINPTR(wstrVersion);
        KLSTD_CHKINPTR(wstrSection);
        KLSTD_CHKINPTR(wstrAttr);
        CAutoPtr<Value> pValue;
        pServer->AttrRead(wstrProxyId, wstrName, wstrVersion, wstrSection, wstrAttr, &pValue);
        ValueForSoap(soap, pValue, r.value);
    HANDLER_END
KL_TMEASURE_END()
};
