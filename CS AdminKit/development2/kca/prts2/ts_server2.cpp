/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ts_server2.cpp
 * \author	Andrew Kazachkov
 * \date	21.03.2003 13:32:11
 * \brief	
 * 
 */

#include <build/general.h>
#include <std/base/kldefs.h>
#include "common/make_uqual_one_way.h"
#include <std/conv/klconv.h>
#include <std/io/klio.h>
#include <std/err/error.h>
#include <std/par/params.h>
//#include <prss/settingsstorage.h>
#include <kca/prci/simple_params.h>
#include <kca/prcp/agentproxy.h>
#include <kca/prts/tasksstorage.h>
#include <kca/prts/taskstorageserver.h>

#include <kca/prts/taskinfo.h>

#include <kca/prts2/ts_server2.h>

#include <kca/prss/errors.h>
#include <kca/prts/prtsdefs.h>
#include <kca/prts/tsk_list.h>

#include <common/measurer.h>

using namespace KLERR;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;
using namespace KLPRCI;

#define KLCS_MODULENAME L"KLPRTS2"

namespace KLPRTS2
{
    void DiffMaker::NewInSrc(data_t::iterator itSrc)
    {
    KLERR_BEGIN
        ts_info_t info;
        info.wstrLocation=itSrc->second;
        info.wstrProduct=itSrc->first.product;
        info.wstrVersion=itSrc->first.version;
        m_pServer->AddTasksStorage(
                        KLSTD_CreateLocallyUniqueString(),
                        m_cidMaster,
                        info,
                        m_bLocalCopyOfServerStorage);
    KLERR_ENDT(3)
    };
    
    void DiffMaker::NewInDst(data_t::iterator itDst)
    {
    KLERR_BEGIN
        product_version_t info;
        info.product=itDst->first.product;
        info.version=itDst->first.version;
        m_pServer->RemoveTasksStorage(info);
    KLERR_ENDT(3)
    };


    static const KLPRSS::product_version_t c_keyGtSServer;//=KLPRSS::product_version_t();

	CTasksStorageServer2::CTasksStorageServer2()
		:	KLSTD::KLBaseImpl<KLPRTS::TaskStorageServer>()
		,	m_bInitialized(false)
		,	m_pSink(NULL)
        ,   m_bDontPublishEvents(false)
        ,   m_bConnectToAgent(false)
	{
		KLSTD_CreateCriticalSection(&m_pCS);
	};

	CTasksStorageServer2::~CTasksStorageServer2()
	{
		KLERR_IGNORE(Deinitialize());
	};


    bool CTasksStorageServer2::RemoveTasksStorage(const product_version_t& key)
    {
        CAutoPtr<KLPRTS::TaskStorageServer> pServer;
        {
            AutoCriticalSection acs(m_pCS);
            servers_t::iterator it=m_mapServers.find(key);
            if(it == m_mapServers.end())
			    return false;
            pServer=it->second.m_pServer;
            m_mapServers.erase(it);
        };
        KLSTD_TRACE3(
                    3,
                    L"Taskstorage '%ls'-'%ls' was removed from '%ls'\n",
                    key.product.c_str(),
                    key.version.c_str(),
                    m_wstrID.c_str());
        pServer=NULL;
        return true;
    }

    void CTasksStorageServer2::AddTasksStorage(
                        const std::wstring&         wstrId,
                        const KLPRCI::ComponentId&  _cidMaster,
                        const ts_info_t&            info,
                        bool						bLocalCopyOfServerStorage)
    {        
        {
            AutoCriticalSection acs(m_pCS);
		    product_version_t key(
								    info.wstrProduct,
								    info.wstrVersion);
		    if(m_mapServers.find(key) != m_mapServers.end())
			    KLSTD_THROW(STDE_EXIST);

            KLPRCI::ComponentId cidMaster;
            cidMaster.productName=info.wstrProduct;
            cidMaster.version=info.wstrVersion;
            cidMaster.componentName=KLPRTS2_COMPONENT_NAME;
            cidMaster.instanceId=wstrId;

		    KLPRTS_CreateTasksStorageServer(
										    wstrId,
										    cidMaster,
										    info.wstrLocation,
										    bLocalCopyOfServerStorage,
                                            m_bConnectToAgent,
                                            m_bDontPublishEvents,
											m_wstrHostId.c_str() );
		    ts_server_data data;
		    KLPRTS_GetServerObjectByID(wstrId, &data.m_pServer, true);
		    data.m_wstrObjectID=wstrId;
		    data.m_pSet.Attach(new CStringSet);
            KLSTD_CHKMEM(data.m_pSet);
		    KLERR_BEGIN
                TSKLIST::GetTasksFilesList(info.wstrLocation, data.m_pSet->x);
                /*
                std::wstring wstrFullName;
                KLSTD_PathAppend(
                            info.wstrLocation,
                            KLPRTS::TASK_STORAGE_FILE_MASK,
                            wstrFullName,
                            true);
                std::vector<std::wstring> names;
                KLSTD_GetFilesByMask(wstrFullName, names);
			    for(unsigned int k=0; k < names.size(); ++k)
				    data.m_pSet->x.insert(names[k]);
                */
		    KLERR_ENDT(3)
            m_mapServers.insert(servers_t::value_type(key, data));
        };
        KLSTD_TRACE3(
                    3,
                    L"Taskstorage '%ls'-'%ls' was added to '%ls'\n",
                    info.wstrProduct.c_str(),
                    info.wstrVersion.c_str(),
                    m_wstrID.c_str());
    }

