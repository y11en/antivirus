/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	helpers.cpp
 * \author	Andrew Kazachkov
 * \date	21.10.2002 18:01:08
 * \brief	
 * 
 */


#include "std/err/klerrors.h"
#include "std/conv/klconv.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/errors.h"
#include "kca/prss/store.h"
#include "kca/prci/simple_params.h"
#include "kca/prss/helpersi.h"
#include "./ss_bulkmodeimp.h"
#include <sstream>

#include <std/trc/trace.h>
#include "common/measurer.h"

#define KLCS_MODULENAME L"KLPRSS"

using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLERR;
using namespace KLPRSS;

namespace KLPRSS
{

//////////////////////////////////////////////////////////////
	;
	KLCSKCA_DECL bool RemoveInstance(
			const wstring&	wstrLocation,
			const wstring&	wstrProduct,
			const wstring&	wstrVersion,
			const wstring&	wstrComponent,
			const wstring&	wstrInstanceName,
            KLPRSS::SettingsStorage*    pBase)
	{
		CAutoPtr<SettingsStorage> pStorage;
        if(pBase)
            pBase->CreateSettingsStorage(wstrLocation, CF_OPEN_ALWAYS, AF_READ|AF_WRITE, &pStorage);
        else
		    KLPRSS_OpenSettingsStorageRW(wstrLocation, &pStorage);
		CAutoPtr<Params> pParams;
		KLPAR_CreateParams(&pParams);
		const wchar_t* path[]=
		{
			wstrComponent.c_str(),
			KLPRSS_VAL_INSTANCES,			
			NULL
		};
		CAutoPtr<ParamsValue> pParamsValue;
		KLPAR::CreateValue(NULL, &pParamsValue);
		KLPAR_AddValue(pParams, path, wstrInstanceName, pParamsValue);
		KLERR_TRY
			pStorage->Delete(wstrProduct, wstrVersion, KLPRSS_SECTION_COMMON_SETTINGS, pParams);
		KLERR_CATCH(pError)
			if(pError->GetId()!=KLPRSS::NOT_EXIST)
				KLERR_RETHROW();
			return false;
		KLERR_ENDTRY;
		return true;
	};
};

using namespace KLPRSS;
using namespace KLPRCI;

static void RegisterComponent(
					const std::wstring&				wstrProductName,
					const std::wstring&				wstrProductVersion,
					const std::wstring&				wstrComponentName,
					KLPAR::Params*					pParams,
					long							lTimeout)
{
	CAutoPtr<Params> pRootParams;
	KLPAR_CreateParams(&pRootParams);	

	CAutoPtr<ParamsValue> pParamsValue;
	KLPAR::CreateValue(pParams, &pParamsValue);
	pRootParams->AddValue(wstrComponentName, pParamsValue);

	std::wstring wstrSS=KLPRSS_GetSettingsStorageLocation(SS_PRODINFO);
	CAutoPtr<SettingsStorage> pStorage;
	::KLPRSS_OpenSettingsStorageRW(wstrSS, &pStorage);
	pStorage->SetTimeout(lTimeout);

	KLPRSS_MAYEXIST(pStorage->CreateSection(wstrProductName, L"", L""));
	KLPRSS_MAYEXIST(pStorage->CreateSection(wstrProductName, wstrProductVersion, L""));
	KLPRSS_MAYEXIST(pStorage->CreateSection(wstrProductName, wstrProductVersion, KLPRSS_SECTION_COMMON_SETTINGS));

	KLERR_TRY
		pStorage->Replace(
				wstrProductName,
				wstrProductVersion,
				KLPRSS_SECTION_COMMON_SETTINGS,
				pRootParams);
	KLERR_CATCH(pError)
		if(
				pError->GetId()==KLPRSS::NOT_EXIST ||
				pError->GetId()==KLSTD::STDE_NOENT ||
				pError->GetId()==STDE_NOTFOUND)
		{
			KLERR_MYTHROW0(KLPRSS::ERR_BADINSTALL);
		};
		KLERR_RETHROW();
	KLERR_ENDTRY
};

