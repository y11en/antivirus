/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	cmpbase.cpp
 * \author	Andrew Kazachkov
 * \date	24.01.2003 14:42:29
 * \brief	
 * 
 */
#include <std/err/klerrors.h>
#include <std/tp/threadspool.h>

#include <kca/basecomp/cmpbase.h>
#include <common/measurer.h>

#define KLCS_MODULENAME L"KLBASECOMP"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;
using namespace KLPRCI;

namespace KLBASECOMP
{

	#ifndef KLBASECOMP_TRACE_LEVEL
		#ifdef _DEBUG
			#define KLBASECOMP_TRACE_LEVEL 3
		#else
			#define KLBASECOMP_TRACE_LEVEL 4
		#endif		
	#endif

	const int c_nTraceLevel=KLBASECOMP_TRACE_LEVEL;

	ComponentBaseImp::ComponentBaseImp()
		:	m_bStopFlag(false)
		,	m_bStopped(false)
		,	m_idPingWorker(0)
		,	m_bInitiaized(false)
	{
		KLSTD_CreateCriticalSection(&m_pDataCS);
		KLSTD_CreateCriticalSection(&m_pCommandCS);
        m_pExtLock = new KLSTD::ObjectLock(m_pDataCS);
        KLSTD_CHKMEM(m_pExtLock);
	};

	ComponentBaseImp::~ComponentBaseImp()
	{
		KLSTD_ASSERT(m_bStopped);
		Deinitialize();
	};

// Интерфейс ComponentBase

	void ComponentBaseImp::Initialize(
					const KLPRCI::ComponentId&	idComponent,
					bool						bDllComponent,
					KLPAR::Params*				pParams,
					unsigned short*				pPorts,
                    const wchar_t*              szwAddress)
	{
		KLSTD_TRACE0(c_nTraceLevel, L"ComponentBaseImp::Initialize...\n");
        m_pQueue = new queue_t(m_pDataCS);
        KLSTD_CHKMEM(m_pQueue);
        m_pQueue->Create(this, &ComponentBaseImp::OnProcessCommand, true);
		m_idComponent=idComponent;
		if(bDllComponent)
			KLPRCI_CreateDllComponentInstance(m_idComponent, pParams, &m_pInstance, pPorts, szwAddress);
		else
			KLPRCI_CreateComponentInstance(m_idComponent, &m_pInstance, pPorts, szwAddress);
		m_pInstance->GetTasksControl(&m_pTasksControl);
		m_pInstance->GetSettings(&m_pSettings);
		m_pInstance->GetStatistics(&m_pStatistics);
		m_pInstance->SetState(KLPRCI::STATE_INITIALIZING);
        ;
		OnInitialize();
		m_bInitiaized=true;
        ;
		long lTimeout=KLSTD_INFINITE;
		KLERR_BEGIN
			lTimeout=m_pInstance->GetPingTimeout();
		KLERR_ENDT(3)
		if(lTimeout > 0)
		    KLTP_GetThreadsPool()->AddWorker(
										    (int*)&m_idPingWorker,
										    L"ComponentBaseImp::PingWorker",
										    this,
										    KLTP::ThreadsPool::RunNormal,
										    true,
										    lTimeout);
        ;
		m_pInstance->SetInstanceControlCallback(this, InstanceControlCallback);
		m_pTasksControl->SetTasksControlCallback(this, TasksControlCallback);
		m_pTasksControl->SetRunTasksCallback(this, RunTasksCallback);
        ;
        m_pExtLock->Allow();
		m_pInstance->SetState(STATE_RUNNING);
        m_pQueue->Activate();
		KLSTD_TRACE0(c_nTraceLevel, L"...OK ComponentBaseImp::Initialize\n");
	};

