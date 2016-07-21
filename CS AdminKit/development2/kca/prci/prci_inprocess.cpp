/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prci_inprocess.cpp
 * \author	Andrew Kazachkov
 * \date	06.01.2004 13:24:22
 * \brief	
 * 
 */

#include "std/base/klbase.h"
#include "std/sign/chksgn.h"
#include "std/conv/klconv.h"
#include "std/err/klerrors.h"
#include "kca/prss/settingsstorage.h"
#include "std/par/params.h"
#include "kca/prci/componentinstance.h"
#include "./prci_actions.h"
#include "./prci_inprocess.h"
#include "transport/avt/accesscheck.h"

#include "common/kl_dll_port.h"
#include "common/measurer.h"

#include <map>

#ifdef _WIN32
#include <std/win32/klos_win32v40.h>
#include <std/win32/klos_win32_errors.h>
#else
typedef void* HMODULE;
#endif

#ifdef N_PLAT_STATIC
 #include <nagent/conn/connector.h> 
#endif

#define KLCS_MODULENAME L"KLPRCI"

using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;

#define _TOSTRING(_name) (#_name)
#define _TOSTRING_C(_name) TOSTRING(_##_name)

#define TOSTRING(_name) _TOSTRING(_name)
#define TOSTRING_C(_name) _TOSTRING_C(_name)

namespace KLSTD
{
    volatile bool g_bFailIfSignMismatch = 0;
    volatile long g_cfSignMismatchModules = 0;
};

namespace KLPRCI
{
#if 0
	struct component_info_t
	{
		component_info_t()
			:	hModule(NULL)
            ,   initComponent(NULL)
            ,   deinitComponent(NULL)
            ,   bUnload(false)
		{
			;
		};
		ComponentId				id;
		HMODULE					hModule;
		KLPRCI::InitComponent	initComponent;
		KLPRCI::DeinitComponent deinitComponent;
		bool					bUnload;
		std::wstring			wstrAsyncId;// for debug purposes only
	};

	typedef std::map<std::wstring, component_info_t> components_t;

    class module_cs_t
    {
    public:
        module_cs_t()
            :   m_lModRef(0)
        {
            KLSTD_CreateCriticalSection(&m_pCS);
        };
        module_cs_t(const module_cs_t& x)
            :   m_pCS(x.m_pCS)
            ,   m_lModRef(x.m_lModRef)
        {
            KLSTD_ASSERT(x.m_lModRef == 0);
            if(x.m_lModRef != 0)
            {
                KLSTD_TRACE0(1, L"Error: x.m_lModRef != 0\n");
            }
        }

        void get_CS(KLSTD::CriticalSection** ppCS)
        {
            m_pCS.CopyTo(ppCS);
        };
        long IncrementModRef()
        {
            return ++m_lModRef;
        };
        long DecrementModRef()
        {
            KLSTD_ASSERT( m_lModRef > 0 );
            return --m_lModRef;
        };
    protected:        
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
        long                                    m_lModRef;
    };

    typedef std::map<HMODULE, module_cs_t>  map_module_cs_t;
    
	class CInProcessComponents : public KLBaseImpl<InProcessComponents>
	{
	public:
		CInProcessComponents()
			:	m_idWorker(0)
			,	m_lToUnoad(0)
			,	m_bShutedDown(false)
		{
			KLSTD_CreateCriticalSection(&m_pDataCS);
            KLSTD_CreateReadWriteLock(&m_pLock);
			KLTP_GetThreadsPool()->AddWorker(
						&m_idWorker,
						L"CInProcessComponents::RunWorker",
						(KLTP::ThreadsPool::Worker*)this);
		};
		
		void UnloadAll()
		{
			for(;;)
			{
				component_info_t info;
                bool bDecrement = false;
				{
					AutoCriticalSection acs(m_pDataCS);
					if(!m_Components.size())
						break;
					components_t::iterator it=m_Components.begin();
					info=it->second;
					if(info.bUnload)
						bDecrement = true;
					m_Components.erase(it);
				};
				KLERR_BEGIN					
					Unload(info);
				KLERR_ENDT(1)
                if(bDecrement)
                    KLSTD_InterlockedDecrement(&m_lToUnoad);
			};
            while(m_lToUnoad > 0)
                KLSTD_Sleep(100);
		};

