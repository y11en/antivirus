/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRSS/SettingsStorage.cpp
 * \author Андрей Казачков
 * \date 2002
 * \brief Реализация интерфейсов модуля Product Settings Storage.
 *
 */
#include "std/base/klbase.h"
#include "std/stress/st_prefix.h"
#include "std/trc/trace.h"
//#include "std/par/paramsi.h"
#include "std/par/helpers.h"
#include "common/measurer.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/errors.h"
#include "kca/prss/store.h"
#include "kca/prss/ss_bulkmode.h"
#include "./ss_smartwrite.h"
#include "std/err/errloc_intervals.h"
#include "std/err/error_localized.h"
#include "kca/prss/errlocids.h"
#include <set>
#include <list>
#include <algorithm>
#include "kca/prss/helpersi.h"
#include "kca/prss/errors.h"
#include "./nameconvertor.h"
#include "std/base/klbaseqi_imp.h"
#include "./ss_bulkmodeimp.h"
#include <common/ak_cached_data.h>
#include <common/measurer.h>

#ifndef _WIN32 //windows has own protected storage
#include <std/klcspwd/strg_unix.h>
#endif

KLCSC_DECL void KLSTD_Initialize();
KLCSC_DECL void KLSTD_Deinitialize();
KLCSC_DECL void KLPAR_Initialize();
KLCSC_DECL void KLPAR_Deinitialize();
/*
	BEGIN DEPENDENCIES
	THIS MODULE DEPENDS ON FOLOWING MODULES:
		KLERR
		KLSTD
		KLPAR
	END DEPENDENCIES
*/
#define KLCS_MODULENAME L"KLPRSS"
#ifdef _WIN32
# include <crtdbg.h>
#endif
using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLERR;
using namespace KLPRSS;
static ss_format_t* g_pDefaultSsFormat = NULL;
namespace KLPRSS
{
    bool g_bRoamingMode = false;
    class RoamingCache
    {
    public:
        bool operator()(int, const std::wstring& wstrName);
    };
    typedef KLSTD::PairCache<
        KLSTD::PairCachePropsHash<bool, int, std::wstring, RoamingCache> 
            > set_prod_roaming_t;
    set_prod_roaming_t*  m_pRoamingData;
};

#ifndef _WIN32 //windows has own protected storage
namespace KLPRSS
{
    class CProtectedStorage
        :   public KLCSPWD::ProtectedStorage
    {
    public:
        CProtectedStorage()
            :   m_bCreated(false)
            ,   m_wstrPath( KLSTD_StGetDefDirW() + 
                                KLPRSS::c_szwProtectedStgFileName + 
                                KLPRSS::c_szwFileExt_Data)
        {
            ;
        };
        virtual ~CProtectedStorage()
        {
            ;
        };
        int PutData(const char* szKey, const void* pData, size_t nData)
        {
            int nResult = 0;
        KL_TMEASURE_BEGIN(L"CProtectedStorage::PutData", 3)
            KLERR_TRY
                KLSTD_CHK(szKey, szKey && szKey[0]);
                KLSTD_CHKINPTR(pData);
                KLSTD_CHK(nData, nData);

                KLSTD_TRACE1(3, L"CProtectedStorage::PutData(%hs)\n", szKey);
               
                KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSettingsStorage;
                KLPRSS_CreateSettingsStorageDirect(
                            m_wstrPath, 
                            KLSTD::CF_OPEN_ALWAYS,
                            KLSTD::AF_READ|KLSTD::AF_WRITE,
                            &pSettingsStorage);
                if(!m_bCreated)
                {
                    KLPRSS_MAYEXIST(pSettingsStorage->CreateSection(
                                KLPRSS::c_szwProtectedStgProduct,
                                L"",
                                L""));
                    KLPRSS_MAYEXIST(pSettingsStorage->CreateSection(
                                KLPRSS::c_szwProtectedStgProduct,
                                KLPRSS::c_szwProtectedStgVersion,
                                L""));
                    KLPRSS_MAYEXIST(pSettingsStorage->CreateSection(
                                KLPRSS::c_szwProtectedStgProduct,
                                KLPRSS::c_szwProtectedStgVersion,
                                KLPRSS::c_szwProtectedStgSection));
                    m_bCreated = true; //Yes its not thread safe but extra CreateSection call isn't a problem 
                };
                KLPAR::ParamsPtr pSection;
                KLSTD_A2CW2 wstrKey(szKey);
                KLSTD::CAutoPtr<KLPAR::BinaryValue> p_binVal;
                KLPAR::CreateValue(binary_wrapper_t(pData, nData), &p_binVal);
                KLPAR::param_entry_t par[]=
                {
                    KLPAR::param_entry_t(wstrKey, p_binVal)
                };
                KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pSection);
                pSettingsStorage->Replace(
                                KLPRSS::c_szwProtectedStgProduct,
                                KLPRSS::c_szwProtectedStgVersion,
                                KLPRSS::c_szwProtectedStgSection,
                                pSection);
            KLERR_CATCH(pError)
                KLERR_SAY_FAILURE(1, pError);
                nResult = -pError->GetId();
                if(!nResult)
                    nResult = -1;
            KLERR_ENDTRY
        KL_TMEASURE_END()
            return nResult;
        };
        int GetData(const char* szKey, void*& pData, size_t& nData)
        {
            int nResult = 0;
            void* pRstData = NULL;            
            KLERR_TRY
                KLSTD_CHK(szKey, szKey && szKey[0]);
                KLSTD_CHK(pData, !pData);
                KLSTD_CHK(nData, !nData);

                KLSTD_TRACE1(3, L"CProtectedStorage::GetData(%hs)\n", szKey);
                
                KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSettingsStorage;
                KLPRSS_CreateSettingsStorageDirect(
                            m_wstrPath, 
                            KLSTD::CF_OPEN_EXISTING,
                            KLSTD::AF_READ,
                            &pSettingsStorage);
                KLPAR::ParamsPtr pSection;
                pSettingsStorage->Read(
                                KLPRSS::c_szwProtectedStgProduct,
                                KLPRSS::c_szwProtectedStgVersion,
                                KLPRSS::c_szwProtectedStgSection,
                                &pSection);
                KLSTD::CAutoPtr<KLPAR::BinaryValue> p_binVal;
                KLPAR::GetValue(pSection, KLSTD_A2CW2(szKey), &p_binVal);
                const size_t nRstData = p_binVal->GetSize();
                pRstData = malloc(nRstData);
                KLSTD_CHKMEM(pRstData);
                memcpy(pRstData, p_binVal->GetValue(), nRstData);
                pData = pRstData;
                nData = nRstData;
                pRstData = NULL;
            KLERR_CATCH(pError)
                KLERR_SAY_FAILURE(1, pError);
                nResult = -pError->GetId();
                if(!nResult)
                    nResult = -1;
            KLERR_ENDTRY
            if(pRstData)
            {
                free(pRstData);
                pRstData = NULL;
            };
            return nResult;
        };
    protected:
        std::wstring    m_wstrPath;
        volatile bool   m_bCreated;
    };

    CProtectedStorage* g_pProtectedStorage = NULL;
};
#endif