	void CTasksStorageServer2::Initialize(
						const KLPRCI::ComponentId& cidMaster,
						const std::wstring&			wstrID,
						const ts_info_t*			pStoragesInfo,
						long						nStoragesInfo,
						bool						bLocalCopyOfServerStorage,
						KLPRTS2::TsSink*			pSink,
                        bool                        bConnectToAgent,
                        bool                        bDontPublishEvents,
                        const std::wstring&         wstrCommonTssId,
						const std::wstring&         wstrHostId )
	{
		AutoCriticalSection acs(m_pCS);        
		KLSTD_CHKINPTR(pStoragesInfo);
		KLSTD_CHK(nStoragesInfo, nStoragesInfo > 0);
		KLSTD_CHK(wstrID, !wstrID.empty());
		Deinitialize();
		m_pSink=pSink;
		m_wstrID=wstrID;
        m_cidMaster=cidMaster;
        m_bLocalCopyOfServerStorage=bLocalCopyOfServerStorage;
        m_bConnectToAgent=bConnectToAgent;
        m_bDontPublishEvents=bDontPublishEvents;
        m_wstrCommonTssId=wstrCommonTssId;
		m_wstrHostId = wstrHostId;
        if(!m_wstrCommonTssId.empty())
        {
            KLPRTS_GetServerObjectByID(m_wstrCommonTssId, &m_pCommonTss, false);
            ts_server_data sd;
            sd.m_pServer=m_pCommonTss;
            sd.m_wstrObjectID=m_wstrCommonTssId;
            m_mapServers.insert(servers_t::value_type(c_keyGtSServer, sd));
        };
		for(int i=0; i < nStoragesInfo; ++i)
		{
		KLERR_BEGIN
			std::wstring wstrId=KLSTD_CreateLocallyUniqueString();
            AddTasksStorage(
                        wstrId, 
                        cidMaster,
                        pStoragesInfo[i],
                        bLocalCopyOfServerStorage);
		KLERR_END
		};		
		m_bInitialized=true;
	};

	KLSTD_NOTHROW void CTasksStorageServer2::Deinitialize() throw()
	{		
		for(;;)
		{
        KLERR_BEGIN
            ts_server_data data;
            {
                AutoCriticalSection acs(m_pCS);
                if(m_mapServers.empty())
                    break;
                servers_t::iterator it=m_mapServers.begin();
                data=it->second;
                m_mapServers.erase(it);
            };
            KLERR_IGNORE(KLPRTS_DeleteTasksStorageServer(data.m_wstrObjectID));            
            data.m_pServer=NULL;
        KLERR_ENDT(1)
		};
        /*
        if(!m_wstrCommonTssId.empty())
        {
            KLERR_IGNORE(KLPRTS_DeleteTasksStorageServer(m_wstrCommonTssId));
            m_pCommonTss=NULL;
        };
        */
		KLSTD_ASSERT(m_mapServers.size() == 0);
		m_bInitialized=false;
		m_pSink=NULL;
	};