		bool UnloadComponent(std::wstring& wstrInstanceId)
		{
			component_info_t info;
            bool bDecrement = false;
			{
				AutoCriticalSection acs(m_pDataCS);
				components_t::iterator it=m_Components.find(wstrInstanceId);
				if(it == m_Components.end())
					return false;
				info=it->second;
				if(info.bUnload)
					bDecrement = true;
				m_Components.erase(it);
			};
			KLERR_BEGIN					
				Unload(info);
			KLERR_ENDT(1);
            if(bDecrement)
                KLSTD_InterlockedDecrement(&m_lToUnoad);
			return true;
		};

		void Shutdown()
		{
            {
                AutoReadWriteLock arwl(m_pLock, true);
                m_bShutedDown=true;
            };			
			if(m_idWorker)
			{
				UnloadAll();
                KLTP_GetThreadsPool()->DeleteWorker(m_idWorker);
                m_idWorker=0;
				KLSTD_ASSERT(m_lToUnoad == 0);				
			}
		}

		void Find(ComponentId&	filter, std::list<ComponentId>& ids)
		{
			ids.clear();
			AutoCriticalSection acs(m_pDataCS);
			for(components_t::iterator it = m_Components.begin(); it != m_Components.end(); ++it)
			{
				ComponentId &id=it->second.id;
				if(
					(filter.productName.empty() || filter.productName==id.productName) &&
					(filter.version.empty() || filter.version==id.version) &&
					(filter.componentName.empty() || filter.componentName==id.componentName) &&
					(filter.instanceId.empty() || filter.instanceId==id.instanceId))
						ids.push_back(id);
			};
		};

        void SetShutdownFlag()
        {
            KLSTD::AutoReadWriteLock arwl(m_pLock, true);
            KLSTD_SetShutdownFlag();
        };

		void LoadComponent(
					ComponentId&	id,
					KLPAR::Params*		pParameters, 
					std::wstring&		asyncId)
		{
            KLSTD_USES_CONVERSION;
			//KLAVT_AccessCheckForComponentDirect(KLAUTH_COMPONENT_START|KLAUTH_COMPONENT_CONTROL, id);
			
			KLSTD_ASSERT(!asyncId.empty());
			ComponentInfo ci;
			KLPRSS_GetComponentInfo(id.productName, id.version, id.componentName, ci, KLPRCI_DEFAULT_SS_TIMEOUT);
			
			if(
			(ci.dwModuleType & MTF_DLL) != MTF_DLL	||
			!(ci.dwStartFlags & MSF_CANBESTARTED))
					KLSTD_THROW(STDE_NOTPERM);

			std::wstring wstrFileName;
			if(!ci.wstrCustomName.empty())
				wstrFileName=ci.wstrCustomName;
			else
				KLSTD_MakePath(ci.wstrFileDirectory, ci.wstrFileName, L"", wstrFileName);
			if(wstrFileName.empty())
				KLSTD_THROW(STDE_NOENT);
            
            KLSTD_CheckDllBeforeLoad(wstrFileName.c_str());

			component_info_t	data;
			data.id=id;
			data.wstrAsyncId=asyncId;
            KLSTD::CAutoPtr<KLSTD::CriticalSection> pCallCS;
			KLERR_TRY				
                //AutoCriticalSection call_acs(m_pCallCS);
                AutoReadWriteLock arwl(m_pLock);
                if(KLSTD_GetShutdownFlag() || m_bShutedDown)
                {
                    KLSTD_TRACE1(
                            2,
                            L"Cannot load component '%ls' - we are shutting down !\n",
                            id.componentName.c_str());
                    KLSTD_THROW(STDE_UNAVAIL);
                };
                ;
				KLSTD_TRACE1(3, L"Loading library '%ls'...\n", wstrFileName.c_str());
				data.hModule = DL_LOAD_LIBRARY(wstrFileName.c_str());
				if(!data.hModule)
					KLSTD_THROW_LASTERROR();
                ;                
                AddModule(data.hModule, &pCallCS);
                KLSTD_ASSERT_THROW(pCallCS);
                ;
                data.initComponent=(KLPRCI::InitComponent)DL_GET_PROC_ADDR(data.hModule, TOSTRING(KLPRCIDLLCMP_INIT));
				if(!data.initComponent)
				{
                    data.initComponent=(KLPRCI::InitComponent)DL_GET_PROC_ADDR( data.hModule, TOSTRING_C(KLPRCIDLLCMP_INIT));
					if(!data.initComponent)
						KLSTD_THROW_LASTERROR();
				};
				data.deinitComponent=(KLPRCI::DeinitComponent)DL_GET_PROC_ADDR( data.hModule, TOSTRING(KLPRCIDLLCMP_DEINIT));
				if(!data.deinitComponent)
				{
					data.deinitComponent=(KLPRCI::DeinitComponent)DL_GET_PROC_ADDR( data.hModule, TOSTRING_C(KLPRCIDLLCMP_INIT));
					if(!data.deinitComponent)
						KLSTD_THROW_LASTERROR();
				};
				KLSTD_TRACE1(3, L"...OK loading library '%ls'\n", wstrFileName.c_str());

				CAutoPtr<Error> pError;
				{
					AutoCriticalSection call_acs(pCallCS);
					KLSTD_TRACE1(3, L"Initializing library '%ls'...\n", wstrFileName.c_str());
					data.initComponent(data.id, pParameters, &pError);
                    id=data.id;
				};
				if(pError)
				{
					KLSTD_TRACE1(3, L"...FAILED initializing library '%ls'\n", wstrFileName.c_str());
					throw pError.Detach();
				}
				else
					KLSTD_TRACE1(3, L"...OK initializing library '%ls'\n", wstrFileName.c_str());
				{
					AutoCriticalSection acs(m_pDataCS);
					components_t::iterator it=m_Components.find(data.id.instanceId);
					if(it != m_Components.end())
						KLSTD_THROW(STDE_EXIST);
					m_Components[data.id.instanceId]=data;
				};
			KLERR_CATCH(perror)
				KLERR_SAY_FAILURE(3, perror);
				if(data.hModule)
				{					
					KLERR_BEGIN
						if(data.deinitComponent)
                        {
                            KLSTD_ASSERT(pCallCS);
                            AutoCriticalSection call_acs(pCallCS);
                            data.deinitComponent(data.id);
                        };
					KLERR_ENDT(1)
					DL_FREE_LIBRARY(data.hModule);
                    RemoveModule(data.hModule);
				};
			KLERR_FINALLY
				KLERR_RETHROW();
			KLERR_ENDTRY
		};

