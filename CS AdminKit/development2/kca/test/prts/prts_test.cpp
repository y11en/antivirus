// Test.cpp: implementation of the CPRTSTest class.
//
//////////////////////////////////////////////////////////////////////

#include "build/general.h"
#include "std/base/klbase.h"
#include "std/err/error.h"
#include "std/trc/trace.h"
#include "std/conv/klconv.h"
#include "soapH.h"
#include "std/par/paramsi.h"
#include "kca/prts/tasksstorage.h"
#include "transport/tr/transport.h"
#include <kca/prts/prxsmacros.h>

#ifdef _WIN32
# include <crtdbg.h>
#endif

#include "prts_test.h"

#define KLCS_MODULENAME L"PRTS_TEST"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRTS;

#include <testmodule.h>
#include <helpers.h>

//////////////////////////////////////////////////////////////////////

// транспортное имя компоненты
std::wstring g_sPRTSTrServerCompName;
KLPRCI::ComponentId g_cidPRTSServerComponent(L"KLPRTS:TEST_PRODUCT", L"", L"KLPRTS:TEST", L"1");

//////////////////////////////////////////////////////////////////////
// CPRTSTest
//////////////////////////////////////////////////////////////////////

const wchar_t TASK_INTEGER_PARAMETER[] = L"TASK_INTEGER_PARAMETER";
const wchar_t TASK_NAME_TEMPLATE[] = L"TASK:%d";
const wchar_t TASK_UPDATED_NAME_TEMPLATE[] = L"UPDATED_TASK:%X";

const wchar_t TASK_CID_INSTANCE_TEMPLATE[] = L"INSTANCE:%d";
const wchar_t TASK_CID_PRODUCT_NAME[] = L"KLTESTER";
const wchar_t TASK_CID_VERSION[] = L"1.0";

//////////////////////////////////////////////////////////////////////

CPRTSTest::CPRTSTest()
{
	time(&m_time);
}

void CPRTSTest::AddTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs)
{
	KLSTD_CHKINPTR(pTasks);

	std::wstring name;
	KLPRCI::ComponentId filter;

	pTasks->ResetTasksIterator(filter, name);

	wchar_t szName[200], szInstance[200];

	for(int i = 0;i < 10;i++)
	{
		CPointer<KLSCH::Task> pTask = KLSCH_CreateTask();
		KLSTD_CHKMEM(pTask);

#if defined (WIN32) || defined(N_PLAT_NLM)
		swprintf(szName, TASK_NAME_TEMPLATE, i);
		swprintf(szInstance, TASK_CID_INSTANCE_TEMPLATE, i);
#endif
#ifdef __unix
        swprintf(szName, 200, TASK_NAME_TEMPLATE, i);
		swprintf(szInstance, 200, TASK_CID_INSTANCE_TEMPLATE, i);
#endif
		
		pTask->SetLastExecutionTime(m_time, 0);

		CAutoPtr<Params> parTaskParams;
		KLPAR_CreateParams(&parTaskParams);
		ADD_PARAMS_VALUE(parTaskParams, TASK_INTEGER_PARAMETER, IntValue, i);

		KLPRCI::ComponentId id(TASK_CID_PRODUCT_NAME, TASK_CID_VERSION, m_sComponentName, szInstance);
		arTaskUniqueIDs.push_back(pTasks->AddTask(id, szName, pTask, parTaskParams));
	}
}