	KLSTD_NOTHROW void ComponentBaseImp::Deinitialize() throw()
	{
        if(m_bStopped)
            return;
    KL_TMEASURE_BEGIN(L"ComponentBaseImp::Deinitialize", c_nTraceLevel);
        if(m_pInstance)
        {
            KLERR_IGNORE(m_pInstance->SetState(STATE_DEINITIALIZING));
        };
        m_pExtLock->Disallow();
        m_bStopFlag = true;        
        ;
        if(m_pInstance)
		    m_pInstance->SetInstanceControlCallback(this, NULL);
        if(m_pTasksControl)
		    m_pTasksControl->SetTasksControlCallback(this, NULL);
        if(m_pTasksControl)
		    m_pTasksControl->SetRunTasksCallback(this, NULL);
        ;
        KL_TMEASURE_BEGIN(L"Waiting for tasks and callbacks termination", 4)
        m_pExtLock->Wait();
        KL_TMEASURE_END();
        ;
		if(m_idPingWorker)
		{
			KLTP_GetThreadsPool()->DeleteWorker(m_idPingWorker);
			m_idPingWorker = 0;
		};
        ;
        if(m_pQueue)
            m_pQueue->Destroy();
        ;
		if(m_bInitiaized)
		{
			m_bInitiaized=false;
			KLERR_IGNORE(OnDeinitialize());
		};
        m_bStopped = true;
        ;
        if(m_pInstance)
        {
            KLERR_IGNORE(m_pInstance->SetState(STATE_SHUTTING_DOWN));
        };
    KL_TMEASURE_END();
	};

	void ComponentBaseImp::LockTasksHost()
	{
        if(!m_pExtLock->Lock())
            KLSTD_THROW(KLSTD::STDE_UNAVAIL);
	};

	void ComponentBaseImp::UnlockTasksHost()
	{
        m_pExtLock->Unlock();
	};

	void ComponentBaseImp::SetStopFlag()
	{
    KL_TMEASURE_BEGIN(L"ComponentBaseImp::SetStopFlag", c_nTraceLevel)
        m_bStopFlag=true;
        m_pExtLock->Disallow();
        if(m_pInstance)
            m_pInstance->InitiateUnload();
    KL_TMEASURE_END()
	};


	bool ComponentBaseImp::GetStopFlag()
	{
		return m_bStopFlag;
	};

	KLPRCI::ComponentId ComponentBaseImp::get_ComponentId()
	{
		return m_idComponent;
	};

	bool ComponentBaseImp::get_ComponentInstance(KLPRCI::ComponentInstance** ppInstance)
	{		
		KLSTD_CHKOUTPTR(ppInstance);
		AutoCriticalSection acs(m_pDataCS);
		m_pInstance.CopyTo(ppInstance);
		return (*ppInstance) != NULL;
	};

	bool ComponentBaseImp::get_ProductTasksControl(KLPRCI::ProductTasksControl** ppTasksControl)
	{		
		KLSTD_CHKOUTPTR(ppTasksControl);
		AutoCriticalSection acs(m_pDataCS);
		m_pTasksControl.CopyTo(ppTasksControl);
		return (*ppTasksControl) != NULL;
	};

	bool ComponentBaseImp::get_ProductStatistics(KLPRCI::ProductStatistics** ppStatistics)
	{		
		KLSTD_CHKOUTPTR(ppStatistics);
		AutoCriticalSection acs(m_pDataCS);
		m_pStatistics.CopyTo(ppStatistics);
		return (*ppStatistics) != NULL;
	};

	bool ComponentBaseImp::get_ProductSettings(KLPRCI::ProductSettings** ppSettings)
	{		
		KLSTD_CHKOUTPTR(ppSettings);
		AutoCriticalSection acs(m_pDataCS);
		m_pSettings.CopyTo(ppSettings);
		return (*ppSettings) != NULL;
	};


	bool ComponentBaseImp::SendCommand(command_t& cmd, bool bSync)
	{
        if(!m_pQueue || (cmd.idCmd == CMDID_FINISH && bSync))
            return false;
        ;   
        bool bResult = false;
		if(bSync)
		{
			AutoCriticalSection acs(m_pCommandCS);
			bResult = OnCommand(cmd, true);
		}
		else
		{
            command_ex_t cmdex;
            cmdex.cmd = cmd;
            cmdex.bSync = bSync;
            bResult = m_pQueue->Send(cmdex);
		};
		return bResult;
	};

	bool ComponentBaseImp::SendCommand(long idCmd, long nOption, KLSTD::KLBase* pData, bool bSync)
	{
		command_t cmd;
		cmd.idCmd=idCmd;
		cmd.nOption=nOption;
		cmd.pData=pData;
		return SendCommand(cmd, bSync);
	};