static void UnregisterComponent(
					const std::wstring&				wstrProductName,
					const std::wstring&				wstrProductVersion,
					const std::wstring&				wstrComponentName,
					long							lTimeout)
{
	std::wstring wstrSS=KLPRSS_GetSettingsStorageLocation(SS_PRODINFO);
	CAutoPtr<SettingsStorage> pStorage;
	::KLPRSS_OpenSettingsStorageRW(wstrSS, &pStorage);
	pStorage->SetTimeout(lTimeout);

	CAutoPtr<ParamsValue> pParamsValue;
	KLPAR::CreateValue(NULL, &pParamsValue);
	CAutoPtr<Params> pRootParams;
	KLPAR_CreateParams(&pRootParams);	
	pRootParams->AddValue(wstrComponentName, pParamsValue);
	KLERR_TRY
		pStorage->Delete(
				wstrProductName,
				wstrProductVersion,
				KLPRSS_SECTION_COMMON_SETTINGS,
				pRootParams);
	KLERR_CATCH(pError)
		if(
				pError->GetId()==KLPAR::NOT_EXIST ||
				pError->GetId()==KLPRSS::NOT_EXIST ||
				pError->GetId()==KLSTD::STDE_NOENT ||
				pError->GetId()==STDE_NOTFOUND)
		{
			KLERR_MYTHROW1(KLPRSS::ERR_NOTREGISTERED, wstrComponentName.c_str());
		};
		KLERR_RETHROW();
	KLERR_ENDTRY
};

static void GetComponentInfo(
                    KLPRSS::SettingsStorage*    pStorage,
					const std::wstring&		    wstrProductName,
					const std::wstring&		    wstrProductVersion,
					const std::wstring&		    wstrComponentName,
					KLPAR::Params**			    ppParams)
{
	CAutoPtr<Params> pRootParams, pParams;
	KLERR_TRY
		pStorage->Read(
				wstrProductName,
				wstrProductVersion,
				KLPRSS_SECTION_COMMON_SETTINGS,
				&pRootParams);
	KLERR_CATCH(pError)
		if(pError->GetId()==KLPRSS::NOT_EXIST)
			KLERR_MYTHROW0(KLPRSS::ERR_BADINSTALL);
		KLERR_RETHROW();
	KLERR_ENDTRY
	;
	KLERR_TRY
		CAutoPtr<ParamsValue> pParamsValue;
		GetValue(pRootParams, wstrComponentName,  &pParamsValue);
		pParams=pParamsValue->GetValue();
		if(!pParams)
			KLSTD_THROW(STDE_NOTFOUND);
		pParams.CopyTo(ppParams);
	KLERR_CATCH(pError)
		if(pError->GetId()==KLPAR::NOT_EXIST)
        {
            KLERR_MYTHROW1(KLPRSS::ERR_NOTREGISTERED, wstrComponentName.c_str());
        };
		if(
				pError->GetId()==KLPRSS::NOT_EXIST ||
				pError->GetId()==KLSTD::STDE_NOENT ||
				pError->GetId()==STDE_NOTFOUND)
		{
			KLERR_MYTHROW0(KLPRSS::ERR_BADINSTALL);
		};
		KLERR_RETHROW();
	KLERR_ENDTRY
}

static void GetComponentInfo(
					const std::wstring&		wstrProductName,
					const std::wstring&		wstrProductVersion,
					const std::wstring&		wstrComponentName,
					KLPAR::Params**			ppParams,
					long					lTimeout)
{
	std::wstring wstrSS=KLPRSS_GetSettingsStorageLocation(SS_PRODINFO);
	CAutoPtr<SettingsStorage> pStorage;
	::KLPRSS_OpenSettingsStorageR(wstrSS, &pStorage);
	pStorage->SetTimeout(lTimeout);
    GetComponentInfo(
                    pStorage,
                    wstrProductName,
                    wstrProductVersion,
                    wstrComponentName,
                    ppParams);
};


