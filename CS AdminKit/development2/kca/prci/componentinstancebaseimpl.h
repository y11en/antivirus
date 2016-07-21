#ifndef __COMPONENTINSTANCEBASEIMPL_H__
#define __COMPONENTINSTANCEBASEIMPL_H__

#include <std/base/klstd.h>
#include <std/thr/locks.h>

#include <transport/tr/common.h>
#include <transport/tr/transport.h>
#include <transport/ev/common.h>
#include <transport/ev/eventsource.h>
#include <transport/avt/accesscontrol.h>
#include <transport/klsecurity/exports.h>
#include <transport/avtl/acllib.h>

#include <kca/prci/componentinstance.h>
#include <kca/prci/componentinstance2.h>
#include <kca/prci/producttaskscontrol2.h>
#include <kca/prci/errors.h>
#include <kca/prci/cmp_helpers.h>

#include <kca/prci/productstatistics2.h>
#include <kca/prci/prci_tasks.h>
#include <kca/prcp/agentproxy.h>

#include "./sink.h"
#include "./prci_actions.h"
#include "./taskdata.h"

#include <map>


namespace KLPRCI
{
    class KLSTD_NOVTABLE ComponentInstanceBase
		:	public	ComponentInstance2
		,	public	ProductTasksControl3
		,	public	ProductStatistics2
		,	public	ProductSettings
        ,   public  TasksControlInternal        
	{
	public:
        ComponentInstanceBase();
        virtual ~ComponentInstanceBase();
        //
		virtual unsigned long AddRef()=0;
		virtual unsigned long Release()=0;
		//
		virtual bool IsInitialized() = 0;;
		virtual void GetTaskParams(long idTask, KLPAR::Params* pFilter, KLPAR::Params** ppParams) = 0;
        virtual long GetPingTimeout() const = 0;
        virtual KLPRCI::ComponentInstanceState GetState() const = 0;
        virtual void GetStatistics(KLPAR::Params * statistics) = 0;
        virtual void Stop() = 0;
        virtual void Suspend() = 0;
        virtual void Resume() = 0;
        virtual long RunTask(
								const std::wstring& taskName,
								const KLPAR::Params* params,
								const std::wstring& asyncId=L"",
								long timeout = 0) = 0;

        virtual void RunMethod(
								const std::wstring& MethodName,
								KLPAR::Params* params,
								long timeout,
                                KLPAR::Params** ppResult) = 0;

		virtual void GetTaskName(long taskId, std::wstring& wstrTaskName) const =0;
        virtual void StopTask(long taskId) = 0;
        virtual void SuspendTask(long taskId) = 0;
        virtual void ResumeTask(long taskId) = 0;
        virtual void TaskReload(long taskId, KLPAR::Params* pNewSetings) = 0;
        virtual void GetTasksList( std::vector<long> & ids) const  = 0;
        virtual KLPRCI::TaskState GetTaskState(long taskId) const = 0;
        virtual int GetTaskCompletion(long taskId) const = 0;
		// Native
		virtual void LockInstance() = 0;
		virtual void UnlockInstance() = 0;
		virtual bool AccessCheckTask_InCall(
								const std::wstring& wstrTask,
								AVP_dword dwAccessMask,
								bool bThrow=true) = 0;
		virtual bool AccessCheckTask_InCall(
								long		idTask,
								AVP_dword dwAccessMask,
								bool bThrow=true) = 0;
    protected:
        //! we can enter from m_pCallCS to m_pDataCS but not otherwise !!!
        //! UnlockInstance must be called from m_pCallCS
        KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_pDataCS;//, m_pCallCS, m_CallStatisticsCS;
	};

	void publishEvent(
			const KLPRCI::ComponentId&      idComponent,
			const std::wstring&             wstrEventName,
            KLPAR::param_entry_t*           pData,
			int                             nData,
			long                            lTimeout,
            long                            lSeverity = -1L,
            KLSTD::precise_time_t           tmPublish = KLSTD::precise_time_t());

    
	class CInstances
	{
		public:
			CInstances();
			virtual ~CInstances();
            typedef KLSTD::CAutoPtr<ComponentInstanceBase> ComponentInstancePtr;
			;
			bool addComponentForSoap(const std::wstring& ID, ComponentInstanceBase* pInstance);
			void getComponentForSoap(const std::wstring& ID, ComponentInstanceBase** ppInstance);
			bool removeComponentForSoap(const std::wstring& ID);
			;
			void addComponentForTransport(const std::wstring& ID, ComponentInstanceBase* pInstance);
			ComponentInstancePtr getComponentForTransport(const std::wstring& ID);
			bool removeComponentForTransport(const std::wstring& ID);
			;
			int getComponentsNumber();
			;
			void Clear();
		private:
            
            typedef std::map<std::wstring, KLSTD::KLAdapt<ComponentInstancePtr> > instances_t;
			instances_t								m_InstancesTr, m_InstancesSoap;
	};

	extern CInstances	g_Instances;
};

#endif //__COMPONENTINSTANCEBASEIMPL_H__