    void CTasksStorageServer2::Update(
				const ts_info_t*    pStoragesInfo,
				long                nStoragesInfo)
    {
        DiffMaker::data_t srcData, dstData;
        DiffMaker data(this, m_cidMaster, m_bLocalCopyOfServerStorage);
        
        for(servers_t::iterator it=m_mapServers.begin(); it!=m_mapServers.end();++it)
        {
            if(it->first == c_keyGtSServer)
                continue;
            dstData.insert(DiffMaker::data_t::value_type(
                                    product_version_t(
                                        it->first.product,
                                        it->first.version),
                                    L""));
        };
        
        for(int i=0; i < nStoragesInfo; ++i)
            srcData.insert(
                    DiffMaker::data_t::value_type(
                            product_version_t(
                                pStoragesInfo[i].wstrProduct,
                                pStoragesInfo[i].wstrVersion),
                            pStoragesInfo[i].wstrLocation));
        
        MakeEqualOneWay(srcData, dstData, data, data, data); 
    }

	void CTasksStorageServer2::ResetTasksIterator(
							const KLPRCI::ComponentId&	cidFilter,
							const std::wstring&			sTaskNameFilter,
							KLPAR::Params**				ppTaskStorageParams)
	{
    KL_TMEASURE_BEGIN(L"CTasksStorageServer2::ResetTasksIterator", 4)
		KLSTD_CHKOUTPTR(ppTaskStorageParams);

		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");
		
		if(m_pSink)
			m_pSink->OnTasksStorageServer2MethodCall(NULL);

		CAutoPtr<Params> pResult;
		KLPAR_CreateParams(&pResult);
		for(servers_t::iterator i=m_mapServers.begin(); i != m_mapServers.end(); ++i)
		{
		KLERR_BEGIN
			CAutoPtr<Params> pData;
			i->second.m_pServer->ResetTasksIterator(cidFilter, sTaskNameFilter, &pData);
			ParamsNames names;
			if(pData)
				pData->GetNames(names);
			for(ParamsNames::iterator j=names.begin(); j != names.end(); ++j)
			{
			KLERR_BEGIN
				CAutoPtr<Value> pValue;
				pData->GetValue(*j, &pValue);
				KLSTD_ASSERT(pValue && pValue->GetType() == Value::PARAMS_T);
				pResult->AddValue(*j, pValue);
			KLERR_END
			};
		KLERR_END
		};
		pResult.CopyTo(ppTaskStorageParams);
    KL_TMEASURE_END()
	};
    
    void CTasksStorageServer2::GetTaskByIDWithPolicyApplied(  const std::wstring taskId,
										KLPAR::Params** ppTaskParams)
    {
        KLSTD_NOTIMP();
    }

	void CTasksStorageServer2::GetTaskByID(
							const std::wstring	idTask,
							KLPAR::Params**		ppTaskParams)
	{		
		KLSTD_CHKOUTPTR(ppTaskParams);

		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");
		
		if(m_pSink)
			m_pSink->OnTasksStorageServer2MethodCall(NULL);

		CAutoPtr<Params> pResult;
		KLSTD::CAutoPtr<KLPRTS::TaskStorageServer>	pServer;
		if(FindServer(idTask, &pServer))
			pServer->GetTaskByID(idTask, &pResult);
		else
			ThrowTaskNotFound(idTask);
		pResult.CopyTo(ppTaskParams);
	};

	std::wstring CTasksStorageServer2::AddTask(
							const KLPAR::Params* pTaskToAddParams)
	{		
		KLSTD_CHKINPTR(pTaskToAddParams);
		
		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");
		
		if(m_pSink)
			m_pSink->OnTasksStorageServer2MethodCall(NULL);

		KLPRTS::CTaskInfoInternal ti(pTaskToAddParams);
		KLPRCI::ComponentId cidTask = ti.GetTaskCID();
		KLPRSS::product_version_t pv(cidTask.productName, cidTask.version);		
        KLSTD_TRACE2(3, L"AddTask for product='%ls', version = '%ls'\n", cidTask.productName.c_str(), cidTask.version.c_str());
        servers_t::iterator it=m_mapServers.find(pv);
		if(it == m_mapServers.end())
            ThrowTsNotFound(pv);
		std::wstring idTask=it->second.m_pServer->AddTask(pTaskToAddParams);
		UpdateCache(pv, idTask);
		return idTask;
	};

	void CTasksStorageServer2::UpdateTask(
							const std::wstring idTask,
							const KLPAR::Params* pTaskToUpdateParams)
	{
		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");
		
		if(m_pSink)
			m_pSink->OnTasksStorageServer2MethodCall(NULL);

		KLSTD::CAutoPtr<KLPRTS::TaskStorageServer>	pServer;
		if(FindServer(idTask, &pServer))
			pServer->UpdateTask(idTask, pTaskToUpdateParams);
		else
			ThrowTaskNotFound(idTask);
	};