KLCSKCA_DECL void KLPRSS_GetInstalledProducts2(
			SettingsStorage*						pStorage,
			std::vector<KLPRSS::product_version_t>&	vecProducts)
{
    KLSTD_CHKINPTR(pStorage);
    vecProducts.clear();
    ;
    KLSTD::CAutoPtr<KLPRSS::SsBulkMode> pSsBulkMode;
    if( pStorage->QueryInterface(
                            KLSTD_IIDOF(KLPRSS::SsBulkMode),
                            (void**)&pSsBulkMode) && pSsBulkMode)
    {
        sections_t secs;
        pSsBulkMode->EnumAllWSections(secs);
        KLSTD::CAutoPtr<KLPAR::ArrayValue> pArray;
        pSsBulkMode->ReadSections(secs, &pArray);
        KLSTD_ASSERT_THROW(pArray);
        KLSTD_ASSERT_THROW(secs.m_vecNames.size() == (unsigned)pArray->GetSize());
        const size_t nSize = secs.m_vecNames.size();
        vecProducts.reserve(nSize);
        for(size_t i = 0; i < nSize; ++i)
        {
        KLERR_BEGIN
            const std::wstring& wstrSection = 
                secs.m_vecSections[secs.m_vecNames[i].m_nSection];
            if(wstrSection != KLPRSS_SECTION_COMMON_SETTINGS)
                continue;
            KLSTD::CAutoPtr<KLPAR::ParamsValue> pValue;
            pArray->GetAt(i, (KLPAR::Value**)&pValue);
            if(!pValue || pValue->GetType() != KLPAR::Value::PARAMS_T)
                continue;
            KLPAR::Params* pSection = pValue->GetValue();
            if(!pSection)
                continue;
            CAutoPtr<Params> pProduct;
            if(pSection->DoesExist(KLPRSS_COMPONENT_PRODUCT))
                pProduct = GetParamsValue(pSection, KLPRSS_COMPONENT_PRODUCT);
            if( !pProduct 
                ||
                (   pProduct && pProduct->DoesExist(KLPRSS_VAL_LOCAL_ONLY) &&
                    GetBoolValue(pProduct, KLPRSS_VAL_LOCAL_ONLY)
                )
            )
            {
                continue;
            };
            const std::wstring& wstrProduct = 
                secs.m_vecProducts[secs.m_vecNames[i].m_nProduct];
            const std::wstring& wstrVersion = 
                secs.m_vecVersions[secs.m_vecNames[i].m_nVersion];
            vecProducts.push_back(KLPRSS::product_version_t(wstrProduct, wstrVersion));
        KLERR_ENDT(1)
        };
    }
    else
    {
	    ParamsNames products;
	    pStorage->GetNames(
			    L"",
			    L"",
			    products);
        ;
        vecProducts.reserve(2*products.size());
        ;
	    for(unsigned int i=0; i < products.size(); ++i)
	    {
		    KLERR_BEGIN
			    ParamsNames versions;
			    pStorage->GetNames(
						    products[i],
						    L"",
						    versions);
			    for(unsigned int j=0; j < versions.size(); ++j)
			    {
                    CAutoPtr<KLPAR::Params> pProduct;
                    GetComponentInfo(
                            pStorage,
                            products[i],
                            versions[j],
                            KLPRSS_COMPONENT_PRODUCT,
                            &pProduct);
                    ;
                    if( pProduct->DoesExist(KLPRSS_VAL_LOCAL_ONLY) &&
                        GetBoolValue(pProduct, KLPRSS_VAL_LOCAL_ONLY))
                    {
                        continue;
                    };
                    ;
				    product_version_t data;
				    data.product=products[i];
				    data.version=versions[j];
				    vecProducts.push_back(data);
			    };
		    KLERR_ENDT(3)
	    };
    };
};

KLCSKCA_DECL void KLPRSS_GetInstalledComponents2(
					SettingsStorage*			pStorage,
					const std::wstring&			wstrProductName,
					const std::wstring&			wstrProductVersion,
					std::vector<std::wstring>&	vecComponents)
{
	vecComponents.clear();
	
	CAutoPtr<Params> pRootParams, pParams;
	KLERR_TRY
		pStorage->Read(
				wstrProductName,
				wstrProductVersion,
				KLPRSS_SECTION_COMMON_SETTINGS,
				&pRootParams);
	KLERR_CATCH(pError)
		if(pError->GetId()!=KLPRSS::NOT_EXIST)
			KLERR_RETHROW();
	KLERR_ENDTRY
	if(pRootParams)
		pRootParams->GetNames(vecComponents);
};