void CPRTSTest::VerifyAddTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs)
{
	KLSTD_CHKINPTR(pTasks);

	std::wstring name;
	KLPRCI::ComponentId filter;

	wchar_t szName[200], szInstance[200];
	int nTaskNumber = 0;

	for(std::vector<std::wstring>::iterator i = arTaskUniqueIDs.begin();i != arTaskUniqueIDs.end();i++)
	{
		std::wstring sTaskName;
		KLPRCI::ComponentId id;
		bool bStartOnEvent;
		CPointer<KLSCH::Task> pTask;
		int nTaskIntegerParam;

#ifdef WIN32
		swprintf(szName, TASK_NAME_TEMPLATE, nTaskNumber);
		swprintf(szInstance, TASK_CID_INSTANCE_TEMPLATE, nTaskNumber);
#endif
#ifdef N_PLAT_NLM
		swprintf(szName, TASK_NAME_TEMPLATE, nTaskNumber);
		swprintf(szInstance, TASK_CID_INSTANCE_TEMPLATE, nTaskNumber);
#endif
#ifdef __unix
		swprintf(szName, 200, TASK_NAME_TEMPLATE, nTaskNumber);
		swprintf(szInstance, 200, TASK_CID_INSTANCE_TEMPLATE, nTaskNumber);
#endif

		CAutoPtr<Params> parTaskParams;
		pTasks->GetTaskByID(*i, id, sTaskName, bStartOnEvent, &pTask, &parTaskParams);
		GET_PARAMS_VALUE(parTaskParams, TASK_INTEGER_PARAMETER, IntValue, INT_T, nTaskIntegerParam);

		int n;
		time_t t;
		pTask->GetLastExecutionTime(t, n);
		
		KLSTD_USES_CONVERSION;

		std::wstring s = KLSTD_A2W(ctime( &m_time ));
		std::wstring s1 = KLSTD_A2W(ctime( &t ));
		if (sTaskName != szName ||
			id.productName != TASK_CID_PRODUCT_NAME ||
			id.version != TASK_CID_VERSION ||
			id.componentName != m_sComponentName ||
			id.instanceId != szInstance ||
			nTaskIntegerParam != nTaskNumber ||
			t != m_time ||
			n != 0)
		{
			KLSTD_THROW(STDE_BADFORMAT);
		}

		nTaskNumber++;
	}
}

void CPRTSTest::UpdateTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs)
{
	KLSTD_CHKINPTR(pTasks);

	std::wstring name;
	KLPRCI::ComponentId filter;

	wchar_t szName[200];

	for(std::vector<std::wstring>::iterator i = arTaskUniqueIDs.begin();i != arTaskUniqueIDs.end();i++)
	{
		std::wstring sTaskName;
		KLPRCI::ComponentId id;
		bool bStartOnEvent;
		CPointer<KLSCH::Task> pTask;

		CAutoPtr<Params> parTaskParams;
		pTasks->GetTaskByID(*i, id, sTaskName, bStartOnEvent, &pTask, &parTaskParams);

#if defined (WIN32) || defined(N_PLAT_NLM)
//#ifdef WIN32
		swprintf(szName, TASK_UPDATED_NAME_TEMPLATE, (*i).data() );
#endif
#ifdef __unix
        swprintf(szName, 200, TASK_UPDATED_NAME_TEMPLATE, (*i).data() );
#endif
		sTaskName = szName;
		pTasks->UpdateTask(*i, id, sTaskName, pTask, parTaskParams);
	}
}

void CPRTSTest::VerifyUpdateTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs)
{
	KLSTD_CHKINPTR(pTasks);

	std::wstring name;
	KLPRCI::ComponentId filter;

	int nTaskNumber = 0;
	wchar_t szName[200], szInstance[200];

	for(std::vector<std::wstring>::iterator i = arTaskUniqueIDs.begin();i != arTaskUniqueIDs.end();i++)
	{
		std::wstring sTaskName;
		KLPRCI::ComponentId id;
		bool bStartOnEvent;
		CPointer<KLSCH::Task> pTask;
		int nTaskIntegerParam;

		CAutoPtr<Params> parTaskParams;
		pTasks->GetTaskByID(*i, id, sTaskName, bStartOnEvent, &pTask, &parTaskParams);
		GET_PARAMS_VALUE(parTaskParams, TASK_INTEGER_PARAMETER, IntValue, INT_T, nTaskIntegerParam);

		int n;
		time_t t;
		pTask->GetLastExecutionTime(t, n);

#if defined (WIN32) || defined(N_PLAT_NLM)
//#ifdef WIN32
		swprintf(szName, TASK_UPDATED_NAME_TEMPLATE, (*i).data() );
		swprintf(szInstance, TASK_CID_INSTANCE_TEMPLATE, nTaskNumber);
#endif
#ifdef __unix
		swprintf(szName, 200, TASK_UPDATED_NAME_TEMPLATE, (*i).data() );
		swprintf(szInstance, 200, TASK_CID_INSTANCE_TEMPLATE, nTaskNumber);
#endif

		if (sTaskName != szName ||
			id.productName != TASK_CID_PRODUCT_NAME ||
			id.version != TASK_CID_VERSION ||
			id.componentName != m_sComponentName ||
			id.instanceId != szInstance ||
			t != m_time ||
			n != 0)
		{
			KLSTD_THROW(STDE_BADFORMAT);
		}

		nTaskNumber++;
	}
}