        KLSTD_NOTHROW void OnComponentClosed(const std::wstring& instanceId) throw()
		{            
			if(m_bShutedDown)
				return;
			KLSTD_TRACE1(3, L"Marking component instanceId='%ls' for unload...\n", instanceId.c_str());
			AutoCriticalSection acs(m_pDataCS);
			components_t::iterator it=m_Components.find(instanceId);
			if(it != m_Components.end())
			{
				if(!it->second.bUnload)
				{
					it->second.bUnload=true;
					KLSTD_InterlockedIncrement(&m_lToUnoad);
				};
			};
			KLSTD_TRACE1(3, L"... OK marking component instanceId='%ls' for unload\n", instanceId.c_str());
		};

		int RunWorker( KLTP::ThreadsPool::WorkerId wId )
		{
			if(wId != m_idWorker)return 0;
			for(;m_lToUnoad;)
			{
				component_info_t	data;
				bool				bResult = false;
				{
					AutoCriticalSection acs(m_pDataCS);
					for(components_t::iterator it=m_Components.begin();it != m_Components.end(); ++it)
					{
						if(it->second.bUnload)
						{
							data=it->second;
							bResult=true;
							m_Components.erase(it);
							break;
						};
					};
				};
				if(bResult)
				{
					KLERR_BEGIN						
						Unload(data);
					KLERR_ENDT(1)
					KLSTD_InterlockedDecrement(&m_lToUnoad);
				};
			};
			return 0;
		};

        void AddModule(HMODULE hModule, KLSTD::CriticalSection** ppCS)
        {
        KL_TMEASURE_BEGIN(L"CInProcessComponents::AddModule", 3)
            KLSTD_CHK(hModule, hModule != NULL);
            KLSTD_CHKOUTPTR(ppCS);
            ;
            AutoCriticalSection acs_data(m_pDataCS);
            map_module_cs_t::iterator it = m_mapModuleCS.find(hModule);
            if(it == m_mapModuleCS.end())
            {
                it = m_mapModuleCS.insert(
                        map_module_cs_t::value_type(hModule, module_cs_t())).first;
            };
            KLSTD_ASSERT_THROW(it != m_mapModuleCS.end());
            it->second.IncrementModRef();
            it->second.get_CS(ppCS);
        KL_TMEASURE_END()
        };

