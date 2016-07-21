#include <std/base/klstd.h>
#include "soapH.h"
#include <std/par/params.h>
#include <kca/prss/settingsstorage.h>
#include "componentinstancebaseimpl.h"
#include "std/par/paramsi.h"
#include "transport/avt/accesscheck.h"

#include "std/thrstore/threadstore.h"
#include "kca/prci/cmp_helpers.h"

#define KLCS_MODULENAME L"KLPRCI"

using namespace KLSTD;
using namespace KLERR;
using namespace KLPRCI;
using namespace KLPAR;

#define HANDLER_BEGIN	\
		ComponentInstanceBase* pInstance=NULL;	\
		KLERR_TRY	\
            KLSTD_CHKINPTR(wstrID);   \
			g_Instances.getComponentForSoap(wstrID, &pInstance);


#define HANDLER_END	\
		KLERR_CATCH(pError)	\
			ExceptionForSoap(soap, pError, r.error);	\
		KLERR_FINALLY	\
			if(pInstance)pInstance->Release();	\
		KLERR_ENDTRY;


SOAP_FMAC1 int SOAP_FMAC2 klprci_GetTasksParamsStub(struct soap* soap, wchar_t * wstrID, long idTask, param__params	filter, struct klprci_GetTasksParamsResponse& r)
{	
	HANDLER_BEGIN
		pInstance->AccessCheckTask_InCall(idTask, KLAUTH_TASK_QUERY_INFORMATION);
		CAutoPtr<Params> pFilter, pResult;
		ParamsFromSoap(filter, &pFilter);
		pInstance->GetTaskParams(idTask, pFilter, &pResult);
		ParamsForSoap(soap, pResult, r.pars);
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_GetPingTimeoutStub(struct soap* soap, wchar_t * wstrID, struct klprci_GetPingTimeoutResponse &r)
{	
	HANDLER_BEGIN		
		r.timeout=pInstance->GetPingTimeout();
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_GetStateStub(struct soap* soap, wchar_t * wstrID, struct klprci_GetStateResponse &r)
{	
	HANDLER_BEGIN
        KLPRCI::ComponentId idComponent;
        pInstance->GetComponentId(idComponent);
        KLAVT_AccessCheckForComponent_InCall(KLAUTH_COMPONENT_QUERY_STATE, idComponent);
		//pInstance->AccessCheck(pSecContext, KLAUTH_COMPONENT_QUERY_STATE);
		r.state=pInstance->GetState();
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_GetStatisticsStub(struct soap* soap, wchar_t * wstrID, param__params statisticsIn, struct klprci_GetStatisticsResponse &r)
{	
	HANDLER_BEGIN
        KLPRCI::ComponentId idComponent;
        pInstance->GetComponentId(idComponent);
        KLAVT_AccessCheckForComponent_InCall(KLAUTH_COMPONENT_QUERY_INFORMATION, idComponent);
		//pInstance->AccessCheck(pSecContext, KLAUTH_COMPONENT_QUERY_INFORMATION);
		CAutoPtr<Params> pStatistics;
		ParamsFromSoap(statisticsIn, &pStatistics);
		pInstance->GetStatistics(pStatistics);
		ParamsForSoap(soap, pStatistics, r.statisticsOut);
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_StopStub(struct soap* soap, wchar_t * wstrID, struct klprci_StopResponse &r)
{
	HANDLER_BEGIN
        KLPRCI::ComponentId idComponent;
        pInstance->GetComponentId(idComponent);
        KLAVT_AccessCheckForComponent_InCall(KLAUTH_COMPONENT_CONTROL, idComponent);
		//pInstance->AccessCheck(pSecContext, KLAUTH_COMPONENT_CONTROL);
		pInstance->Stop();
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_SuspendStub(struct soap* soap, wchar_t * wstrID, struct klprci_SuspendResponse &r)
{	
	HANDLER_BEGIN
        KLPRCI::ComponentId idComponent;
        pInstance->GetComponentId(idComponent);
        KLAVT_AccessCheckForComponent_InCall(KLAUTH_COMPONENT_CONTROL, idComponent);
		//pInstance->AccessCheck(pSecContext, KLAUTH_COMPONENT_CONTROL);
		pInstance->Suspend();
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_ResumeStub(struct soap* soap, wchar_t * wstrID, struct klprci_ResumeResponse &r)
{
	HANDLER_BEGIN
        KLPRCI::ComponentId idComponent;
        pInstance->GetComponentId(idComponent);
        KLAVT_AccessCheckForComponent_InCall(KLAUTH_COMPONENT_CONTROL, idComponent);
		//pInstance->AccessCheck(pSecContext, KLAUTH_COMPONENT_CONTROL);
		pInstance->Resume();
	HANDLER_END
	return SOAP_OK;
};

namespace KLPRCI
{
    class CConnectionInfo
        :   public KLSTD::KLBaseImpl<KLPRCI::ConnectionInfo>
    {
    public:
        CConnectionInfo()
        {
            ;
        };

        void Create(    const std::wstring&   wstrLocalName,
                        const std::wstring&   wstrRemoteName)
        {
            m_wstrLocalName = wstrLocalName;
            m_wstrRemoteName = wstrRemoteName;
        };

        virtual ~CConnectionInfo()
        {
            ;
        };

        void GetConnectionInfo(
                            std::wstring&   wstrLocalName,
                            std::wstring&   wstrRemoteName)
        {
            wstrLocalName = m_wstrLocalName;
            wstrRemoteName = m_wstrRemoteName;
        };
    protected:
        std::wstring    m_wstrLocalName, m_wstrRemoteName;
    };

    class AutoConnectionInfo
    {
    public:
        AutoConnectionInfo(struct soap* soap)
        {
        KLERR_BEGIN
            KLTRAP::Transport::ConnectionFullName cfn;
            KLTR_GetTransport()->GetConnectionName(soap, cfn);
            KLSTD::CAutoPtr<CConnectionInfo> pCi;
            pCi.Attach(new CConnectionInfo);
            KLSTD_CHKMEM(pCi);
            pCi->Create(    cfn.localComponentName, 
                            cfn.remoteComponentName);
            KLSTD_GetGlobalThreadStore()->AddObject(
                                    KLPRCI::c_szwTskConnectionInfo, 
                                    pCi);
        KLERR_ENDT(1)
        }

        ~AutoConnectionInfo()
        {
            KLERR_IGNORE(KLSTD_GetGlobalThreadStore()->RemoveObject(
                                    KLPRCI::c_szwTskConnectionInfo));
        }
    };
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_RunTaskStub(struct soap* soap, wchar_t * wstrID, wchar_t *name, param__params settings, xsd__wstring asyncID, long timeout, struct klprci_RunTaskResponse &r)
{
	HANDLER_BEGIN
		KLSTD_CHKINPTR(name);
		pInstance->AccessCheckTask_InCall(std::wstring(name), KLAVT_READ);
		CAutoPtr<Params> pStatistics;
		ParamsFromSoap(settings, &pStatistics);		
        AutoConnectionInfo ac(soap);
		r.taskId=pInstance->RunTask(name, pStatistics, asyncID, timeout);
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_RunMethodStub(
                            struct soap* soap, 
                            wchar_t * wstrID,
                            wchar_t *name, 
                            param__params settings, 
                            long timeout, 
                            struct klprci_RunMethodResponse &r)
{
	HANDLER_BEGIN
		KLSTD_CHKINPTR(name);
		pInstance->AccessCheckTask_InCall(std::wstring(name), KLAUTH_TASK_CONTROL);
		CAutoPtr<Params> pSettings;
		ParamsFromSoap(settings, &pSettings);
        AutoConnectionInfo ac(soap);
        KLPAR::ParamsPtr pRes;
		pInstance->RunMethod(name, pSettings, timeout, &pRes);
        ParamsForSoap(soap, pRes, r.res);
	HANDLER_END
	return SOAP_OK;
};


SOAP_FMAC1 int SOAP_FMAC2 klprci_GetTasksListStub(struct soap* soap, wchar_t * wstrID, struct klprci_GetTasksListResponse &r)
{
	HANDLER_BEGIN
        KLPRCI::ComponentId idComponent;
        pInstance->GetComponentId(idComponent);
        KLAVT_AccessCheckForComponent_InCall(KLAUTH_COMPONENT_QUERY_INFORMATION, idComponent);
		//pInstance->AccessCheck(pSecContext, KLAUTH_COMPONENT_QUERY_INFORMATION);
		std::vector<long> tasks;
		pInstance->GetTasksList(tasks);
		if(tasks.size())
		{
			r.taskslist.__size=tasks.size();
			r.taskslist.__ptr=(long*)soap_malloc(soap, r.taskslist.__size*sizeof(long));
			for(int i=0; i < r.taskslist.__size; ++i)
				r.taskslist.__ptr[i]=tasks[i];
		};
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_GetTaskNameStub(struct soap* soap, wchar_t * wstrID, long idTask, struct klprci_GetTaskNameResponse &r)
{
	HANDLER_BEGIN		
		std::wstring wstrName;
		pInstance->GetTaskName(idTask, wstrName);
		pInstance->AccessCheckTask_InCall(wstrName, KLAUTH_TASK_QUERY_INFORMATION);
		r.name=soap_strdup(soap, wstrName.c_str());
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_TaskStopStub(struct soap* soap, wchar_t * wstrID, long idTask, struct klprci_TaskStopResponse &r)
{
	HANDLER_BEGIN
		pInstance->AccessCheckTask_InCall(idTask, KLAUTH_TASK_CONTROL);
		pInstance->StopTask(idTask);
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_TaskSuspendStub(struct soap* soap, wchar_t * wstrID, long idTask, struct klprci_TaskSuspendResponse &r)
{
	HANDLER_BEGIN
		pInstance->AccessCheckTask_InCall(idTask, KLAUTH_TASK_CONTROL);
		pInstance->SuspendTask(idTask);
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_TaskResumeStub(struct soap* soap, wchar_t * wstrID, long idTask, struct klprci_TaskResumeResponse &r)
{
	HANDLER_BEGIN
		pInstance->AccessCheckTask_InCall(idTask, KLAUTH_TASK_CONTROL);
		pInstance->ResumeTask(idTask);
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_TaskGetStateStub(struct soap* soap, wchar_t * wstrID, long idTask, struct klprci_GetTaskStateResponse &r)
{
	HANDLER_BEGIN
		pInstance->AccessCheckTask_InCall(idTask, KLAUTH_TASK_QUERY_STATE);
		r.state=pInstance->GetTaskState(idTask);
	HANDLER_END
	return SOAP_OK;
};

SOAP_FMAC1 int SOAP_FMAC2 klprci_TaskGetCompletionStub(struct soap* soap, wchar_t * wstrID, long idTask, struct klprci_GetTaskCompletionResponse &r)
{
	HANDLER_BEGIN
		pInstance->AccessCheckTask_InCall(idTask, KLAUTH_TASK_QUERY_STATE);
		r.percent=pInstance->GetTaskCompletion(idTask);
	HANDLER_END
	return SOAP_OK;
};


SOAP_FMAC1 int SOAP_FMAC2 klprci_TaskReloadStub(
                                    struct soap*                        soap,
                                    xsd__wstring                        wstrID,
						            xsd__int                            idTask,
                                    param__params                       settings,
						            struct klprci_TaskReloadResponse& r)
{
	HANDLER_BEGIN
        pInstance->AccessCheckTask_InCall(idTask, KLAUTH_TASK_CONTROL);
        CAutoPtr<Params> pParams;
        ParamsFromSoap(settings, &pParams);
        pInstance->TaskReload(idTask, pParams);
	HANDLER_END
	return SOAP_OK;
};