KLCSKCA_DECL void KLPRSS_GetInstalledProducts(
			std::vector<KLPRSS::product_version_t>&	vecProducts,
			long									lTimeout)
{
	vecProducts.clear();
	std::wstring wstrSS=KLPRSS_GetSettingsStorageLocation(SS_PRODINFO);
	CAutoPtr<SettingsStorage> pStorage;
	::KLPRSS_OpenSettingsStorageR(wstrSS, &pStorage);
	pStorage->SetTimeout(lTimeout);
	KLPRSS_GetInstalledProducts2(pStorage, vecProducts);
};

KLCSKCA_DECL void KLPRSS_GetInstalledComponents(
					const std::wstring&			wstrProductName,
					const std::wstring&			wstrProductVersion,
					std::vector<std::wstring>&	vecComponents,
					long						lTimeout)
{
	vecComponents.clear();
	std::wstring wstrSS=KLPRSS_GetSettingsStorageLocation(SS_PRODINFO);
	CAutoPtr<SettingsStorage> pStorage;
	::KLPRSS_OpenSettingsStorageR(wstrSS, &pStorage);
	pStorage->SetTimeout(lTimeout);

	KLPRSS_GetInstalledComponents2(
				pStorage,
				wstrProductName,
				wstrProductVersion,
				vecComponents);
};

KLCSKCA_DECL void KLPRSS_RegisterComponent(
					const std::wstring&				wstrProductName,
					const std::wstring&				wstrProductVersion,
					const KLPRSS::ComponentInfo&	ci,
					long							lTimeout)
{
    KLSTD_ASSERT(ci.nStructVersion == KLPRSS_COMPONENET_INFO_VERSION);
	CAutoPtr<Params> pParams;
	CAutoPtr<ArrayValue> pEvents, pTasks, pTasksComplemented;
    KLPAR::CreateStringValueArray(ci.pwstrEvents, &pEvents);
	KLPAR::CreateStringValueArray(ci.pwstrTasks, &pTasks);    
    KLPAR::CreateStringValueArray(ci.pwstrSsDataTasks, &pTasksComplemented);

	param_entry_t pars[]=
	{
		param_entry_t(KLPRSS_VAL_NAME,			ci.wstrComponentName.c_str()),
		param_entry_t(KLPRSS_VAL_VERSION,		ci.wstrVersion.c_str()),
		param_entry_t(KLPRSS_VAL_INSTALLTIME,	time_wrapper_t(ci.tmInstallTime)),
		param_entry_t(KLPRSS_VAL_MODULETYPE,	(long)ci.dwModuleType),
		param_entry_t(KLPRSS_VAL_STARTFLAGS,	(long)ci.dwStartFlags),
		param_entry_t(KLPRSS_VAL_FILENAME,		ci.wstrFileName.c_str()),
		param_entry_t(KLPRSS_VAL_FILEPATH,		ci.wstrFileDirectory.c_str()),
		param_entry_t(KLPRSS_VAL_CUSTOMNAME,	ci.wstrCustomName.c_str()),
		param_entry_t(KLPRSS_VAL_WELLKNOWNNAME,	ci.wstrWellKnown.c_str()),
		param_entry_t(KLPRSS_VAL_EVENTS,		pEvents),
		param_entry_t(KLPRSS_VAL_TASKS,			pTasks),
        param_entry_t(KLPRSS_VAL_TASKS_COMPLEMENTED, pTasksComplemented),
		param_entry_t(KLPRSS_VAL_KILLTIMEOUT,	ci.lKillTimeout),
        param_entry_t(KLPRSS_VAL_PINGTIMEOUT,   ci.lPingTimeout)
	};
    KLPAR::CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pParams);	
	RegisterComponent(
				wstrProductName,
				wstrProductVersion,
				ci.wstrComponentName,
				pParams,
				lTimeout);
};


KLCSKCA_DECL void KLPRSS_UnregisterComponent(
					const std::wstring&		wstrProductName,
					const std::wstring&		wstrProductVersion,
					const std::wstring&		wstrComponentName,
					long					lTimeout)
{
	UnregisterComponent(
		wstrProductName,
		wstrProductVersion,
		wstrComponentName,
		lTimeout);
};