namespace KLPRSS
{
    void InitCache();
    void DeinitCache();
};

IMPLEMENT_MODULE_INIT_DEINIT(KLPRSS)

IMPLEMENT_MODULE_LOCK(KLPRSS)

IMPLEMENT_MODULE_INIT_BEGIN2( KLCSKCA_DECL, KLPRSS)
KL_TMEASURE_BEGIN(L"KLPRSS_Init", 1)
	CALL_MODULE_INIT(KLSTD);
	CALL_MODULE_INIT(KLPAR);
    ;
    MODULE_LOCK_INIT(KLPRSS);
	KLERR_InitModuleDescriptions(
		KLCS_MODULENAME,
		KLPRSS::c_errorDescriptions,
		KLSTD_COUNTOF(KLPRSS::c_errorDescriptions));
	KLERR_InitModuleLocalizationDefaults(
		KLERR_LOCMOD_PRSS,
		KLPRSS::c_LocErrDescriptions,
		KLSTD_COUNTOF(KLPRSS::c_LocErrDescriptions));
#ifdef _WIN32
    Profiles_Initialize();
#endif
#ifndef _WIN32
    g_pProtectedStorage = new KLPRSS::CProtectedStorage;
    KLCSPWD::SetStorageCallbacks(g_pProtectedStorage);
#endif
    g_pDefaultSsFormat = NULL;
    g_bRoamingMode = false;
    m_pRoamingData = new set_prod_roaming_t;
    m_pRoamingData->Create(RoamingCache(), g_cKLPRSS_Lock);
    KLPRSS::InitCache();
KL_TMEASURE_END()
IMPLEMENT_MODULE_INIT_END()


IMPLEMENT_MODULE_DEINIT_BEGIN2( KLCSKCA_DECL, KLPRSS)    
KL_TMEASURE_BEGIN(L"KLPRSS_Deinit", 1)
    KLPRSS::DeinitCache();
    if(g_pDefaultSsFormat)
    {
        free(g_pDefaultSsFormat);
        g_pDefaultSsFormat = NULL;
    };
#ifndef _WIN32
    KLCSPWD::SetStorageCallbacks(NULL);
    delete g_pProtectedStorage;
    g_pProtectedStorage = NULL;
#endif
#ifdef _WIN32
    Profiles_Deinitialize();
#endif
    KLERR_DeinitModuleLocalizationDefaults(KLERR_LOCMOD_PRSS);
	KLERR_DeinitModuleDescriptions(KLCS_MODULENAME);
    ;
    MODULE_LOCK_DEINIT(KLPRSS);
    ;
	CALL_MODULE_DEINIT(KLPAR);
	CALL_MODULE_DEINIT(KLSTD);
    g_bRoamingMode = false;
    delete m_pRoamingData;
    m_pRoamingData = NULL;
KL_TMEASURE_END()
IMPLEMENT_MODULE_DEINIT_END()

namespace KLPRSS
{
    const wchar_t* NullIfEmpty(const wchar_t* p)
    {
        return (p && p[0]) ? p : NULL;
    };
    
