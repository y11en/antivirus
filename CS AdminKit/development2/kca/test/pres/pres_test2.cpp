// Test.cpp: implementation of the CTest2 class.
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

#define KLCS_MODULENAME			L"PRES_TEST"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRES;

//////////////////////////////////////////////////////////////////////

// транспортное имя компоненты
//std::wstring g_sTrServerCompName;
//KLPRCI::ComponentId g_cidServerComponent(L"KLPRES:TEST_PRODUCT", L"1.0", L"KLPRES:TEST", L"1");


//////////////////////////////////////////////////////////////////////
// CTest2
//////////////////////////////////////////////////////////////////////

const wchar_t TEST_EVENT_TYPE_MASK[] = L"TEST_EVENT_%X";
const wchar_t TEST_EVENT_TYPE_1[] = L"TEST_EVENT_TYPE_1";
const wchar_t TEST_EVENT_TYPE_2[] = L"TEST_EVENT_TYPE_2";

const wchar_t EVENT_BODY_NOT_SAVED_PARAM_NAME[] = L"PRES_TEST_NOT_SAVED_EVENT_NAME";
const wchar_t EVENT_BODY_PARAM_NAME[] = L"PRES_TEST_EVENT_NAME";
const wchar_t EVENT_BODY_PARAM_PARAM_TEXT[] = L"PRES_TEST_BODY_PARAM_PARAM_TEXT";
const wchar_t EVENT_BODY_PARAM_PARAM[] = L"PRES_TEST_BODY_PARAM_PARAM";

const int EVENT_BODY_PARAM_VAL = 111;
const int EVENT_BODY_PARAM_VAL_2 = 222;

const int NUBER_OF_ADDED_EVENTS = 5;
const int MAX_EVENT_NUMBER = 5;

//////////////////////////////////////////////////////////////////////

CTest2::CTest2()
{
}

std::wstring CTest2::CreateEventType(const int n)
{
	wchar_t szName[100];
	KLSTD_SWPRINTF(szName, sizeof(szName), TEST_EVENT_TYPE_MASK, n);
	return szName;
}

void CTest2::AddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	std::wstring sID;
	SubscriptionInfo si;
	si.subscriber = m_cidSubscriber;
	si.nCloseWriteOpenedJournalTimeout = 100;
	si.filter = TEST_PUBLISHER;
	si.maxEvents = MAX_EVENT_NUMBER;
	si.eventTypes.push_back(TEST_EVENT_TYPE_1);
	si.eventTypes.push_back(TEST_EVENT_TYPE_2);
	si.bUseRecNumIndex = true;

	// Mask
	//
	// Params
	//	|
	//	+-- EVENT_CLASS_MASK_EVENT_TYPE
	//	+-- EVENT_CLASS_MASK_EVENT_TIME
	//	+-- EVENT_CLASS_MASK_EVENT_PARAMS	[ params ]
	//		|
	//		+--EVENT_BODY_PARAM_NAME
	//		+--EVENT_BODY_PARAM_PARAM		[ params ]
	//			|
	//			+--EVENT_BODY_PARAM_PARAM_TEXT

	CAutoPtr<Params> parBodyParams;
	KLPAR_CreateParams(&parBodyParams);
	ADD_PARAMS_VALUE(parBodyParams, EVENT_BODY_PARAM_PARAM_TEXT, StringValue, 0);

	CAutoPtr<Params> parBody;
	KLPAR_CreateParams(&parBody);
	ADD_PARAMS_VALUE(parBody, EVENT_BODY_PARAM_NAME, IntValue, 0);
	ADD_PARAMS_VALUE(parBody, EVENT_BODY_PARAM_PARAM, ParamsValue, parBodyParams);

	KLPAR_CreateParams(&si.parEventParametersMask);
	ADD_PARAMS_VALUE(si.parEventParametersMask, EVENT_CLASS_MASK_EVENT_TYPE, IntValue, 0);
	ADD_PARAMS_VALUE(si.parEventParametersMask, EVENT_CLASS_MASK_EVENT_TIME, DateTimeValue, 0);
	ADD_PARAMS_VALUE(si.parEventParametersMask, EVENT_CLASS_MASK_EVENT_PARAMS, ParamsValue, parBody);

// Persist subscribe on all publishers and all events persist
	si.isSubscriptionPersist = true;
	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);
/*
// Persist subscribe on all publishers and all events non persist
	si.isSubscriptionPersist = true;
	sID = L"";
	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);

// Persist subscribe on all publishers and 2 events persist
	si.isSubscriptionPersist = true;
	sID = L"";
	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);

// Non persist subscribe on the publisher and 2 events persist
	si.isSubscriptionPersist = false;
	sID = L"";
	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);

// Non persist subscribe on the publisher and 2 events non persist
	si.isSubscriptionPersist = false;
	sID = L"";
	pEvents->StartSubscription(si, sID);
	arSubscriptionIDs.push_back(sID);
	*/
}

void CTest2::VerifyAddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
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
		KLSTD_THROW(STDE_BADFORMAT);
}

