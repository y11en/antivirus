// Test.cpp: implementation of the CTest4 class.
//
//////////////////////////////////////////////////////////////////////

#include "build/general.h"
#include "std/base/klbase.h"
#include "std/err/error.h"
#include "std/par/paramsi.h"
#include "std/trc/trace.h"
#include "kca/prts/tasksstorage.h"
#include "transport/tr/transport.h"
#include "transport/ev/eventsource.h"
#include <kca/prts/prxsmacros.h>

#ifdef _WIN32
	#include <crtdbg.h>
#endif

#include "pres_test.h"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRES;

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CTest4
//////////////////////////////////////////////////////////////////////

const wchar_t TEST_EVENT_TYPE_1[] = L"TEST_EVENT_TYPE_1";

const int NUMBER_OF_ADDING_EVENTS = 100;
const int MAX_EVENT_NUMBER = 100;

//////////////////////////////////////////////////////////////////////

CTest4::CTest4()
{
}

void CTest4::AddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	std::wstring sID;
	SubscriptionInfo si;
	si.subscriber = m_cidSubscriber;
	si.filter = TEST_PUBLISHER;
	si.maxEvents = MAX_EVENT_NUMBER;
	si.eventTypes.push_back(TEST_EVENT_TYPE_1);

// Persist subscribe on all publishers and all events non persist
//	si.nCloseWriteOpenedJournalTimeout = 2000;
	si.isSubscriptionPersist = true;
	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);
}

void CTest4::VerifyAddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	pEvents->ResetSubscriptionsIterator(m_cidSubscriber);

	int nSubscriptions = 0;
	std::wstring sID;
    do
	{
		CPointer<SubscriptionInfo> pSI;
		if (!pEvents->GetNextSubscription(&pSI, sID)) break;

		if (COMPARE_COMPONENT_ID(pSI->subscriber, m_cidSubscriber)) 
			nSubscriptions++;
		else
			KLSTD_THROW(STDE_BADFORMAT);
	}
	while(true);

	if ((unsigned)nSubscriptions != arSubscriptionIDs.size()) 
	{
		KLSTD_TRACE2( 1, L"Error: nSubscriptions  - %d arSubscriptionIDs.size - %d\n", nSubscriptions, arSubscriptionIDs.size() );
		KLSTD_THROW(STDE_BADFORMAT);
	}
}

void CTest4::AddEvents(EventsStorage* pEvents, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs)
{
	CAutoPtr<Params> parEventBody;
	KLPAR_CreateParams(&parEventBody);
	ADD_PARAMS_VALUE(parEventBody, L"TEST_PARAM_111", IntValue, 111);

	for(int i = 0;i < NUMBER_OF_ADDING_EVENTS;i++)
	{
		pComponent->Publish(TEST_EVENT_TYPE_1, parEventBody, -1);
//		if ((i % 300) == 0) Sleep(1000);
	}
}

void CTest4::VerifyAddEvents(EventsStorage* pEvents, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs)
{
	for(std::vector<std::wstring>::iterator i = arSubscriptionIDs.begin();i != arSubscriptionIDs.end();i++)
	{
		long nCount = pEvents->GetEventsCount(*i);
		pEvents->MoveToEventWithIndex(*i, nCount - 1);

		KLPRCI::ComponentId id;
		std::wstring wstrType;
		KLSTD::CAutoPtr<KLPAR::Params> parBody;
		time_t t;
		long lEventID;
		pEvents->GetEvent(id, wstrType, &parBody, t, lEventID, *i);

		if (lEventID != ((NUMBER_OF_ADDING_EVENTS * 2) - 1)) KLSTD_THROW(STDE_BADFORMAT);
	}
}

class CShrinkWorker : public KLSTD::KLBaseImpl<KLTP::ThreadsPool::Worker>
{
public:
	CShrinkWorker(EventsStorage* pEventsStorage, const std::wstring& wstrSubscriptionID, bool& bCancel):
		m_pEventsStorage(pEventsStorage),
		m_wstrSubscriptionID(wstrSubscriptionID),
		m_bCancel(bCancel)
	{
	}
	virtual int RunWorker(KLTP::ThreadsPool::WorkerId WID)
	{
		while(!m_bCancel)
		{
			KLERR_BEGIN
			{
				m_pEventsStorage->DeleteEvents(m_wstrSubscriptionID, 0);
				KLSTD_Sleep(1000);
			}
			KLERR_ENDT(1)
		}

		return 0;
	}

private:
	bool& m_bCancel;
	KLSTD::CAutoPtr<KLPRES::EventsStorage> m_pEventsStorage;
	std::wstring m_wstrSubscriptionID;
};