KLCSKCA_DECL void KLPRSS_GetComponentInfo(
					const std::wstring&		wstrProductName,
					const std::wstring&		wstrProductVersion,
					const std::wstring&		wstrComponentName,
					KLPRSS::ComponentInfo&	ci,
					long					lTimeout)
{
    KLSTD_ASSERT(ci.nStructVersion == KLPRSS_COMPONENET_INFO_VERSION);
	CAutoPtr<Params> pParams;
	GetComponentInfo(
				wstrProductName,
				wstrProductVersion,
				wstrComponentName,
				&pParams,
				lTimeout);
	;
	KLERR_TRY
		CAutoPtr<StringValue>	pString;
		CAutoPtr<ArrayValue>	pArray;
		
		ci.wstrComponentName=GetStringValue(pParams, KLPRSS_VAL_NAME);
		ci.wstrVersion=GetStringValue(pParams, KLPRSS_VAL_VERSION);
		ci.tmInstallTime=GetDateTimeValue(pParams, KLPRSS_VAL_INSTALLTIME);
		ci.dwModuleType=GetIntValue(pParams, KLPRSS_VAL_MODULETYPE);
		ci.dwStartFlags=GetIntValue(pParams, KLPRSS_VAL_STARTFLAGS);
		ci.wstrFileName=GetStringValue(pParams, KLPRSS_VAL_FILENAME);
		ci.wstrFileDirectory=GetStringValue(pParams, KLPRSS_VAL_FILEPATH);
		ci.wstrCustomName=GetStringValue(pParams, KLPRSS_VAL_CUSTOMNAME);
		ci.wstrWellKnown=GetStringValue(pParams, KLPRSS_VAL_WELLKNOWNNAME);
		ci.lKillTimeout=GetIntValue(pParams, KLPRSS_VAL_KILLTIMEOUT);        
        ci.lPingTimeout=
                    pParams->DoesExist(KLPRSS_VAL_PINGTIMEOUT)
                        ?GetIntValue(pParams, KLPRSS_VAL_PINGTIMEOUT)
                        :0;		
		if(pParams->GetValueNoThrow(KLPRSS_VAL_TASKS, (Value**)&pArray)){
			KLPAR_CHKTYPE(pArray, ARRAY_T, KLPRSS_VAL_TASKS);
			int nSize=pArray->GetSize();
			ci.pwstrTasks.resize(nSize);
			for(int i=0; i < nSize; ++i){
				pString=NULL;
				pArray->GetAt(i, (Value**)&pString);
				KLPAR_CHKTYPE(pString, STRING_T, L"");
				ci.pwstrTasks[i]=pString->GetValue();
			};
			pArray=NULL;
		}
		else
			ci.pwstrTasks.resize(0);

		if(pParams->GetValueNoThrow(KLPRSS_VAL_EVENTS, (Value**)&pArray)){
			KLPAR_CHKTYPE(pArray, ARRAY_T, KLPRSS_VAL_EVENTS);			
			int nSize=pArray->GetSize();
			ci.pwstrEvents.resize(nSize);
			for(int i=0; i < nSize; ++i){
				pString=NULL;
				pArray->GetAt(i, (Value**)&pString);
				KLPAR_CHKTYPE(pString, STRING_T, L"");
				ci.pwstrEvents[i]=pString->GetValue();
			};
			pArray=NULL;
		}
		else
			ci.pwstrEvents.resize(0);

		if(pParams->GetValueNoThrow(KLPRSS_VAL_TASKS_COMPLEMENTED, (Value**)&pArray)){
			KLPAR_CHKTYPE(pArray, ARRAY_T, KLPRSS_VAL_TASKS_COMPLEMENTED);
			int nSize=pArray->GetSize();
			ci.pwstrSsDataTasks.resize(nSize);
			for(int i=0; i < nSize; ++i){
				pString=NULL;
				pArray->GetAt(i, (Value**)&pString);
				KLPAR_CHKTYPE(pString, STRING_T, L"");
				ci.pwstrSsDataTasks[i]=pString->GetValue();
			};
			pArray=NULL;
		}
		else
			ci.pwstrSsDataTasks.resize(0);
	KLERR_CATCH(pError)
		if(pError->GetId()==KLPAR::NOT_EXIST || pError->GetId()==KLPAR::WRONG_VALUE_TYPE)
			KLERR_MYTHROW1(KLPRSS::ERR_BADREGISTRATION, wstrComponentName.c_str());
		KLERR_RETHROW();
	KLERR_ENDTRY
};