    //!obsolete
    void KLCSKCA_DECL _GetNames(
            KLPRSS::SettingsStorage*    pSs,
            const wchar_t*			    name, 
            const wchar_t*              version,
            std::vector<std::wstring>&	vecNames)
    {
        KLSTD_CHKINPTR(pSs);
        KLSTD::klwstrarr_t names;
        pSs->GetNames(name, version, names.outref());
        vecNames.clear();
        const size_t nSize = names.size();
        for(size_t i = 0; i < names.size(); ++i)
            vecNames.push_back(names[i]);
    };
    ss_format_t* GetDefaultFormat()
    {
        KLCS_MODULE_LOCK(KLPRSS, acs);
        ss_format_t* pResult = NULL;
        if(g_pDefaultSsFormat)
        {
            pResult = (ss_format_t*)malloc(g_pDefaultSsFormat->nSize);
            KLSTD_CHKMEM(pResult);
            memcpy(pResult, g_pDefaultSsFormat, g_pDefaultSsFormat->nSize);
        }
        else
        {
            ss_format_t original(SSF_ID_DEFAULT);
            pResult = (ss_format_t*)malloc(original.nSize);
            KLSTD_CHKMEM(pResult);
            memcpy(pResult, &original, original.nSize);
        };
        return pResult;
    };
    KLCSKCA_DECL void SetDefaultFormat(const ss_format_t* pFormat)
    {
        if(pFormat)
            CheckFormat(pFormat);
        KLCS_MODULE_LOCK(KLPRSS, acs);
        if(g_pDefaultSsFormat)
        {
            free(g_pDefaultSsFormat);
            g_pDefaultSsFormat = NULL;
        };
        if(pFormat)
        {
            g_pDefaultSsFormat = (ss_format_t*)malloc(pFormat->nSize);
            KLSTD_CHKMEM(g_pDefaultSsFormat);
            memcpy(g_pDefaultSsFormat, pFormat, pFormat->nSize);
        };
    };
    void CheckFormat(const ss_format_t* pFormat)
    {        
        KLSTD_ASSERT(pFormat);
        KLSTD_ASSERT(pFormat->nVersion == 1);
        KLSTD_ASSERT(pFormat->nSize == sizeof(ss_format_t));
        KLSTD_CHK(pFormat,  pFormat &&
                            pFormat->nVersion == 1 &&
                            pFormat->nSize == sizeof(ss_format_t));
    }
    ss_format_t* DupFormat(const ss_format_t* pFormat)
    {
        ss_format_t* pResult = NULL;
        if(pFormat)
        {
            CheckFormat(pFormat);
            pResult=(ss_format_t*)malloc(pFormat->nSize);
            KLSTD_CHKMEM(pResult);
            memcpy(pResult, pFormat,  pFormat->nSize);
        }
        else
            pResult = KLPRSS::GetDefaultFormat();
        return pResult;
    }
    
	typedef std::vector<KLSTD::KLAdapt< KLSTD::CAutoPtr< KLPRSS::Storage> > > storages_t;
	bool KLSTD_FASTCALL CheckName(const std::wstring& name, bool bCheckEmpty=true)
	{
		int nName=name.size();
		if(bCheckEmpty && !nName)return false;
		if(nName > c_nMaxNameLength)return false;
		for(int i=0; i < nName; ++i)
		{
			wchar_t ch=name[i];
			if((unsigned)ch < 32 || wcschr(L"\\/:*?\"<>", ch))
				return false;
		};
		return true;
	};

