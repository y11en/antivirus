// Test.cpp: implementation of the CTest class.
//
//////////////////////////////////////////////////////////////////////

#include "build/general.h"
#include "std/base/klbase.h"
#include "std/err/error.h"
#include "std/par/paramsi.h"
#include "std/trc/trace.h"
#include "kca/prts/tasksstorage.h"
#include "transport/tr/transport.h"
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

// транспортное имя компоненты
std::wstring g_sTrServerCompName;
KLPRCI::ComponentId g_cidServerComponent(L"KLPRES_TEST_PRODUCT", L"1.0", L"KLPRES_TEST", L"1");



//////////////////////////////////////////////////////////////////////
// CTest
//////////////////////////////////////////////////////////////////////

const wchar_t TEST_EVENT_TYPE_MASK[] = L"TEST_EVENT_%X";
const wchar_t TEST_EVENT_TYPE_1[] = L"TEST_EVENT_TYPE_1";
const wchar_t TEST_EVENT_TYPE_2[] = L"TEST_EVENT_TYPE_2";

const wchar_t EVENT_BODY_PARAM_NAME[] = L"PRES_TEST_EVENT_NAME";
const int EVENT_BODY_PARAM_VAL = 111;
const int EVENT_BODY_PARAM_VAL_2 = 222;

const int NUBER_OF_ADDED_EVENTS = 1;
const int MAX_EVENT_NUMBER = 1;

//////////////////////////////////////////////////////////////////////

CTest::CTest()
{
}

std::wstring CTest::CreateEventType(const int n)
{
	wchar_t szName[100];
	KLSTD_SWPRINTF(szName, sizeof(szName), TEST_EVENT_TYPE_MASK, n);
	return szName;
}

void CTest::AddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	std::wstring sID;
	SubscriptionInfo si;
	si.subscriber = m_cidSubscriber;
	si.filter = TEST_PUBLISHER;
	si.maxEvents = MAX_EVENT_NUMBER;
	si.eventTypes.push_back(TEST_EVENT_TYPE_1);
	si.eventTypes.push_back(TEST_EVENT_TYPE_2);

// Persist subscribe on all publishers and all events persist
	si.isSubscriptionPersist = true;
	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);
	sID.clear();

// Persist subscribe on all publishers and all events non persist
	si.nCloseWriteOpenedJournalTimeout = 20000;
	si.isSubscriptionPersist = true;
	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);

/*	sID.clear();
// Persist subscribe on all publishers and 2 events persist
	si.isSubscriptionPersist = false;////true;

	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);
	sID.clear();// = L"";

// Non persist subscribe on the publisher and 2 events persist
	si.isSubscriptionPersist = false;

	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);
	sID.clear();// = L"";

	si.isSubscriptionPersist = false;

	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);
*/
}

void CTest::VerifyAddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
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

void CTest::AddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs)
{
	CAutoPtr<Params> parEventBody;
	KLPAR_CreateParams(&parEventBody);

	if (bEventType1)
		ADD_PARAMS_VALUE(parEventBody, EVENT_BODY_PARAM_NAME, IntValue, EVENT_BODY_PARAM_VAL)
	else
		ADD_PARAMS_VALUE(parEventBody, EVENT_BODY_PARAM_NAME, IntValue, EVENT_BODY_PARAM_VAL_2)

	for(int i = 0;i < NUBER_OF_ADDED_EVENTS;i++)
	{
		pComponent->Publish(bEventType1?TEST_EVENT_TYPE_1:TEST_EVENT_TYPE_2, parEventBody, -1);
	}
}

void CTest::VerifyAddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs)
{
	for(std::vector<std::wstring>::iterator i = arSubscriptionIDs.begin();i != arSubscriptionIDs.end();i++)
	{
		pEvents->ResetEventsIterator(*i);

		int nEventCount = 0;
		do
		{
			KLPRCI::ComponentId cidPublisher;
			std::wstring sEventType;
			long eventID;
			CAutoPtr<Params> parEventBody;
			time_t timeBirth;

			if (!pEvents->GetEvent(cidPublisher, sEventType, &parEventBody, timeBirth, eventID)) break;

			// Verify event
			if (sEventType != (bEventType1?TEST_EVENT_TYPE_1:TEST_EVENT_TYPE_2)) 
			KLSTD_THROW(STDE_BADFORMAT);

			nEventCount++;
		}
		while(pEvents->NextEvent());

		if (nEventCount != NUBER_OF_ADDED_EVENTS)
			KLSTD_THROW(STDE_BADFORMAT);
	}
}

void CTest::RemoveExpiredEvents(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	for(std::vector<std::wstring>::iterator i = arSubscriptionIDs.begin();i != arSubscriptionIDs.end();i++)
	{
		pEvents->RemoveExpiredEvents(*i);
	}
}