KLCSKCA_DECL void KLPRSS_GetProductInfo(
					const std::wstring&		wstrProductName,
					const std::wstring&		wstrProductVersion,
					KLPAR::Params**			ppRegData,
					long					lTimeout)
{
	KLSTD_CHKOUTPTR(ppRegData);
	GetComponentInfo(
			wstrProductName,
			wstrProductVersion,
			KLPRSS_COMPONENT_PRODUCT,
			ppRegData,
			lTimeout);
};

KLCSKCA_DECL void KLPRSS_GetProductInfo2(
                    KLPRSS::SettingsStorage*    pStorage,
					const std::wstring&		    wstrProductName,
					const std::wstring&		    wstrProductVersion,
					KLPAR::Params**			    ppRegData)
{
	KLSTD_CHKOUTPTR(ppRegData);
	GetComponentInfo(
            pStorage,
			wstrProductName,
			wstrProductVersion,
			KLPRSS_COMPONENT_PRODUCT,
			ppRegData);
};

KLCSKCA_DECL void KLPRSS_RegisterProduct(
					const std::wstring&		wstrProductName,
					const std::wstring&		wstrProductVersion,
					KLPAR::Params*			pRegData,
					long					lTimeout)
{
	KLSTD_CHKINPTR(pRegData);
	RegisterComponent(
			wstrProductName,
			wstrProductVersion,
			KLPRSS_COMPONENT_PRODUCT,
			pRegData,
			lTimeout);
};


KLCSKCA_DECL void KLPRSS_UnregisterProduct(
					const std::wstring&		wstrProductName,
					const std::wstring&		wstrProductVersion,
					long					lTimeout)
{
	UnregisterComponent(
			wstrProductName,
			wstrProductVersion,
			KLPRSS_COMPONENT_PRODUCT,
			lTimeout);
};

static std::wstring MakeStr(const wchar_t* type)
{
    //std::basic_ostringstream<wchar_t> os;
	std::wstring wstrResult;
	wstrResult.reserve(wcslen(type) + 32);
    wchar_t q[]=L"\"", e[]=L"=";
    //os  << c_szwSSP_Marker << c_szwSSP_Type << e << q << type << q << L"; ";
	//return os.str();
	wstrResult += c_szwSSP_Marker;
	wstrResult += c_szwSSP_Type;
	wstrResult += e;
	wstrResult += q;
	wstrResult += type;
	wstrResult += q;
	wstrResult += L"; ";
	return wstrResult;    
};

static const wchar_t q[]=L"\"", e[]=L"=";
static const std::wstring
    g_wstrProdinfo=MakeStr(c_szwSST_ProdinfoSS),
    g_wstrRuntime=MakeStr(c_szwSST_RuntimeSS),
    g_wstrSettingsDirect=MakeStr(c_szwSST_HostSS),
    g_wstrSettingsUser=MakeStr(c_szwSST_CurrentUser),
    g_wstrSettingsHost=MakeStr(c_szwSST_LocalMachine),
    g_wstrSettingsSmart=MakeStr(c_szwSST_Smart);

const std::wstring& KLPRSS_GetSettingsStorageLocationI(
                               KLPRSS::SS_TYPE  nType,
                               SS_OPEN_TYPE     nOpenType)
{
    if(nType == SS_PRODINFO)
        return g_wstrProdinfo;
    else if(nType == SS_RUNTIME)
        return g_wstrRuntime;
    else if(nType == SS_SETTINGS)
    {
        switch(nOpenType)
        {
        case SSOT_DIRECT:
            return g_wstrSettingsDirect;
        case SSOT_CURRENT_USER:
            return g_wstrSettingsUser;
        case SSOT_LOCAL_MACHINE:
        case SSOT_SMART:
            return g_wstrSettingsHost;
        };
    };
    KLSTD_THROW_BADPARAM(nOpenType);
};



KLCSKCA_DECL std::wstring KLPRSS_GetSettingsStorageLocation(
                               KLPRSS::SS_TYPE  nType,
                               SS_OPEN_TYPE     nOpenType)

{
    return KLPRSS_GetSettingsStorageLocationI(nType, nOpenType);
};

KLCSKCA_DECL std::wstring KLPRSS_GetPolicyLocation(
                                const std::wstring&    wstrProduct,
                                const std::wstring&    wstrVersion)
{
    return KLPRSS_MakeTypeP(c_szwSST_HostSP, wstrProduct, wstrVersion);
}