void CTest2::AddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs)
{
	CAutoPtr<Params> parEventBody;
	KLPAR_CreateParams(&parEventBody);

	// this param will not be saved in storage !!!
	ADD_PARAMS_VALUE(parEventBody, EVENT_BODY_NOT_SAVED_PARAM_NAME, IntValue, EVENT_BODY_PARAM_VAL)

	CAutoPtr<Params> parBodyParams;
	KLPAR_CreateParams(&parBodyParams);
	ADD_PARAMS_VALUE(parBodyParams, EVENT_BODY_PARAM_PARAM_TEXT, StringValue, L"event_body_param_param_text");
	ADD_PARAMS_VALUE(parBodyParams, EVENT_BODY_NOT_SAVED_PARAM_NAME, StringValue, L"not saved param param");

	ADD_PARAMS_VALUE(parEventBody, EVENT_BODY_PARAM_PARAM, ParamsValue, parBodyParams);
	if (bEventType1)
		ADD_PARAMS_VALUE(parEventBody, EVENT_BODY_PARAM_NAME, IntValue, EVENT_BODY_PARAM_VAL)
	else
		ADD_PARAMS_VALUE(parEventBody, EVENT_BODY_PARAM_NAME, IntValue, EVENT_BODY_PARAM_VAL_2)

	for(int i = 0;i < NUBER_OF_ADDED_EVENTS;i++)
	{
		pComponent->Publish(bEventType1?TEST_EVENT_TYPE_1:TEST_EVENT_TYPE_2, parEventBody, -1);
	}

	KLSTD_Sleep(15000);
}

void CTest2::VerifyAddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs)
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
			time_t timeBirth = 0;

			if (!pEvents->GetEvent(cidPublisher, sEventType, &parEventBody, timeBirth, eventID)) break;

			CAutoPtr<Params> parParamParam;
			GET_PARAMS_VALUE(parEventBody, EVENT_BODY_PARAM_PARAM, ParamsValue, PARAMS_T, parParamParam);
			std::wstring sParamText;
			GET_PARAMS_VALUE(parParamParam, EVENT_BODY_PARAM_PARAM_TEXT, StringValue, STRING_T, sParamText);
			bool bOk = false;
			try
			{
				int n;
				GET_PARAMS_VALUE(parEventBody, EVENT_BODY_NOT_SAVED_PARAM_NAME, IntValue, INT_T, n);
				GET_PARAMS_VALUE(parParamParam, EVENT_BODY_NOT_SAVED_PARAM_NAME, StringValue, STRING_T, sParamText);
			}
			catch(KLERR::Error* pErr)
			{
				bOk = true;
				pErr->Release();
			}

			// Verify event
			if (!bOk ||
				sEventType != (bEventType1?TEST_EVENT_TYPE_1:TEST_EVENT_TYPE_2) ||
				timeBirth == 0 ||
				!cidPublisher.productName.empty() ||        
				!cidPublisher.version.empty() ||
				!cidPublisher.componentName.empty() ||
				!cidPublisher.instanceId.empty())
			{
				KLSTD_THROW(STDE_BADFORMAT);
			}

			nEventCount++;
		}
		while(pEvents->NextEvent());

		if (nEventCount != NUBER_OF_ADDED_EVENTS)
		{
			KLSTD_TRACE1(1, L"[KLPRESTEST] STDE_BADFORMAT - (%s)\n", (*i).c_str() );
			KLSTD_THROW(STDE_BADFORMAT);
		}
	}
}

void CTest2::RemoveExpiredEvents(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	for(std::vector<std::wstring>::iterator i = arSubscriptionIDs.begin();i != arSubscriptionIDs.end();i++)
	{
		pEvents->RemoveExpiredEvents(*i);
	}
}

void CTest2::RemoveAllEventsAndValidate(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	for(std::vector<std::wstring>::iterator i = arSubscriptionIDs.begin();i != arSubscriptionIDs.end();i++)
	{
		RemoveEvents(*i, true, pEvents, arSubscriptionIDs);
	}
}

void CTest2::RemoveEvents(std::wstring sSubscriber, bool bUseClearEventsFunction, EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	pEvents->PauseSubscription(sSubscriber);
	pEvents->ClearEventsList(sSubscriber);

	// Must be empty list
	pEvents->ResetEventsIterator(sSubscriber);
	if (pEvents->IsEvent())
	{
		KLSTD_THROW(STDE_BADFORMAT);
	}
}

void CTest2::RemoveSubscriptionsAndVerify(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs)
{
	for(std::vector<std::wstring>::iterator i = arSubscriptionIDs.begin();i != arSubscriptionIDs.end();i++)
	{
		pEvents->StopSubscription(*i);
	}

	SubscriptionInfo* si = NULL;
	std::wstring s;

	pEvents->ResetSubscriptionsIterator(m_cidSubscriber);
	if (pEvents->GetNextSubscription(&si, s))
	{
		KLSTD_THROW(STDE_BADFORMAT);
	}
}

bool CTest2::Run()
{
	bool bResult = true;
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

	// Perform test
	KLPRCI::ComponentId cidLocalComponent( L"KLPRES_TEST_PRODUCT", L"1.0", KLSTD_CreateLocallyUniqueString().c_str(), L"1");

	KLTRAP::ConvertComponentIdToTransportName(trLocalCompName, cidLocalComponent);
	KLTR_GetTransport()->AddClientConnection(trLocalCompName.c_str(), g_sTrServerCompName.c_str(), TR_SERVER_ADDRESSL);

	CAutoPtr<EventsStorage> pEvents;
	KLPRES_CreateEventsStorageProxy(std::wstring(EVENT_STORAGE_NAME_LOCAL), cidLocalComponent, g_cidServerComponent, &pEvents);

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
	KLSTD_Sleep(3000);	// Wait while events will be added by eventing thread
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

// Now we remove expired events of type1, because
// all of subscriptions have maxEvent = MAX_EVENT_NUMBER
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

	return bResult;
};