        void RemoveModule(HMODULE hModule)
        {
        KL_TMEASURE_BEGIN(L"CInProcessComponents::RemoveModule", 3)
            KLSTD_ASSERT(hModule != NULL);
            ;
            AutoCriticalSection acs_data(m_pDataCS);
            map_module_cs_t::iterator it = m_mapModuleCS.find(hModule);
            if(it != m_mapModuleCS.end())
            {
                long lResult = it->second.DecrementModRef();
                KLSTD_ASSERT(lResult >= 0);
                if(lResult == 0)
                    m_mapModuleCS.erase(it);
            }
            else
            {
                KLSTD_TRACE1(1, L"Error in RemoveModule: Cannot find module 0x%X\n", hModule);
            };
            KLSTD_ASSERT(it != m_mapModuleCS.end());
        KL_TMEASURE_END()
        };

        void GetModuleCS(HMODULE hModule, KLSTD::CriticalSection** ppCS)
        {
        KL_TMEASURE_BEGIN(L"CInProcessComponents::GetModule", 3)
            KLSTD_CHK(hModule, hModule != NULL);
            KLSTD_CHKOUTPTR(ppCS);
            ;
            AutoCriticalSection acs_data(m_pDataCS);
            map_module_cs_t::iterator it = m_mapModuleCS.find(hModule);
            KLSTD_ASSERT(it != m_mapModuleCS.end());
            if(it != m_mapModuleCS.end())
                it->second.get_CS(ppCS);
        KL_TMEASURE_END()
        };
	protected:
		void Unload(component_info_t& data)
		{
            KLSTD_USES_CONVERSION;
			KLSTD_TRACE1(3, L"Unloading component '%ls'...\n", data.wstrAsyncId.c_str());
			KLERR_BEGIN
				if(data.deinitComponent)
                {
                    KLSTD::CAutoPtr<KLSTD::CriticalSection> pCallCS;
                    GetModuleCS(data.hModule, &pCallCS);
                    KLSTD_ASSERT(pCallCS);
                    AutoCriticalSection acs_call(pCallCS);
                    data.deinitComponent(data.id);
                };
                DL_FREE_LIBRARY(data.hModule);
                RemoveModule(data.hModule);
			KLERR_ENDT(1)
			KLSTD_TRACE1(3, L"...OK unloading component '%ls'\n", data.wstrAsyncId.c_str());
		};
		bool						            m_bShutedDown;
		components_t				            m_Components;
		volatile long				            m_lToUnoad;
		CAutoPtr<CriticalSection>	            m_pDataCS;
		KLTP::ThreadsPool::WorkerId	            m_idWorker;
        KLSTD::CAutoPtr<KLSTD::ReadWriteLock>   m_pLock;
        map_module_cs_t                         m_mapModuleCS;
	};

    KLSTD::CAutoPtr<InProcessComponents> Create_InProcess()
    {
        KLSTD::CAutoPtr<InProcessComponents> pResult;
        pResult.Attach(new CInProcessComponents);
        return pResult;
    }
#endif
};

KLCSKCA_DECL void KLPRCI_SetSignCheckMode(bool bFailIfMismatched)
{
    g_bFailIfSignMismatch = bFailIfMismatched;
};

KLCSKCA_DECL void KLPRCI_CheckModuleSignature(void* hModule)
{
#ifdef _WIN32
    KLSTD_USES_CONVERSION;
    KLSTD_TRACE1(3, L"Checking signature for loaded module 0x%X\n", hModule);
    TCHAR szBuffer[MAX_PATH*2+1] = _T("");
    if(!GetModuleFileName((HMODULE)hModule, szBuffer, KLSTD_COUNTOF(szBuffer)-1))
    {
        KLSTD_THROW_LASTERROR2();
    };    
    if(!KLSTD_CheckFileSignature(KLSTD_T2CW(szBuffer), g_bFailIfSignMismatch))
        KLSTD_InterlockedIncrement(&g_cfSignMismatchModules);
#else
# warning "Fix me!"
#endif
};

KLCSKCA_DECL bool KLPRCI_GetModuleSignatureMisMatch()
{
    return g_cfSignMismatchModules != 0;
}