	void ComponentBaseImp::LinkTask(long idTask, TaskBase* pTask)
	{
		KLSTD_TRACE1(c_nTraceLevel, L"ComponentBaseImp::LinkTask(%d)...\n", idTask);
		AutoCriticalSection acs(m_pDataCS);
		tasks_t::iterator it =m_Tasks.find(idTask);
		if(it != m_Tasks.end())
			KLSTD_THROW(STDE_EXIST);
		m_Tasks[idTask] = pTask;
		KLSTD_TRACE1(c_nTraceLevel, L"...OK ComponentBaseImp::LinkTask(%d)\n", idTask);
	};

	bool ComponentBaseImp::UnlinkTask(long idTask)
	{
		KLSTD_TRACE1(c_nTraceLevel, L"ComponentBaseImp::UnlinkTask(%d)...\n", idTask);
		CAutoPtr<TaskBase> pTask;		
		{
			AutoCriticalSection acs(m_pDataCS);
			tasks_t::iterator it =m_Tasks.find(idTask);
			if(it == m_Tasks.end())
				return false;
			pTask=it->second;
			m_Tasks.erase(it);
		};
		pTask=NULL;
		KLSTD_TRACE1(c_nTraceLevel, L"...OK ComponentBaseImp::UnlinkTask(%d)\n", idTask);
		return true;
	};

	void ComponentBaseImp::GetRunningTasks(std::vector<long>& vecTaskIds)
	{
		KLSTD_TRACE0(c_nTraceLevel, L"ComponentBaseImp::GetRunningTasks...\n");
		{
		AutoCriticalSection acs(m_pDataCS);
		vecTaskIds.clear();
		vecTaskIds.reserve(m_Tasks.size());
		for(tasks_t::iterator it= m_Tasks.begin(); it != m_Tasks.end(); ++it)
			vecTaskIds.push_back(it->first);
		};
		KLSTD_TRACE0(c_nTraceLevel, L"...OK ComponentBaseImp::GetRunningTasks\n");
	};

	bool ComponentBaseImp::GetRunningTaskAt(long idTask, TaskBase** ppTask)
	{
		KLSTD_TRACE1(c_nTraceLevel, L"ComponentBaseImp::GetRunningTaskAt(%d)...\n", idTask);
		{
		AutoCriticalSection acs(m_pDataCS);
		tasks_t::iterator it =m_Tasks.find(idTask);
		if(it != m_Tasks.end())
			it->second.CopyTo(ppTask);
		};
		KLSTD_TRACE1(c_nTraceLevel, L"...OK ComponentBaseImp::GetRunningTaskAt(%d)\n", idTask);
		return (*ppTask)!=NULL;
	};

//Интерфейс ComponentBaseCallbacks

	void ComponentBaseImp::OnInitialize()
	{
		;// Ничего не делаем
	};

	KLSTD_NOTHROW void ComponentBaseImp::OnDeinitialize() throw()
	{
		;// Ничего не делаем;
	};

    void ComponentBaseImp::OnProcessCommand(command_ex_t& cmd)
    {
        AutoCriticalSection acs(m_pCommandCS);
        OnCommand(cmd.cmd, cmd.bSync);
    };

    bool ComponentBaseImp::OnCommand(command_t& cmd, bool bSync)
    {
        return false;        
    };


	void ComponentBaseImp::OnInstanceControl(KLPRCI::InstanceAction action)
	{
		if( action == INSTANCE_STOP)
			SetStopFlag();
		else
			KLSTD_NOTIMP();
	};

	void ComponentBaseImp::OnTasksControl(long taskId, KLPRCI::TaskAction action)
	{
		CAutoPtr<TaskBase> pTask;
		{
			AutoCriticalSection acs(m_pDataCS);
			std::map<long, KLSTD::CAutoPtr<TaskBase> >::iterator it =
														m_Tasks.find(taskId);
			if(it == m_Tasks.end())
				KLSTD_THROW(STDE_NOTFOUND);
			pTask=it->second;
		};
		pTask->OnControl(action);
	};

	void ComponentBaseImp::OnRunTasks(
					const std::wstring&		wstrTaskName,
					KLPAR::Params*			pTaskParams,
					long					idTask,
					long					lTimeout)
	{
		KLSTD_NOTIMP();
	};

