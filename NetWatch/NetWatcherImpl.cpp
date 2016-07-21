// NetWatcherImpl.cpp
//
#include "cnetwatchertask.h"
#include "NetWatcher.h"


class NetWatcherImpl : public IDL_NAME(cNetWatcher)
{
public:
	virtual tERROR pr_call ObjectInitDone();
	virtual tERROR pr_call ObjectPreClose();
	virtual tERROR pr_call ObjectClose();
	virtual tERROR pr_call MsgReceive();

	virtual tERROR pr_call SetSettings(const cSerializable* settings);
	virtual tERROR pr_call GetSettings(cSerializable* settings);
	virtual tERROR pr_call AskAction(Task::tActionId actionId, cSerializable* actionInfo);
	virtual tERROR pr_call SetState(TaskManager::tTaskRequestState state);
	virtual tERROR pr_call GetStatistics(cSerializable* statistics);
	virtual tERROR pr_call Dummy( tDWORD param1 );
//private: 
	CNetWatcherTask * NetWatcherTask;
//	cRulesApply * RulesApply;
};

tERROR pr_call NetWatcherImpl::ObjectInitDone()
{
	//hOBJECT* service1;
	//tDWORD err=((cTaskManager*)m_tm)->GetService(TASKID_TM_ITSELF, *this, "netwatch", (hOBJECT*)&service1);
	NetWatcherTask= new CNetWatcherTask();
	NetWatcherTask->m_task=(hOBJECT)this;
	//tDWORD err=((cTaskManager*)m_tm)->GetService(TASKID_TM_ITSELF,(hOBJECT)this, AVP_PROFILE_NETWORK_WATCHER, (hOBJECT*)&service1);
	
//	RulesApply = new cRulesApply();
//	RulesApply->ObjectInit((hOBJECT)this);
	//NetWatcherTask->RulesApply=RulesApply;
	return errOK;
}

tERROR pr_call NetWatcherImpl::ObjectPreClose()
{
	NetWatcherTask->PreClose();
	delete NetWatcherTask;
	NetWatcherTask=0;
	return errOK;
}

tERROR pr_call NetWatcherImpl::ObjectClose()
{
	
	return errOK;
}

tERROR pr_call NetWatcherImpl::MsgReceive()
{
	return errOK;
}

tERROR pr_call NetWatcherImpl::SetSettings(const cSerializable* p_settings)
{
	if(!p_settings->isBasedOn(cNWSettings::eIID) || !NetWatcherTask)
		return errNOT_SUPPORTED;
	return NetWatcherTask->SetSettings(p_settings);

}

tERROR pr_call NetWatcherImpl::GetSettings(cSerializable* p_settings)
{
	if(!p_settings->isBasedOn(cNWSettings::eIID) || !NetWatcherTask)
		return errNOT_SUPPORTED;
	
	return NetWatcherTask->GetSettings(p_settings);

}

tERROR pr_call NetWatcherImpl::AskAction(Task::tActionId actionId, cSerializable* actionInfo)
{
	if(actionId==1 && NetWatcherTask)
		return NetWatcherTask->GetNWdata(actionInfo);
	return errUNEXPECTED;
}

tERROR pr_call NetWatcherImpl::SetState(TaskManager::tTaskRequestState state)
{
	//tTaskRequestState x=state;
	//TASK_REQUEST_RUN
	return NetWatcherTask->SetTaskState((tTaskRequestState )state);
	
}

tERROR pr_call NetWatcherImpl::GetStatistics(cSerializable* statistics)
{
	return errOK;
}

tERROR pr_call NetWatcherImpl::Dummy( tDWORD param1 )
{
	return errOK;
}


CPP_VTBL_NETWATCHER_DEFINE(NetWatcherImpl)