void CPRTSTest::DeleteTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs)
{
	// Iterate only through tasks added by this instance of test
	KLPRCI::ComponentId filter(TASK_CID_PRODUCT_NAME, TASK_CID_VERSION, m_sComponentName, L"");
	std::wstring name;

	pTasks->ResetTasksIterator(filter, name);

	std::wstring TaskID;
	CAutoPtr<Params> params;
	CPointer<KLSCH::Task> pTask;
	bool bStartOnEvent;

	while(pTasks->GetNextTask(TaskID, filter, name, bStartOnEvent, &pTask, &params))
	{
		if (filter.componentName == m_sComponentName &&	IsItMyOwnTask(TaskID, arTaskUniqueIDs))
			pTasks->DeleteTask(TaskID);

		pTask = NULL;
		params = NULL;
	}
}

void CPRTSTest::VerifyDeleteTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs)
{
	// Iterate through all tasks
	KLPRCI::ComponentId filter;
	std::wstring name;

	pTasks->ResetTasksIterator(filter, name);

	std::wstring TaskID;
	CAutoPtr<Params> params;
	CPointer<KLSCH::Task> pTask;
	bool bStartOnEvent;

	while(pTasks->GetNextTask(TaskID, filter, name, bStartOnEvent, &pTask, &params))
	{
		if (filter.componentName == m_sComponentName &&	IsItMyOwnTask(TaskID, arTaskUniqueIDs)) 
			KLSTD_THROW(STDE_BADFORMAT);

		pTask = NULL;
		params = NULL;
	}
}

bool CPRTSTest::IsItMyOwnTask(std::wstring TaskID, std::vector<std::wstring>& arTaskUniqueIDs)
{
	for(std::vector<std::wstring>::iterator i = arTaskUniqueIDs.begin();i != arTaskUniqueIDs.end();i++)
		if (*i == TaskID) return true;

	return false;
}

bool CPRTSTest::Run()
{
	bool bResult = true;

	// Perform test
	std::wstring trLocalCompName;

	m_sComponentName = KLSTD_CreateLocallyUniqueString();
	KLPRCI::ComponentId cidLocalComponent( L"KLPRTS:TEST_PRODUCT", L"", m_sComponentName.c_str(), L"1");
	KLTRAP::ConvertComponentIdToTransportName(trLocalCompName, cidLocalComponent);

	KLTR_GetTransport()->AddClientConnection(trLocalCompName.c_str(), g_sPRTSTrServerCompName.c_str(), TR_PRTS_SERVER_ADDRESSL);
	KLTR_GetTransport()->SetTimeouts(600000, 600000, 600000);

	CAutoPtr<TasksStorage> pTasks;
	KLPRTS_CreateTasksStorageProxy(KLPRTS_TASK_STORAGE_TEST, cidLocalComponent, g_cidPRTSServerComponent, &pTasks);

	std::vector<std::wstring> arTaskUniqueIDs;

// Add 10 tasks
	AddTasks(pTasks, arTaskUniqueIDs);
	VerifyAddTasks(pTasks, arTaskUniqueIDs);

// Update added tasks
	UpdateTasks(pTasks, arTaskUniqueIDs);
	VerifyUpdateTasks(pTasks, arTaskUniqueIDs);

// Delete added tasks
	DeleteTasks(pTasks, arTaskUniqueIDs);
	VerifyDeleteTasks(pTasks, arTaskUniqueIDs);

	KLTR_GetTransport()->CloseClientConnection(trLocalCompName.c_str(), g_sPRTSTrServerCompName.c_str());

	return bResult;
};