	void ComponentBaseImp::OnSettingsChange(
					const std::wstring&	productName, 
					const std::wstring&	version,
					const std::wstring&	section, 
					const std::wstring&	parameterName,
					const KLPAR::Value*	oldValue,
					const KLPAR::Value*	newValue)
	{
		;// Ничего не делаем
	};

//Реализация KLTP::ThreadsPool
	int ComponentBaseImp::RunWorker(KLTP::ThreadsPool::WorkerId wId)
	{
        KLSTD::CAutoObjectLock acl(*m_pExtLock);
        if(!acl)
            return 0;
		if(wId == m_idPingWorker)
		{
			KLERR_BEGIN
                CAutoPtr<ComponentInstance> pInstance;
                get_ComponentInstance(&pInstance);
				if(pInstance)
					pInstance->Ping();
			KLERR_END
		};
		return 0;
	};

// Колбэки ComponentInstance
	void ComponentBaseImp::InstanceControlCallback(
					void* context,
					KLPRCI::InstanceAction action)
	{
		KLSTD_TRACE1(c_nTraceLevel, L"ComponentBaseImp::InstanceControlCallback(%d)...\n", action);
		ComponentBaseImp* pThis=(ComponentBaseImp*)context;
		KLSTD_ASSERT_THROW(pThis);
        KLSTD::CAutoObjectLock acl(*pThis->m_pExtLock);
        if(!acl)
        {
            KLSTD_THROW(KLSTD::STDE_UNAVAIL);
        };
        pThis->OnInstanceControl(action);
		KLSTD_TRACE1(c_nTraceLevel, L"...OK ComponentBaseImp::InstanceControlCallback(%d)\n", action);
	};

	void ComponentBaseImp::RunTasksCallback(
					void*					context,
					const std::wstring&		taskName,
					KLPAR::Params*			params,
					long					taskId,
					long					timeout)
	{
		KLSTD_TRACE2(c_nTraceLevel, L"ComponentBaseImp::RunTasksCallback('%ls', %d)...\n", taskName.c_str(), taskId);
		ComponentBaseImp* pThis=(ComponentBaseImp*)context;
		KLSTD_ASSERT_THROW(pThis);
        KLSTD::CAutoObjectLock acl(*pThis->m_pExtLock);
        if(!acl)
        {
            KLSTD_THROW(KLSTD::STDE_UNAVAIL);
        };
		KLERR_TRY
            pThis->OnRunTasks(taskName, params, taskId, timeout);
		KLERR_CATCH(pError)
			KLERR_SAY_FAILURE(c_nTraceLevel, pError);
		KLERR_FINALLY
			KLSTD_TRACE2(c_nTraceLevel, L"...OK ComponentBaseImp::RunTasksCallback('%ls', %d)\n", taskName.c_str(), taskId);
			KLERR_RETHROW();
		KLERR_ENDTRY
	};


	void ComponentBaseImp::TasksControlCallback(
					void*				context,
					long				taskId,
					KLPRCI::TaskAction	action)
	{
		KLSTD_TRACE2(c_nTraceLevel, L"ComponentBaseImp::TasksControlCallback(%d, %d)...\n", taskId, action);
		ComponentBaseImp* pThis=(ComponentBaseImp*)context;
		KLSTD_ASSERT_THROW(pThis);
        KLSTD::CAutoObjectLock acl(*pThis->m_pExtLock);
        if(!acl)
        {
            KLSTD_THROW(KLSTD::STDE_UNAVAIL);
        };
		KLERR_TRY
            pThis->OnTasksControl(taskId, action);
		KLERR_CATCH(pError)
			KLERR_SAY_FAILURE(c_nTraceLevel, pError);
		KLERR_FINALLY
			KLSTD_TRACE2(c_nTraceLevel, L"...OK ComponentBaseImp::TasksControlCallback(%d, %d)\n", taskId, action);
			KLERR_RETHROW();
		KLERR_ENDTRY
	};

	void ComponentBaseImp::SettingsChangeCallback(
						void*				context,
						const std::wstring&	productName, 
						const std::wstring&	version,
						const std::wstring&	section, 
						const std::wstring&	parameterName,
						const KLPAR::Value*	oldValue,
						const KLPAR::Value*	newValue )
	{
		KLSTD_TRACE3(c_nTraceLevel, L"ComponentBaseImp::SettingsChangeCallback('%ls', '%ls', '%ls')...\n", productName.c_str(), version.c_str(), section.c_str());
		ComponentBaseImp* pThis=(ComponentBaseImp*)context;
		KLSTD_ASSERT_THROW(pThis);
        KLSTD::CAutoObjectLock acl(*pThis->m_pExtLock);
        if(!acl)
        {
            KLSTD_THROW(KLSTD::STDE_UNAVAIL);
        };
		pThis->OnSettingsChange(
							productName,
							version,
							section,
							parameterName,
							oldValue,
							newValue);
		KLSTD_TRACE3(c_nTraceLevel, L"...OK ComponentBaseImp::SettingsChangeCallback('%ls', '%ls', '%ls')\n", productName.c_str(), version.c_str(), section.c_str());
	};


