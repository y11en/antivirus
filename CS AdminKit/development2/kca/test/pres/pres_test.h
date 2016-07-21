// Test.h: interface for the CTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PRES_TEST_H)
#define PRES_TEST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "build/general.h"
#include <kca/prci/componentinstance.h>
#include <kca/pres/eventsstorage.h>
#include <exception>
#include <list>
#include <vector>

#include <testmodule.h>
#include <helpers.h>

using namespace KLPAR;
using namespace KLERR;
using namespace KLSTD;
using namespace KLPRES;


	
using namespace KLTST2;

//////////////////////////////////////////////////////////////////////

#define KLCS_MODULENAME							L"PRES_TEST"

#ifdef _WIN32
#define SUBSCRIPTION_STORAGE_ROOT_W				L"c:\\pres_test"
#define SUBSCRIPTION_STORAGE_ROOT				TEXT("c:\\pres_test")
#define SUBSCRIPTION_STORAGE_FILE_PATH_W	    SUBSCRIPTION_STORAGE_ROOT_W L"\\pr_subsrc.xml"
#define SUBSCRIPTION_STORAGE_FILE_PATH_CTRL_W	SUBSCRIPTION_STORAGE_ROOT_W L"\\pr_subsrc.ctrl"
#elif defined (N_PLAT_NLM)
#define SUBSCRIPTION_STORAGE_ROOT_W				L"sys:/klab/adminkit/test/csdata/pres_test"
#define SUBSCRIPTION_STORAGE_ROOT				L"sys:/klab/adminkit/test/csdata/pres_test"
#define SUBSCRIPTION_STORAGE_FILE_PATH_W	    SUBSCRIPTION_STORAGE_ROOT_W L"/pr_subsrc.xml"
#define SUBSCRIPTION_STORAGE_FILE_PATH_CTRL_W	SUBSCRIPTION_STORAGE_ROOT_W L"/pr_subsrc.ctrl"
#else
#define SUBSCRIPTION_STORAGE_ROOT_W				L"./pres_test"
#define SUBSCRIPTION_STORAGE_ROOT				L"./pres_test"
#define SUBSCRIPTION_STORAGE_FILE_PATH_W	    SUBSCRIPTION_STORAGE_ROOT_W L"/pr_subsrc.xml"
#define SUBSCRIPTION_STORAGE_FILE_PATH_CTRL_W	SUBSCRIPTION_STORAGE_ROOT_W L"/pr_subsrc.ctrl"
#endif


#ifdef _WIN32
#define EVENTS_STORAGE_FOLDER					SUBSCRIPTION_STORAGE_ROOT TEXT("\\pres_events")
#define EVENTS_STORAGE_FOLDER_W					SUBSCRIPTION_STORAGE_ROOT_W L"\\pres_events"
#else
#define EVENTS_STORAGE_FOLDER					SUBSCRIPTION_STORAGE_ROOT L"/pres_events"
#define EVENTS_STORAGE_FOLDER_W					SUBSCRIPTION_STORAGE_ROOT_W L"/pres_events"
#endif

#define TR_SERVER_ADDRESSL						L"http://127.0.0.1:17071"
#define EVENT_STORAGE_NAME_LOCAL2				L"Second_instance_of_PRESS"

extern std::wstring g_sTrServerCompName;	// транспортное имя компоненты
extern KLPRCI::ComponentId g_cidServerComponent;

//////////////////////////////////////////////////////////////////////
// CTest

class CTest
{
public:
	CTest();

	bool Run();

private:
	time_t m_time;
	KLPRCI::ComponentId m_cidSubscriber;
	KLPRCI::ComponentId TEST_PUBLISHER;

// Steps
	void AddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void VerifyAddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

	void AddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs);
	void VerifyAddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs);

	void RemoveExpiredEvents(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void RemoveAllEventsAndValidate(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
    void RemoveRanges(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void RemoveEvents(std::wstring sSubscriber, bool bUseClearEventsFunction, EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

	void RemoveSubscriptionsAndVerify(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

// Implementation
	std::wstring CreateEventType(const int n);
};

//////////////////////////////////////////////////////////////////////
// CTest2

class CTest2
{
public:
	CTest2();

	bool Run();
private:
	time_t m_time;
	KLPRCI::ComponentId m_cidSubscriber;
	KLPRCI::ComponentId TEST_PUBLISHER;

// Steps
	void AddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void VerifyAddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

	void AddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs);
	void VerifyAddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs);

	void RemoveExpiredEvents(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void RemoveAllEventsAndValidate(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void RemoveEvents(std::wstring sSubscriber, bool bUseClearEventsFunction, EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

	void RemoveSubscriptionsAndVerify(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

// Implementation
	std::wstring CreateEventType(const int n);
};

//////////////////////////////////////////////////////////////////////
// CTest3

class CTest3
{
public:
	CTest3();

	bool Run();
private:
	time_t m_time;
	KLPRCI::ComponentId m_cidSubscriber;
	KLPRCI::ComponentId TEST_PUBLISHER;

// Steps
	void AddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void VerifyAddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

	void AddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs);
	void VerifyAddEvents(EventsStorage* pEvents, bool bEventType1, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs);

	void RemoveExpiredEvents(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void RemoveAllEventsAndValidate(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void RemoveEvents(std::wstring sSubscriber, bool bUseClearEventsFunction, EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

	void RemoveSubscriptionsAndVerify(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

// Implementation
	std::wstring CreateEventType(const int n);
};

//////////////////////////////////////////////////////////////////////
// CTest3

class CTest4
{
public:
	CTest4();

	bool Run();

private:
	time_t m_time;
	KLPRCI::ComponentId m_cidSubscriber;
	KLPRCI::ComponentId TEST_PUBLISHER;

// Steps
	void AddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);
	void VerifyAddSubscriptions(EventsStorage* pEvents, std::vector<std::wstring>& arSubscriptionIDs);

	void AddEvents(EventsStorage* pEvents, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs);
	void VerifyAddEvents(EventsStorage* pEvents, KLPRCI::ComponentInstance* pComponent, std::vector<std::wstring>& arSubscriptionIDs);
};

class CPRESGeneralTest2 : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
	IMPLEMENT_TEST2_INSTANTIATE(CPRESGeneralTest2)

	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	virtual long Run();

protected:
	CTest	m_Test;
};

class CPRESGeneralTest3 : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
	IMPLEMENT_TEST2_INSTANTIATE(CPRESGeneralTest3)

	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	virtual long Run();

protected:
	CTest2	m_Test;
};

class CPRESGeneralTest4 : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
	IMPLEMENT_TEST2_INSTANTIATE(CPRESGeneralTest4)

	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	virtual long Run();

protected:
	CTest3	m_Test;
};

class CPRESGeneralTest5 : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
	IMPLEMENT_TEST2_INSTANTIATE(CPRESGeneralTest5)

	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	virtual long Run();

protected:
	CTest4	m_Test;
};

void PRESTestInit();
void PRESTestDeInit();

#endif // !defined(PRES_TEST_H)