void CTest::RemoveAllEventsAndValidate(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	for(std::vector<std::wstring>::iterator i = arSubscriptionIDs.begin();i != arSubscriptionIDs.end();i++)
	{
		RemoveEvents(*i, true, pEvents, arSubscriptionIDs);
	}
}

void CTest::RemoveRanges(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	for(std::vector<std::wstring>::iterator i = arSubscriptionIDs.begin();i != arSubscriptionIDs.end();i++)
	{
        RangeVector rangeVector;
        rangeVector.push_back( Range( 2, 100 ) );
        pEvents->DeleteRanges( *i, rangeVector );
        long nRecCount = pEvents->GetEventsCount( *i );
        if( nRecCount != 1 )
        {
            KLSTD_TRACE2( 1, L"Error: after DeleteRanges record count is %d for %ls\n", nRecCount, i->c_str() );
        }
	}
}

void CTest::RemoveEvents(std::wstring sSubscriber, bool bUseClearEventsFunction, EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	pEvents->PauseSubscription(sSubscriber);
	pEvents->ClearEventsList(sSubscriber);

	// Must be empty list
	std::wstring wstrJournalID = pEvents->ResetEventsIteratorEx(sSubscriber);
	if (pEvents->IsEvent())
	{
		KLSTD_THROW(STDE_BADFORMAT);
	}
}

void CTest::RemoveSubscriptionsAndVerify(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	KLSTD_TRACE0(1, L"CTest::RemoveSubscriptionsAndVerify");
	for(std::vector<std::wstring>::iterator i = arSubscriptionIDs.begin();i != arSubscriptionIDs.end();i++)
	{
		KLSTD_TRACE1(1, L"CTest::RemoveSubscriptionsAndVerify::StopSubscription(%s)", (*i).c_str());
		pEvents->StopSubscription(*i);
	}

	SubscriptionInfo* si = NULL;
	std::wstring s;

	KLSTD_TRACE0(1, L"CTest::RemoveSubscriptionsAndVerify::ResetSubscriptionsIterator");
	pEvents->ResetSubscriptionsIterator(m_cidSubscriber);
	if (pEvents->GetNextSubscription(&si, s))
	{
		KLSTD_THROW(STDE_BADFORMAT);
	}
}

bool CTest::Run()
{
	std::wstring trLocalCompName;
	CAutoPtr<KLPRCI::ComponentInstance> pComponent;

	try
	{
		m_cidSubscriber.productName = L"KLPRES_TEST_PRODUCT";
		m_cidSubscriber.version = L"1.0";
		m_cidSubscriber.componentName = L"KLPRES_TEST_SUBSCRIBER";
		m_cidSubscriber.instanceId = KLSTD_CreateLocallyUniqueString();

		TEST_PUBLISHER.productName = L"KLPRES_TEST_PRODUCT";
		TEST_PUBLISHER.version = L"1.0";
		TEST_PUBLISHER.componentName = L"KLPRES_TEST_PUBLISHER";
		TEST_PUBLISHER.instanceId = KLSTD_CreateLocallyUniqueString();

		time(&m_time);

		KLPRCI::ComponentId cidLocalComponent( L"KLPRES_TEST_PRODUCT", L"1.0", KLSTD_CreateLocallyUniqueString().c_str(), L"1");

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
// Add events 5 events of type1
		AddEvents(pEvents, true, pComponent, arSubscriptionIDs);

		KLSTD_Sleep(10000);	// Wait while events will be added by eventing thread
		
		try
		{
			VerifyAddEvents(pEvents, true, pComponent, arSubscriptionIDs);
		}
		catch(Error* err)
		{
			std::wstring s = err->GetMsg();
			throw;
		}

	// Add events 5 events of type2. 
		AddEvents(pEvents, false, pComponent, arSubscriptionIDs);
		KLSTD_Sleep(3000);	// Wait while events will be added by eventing thread

	//// Now we remove expired events of type1, because
	//// all of subscriptions have maxEvent = MAX_EVENT_NUMBER
	//	RemoveExpiredEvents(pEvents, arSubscriptionIDs);
	//
	//	try
	//	{
	//		VerifyAddEvents(pEvents, false, pComponent, arSubscriptionIDs);
	//	}
	//	catch(Error* err)
	//	{
	//		std::wstring s = err->GetMsg();
	//		throw;
	//	}
    
		RemoveRanges(pEvents, arSubscriptionIDs);

		RemoveAllEventsAndValidate(pEvents, arSubscriptionIDs);

		RemoveSubscriptionsAndVerify(pEvents, arSubscriptionIDs);

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