KLCSKCA_DECL std::wstring KLPRSS_GetPrivateSettingsLocation(
                                const std::wstring&    wstrProduct,
                                const std::wstring&    wstrVersion)
{
    return KLPRSS_MakeTypeP(c_szwSST_PrivateSS, wstrProduct, wstrVersion);
}

KLCSKCA_DECL bool KLPRSS_RemoveSettingsStorage(const std::wstring& wstrLocation, long lTimeout)
{
    CAutoPtr<Storage> pStorage;
    const std::wstring& wstrPath=KLPRSS_TypeToFileSystemPath(wstrLocation);
	CreateStorage(
                wstrPath,
                CF_OPEN_EXISTING,
                AF_WRITE,
                &pStorage);
    pStorage->remove(lTimeout);
    return true;
};

KLCSKCA_DECL void KLPRSS_CopySettingsStorage(
                        const std::wstring& wstrExistingSs,
                        const std::wstring& wstrNewSs,
                        bool                bFailIfExist,
                        long                lTimeout)
{
    KLSTD_CHK(wstrExistingSs, !wstrExistingSs.empty() && wstrExistingSs[0] != c_chwSSP_Marker);
    KLSTD_CHK(wstrNewSs, !wstrNewSs.empty() && wstrNewSs[0] != c_chwSSP_Marker);
    KLSTD_CHK(lTimeout, lTimeout == KLSTD_INFINITE || lTimeout >= 0);
    KL_TMEASURE_BEGIN(L"KLPRSS_CopySettingsStorage", 3);
        CAutoPtr<Storage> pSrcStore, pDstStore;
        KLPRSS::CreateStorage(
                    wstrExistingSs,
                    CF_OPEN_EXISTING,
                    AF_READ,
                    &pSrcStore);
        KLPRSS::CreateStorage(
                    wstrNewSs,
                    bFailIfExist ? CF_CREATE_NEW : CF_CREATE_ALWAYS,
                    AF_WRITE,
                    &pDstStore);

        pSrcStore->trans_begin(false, lTimeout, true);
        pDstStore->trans_begin(true, lTimeout, true);
        KLERR_TRY
            KLPRSS::sections_t sects;
            {
                CBulkMode bm;
                bm.Initialize(NULL, NULL, pSrcStore, &lTimeout);            
                bm.EnumAllWSectionsI(sects, false);
            };
            for(unsigned int i=0; i < sects.m_vecNames.size(); ++i)
            {
                section_name_t& sec=sects.m_vecNames[i];
                const wchar_t* path[]=
                {
                    sects.m_vecProducts[sec.m_nProduct].c_str(),
                    sects.m_vecVersions[sec.m_nVersion].c_str(),
                    sects.m_vecSections[sec.m_nSection].c_str(),
                    NULL
                };
                if(sects.m_vecSections[sec.m_nSection] == KLPRSS_VERSION_INFO)
                    continue;
                pSrcStore->region_load(path, false);
                CAutoPtr<Params> pData;
                pSrcStore->entry_read(&pData);
                ;
                {
                    const wchar_t* path1[]=
                    {
                        NULL
                    };
                    pDstStore->region_load(path1, true);
                    pDstStore->entry_create(sects.m_vecProducts[sec.m_nProduct].c_str(), false);
                };
                {
                    const wchar_t* path2[]=
                    {
                        sects.m_vecProducts[sec.m_nProduct].c_str(),
                        NULL
                    };
                    pDstStore->region_load(path2, true);
                    pDstStore->entry_create(sects.m_vecVersions[sec.m_nVersion].c_str(), false);
                };
                {
                    const wchar_t* path3[]=
                    {
                        sects.m_vecProducts[sec.m_nProduct].c_str(),
                        sects.m_vecVersions[sec.m_nVersion].c_str(),
                        NULL
                    };
                    pDstStore->region_load(path3, true);
                    pDstStore->entry_create(sects.m_vecSections[sec.m_nSection].c_str(), false);
                };

                pDstStore->region_load(path, true);
                pDstStore->entry_write(CF_CREATE_ALWAYS, pData);
            };
            pSrcStore->trans_end(true);
            pDstStore->trans_end(true);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(2, pError);
            pSrcStore->trans_end(false);
            pDstStore->trans_end(false);
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
};