	void CTasksStorageServer2::ReplaceTask(
							const std::wstring idTask,
							const KLPAR::Params* pTaskToUpdateParams)
	{
		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");
		
		if(m_pSink)
			m_pSink->OnTasksStorageServer2MethodCall(NULL);

		KLSTD::CAutoPtr<KLPRTS::TaskStorageServer>	pServer;
		if(FindServer(idTask, &pServer))
			pServer->ReplaceTask(idTask, pTaskToUpdateParams);
		else
			ThrowTaskNotFound(idTask);
	};

	void CTasksStorageServer2::DeleteTask(const std::wstring idTask)
	{
		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");
		
		if(m_pSink)
			m_pSink->OnTasksStorageServer2MethodCall(NULL);

		KLSTD::CAutoPtr<KLPRTS::TaskStorageServer>	pServer;
		if(FindServer(idTask, &pServer))
			pServer->DeleteTask(idTask);
		else
			ThrowTaskNotFound(idTask);
	};

	void CTasksStorageServer2::SetTaskStartEvent(
							const std::wstring			idTask,
							const KLPRCI::ComponentId&	filter,
							const std::wstring&			eventType,
							const KLPAR::Params*		bodyFilter)
	{
		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");
		
		KLSTD::CAutoPtr<KLPRTS::TaskStorageServer>	pServer;
		
		if(m_pSink)
			m_pSink->OnTasksStorageServer2MethodCall(NULL);

		if(FindServer(idTask, &pServer))
			pServer->SetTaskStartEvent(idTask, filter, eventType, bodyFilter);
		else
			ThrowTaskNotFound(idTask);
	};

	void CTasksStorageServer2::GetTaskStartEvent(
							const std::wstring			idTask,
							KLPRCI::ComponentId&		filter,
							std::wstring&				eventType,
							KLPAR::Params**				bodyFilter)
	{
		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");
		
		
		if(m_pSink)
			m_pSink->OnTasksStorageServer2MethodCall(NULL);

		KLSTD::CAutoPtr<KLPRTS::TaskStorageServer>	pServer;
		if(FindServer(idTask, &pServer))
			pServer->GetTaskStartEvent(idTask, filter, eventType, bodyFilter);
		else
			ThrowTaskNotFound(idTask);
	};

	KLSTD_NOTHROW void CTasksStorageServer2::UpdateCache(
						const KLPRSS::product_version_t&	pvi,
						const std::wstring&					TaskId,
						bool								bAdd) throw()
	{
		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");
        if(pvi == c_keyGtSServer)return;
		servers_t::iterator it=m_mapServers.find(pvi);
		if(it==m_mapServers.end())
			return;
		if(!it->second.m_pSet && !bAdd)
			return;
		if(!it->second.m_pSet)
        {
            it->second.m_pSet.Attach(new CStringSet);
            KLSTD_CHKMEM(it->second.m_pSet);
        };
		std::set<std::wstring>::iterator it2=it->second.m_pSet->x.find(TaskId);
		if(bAdd && it2==it->second.m_pSet->x.end())
			it->second.m_pSet->x.insert(TaskId);
		else if(!bAdd && it2!=it->second.m_pSet->x.end())
			it->second.m_pSet->x.erase(it2);
	};

	bool CTasksStorageServer2::FindServer(
									const std::wstring&				idTask,
									KLPRTS::TaskStorageServer**		ppServer,
									bool							bFastSearchOnly)
	{
		AutoCriticalSection acs(m_pCS);
		if(!m_bInitialized)KLSTD_NOINIT(L"TasksStorageServer2");

        if(m_pCommonTss && wcsncmp(
                idTask.c_str(),
                KLPRTS::KLPRTS_LOCAL_TASK_ATTRIBUTE,
                wcslen(KLPRTS::KLPRTS_LOCAL_TASK_ATTRIBUTE)) != 0)
        {
            m_pCommonTss.CopyTo(ppServer);
            return true;
        };

		for(servers_t::iterator it=m_mapServers.begin(); it != m_mapServers.end(); ++it)
		{
			if(it->second.m_pSet)
			{
				CStringSet*	pSet=it->second.m_pSet;
				if(pSet->x.find(idTask)!=pSet->x.end())
				{
					it->second.m_pServer.CopyTo(ppServer);
					return true;
				};
			};
		};
		if(bFastSearchOnly)
			return false;
		for(servers_t::iterator i=m_mapServers.begin(); i != m_mapServers.end(); ++i)
		{
			CAutoPtr<Params> pData;
			KLERR_BEGIN
				i->second.m_pServer->GetTaskByID(idTask, &pData);
			KLERR_END
			if(pData != NULL)
			{
				i->second.m_pServer.CopyTo(ppServer);
				UpdateCache(i->first, idTask);
				return true;
			};			
		};
		return false;
	};