	class CSettingsStorage2
        :   public KLBaseImpl<SsExtendedWrite>
	{
	public:        
        KLSTD_INTERAFCE_MAP_BEGIN(KLPRSS::SettingsStorage)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPRSS::SsExtendedWrite)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLPRSS::SsBulkMode, m_objBulMode)
        KLSTD_INTERAFCE_MAP_END()
		// first has the lowest priority, the last has the highest priority
		CSettingsStorage2(
						const std::wstring&     wstrLocation,
						AVP_dword		        dwCreation,
						AVP_dword		        dwAccess,
                        const std::wstring&     wstrSSUser,
                        const ss_format_t*      pFormat)
        :   m_bUsePolicy(false)
        ,   m_wstrAlternativeSSUser(wstrSSUser)
        ,   m_bSsRuntime(false)
		{
            KLSTD_CHK(wstrLocation, !wstrLocation.empty());
            ;
			m_lTimeout=KLSTD_INFINITE;
			::KLSTD_CreateCriticalSection(&m_pCS);
            if(wstrLocation[0] == c_chwSSP_Marker)
            {
                CAutoPtr<Params> pData;
                KLPRSS_ExpandType(wstrLocation, &pData);
                const std::wstring& wstrType=GetStringValue(pData, c_szwSSP_Type);
                m_vecStorages.reserve(3);
                bool bGeneralCase=false;
                AVP_dword dwFlags=0;
                if(wstrType == c_szwSST_LocalMachine)
                    dwFlags=KLPRSS_SSF_MACHINE|KLPRSS_SSF_POLICY;
                else
                if(wstrType == c_szwSST_CurrentUser)
                    dwFlags=KLPRSS_SSF_USER|KLPRSS_SSF_POLICY;
                else
                if(wstrType == c_szwSST_Smart)
                    dwFlags=KLPRSS_SSF_MACHINE|KLPRSS_SSF_POLICY|KLPRSS_SSF_USER;
                else
                if(wstrType == c_szwSST_HostSS)
                    dwFlags=KLPRSS_SSF_MACHINE;
                else
                {
                    if(c_szwSST_RuntimeSS == wstrType)
                        m_bSsRuntime = true;
                    bGeneralCase=true;
                };                
                bool bHasUser=( (dwFlags & KLPRSS_SSF_USER)!=0 );
                if(!bGeneralCase)
                {
                    m_bUsePolicy=( (dwFlags & KLPRSS_SSF_POLICY) != 0 );
                    if(dwFlags & KLPRSS_SSF_MACHINE)
                    {
                        std::wstring wstrPath;
                        KLPRSS_ResolveValue(c_szwSST_HostSS, pData, wstrPath);
                        CAutoPtr<Storage> pStorage;
				        CreateStorage(
								        wstrPath,
                                        bHasUser? CF_OPEN_EXISTING : dwCreation,
                                        bHasUser? AF_READ : dwAccess,
								        &pStorage,
                                        pFormat);
                        m_vecStorages.push_back(pStorage);
                        if(!bHasUser)
                            m_pStorageW=pStorage;
                    };
                    if(bHasUser)
                    {
                        std::wstring wstrPath;
                        if(m_wstrAlternativeSSUser.empty())
                            KLPRSS_ResolveValue(c_szwSST_CUserSS, pData, wstrPath);
                        else
                            wstrPath=m_wstrAlternativeSSUser;
                        CAutoPtr<Storage> pStorage;
				        CreateStorage(
								        wstrPath,
                                        CF_OPEN_ALWAYS,
                                        dwAccess,
								        &pStorage,
                                        pFormat);
                        m_vecStorages.push_back(pStorage);
                        m_pStorageW=pStorage;
                    }; 
                    /*
                    if(m_bUsePolicy)
                    {
                    KLERR_BEGIN
                        KLSTD_TRACE0(3, L"Attempt to find universal policy...\n");
                        std::wstring wstrPath, wstrType;
                        CAutoPtr<Params> pPathData;
                        wstrType=KLPRSS_MakeTypeP(
                                        c_szwSST_HostSP,
                                        KLPRSS_PRODUCT_CORE,
                                        KLPRSS_VERSION_INDEPENDENT);
                        KLPRSS_ExpandType(
                                        wstrType,
                                        &pPathData);
                        KLPRSS_ResolveValue(
                                        c_szwSST_HostSP,
                                        pPathData,
                                        wstrPath);
                        CAutoPtr<Storage> pStorage;
				        CreateStorage(
								        wstrPath,
                                        CF_OPEN_EXISTING,
                                        AF_READ,
								        &pStorage);
                        m_pStorageUP=pStorage;
                        KLSTD_TRACE1(3, L"...Attempt to find universal policy OK ('%ls')\n", wstrPath.c_str());
                    KLERR_ENDT(3)
                    };
                    */
                }
                else
                {
                    std::wstring wstrPath;
                    KLPRSS_ResolveValue(wstrType, pData, wstrPath);
                    m_vecStorages.resize(1);
				    CreateStorage(
								    wstrPath,
								    dwCreation,
								    dwAccess,
								    &m_vecStorages[0].m_T,
                                    pFormat);
                    m_pStorageW=m_vecStorages[0];
                    m_wstrMainPath = wstrPath;
                };
            }
            else
            {
                m_vecStorages.resize(1);
				CreateStorage(
								wstrLocation,
								dwCreation,
								dwAccess,
								&m_vecStorages[0].m_T,
                                pFormat);
                m_pStorageW=m_vecStorages[0];
                m_wstrMainPath = wstrLocation;
            };
            m_objBulMode.Initialize(this, m_pCS, m_pStorageW, &m_lTimeout);
		};
		virtual ~CSettingsStorage2()
		{
		};
        
        void OnSaving(Storage* pStorage)
        {
			SS_OnSaving(pStorage);
        };
        bool ReadPolicy(const wchar_t** path, CAutoPtr<Params>& pMandatory, CAutoPtr<Params>& pDefault)
        {
            bool bResult=false;
            CAutoPtr<Params> pPolDef, pPolMand, pUPolDef, pUPolMand;
            if(m_bUsePolicy && path && path[0] && path[1])
            {
                CAutoPtr<Params> pPolicy;
                CAutoPtr<Storage> pStorage;
            KLERR_TRY
                std::wstring wstrPath = KLPRSS_GetFsPolicyPath(path[0], path[1]);
				CreateStorage(
								wstrPath,
                                CF_OPEN_EXISTING,
                                AF_READ,
								&pStorage);
                pStorage->trans_begin(false, m_lTimeout);
				pStorage->region_load(path, false);
				pStorage->entry_read(&pPolicy);
				pStorage->trans_end();
                KLPRSS_GetMandatoryAndDefault(pPolicy, &pPolMand, &pPolDef);
                bResult=true;
            KLERR_CATCH(pError)
                if(pStorage)
                    pStorage->trans_end(false);
            KLERR_ENDTRY
            };
            if(m_pStorageUP != NULL &&  path && path[0] && path[1] && path[2])
            {
                CAutoPtr<Params> pPolicy;
			    const wchar_t* path_up[]=
                {
				    KLPRSS_PRODUCT_CORE,
				    KLPRSS_VERSION_INDEPENDENT,
				    path[2],
				    NULL
			    };
            KLERR_TRY
                m_pStorageUP->trans_begin(false, m_lTimeout);
				m_pStorageUP->region_load(path_up, false);
				m_pStorageUP->entry_read(&pPolicy);
				m_pStorageUP->trans_end();
                KLPRSS_GetMandatoryAndDefault(pPolicy, &pUPolMand, &pUPolDef);
                bResult=true;
            KLERR_CATCH(pError)
                if(m_pStorageUP)
                    m_pStorageUP->trans_end(false);
            KLERR_ENDTRY
            };
            if(bResult)
            {
                KLPAR_Concatenate(pPolDef, pUPolDef, &pDefault);
                KLPAR_Concatenate(pPolMand, pUPolMand, &pMandatory);
            };
            return bResult;
        };
        virtual void Read(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section,
			KLPAR::Params**     ppParams)
        {
            ReadI(name, version, section, NULL, ppParams);
        };
        virtual void Read2(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section,
            KLPAR::Params*      pExtra,
			KLPAR::Params**     ppParams)
        {
            ReadI(name, version, section, pExtra, ppParams);
        };