bool CTest4::Run()
{
	std::wstring trLocalCompName;
	CAutoPtr<KLPRCI::ComponentInstance> pComponent;

	try
	{
		m_cidSubscriber.productName = L"KLPRES_TEST_PRODUCT_4";
		m_cidSubscriber.version = L"4.0.0.0";
		m_cidSubscriber.componentName = L"KLPRES_TEST_SUBSCRIBER_4";
		m_cidSubscriber.instanceId = KLSTD_CreateLocallyUniqueString();

		TEST_PUBLISHER.productName = L"KLPRES_TEST_PRODUCT_4";
		TEST_PUBLISHER.version = L"4.0.0.0";
		TEST_PUBLISHER.componentName = L"KLPRES_TEST_PUBLISHER_4";
		TEST_PUBLISHER.instanceId = KLSTD_CreateLocallyUniqueString();

		time(&m_time);

		KLPRCI::ComponentId cidLocalComponent(L"KLPRES_TEST_PRODUCT_4", 
											  L"4.0.0.0", 
											  KLSTD_CreateLocallyUniqueString().c_str(), 
											  L"1");
		KLTRAP::ConvertComponentIdToTransportName(trLocalCompName, cidLocalComponent);
		KLTR_GetTransport()->AddClientConnection(trLocalCompName.c_str(), g_sTrServerCompName.c_str(), TR_SERVER_ADDRESSL);

		CAutoPtr<EventsStorage> pEvents;
		KLPRES_CreateEventsStorageProxy(EVENT_STORAGE_NAME_LOCAL, cidLocalComponent, g_cidServerComponent, &pEvents);

		KLPRCI_CreateComponentInstance(TEST_PUBLISHER, &pComponent);

		std::vector<std::wstring> arSubscriptionIDs;

// Add subscriptions
		AddSubscriptions(pEvents, arSubscriptionIDs);
		try
		{
			VerifyAddSubscriptions(pEvents, arSubscriptionIDs);
		}
		catch(Error* err)
		{
			std::wstring s = err->GetMsg();
			throw;
		}

		KLPRES_CreateEventsStorageServer(EVENT_STORAGE_NAME_LOCAL2, L"product", L"version", SUBSCRIPTION_STORAGE_FILE_PATH_W, EVENTS_STORAGE_FOLDER_W);

		CAutoPtr<EventsStorage> pEvents2;
		KLPRES_CreateEventsStorageProxy(EVENT_STORAGE_NAME_LOCAL2, cidLocalComponent, g_cidServerComponent, &pEvents2);

		bool bCancel = false;

		KLTP::ThreadsPool* pGlobalTreadsPool = KLTP_GetThreadsPool();

		KLTP::ThreadsPool::WorkerId widRefreshingNodeWorker;
		KLSTD::CAutoPtr<CShrinkWorker> pWorker;
		pWorker.Attach(new CShrinkWorker(pEvents2, arSubscriptionIDs[0], bCancel));
		pGlobalTreadsPool->AddWorker(&widRefreshingNodeWorker, 
									 KLSTD_CreateGUIDString().c_str(), 
									 pWorker, 
									 KLTP::ThreadsPool::RunOnce, 
									 true, 
									 0);

// Add events
		AddEvents(pEvents, pComponent, arSubscriptionIDs);

//		KLSTD_Sleep(60000);	// Wait while events will be added by eventing thread
		KLEV_GetEventSource()->WaitForCleanEventsQueue(trLocalCompName, true);
		
		bCancel = true;
		pGlobalTreadsPool->DeleteWorker(widRefreshingNodeWorker);

		try
		{
			VerifyAddEvents(pEvents, pComponent, arSubscriptionIDs);
		}
		catch(Error* err)
		{
			std::wstring s = err->GetMsg();
			throw;
		}

		pComponent->SetState(KLPRCI::STATE_SHUTTING_DOWN);
		KLTR_GetTransport()->CloseClientConnection(trLocalCompName.c_str(), g_sTrServerCompName.c_str());
	}
	catch(...)
	{
        if(pComponent)
		    pComponent->SetState(KLPRCI::STATE_SHUTTING_DOWN);
		KLTR_GetTransport()->CloseClientConnection(trLocalCompName.c_str(), g_sTrServerCompName.c_str());
		throw;
	}

	return true;
};