	void CTasksStorageServer2::ThrowTaskNotFound(const std::wstring& wstrTaskId)
	{
        KLSTD_TRACE2(
                        3,
                        L"Cannot find in '%ls' task '%ls'\n",
                        m_wstrID.c_str(),
                        wstrTaskId.c_str());
        KLERR_THROW1(L"KLPRSS", KLPRSS::NOT_EXIST, wstrTaskId.c_str());
	};

    void CTasksStorageServer2::ThrowTsNotFound(const product_version_t& pv)
    {
        KLSTD_TRACE3(
                        3,
                        L"Cannot find in '%ls' tasksstorage for '%ls' v. %ls\n",
                        m_wstrID.c_str(),
                        pv.product.c_str(),
                        pv.version.c_str());
        KLSTD_THROW(STDE_NOTFOUND);
    }
	
	std::wstring& CTasksStorageServer2::getID()
	{
		return m_wstrID;
	};
    
    void CTasksStorageServer2::PutTask(
							const std::wstring TaskID,
							const KLPAR::Params* pTaskParams)
    {
        KLSTD_NOTIMP();
    };

	std::wstring CTasksStorageServer2::GetHostId() const
	{
		return m_wstrHostId;
	}
};

using namespace KLPRTS2;

namespace KLPRTS2
{
    /*
    typedef std::map<std::wstring, KLSTD::KLAdapt<CAutoPtr<CTasksStorageServer2> > > server_objects_t;
    server_objects_t g_mapServers;
    */
};


IMPLEMENT_MODULE_INIT_DEINIT(KLPRTS2)

IMPLEMENT_MODULE_INIT_BEGIN2(KLCSKCA_DECL,KLPRTS2)
    ;
IMPLEMENT_MODULE_INIT_END()


IMPLEMENT_MODULE_DEINIT_BEGIN2(KLCSKCA_DECL,KLPRTS2)
    ;
IMPLEMENT_MODULE_DEINIT_END()

KLCSKCA_DECL void KLPRTS2_GetTasksStorageServer(
                const std::wstring&			wstrServerObjectID, 
                CTasksStorageServer2**      ppServer)
{
    KLPRTS_GetServerObjectByID(wstrServerObjectID, (KLPRTS::TaskStorageServer**)ppServer, true);
};


KLCSKCA_DECL bool KLPRTS2_UpdateTasksStorageServer(
							const std::wstring&			wstrServerObjectID, 
							KLPRTS2::ts_info_t*			pLocations,
							long						nLocations)
{
    bool bResult=false;
    KLERR_BEGIN
        CAutoPtr<CTasksStorageServer2> pServer;
        KLPRTS2_GetTasksStorageServer(wstrServerObjectID, &pServer);
        pServer->Update(pLocations, nLocations);
        bResult=true;
    KLERR_ENDT(3)
    return bResult;
};

KLCSKCA_DECL void KLPRTS2_DeleteTasksStorageServer(const std::wstring& sServerObjectID)
{
    KLERR_IGNORE(KLPRTS_DeleteTasksStorageServer(sServerObjectID));
}

KLCSKCA_DECL void KLPRTS2_CreateTasksStorageServer(
							const std::wstring&			wstrServerObjectID, 
							const KLPRCI::ComponentId&	cidMaster, 
							KLPRTS2::ts_info_t*			pLocations,
							long						nLocations,
                            AVP_dword                   dwFlags,
							KLPRTS2::TsSink*			pSink,
                            const std::wstring&         wstrCommonId,
							const std::wstring&         wstrHostId )
{
KL_TMEASURE_BEGIN(L"KLPRTS2_CreateTasksStorageServer", 3)
	CAutoPtr<CTasksStorageServer2> pServer;
	pServer.Attach(new CTasksStorageServer2);
    KLSTD_CHKMEM(pServer);
	pServer->Initialize(
						cidMaster,
						wstrServerObjectID,
						pLocations,
						nLocations,
						(dwFlags & CTSF_LOCAL_COPY) != 0,
						pSink,
                        (dwFlags & CTSF_CONNECT_TO_AGENT) != 0,
                        (dwFlags & CTSF_NOT_PUBLISH_EVENTS) != 0,
                        wstrCommonId,
						wstrHostId );
    KLPRTS_AddServerObject(pServer);
KL_TMEASURE_END()
};