	TaskBaseImp::TaskBaseImp(bool bAutoImpersonate)
		:	m_pComponent(NULL)
		,	m_idTask(0)
		,	m_idMainWorker(0)
		,	m_bAutoImpersonate(bAutoImpersonate)
        ,   m_bHostLocked(false)
	{
		;
	};

	TaskBaseImp::~TaskBaseImp()
	{
		KLSTD_ASSERT(m_idMainWorker == 0);
		KLSTD_ASSERT(m_pComponent);
		if(m_pComponent)
		{
            if(m_bHostLocked)
			    m_pComponent->UnlockTasksHost();
			m_pComponent=NULL;
		};
	};

	void TaskBaseImp::PreInitialize(long idTask, ComponentBase* pComponent)
	{
		KLSTD_CHKINPTR(pComponent);
		
		m_pComponent=pComponent;
		m_pComponent->LockTasksHost();
        m_bHostLocked = true;

		KLSTD_CHK(idTask, idTask > 0);
		m_idTask = idTask;

		m_pComponent->get_ComponentInstance(&m_pInstance);
		m_pInstance->GetTasksControl(&m_pTasksControl);
		KLPRCI_GetClientContext(&m_pClientSecurityContext);
		KLSTD_ASSERT(m_pClientSecurityContext!=NULL);
        KLERR_TRY
            OnInitialize();
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
            KLERR_IGNORE(OnDeinitialize());
        KLERR_FINALLY
            KLERR_RETHROW();
        KLERR_ENDTRY
	};

	void TaskBaseImp::PostInitialize(long lTimeout)
	{
		if(lTimeout <= 0)
			lTimeout=-1;
        m_pComponent->LinkTask(m_idTask, this);
		KLTP_GetThreadsPool()->AddWorker(
						(int*)&m_idMainWorker,
						GetTaskName(),
						this,
						KLTP::ThreadsPool::RunNormal,
						true,
						lTimeout);		
	};

    void TaskBaseImp::OnInitialize()
    {
        ;// do nothing
    };

    void TaskBaseImp::OnDeinitialize()
    {
        ;// do nothing
    };
	
	void TaskBaseImp::Initialize(long idTask, ComponentBase* pComponent)
	{
		PreInitialize(idTask, pComponent);
        PostInitialize();
	};

	int TaskBaseImp::RunWorker(KLTP::ThreadsPool::WorkerId wId)
	{
		if(wId == m_idMainWorker)
		{
			KLSTD_ASSERT(m_pComponent);
			KLSTD_ASSERT(m_pTasksControl);

			bool bResult=false;

			KLERR_BEGIN
				if(!m_bAutoImpersonate)
					bResult=OnExecute();
				else
				{
					AutoImpersonate ai(m_pClientSecurityContext);
					bResult=OnExecute();
				};
			KLERR_ENDT(3)

			if(!bResult)
			{				
				/*
					Устанавливаем состояние задачи в TASK_FAILURE на случай,
					если разработчик забыл установить состояние задачи в
					TASK_COMPLETED или TASK_FAILURE. Если он не забыл, то
					данный код ничего не изменит.
				*/
				KLERR_BEGIN
					
					if(m_pTasksControl)
						m_pTasksControl->SetState(m_idTask, TASK_FAILURE);
				KLERR_END

				/*
					Удаляемся из компонентского хранилища. Это приведёт к
					уменьшению счётчика ссылок, однако до нуля он не дойдёт,
					по крайней мере, до выхода из данного метода, ибо пул
					потоков делает AddRef перед вызовом данного метода.
				*/
                KLERR_BEGIN
				if(m_pComponent)
					m_pComponent->UnlinkTask(m_idTask);
                KLERR_END
                    ;
                KLERR_BEGIN
				if(m_idMainWorker)
				{
					KLTP_GetThreadsPool()->DeleteWorker(m_idMainWorker);
					m_idMainWorker = 0;
				};
                KLERR_END
                ;
                KLERR_IGNORE(OnDeinitialize());
			};
		};
		return 0;
	};
};