#define KLPRSS_RT_START()   \
            bool _bRtStop = false; \
            for(size_t _nRtAttempt = 0; !_bRtStop; ++_nRtAttempt) \
            {   \
            KLERR_TRY
#define KLPRSS_RT_END()   \
            _bRtStop = true;    \
            KLERR_CATCH(pError) \
                KLERR_SAY_FAILURE(1, pError);   \
                if( 0 == _nRtAttempt &&     \
                    m_bSsRuntime && \
                    pError->GetId() == KLSTD::STDE_BADFORMAT )  \
                {   \
                    KLERR_IGNORE(KLPRSS_RemoveSettingsStorage(m_wstrMainPath, m_lTimeout)); \
                }  \
                else    \
                {   \
                    KLERR_RETHROW();    \
                };  \
            KLERR_ENDTRY    \
            };

		void ReadI(
				const std::wstring & name,
				const std::wstring & version,
				const std::wstring & section,
                KLPAR::Params*      pExtra,
				KLPAR::Params**     ppParams)
		{
			KLSTD_CHKOUTPTR(ppParams);
			if(!CheckName(name))
				KLSTD_THROW_BADPARAM(name);
			if(!CheckName(version))
				KLSTD_THROW_BADPARAM(version);
			if(!CheckName(section))
				KLSTD_THROW_BADPARAM(section);
			*ppParams=NULL;
			;			
			CAutoPtr<Params> pResult;
			storages_t::iterator it;
			const wchar_t* path[]={
				name.empty()?NULL: name.c_str(),
				version.empty()?NULL: version.c_str(),
				section.empty()?NULL: section.c_str(),
				NULL
			};
			{
				int nRead=0;
				AutoCriticalSection cs(m_pCS);
                CAutoPtr<Params> pMandatory;
                if(ReadPolicy(path, pMandatory, pResult))
                    ++nRead;
				for(it=m_vecStorages.begin(); it != m_vecStorages.end(); ++it)
				{
					CAutoPtr<Params> pMaster, pSlave;
					Storage* pStorage=it->m_T;
					KLERR_TRY
                    KLPRSS_RT_START()
						pStorage->trans_begin(false, m_lTimeout);
						pStorage->region_load(path, false);
						pStorage->entry_read(&pMaster);
						pStorage->trans_end();
                    KLPRSS_RT_END()
						++nRead;
					KLERR_CATCH(pError)
						pStorage->trans_end(false);
						if(pError->GetId() != KLPRSS::NOT_EXIST)
							KLERR_RETHROW();					
					KLERR_ENDTRY
					if(pMaster)
					{
						pSlave.Attach(pResult.Detach());
						KLPAR_Concatenate(pMaster, pSlave, &pResult);
					};
				};
                if(pExtra)
                {
                    CAutoPtr<Params> pSlave;
                    pSlave.Attach(pResult.Detach());
                    KLPAR_Concatenate(pExtra, pSlave, &pResult);
                    ++nRead;
                };
                if(pMandatory)
                {
                    CAutoPtr<Params> pSlave;
                    pSlave.Attach(pResult.Detach());
                    KLPAR_Concatenate(pMandatory, pSlave, &pResult);
                };
				if(nRead == 0)
					KLERR_MYTHROW1(KLPRSS::NOT_EXIST, section.c_str());
			};
			pResult.CopyTo(ppParams);
		};
		void GetNames(
            const wchar_t*     name, 
            const wchar_t*     version,
			KLSTD::AKWSTRARR&  names)
		{
			const wchar_t* path[]={
				NullIfEmpty(name),
				NullIfEmpty(version),
				NULL
			};
			std::set<std::wstring> setNames;
			{
				storages_t::iterator it;
				AutoCriticalSection cs(m_pCS);
				for(it=m_vecStorages.begin(); it != m_vecStorages.end(); ++it)
				{
					Storage* pStorage=it->m_T;
					std::vector<std::wstring> vecNames;
					KLERR_TRY
                        KLPRSS_RT_START()
						pStorage->trans_begin(false, m_lTimeout);
						pStorage->region_load(path, false);
						pStorage->entry_enumerate(vecNames);
						pStorage->trans_end(true);
                        KLPRSS_RT_END()
					KLERR_CATCH(pError)
						pStorage->trans_end(false);
						KLERR_RETHROW();
					KLERR_ENDTRY
					for(unsigned int j=0; j < vecNames.size(); ++j)
					{
						if(vecNames[j] != KLPRSS_VERSION_INFO)
							setNames.insert(vecNames[j]);
					};
				};
			};
            klwstrarr_t result;
            result.create(setNames.size());
			//names.clear();
			//names.reserve(setNames.size());
			std::set<std::wstring>::iterator itName;
            size_t i = 0;
			for(itName=setNames.begin(); itName != setNames.end(); ++itName, ++i)
            {
                KLSTD::klwstr_t str = (*itName).c_str();
                result.setat(i, str.detach());
            };
            names = result.detach();
				//names.push_back(*itName);
		};
        void WriteSectionUsingPolicy(
                        const std::wstring& wstrProduct,
                        const std::wstring& wstrVersion,
                        const std::wstring& wstrSection,
                        AVP_dword           dwFlags,
                        KLPAR::Params*      pData,
                        KLPAR::Params*      pPolicy)
        {
            WriteSection(
                        wstrProduct,
                        wstrVersion,
                        wstrSection,
                        pData,
                        dwFlags,                        
                        pPolicy);
        };
        
        void WriteSection(
				const std::wstring& name, 
				const std::wstring& version,
				const std::wstring& section,
				KLPAR::Params*      settings,
                AVP_dword           flags,
                KLPAR::Params*      pPolicy = NULL)
        {
			KLSTD_CHKINPTR(settings);
			if(!CheckName(name))
				KLSTD_THROW_BADPARAM(name);
			if(!CheckName(version))
				KLSTD_THROW_BADPARAM(version);
			if(!CheckName(section))
				KLSTD_THROW_BADPARAM(section);
			const wchar_t* path[]={
				name.empty()?NULL: name.c_str(),
				version.empty()?NULL: version.c_str(),
				section.empty()?NULL: section.c_str(),
				NULL
			};
            CAutoPtr<Params> pMandatory, pDefault, pDataToWrite;
            bool bUsingPolicy = false;
            if(pPolicy)
            {
                KLPRSS_GetMandatoryAndDefault(pPolicy, &pMandatory, &pDefault);
                bUsingPolicy=true;
            }
            else
            {
                if(ReadPolicy(path, pMandatory, pDefault))
                    bUsingPolicy=true;
                else
                    pDataToWrite = settings;
            };
			AutoCriticalSection cs(m_pCS);
			KLERR_TRY
                KLPRSS_RT_START()
				m_pStorageW->trans_begin(true, m_lTimeout);
				m_pStorageW->region_load(path, true);
                if(bUsingPolicy)
                {
                    CAutoPtr<Params> pDst;
                    m_pStorageW->entry_read(&pDst);
                    PrepareForSmartWrite(
                                            pDst,
                                            pDefault,
                                            pMandatory,
                                            settings,
                                            &pDataToWrite);
                };
				m_pStorageW->entry_write(flags, pDataToWrite);
                OnSaving(m_pStorageW);
				m_pStorageW->trans_end(true);
                KLPRSS_RT_END()
			KLERR_CATCH(pError)
				m_pStorageW->trans_end(false);
				KLERR_RETHROW();
			KLERR_ENDTRY
        }
		void Update(
				const wchar_t* name, 
				const wchar_t* version,
				const wchar_t* section,
				KLPAR::Params * settings )
		{
            WriteSection(
                        name, 
                        version,
                        section,
                        settings,
                        CF_OPEN_EXISTING);
		};
		void Add(
				const wchar_t* name, 
				const wchar_t* version,
				const wchar_t* section,
				KLPAR::Params * settings ) 
		{
            WriteSection(
                        name, 
                        version,
                        section,
                        settings,
                        CF_CREATE_NEW);
		};
		void Replace(
				const wchar_t* name, 
				const wchar_t* version,
				const wchar_t* section,
				KLPAR::Params * settings ) 
		{
            WriteSection(
                        name, 
                        version,
                        section,
                        settings,
                        CF_OPEN_ALWAYS);
		};
		void Clear(
				const wchar_t* name, 
				const wchar_t* version,
				const wchar_t* section,
				KLPAR::Params * settings )
		{
            WriteSection(
                        name, 
                        version,
                        section,
                        settings,
                        CF_CREATE_ALWAYS);
		};
		void Delete(
				const wchar_t* name,
				const wchar_t* version,
				const wchar_t* section,
				KLPAR::Params * settings)
		{
            WriteSection(
                        name, 
                        version,
                        section,
                        settings,
                        CF_CLEAR);
		};
		static void modify_path(wchar_t** path, wchar_t* &szEntry)
		{
			szEntry=NULL;
			for(int i=0; path[i]; ++i)
			{
				if(path[i] && !path[i+1]){
					szEntry=path[i];
					path[i]=NULL;
					break;
				};
			};
		};
		void CreateSection(
				const wchar_t* name, 
				const wchar_t* version,
				const wchar_t* section )
		{
			if(!CheckName(name))
				KLSTD_THROW_BADPARAM(name);
			if(!CheckName(version, false))
				KLSTD_THROW_BADPARAM(version);
			if(!CheckName(section, false))
				KLSTD_THROW_BADPARAM(section);

			const wchar_t* path[]={
				NullIfEmpty(name),
				NullIfEmpty(version),
				NullIfEmpty(section),
				NULL
			};
			wchar_t* szEntry=NULL;
			modify_path((wchar_t**)path, szEntry);
			AutoCriticalSection cs(m_pCS);
			KLERR_TRY
                KLPRSS_RT_START()
				m_pStorageW->trans_begin(true, m_lTimeout);
				m_pStorageW->region_load(path, true);
				m_pStorageW->entry_create(szEntry);
                OnSaving(m_pStorageW);
				m_pStorageW->trans_end(true);
                KLPRSS_RT_END()
            KLERR_CATCH(pError)
				m_pStorageW->trans_end(false);
				KLERR_RETHROW();
            KLERR_ENDTRY
		};
		void DeleteSection(
				const wchar_t* name, 
				const wchar_t* version,
				const wchar_t* section )
		{
			if(!CheckName(name))
				KLSTD_THROW_BADPARAM(name);
			if(!CheckName(version, false))
				KLSTD_THROW_BADPARAM(version);
			if(!CheckName(section, false))
				KLSTD_THROW_BADPARAM(section);
			const wchar_t* path[]={
				NullIfEmpty(name),
				NullIfEmpty(version),
				NullIfEmpty(section),
				NULL
			};
			wchar_t* szEntry=NULL;
			modify_path((wchar_t**)path, szEntry);
			AutoCriticalSection cs(m_pCS);
			KLERR_TRY
                KLPRSS_RT_START()
				m_pStorageW->trans_begin(true, m_lTimeout);
				m_pStorageW->region_load(path, true);
				m_pStorageW->entry_destroy(szEntry);
                OnSaving(m_pStorageW);
				m_pStorageW->trans_end(true);
                KLPRSS_RT_END()
			KLERR_CATCH(pError)
				m_pStorageW->trans_end(false);
				KLERR_RETHROW();
			KLERR_ENDTRY
		};
		
		void SetTimeout( long timeout )
		{
			if(timeout < 0 && timeout!=KLSTD_INFINITE)
				KLSTD_THROW_BADPARAM(timeout);
			m_lTimeout=timeout;
		};
		void AttrRead(
            const wchar_t*		name, 
            const wchar_t*		version,
            const wchar_t*		section,
			const wchar_t*		attr,
			Value**					ppValue) const
		{
			KLSTD_CHKOUTPTR(ppValue);
			if(!CheckName(name))
				KLSTD_THROW_BADPARAM(name);
			if(!CheckName(version))
				KLSTD_THROW_BADPARAM(version);
			if(!CheckName(section))
				KLSTD_THROW_BADPARAM(section);
			;
			const wchar_t* path[]={
				NullIfEmpty(name),
				NullIfEmpty(version),
				NullIfEmpty(section),
				NULL
			};
			AutoCriticalSection cs(m_pCS);
			KLERR_TRY
                KLPRSS_RT_START()
				m_pStorageW->trans_begin(false, m_lTimeout);
				m_pStorageW->region_load(path, false);
				m_pStorageW->attr_get(attr, ppValue);
				m_pStorageW->trans_end(true);
                KLPRSS_RT_END()
			KLERR_CATCH(pError)
				m_pStorageW->trans_end(false);
				KLERR_RETHROW();
			KLERR_ENDTRY
		};
        void CreateSettingsStorage(
					const wchar_t*              location,
					AVP_dword                   dwCreationFlags,
					AVP_dword                   dwAccessFlags,
					KLPRSS::SettingsStorage**   ppStorage)
        {
            KLPRSS_CreateSettingsStorageDirect(
                                            location,
                                            dwCreationFlags,
                                            dwAccessFlags,
                                            ppStorage);
        };
	protected:
        CBulkMode                   m_objBulMode;
		long						m_lTimeout;
		CAutoPtr<CriticalSection>	m_pCS;
        storages_t					m_vecStorages;  //Storages to read from
		CAutoPtr<Storage>			m_pStorageW;    //Storage to write to
        CAutoPtr<Storage>			m_pStorageUP;   //Universal policy storage
        bool                        m_bUsePolicy;   //Whether to use product policies
        const std::wstring&         m_wstrAlternativeSSUser;
        std::wstring                m_wstrMainPath;//for simple storages only (m_pStorageW)
        bool                        m_bSsRuntime;
	};
};
using namespace KLPRSS;
KLCSKCA_DECL void KLPRSS_CreateSettingsStorageDirect2(
					const std::wstring&         location,
					AVP_dword                   dwCreationFlags,
					AVP_dword                   dwAccessFlags,
					KLPRSS::SettingsStorage**   ppStorage,
                    const std::wstring&         wstrSSUser,
                    const KLPRSS::ss_format_t*  pFormat)
{
    KLSTD_CHKOUTPTR(ppStorage);
	if(dwCreationFlags & ~(CF_CREATE_ALWAYS | KLPRSS_CF_USE_CACHE))
		KLSTD_THROW_BADPARAM(dwCreationFlags);
	if(dwCreationFlags & CF_CLEAR)
		KLSTD_THROW(STDE_NOTPERM);
    
	*ppStorage=new CSettingsStorage2(
                                location,
                                dwCreationFlags,
                                dwAccessFlags,
                                wstrSSUser,
                                pFormat);
	KLSTD_CHKMEM(*ppStorage);
};
KLCSKCA_DECL void KLPRSS_CreateSettingsStorageDirect(
					const std::wstring&			location,
					AVP_dword					dwCreationFlags,
					AVP_dword					dwAccessFlags,
					KLPRSS::SettingsStorage**	ppStorage,
                    const std::wstring&         wstrSSUser)
{
    KLPRSS_CreateSettingsStorageDirect2(
                                    location,
                                    dwCreationFlags,
                                    dwAccessFlags,
                                    ppStorage,
                                    wstrSSUser,
                                    NULL);
};
#ifdef KLSSINST_EXPORTS
    KLCSKCA_DECL void KLPRSS_CreateSettingsStorage(
					const std::wstring&			location,
					AVP_dword					dwCreationFlags,
					AVP_dword					dwAccessFlags,
					KLPRSS::SettingsStorage**	ppStorage,
                    KLPRSS::ss_server_t*        pServerId)
    {
        KLPRSS_CreateSettingsStorageDirect(
                                    location,
                                    dwCreationFlags,
                                    dwAccessFlags,
                                    ppStorage);
    };
    
    KLCSKCA_DECL void KLPRSS_CreateSettingsStorage2(
					const std::wstring&         location,
					AVP_dword                   dwCreationFlags,
					AVP_dword                   dwAccessFlags,
					KLPRSS::SettingsStorage**   ppStorage,
                    KLPRSS::ss_server_t*        pServerId,
                    const KLPRSS::ss_format_t*  pFormat)
    {
        KLPRSS_CreateSettingsStorageDirect2(
                                    location,
                                    dwCreationFlags,
                                    dwAccessFlags,
                                    ppStorage,
                                    L"",
                                    pFormat);
    };
#endif
    
KLCSKCA_DECL bool KLPRSS_GetRoamingMode()
{
    return g_bRoamingMode;
};
KLCSKCA_DECL void KLPRSS_SetRoamingMode(bool bMode)
{
    g_bRoamingMode = bMode;
};

bool RoamingCache::operator()(int, const std::wstring& wstrName)
{
    KLSTD_CHK(wstrName, !wstrName.empty());
    std::vector<std::wstring> vecNames;
    KLSTD_SplitString(wstrName, L"/", vecNames);
    KLSTD_ASSERT_THROW(2 == vecNames.size());
    bool bResult = false;
    KLERR_BEGIN
        KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSs;
        KLPRSS_OpenSettingsStorageR(
                    KLPRSS_TypeToFileSystemPath(
                        KLPRSS_MakeTypeP(
                            KLPRSS::c_szwSST_HostSPR, 
                            vecNames[0],
                            vecNames[1])),
                    &pSs);
        KLPAR::ParamsPtr pX;
        pSs->Read(  KLPRSS_PRODUCT_CORE, 
                    KLPRSS_VERSION_INDEPENDENT, 
                    KLPRSS_VERSION_INFO, 
                    &pX);
        bResult = true;
    KLERR_ENDT(4)
    return bResult;
};
KLCSKCA_DECL bool KLPRSS_GetRoamingSsExistence(
                        const KLPRSS::product_version_t& product)
{
    KLCS_MODULE_LOCK(KLPRSS, acs);
    if(!m_pRoamingData)
    {
        KLSTD_NOINIT(L"KLPRSS");
    };
    return (*m_pRoamingData)(int(0), product.product + L"/" + product.version);
};
KLCSKCA_DECL void KLPRSS_ReinitRoamingSsExistence()
{    
KLERR_BEGIN
    KLCS_MODULE_LOCK(KLPRSS, acs);
    if(!m_pRoamingData)
    {
        KLSTD_NOINIT(L"KLPRSS");
    };
    m_pRoamingData->ResetCache();
KLERR_ENDT(1)
};
KLCSKCA_DECL void KLPRSS_ReinitRoamingSsExistenceForProduct(
                        const KLPRSS::product_version_t& product)
{
    KLCS_MODULE_LOCK(KLPRSS, acs);
    if(!m_pRoamingData)
    {
        KLSTD_NOINIT(L"KLPRSS");
    };
    m_pRoamingData->ResetCache(product.product + L"/" + product.version);
};
KLCSKCA_DECL std::wstring KLPRSS_GetFsPolicyPath(
                    const std::wstring& wstrProduct, 
                    const std::wstring& wstrVersion)
{
    std::wstring wstrResult;
    if( KLPRSS_GetRoamingMode() && 
        KLPRSS_GetRoamingSsExistence(
            KLPRSS::product_version_t(wstrProduct, wstrVersion)) )
    {
        wstrResult = KLPRSS_TypeToFileSystemPath(
                        KLPRSS_MakeTypeP(
                            c_szwSST_HostSPR, 
                            wstrProduct, 
                            wstrVersion));
    }
    else
    {
        wstrResult = KLPRSS_TypeToFileSystemPath(
                        KLPRSS_MakeTypeP(
                            c_szwSST_HostSP, 
                            wstrProduct, 
                            wstrVersion));
    };
    return wstrResult;
};

/*!
  \brief	Converts version from a.b.c[.d] into 64-bit number (d is considered to be 0 if absent)

  \param	szwVersion  version as a string
  \return	AVP_qword version as a number
*/
KLCSKCA_DECL AVP_qword KLPRSS_ConvertProductVersion(const wchar_t* szwVersion)
{
    std::vector<std::wstring> vecComponents;
    vecComponents.reserve(4);
    KLSTD_SplitString(KLSTD::FixNullString(szwVersion), L".", vecComponents);
    size_t nComponents = std::min(vecComponents.size(), size_t(4));
    if(3 == nComponents)
    {
        vecComponents.push_back(L"0");
        ++nComponents;
    };
    AVP_qword lValue = 0;
    for(size_t i = 0; i < nComponents; ++i)
    {
        wchar_t* pEndPtr = NULL;
        const size_t nOffset = nComponents - i - 1;
        AVP_qword lComponent = wcstoul(vecComponents[nOffset].c_str(), &pEndPtr, 10);
        lValue |= lComponent << (i*16);
    };
    return lValue;
}
