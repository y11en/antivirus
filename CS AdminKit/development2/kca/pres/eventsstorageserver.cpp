#include "build/general.h"
#include "std/base/klbase.h"
#include "std/trc/trace.h"
#include "std/par/params.h"
#include "std/par/paramsi.h"
#include "std/par/errors.h"
#include "transport/tr/transport.h"
#include "transport/tr/transportproxy.h"
#include "kca/prci/componentid.h"
#include "kca/prss/settingsstorage.h"
#include "soapH.h"
#include "kca/prts/prxsmacros.h"
#include "transport/avt/accesscheck.h"

#include "presdefs.h"
#include "kca/pres/eventsstorage.h"
#include "pressubscriptionlist.h"
#include "localeventsstorage.h"

#ifdef N_PLAT_NLM
# include "std/conv/wcharcrt.h"
#endif

#include "modulestub/pressoapstub.h"

namespace KLPRES {

using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;

const wchar_t KLCS_MODULENAME[] = L"PRES:EventsStorage";
const long EVENTS_CHUNK_SIZE = 100;
//const long EVENTS_CHUNK_SIZE = 3;

///////////////////////////////////////////////////////////////////////////
// CEventsStorageServer

#define MAKE_SURE_THAT_INITIALIZED	if (!m_bInitialized) KLSTD_NOINIT(L"KLPRETS::CSubscriptionList");

///////////////////////////////////////////////////////////////////////////

CEventsStorageServer::CEventsStorageServer(const std::wstring& sServerObjectID):
	m_sServerObjectID(sServerObjectID),
	m_bInitialized(false)
{
	m_pSubscriptions.Attach(new CSubscriptionList());
	KLSTD_CHKMEM(m_pSubscriptions);

	IMPLEMENT_LOCK;
}

void CEventsStorageServer::Initialize(const KLPRCI::ComponentId& idAgent,
									  const std::wstring& sSubscriptionsStorageFilePath,
									  const std::wstring& sEventsStorageFolder,
                                      bool bSubscribeToEventsToStore)
{
	AUTOLOCK;

	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::Initialize: %ls\n", sEventsStorageFolder.c_str());

	m_pSubscriptions->Initialize(idAgent, sSubscriptionsStorageFilePath, sEventsStorageFolder, bSubscribeToEventsToStore);
	m_bInitialized = true;
	
	KLSTD_TRACE0(3, L"[KLPRES] CEventsStorageServer::Initialize:Done\n");
}

// Subscriptions
void CEventsStorageServer::StartSubscription(const std::wstring sSubscriptionID, const bool bPersistent, Params* parSubscription, std::wstring& sID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::StartSubscription: %ls\n", sSubscriptionID.c_str());
	m_pSubscriptions->AddSubscription(sSubscriptionID, bPersistent, parSubscription, sID);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::StartSubscription:Done:%ls\n", sID.c_str());
}

void CEventsStorageServer::PauseResumeSubscription(const std::wstring sSubscriptionID, const bool bPause)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::PauseResumeSubscription: %ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		pSubscription->PauseResume(bPause);
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::PauseResumeSubscription:Done %ls\n", sSubscriptionID.c_str());
}

void CEventsStorageServer::UpdateSubscription(const std::wstring sSubscriptionID, Params* parSubscriptions)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::UpdateSubscription: %ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		pSubscription->Update(parSubscriptions);
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::UpdateSubscription:Done %ls\n", sSubscriptionID.c_str());
}

std::wstring CEventsStorageServer::ResetSubscriptionsIterator(const KLPRCI::ComponentId &cidFilter, int nTimeout)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;
	KLSTD_TRACE0(3, L"[KLPRES] CEventsStorageServer::ResetSubscriptionsIterator\n");
	std::wstring rc;
    rc = m_pSubscriptions->ResetIterator(cidFilter, nTimeout);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::ResetSubscriptionsIterator:Done:%ls\n", rc.c_str());
	return rc;
}

void CEventsStorageServer::ReleaseSubscriptionsIterator( const std::wstring & wstrSubscrIteratorId )
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::ReleaseSubscriptionsIterator:%ls\n", wstrSubscrIteratorId.c_str());
	m_pSubscriptions->ReleaseIterator(wstrSubscrIteratorId);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::ReleaseSubscriptionsIterator:Done %ls\n", wstrSubscrIteratorId.c_str());
}

bool CEventsStorageServer::GetNextSubscription(
    const std::wstring & wstrSubscrIteratorId,
    std::wstring & wstrSubscrId,
    Params** pparSubscription )
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::GetNextSubscription:%ls\n", wstrSubscrIteratorId.c_str());
	return m_pSubscriptions->GetNextSubscription(wstrSubscrIteratorId, wstrSubscrId, pparSubscription);
}


void CEventsStorageServer::StopSubscription(const std::wstring sID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::StopSubscription:%ls\n", sID.c_str());
	m_pSubscriptions->RemoveSubscription(sID);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::StopSubscription:Done%ls\n", sID.c_str());
}

// Events
void CEventsStorageServer::ResetEventsIterator(const std::wstring sSubscriptionID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::ResetEventsIterator:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		pSubscription->m_pEvents->ResetIterator();
	else
		KLSTD_THROW1(STDE_BADPARAM, L"sSubscriptionID");
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::ResetEventsIterator:Done:%ls\n", sSubscriptionID.c_str());
}

std::wstring CEventsStorageServer::ResetEventsIteratorEx(const std::wstring& sSubscriptionID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	std::wstring wstrJournalID;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::ResetEventsIterator:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		wstrJournalID = pSubscription->m_pEvents->ResetIterator();
	else
		KLSTD_THROW1(STDE_BADPARAM, L"sSubscriptionID");

	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::ResetEventsIterator:Done:%ls\n", sSubscriptionID.c_str());

	return wstrJournalID;
}

void CEventsStorageServer::SetEventsIterator(const std::wstring sSubscriptionID, long nEventID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::SetEventsIterator:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		pSubscription->m_pEvents->SetIterator(nEventID);
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::SetEventsIterator:Done:%ls\n", sSubscriptionID.c_str());
}

std::wstring CEventsStorageServer::SetEventsIteratorEx(const std::wstring sSubscriptionID, long nEventID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	std::wstring wstrJournalID;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::SetEventsIterator:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		wstrJournalID = pSubscription->m_pEvents->SetIterator(nEventID);
	else
		KLSTD_THROW(STDE_BADPARAM);

	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::SetEventsIterator:Done:%ls\n", sSubscriptionID.c_str());

	return wstrJournalID;
}

void CEventsStorageServer::MoveToEventWithIndex(const std::wstring sSubscriptionID, long nIndex)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::MoveToEventWithIndex:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		pSubscription->m_pEvents->MoveToEventWithIndex(nIndex);
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::MoveToEventWithIndex:Done:%ls\n", sSubscriptionID.c_str());
}

long CEventsStorageServer::GetEventsCount(const std::wstring sSubscriptionID)
{
	MAKE_SURE_THAT_INITIALIZED;

	long rc = 0;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::GetEventsCount:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		rc = pSubscription->m_pEvents->GetEventsCount();
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::GetEventsCount:Done:%ls\n", sSubscriptionID.c_str());

	return rc;
}

void CEventsStorageServer::GetNext(const std::wstring sSubscriptionID, KLPAR::Params** ppEvents, const long nChunkSize)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKOUTPTR(ppEvents);

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::GetNext:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		pSubscription->m_pEvents->GetNext(ppEvents, nChunkSize);
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::GetNextDone:%ls\n", sSubscriptionID.c_str());
}

bool CEventsStorageServer::NextEventLocal(const std::wstring sSubscriptionID)
{
	MAKE_SURE_THAT_INITIALIZED;

	bool rc = false;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::NextEventLocal:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		rc = pSubscription->m_pEvents->NextEventLocal();
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::NextEventLocal:Done:%ls\n", sSubscriptionID.c_str());

	return rc;
}

bool CEventsStorageServer::IsEOFLocal(const std::wstring sSubscriptionID)
{
	MAKE_SURE_THAT_INITIALIZED;

	bool rc = false;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::IsEOFLocal:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		rc = pSubscription->m_pEvents->IsEOFLocal();
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::IsEOFLocal:Done:%ls\n", sSubscriptionID.c_str());

	return rc;
}

bool CEventsStorageServer::GetEventLocal(const std::wstring sSubscriptionID, CEventInfo** ppEvent)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKOUTPTR(ppEvent);

	bool rc = false;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::GetEventLocal:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		rc = pSubscription->m_pEvents->GetEventLocal(ppEvent);
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::GetEventLocal:Done:%ls\n", sSubscriptionID.c_str());

	return rc;
}

void CEventsStorageServer::ClearEventsList(const std::wstring sSubscriptionID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::ClearEventsList:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		pSubscription->m_pEvents->ClearEventsList();
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::ClearEventsList:Done:%ls\n", sSubscriptionID.c_str());
}

void CEventsStorageServer::DeleteEvents(const std::wstring sSubscriptionID, const long nStopEventID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::DeleteEvents:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		pSubscription->m_pEvents->DeleteEvents(nStopEventID);
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::DeleteEvents:Done%ls\n", sSubscriptionID.c_str());
}

void CEventsStorageServer::DeleteOneEventByID(const std::wstring subscription, const long nEventID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::DeleteOneEventByID:%ls\n", subscription.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(subscription);
	if (pSubscription) 
		pSubscription->m_pEvents->DeleteOneEventByID(nEventID);
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::DeleteOneEventByID:Done:%ls\n", subscription.c_str());
}

void CEventsStorageServer::DeleteRanges(const std::wstring subscription, const RangeVector & vectRanges)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::DeleteOneEventByID:%ls\n", subscription.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(subscription);
	if (pSubscription) 
		pSubscription->m_pEvents->DeleteRanges(vectRanges);
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::DeleteOneEventByID:Done:%ls\n", subscription.c_str());
}

void CEventsStorageServer::RemoveExpiredEvents(const std::wstring sSubscriptionID)
{
	MAKE_SURE_THAT_INITIALIZED;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::RemoveExpiredEvents:%ls\n", sSubscriptionID.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(sSubscriptionID);
	if (pSubscription) 
		pSubscription->m_pEvents->RemoveExpiredEvents();
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::RemoveExpiredEvents:Done:%ls\n", sSubscriptionID.c_str());
}

AVP_longlong CEventsStorageServer::GetEventStorageFileSize(const std::wstring subscription)
{
	MAKE_SURE_THAT_INITIALIZED;

	AVP_longlong rc = 0;

	AUTOLOCK;

	// Gets pointer to subscription from list (not copy!!!)
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::GetEventStorageFileSize:%ls\n", subscription.c_str());
	CAutoPtr<CSubscription> pSubscription = m_pSubscriptions->GetSubscriptionByID(subscription);
	if (pSubscription) 
		rc = pSubscription->m_pEvents->GetEventStorageFileSize();
	else
		KLSTD_THROW(STDE_BADPARAM);
	KLSTD_TRACE1(3, L"[KLPRES] CEventsStorageServer::GetEventStorageFileSize:%ls\n", subscription.c_str());

	return rc;
}

std::wstring& CEventsStorageServer::getID()
{
	AUTOLOCK;
	return m_sServerObjectID;
}

void CEventsStorageServer::Deinitialize()
{
}

static volatile long g_cModuleRef=0;

//////////////////////////////////////////////////////////////////////////
// CEventStorageServerList

class CEventStorageServerList :  public KLSTD::KLBaseImpl<KLSTD::KLBase>
{
public:
	CEventStorageServerList()
	{
		IMPLEMENT_LOCK;
		KLSTD_InterlockedIncrement(&g_cModuleRef);
	}

	virtual ~CEventStorageServerList()
	{
		KLSTD_InterlockedDecrement(&g_cModuleRef);
	}

	void Add(CEventsStorageServer* pServer)
	{
		KLSTD_CHKINPTR(pServer);

		AUTOLOCK;
	
		CAutoPtr<CEventsStorageServer>* pServerAutoPtr = new CAutoPtr<CEventsStorageServer>(pServer);
		KLSTD_CHKMEM(pServerAutoPtr);
		m_storagesList.Add(pServerAutoPtr);
	}

	void GetServerObjectByID(const std::wstring& sServerObjectID, CEventsStorageServer** ppEventsStorageServer, bool bThrowNotExistException = true)
	{
		KLSTD_CHKOUTPTR(ppEventsStorageServer);

		AUTOLOCK;

		for(unsigned int i = 0;i < m_storagesList.Count();i++)
		{
//			CAutoPtr<CEventsStorageServer>* pServerAutoPtr = (*this)[i];
//			KLSTD_CHKMEM(pServerAutoPtr);
			CAutoPtr<CEventsStorageServer> pServer(*((m_storagesList)[i]));
			KLSTD_CHKMEM(pServer);
			if (sServerObjectID == pServer->getID()) 
			{
				pServer.CopyTo(ppEventsStorageServer);
				return;
			}
		}

		if (bThrowNotExistException) KLSTD_THROW(KLSTD::STDE_NOTFOUND);
	}

	void DeleteServerObjectByID(const std::wstring& sServerObjectID, bool bThrowNotExistException = false)
	{
		CAutoPtr<CEventsStorageServer> pServerTDelete;

		LOCK
		{
			for(unsigned int i = 0;i < m_storagesList.Count();i++)
			{
				CAutoPtr<CEventsStorageServer> pServer(*((m_storagesList)[i]));
				KLSTD_CHKMEM(pServer);
				if (sServerObjectID == pServer->getID())
				{
					pServerTDelete = pServer;
					m_storagesList.RemoveInd(i);				
					break;
				}
			}
		}
		UNLOCK

		if (bThrowNotExistException) KLSTD_THROW(KLSTD::STDE_NOTFOUND);
	}

protected:
	DECLARE_LOCK;

	CPArray<CAutoPtr<CEventsStorageServer> >	m_storagesList;
};

KLSTD::CAutoPtr<CEventStorageServerList> g_pEventStorageServerList;

//IMPLEMENT_MODULE_LOCK(KLPRES)

KLCSKCA_DECL void KLPRES_GetEventStorageServerList( CEventStorageServerList** ppStorageServerList )
{
	KLSTD_CHKOUTPTR(ppStorageServerList);
	//KLCS_MODULE_LOCK(KLPRES, acs);
    KLCS_GLOBAL_MODULE_LOCK(acs);
	g_pEventStorageServerList.CopyTo(ppStorageServerList);
	if(!*ppStorageServerList)
		KLSTD_THROW(STDE_UNAVAIL);
};

//////////////////////////////////////////////////////////////////////////
// CLocalEventsStorage

#define CALL_METHOD2(method)		m_pEventStorageServer->method

#define MAKE_SURE_THAT_INITIALIZED2	\
if (!m_pEventStorageServer) { \
	KLSTD_TRACE1(1, L"Proxy's server object id '%s' does not match any events storage server", m_sServerObjectID.c_str()); \
	KLSTD_THROW(STDE_NOTFOUND); \
}

CLocalEventsStorage::CLocalEventsStorage(const std::wstring& sServerObjectID)
{
	IMPLEMENT_LOCK;

	m_sServerObjectID = sServerObjectID;
	KLSTD::CAutoPtr<KLPRES::CEventStorageServerList> pStorageList;
	KLPRES_GetEventStorageServerList( &pStorageList );
	pStorageList->GetServerObjectByID(m_sServerObjectID, &m_pEventStorageServer, false);
}

CLocalEventsStorage::~CLocalEventsStorage()
{
	AUTOLOCK;

	if( ! m_wstrSubscrIteratorId.empty() )
    {
        KLERR_BEGIN
            CALL_METHOD2( ReleaseSubscriptionsIterator( m_wstrSubscrIteratorId ) );
            m_wstrSubscrIteratorId.clear();// = L"";
        KLERR_END
    }
}

void CLocalEventsStorage::ResetSubscriptionsIterator(const KLPRCI::ComponentId &subscriber, int nTimeout /* = 60 */ )
{
	MAKE_SURE_THAT_INITIALIZED2;

	AUTOLOCK;
	/* m_arSubscriptionNames.clear();
	m_itSubscriptionNames = m_arSubscriptionNames.begin();
	m_parSubscriptions = NULL; */

    if( ! m_wstrSubscrIteratorId.empty() )
    {
        CALL_METHOD2( ReleaseSubscriptionsIterator( m_wstrSubscrIteratorId ) );
        m_wstrSubscrIteratorId.clear();// = L"";
    }

	m_wstrSubscrIteratorId = CALL_METHOD2(ResetSubscriptionsIterator(subscriber, 0));

	/* m_parSubscriptions->GetNames(m_arSubscriptionNames);
	m_itSubscriptionNames = m_arSubscriptionNames.begin(); */
}

bool CLocalEventsStorage::GetNextSubscription(SubscriptionInfo** info, std::wstring &subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;
	KLSTD_CHKOUTPTR(info);
	//if (!m_parSubscriptions) KLSTD_NOINIT(L"KLPRES::m_parSubscriptions");

	bool rc = false;

	AUTOLOCK;

	CAutoPtr<Params> parSubscription;
    rc = CALL_METHOD2( GetNextSubscription( m_wstrSubscrIteratorId, subscription, & parSubscription ) );
    if( rc )
    {
		CSubscription::Deserialize(parSubscription, info);
    }

/* 	if (m_itSubscriptionNames != m_arSubscriptionNames.end())
	{
		CAutoPtr<ParamsValue> pParamsValue = (ParamsValue*)m_parSubscriptions->GetValue2(*m_itSubscriptionNames, true);
		CAutoPtr<Params> parSubscription = pParamsValue->GetValue();
		CSubscription::Deserialize(parSubscription, info);
		subscription = *m_itSubscriptionNames;

		m_itSubscriptionNames++;

		rc = true;
	} */

	return rc;
}

void CLocalEventsStorage::StartSubscription(const SubscriptionInfo &info, std::wstring &subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	CAutoPtr<Params> parSubscription;
	CSubscription::Serialize(&info, &parSubscription);
	
	CALL_METHOD2(StartSubscription(subscription, info.isSubscriptionPersist, parSubscription, subscription));
}

void CLocalEventsStorage::UpdateSubscription(const std::wstring subscription, const SubscriptionInfo &info)
{
	MAKE_SURE_THAT_INITIALIZED2;

	CAutoPtr<Params> parSubscription;
	CSubscription::Serialize(&info, &parSubscription);
	CALL_METHOD2(UpdateSubscription(subscription, parSubscription));
}

void CLocalEventsStorage::PauseSubscription(const std::wstring subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	CALL_METHOD2(PauseResumeSubscription(subscription, true));
}

void CLocalEventsStorage::ResumeSubscription(const std::wstring subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	CALL_METHOD2(PauseResumeSubscription(subscription, false));
}

void CLocalEventsStorage::StopSubscription(const std::wstring subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	CALL_METHOD2(StopSubscription(subscription));
}

void CLocalEventsStorage::ResetEventsIterator(const std::wstring subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	AUTOLOCK;

	CALL_METHOD2(ResetEventsIterator(subscription));
	m_sIteratedSubscriptionID = subscription;
}

std::wstring CLocalEventsStorage::ResetEventsIteratorEx(const std::wstring& subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	AUTOLOCK;

	std::wstring wstrJournalID = CALL_METHOD2(ResetEventsIteratorEx(subscription));
	m_sIteratedSubscriptionID = subscription;

	return wstrJournalID;
}

void CLocalEventsStorage::SetEventsIterator(const std::wstring subscription, long nEventID)
{
	MAKE_SURE_THAT_INITIALIZED2;

	AUTOLOCK;

	CALL_METHOD2(SetEventsIterator(subscription, nEventID));
	m_sIteratedSubscriptionID = subscription;
}

std::wstring CLocalEventsStorage::SetEventsIteratorEx(const std::wstring subscription, long eventID)
{
	MAKE_SURE_THAT_INITIALIZED2;

	AUTOLOCK;

	std::wstring wstrJournalID = CALL_METHOD2(SetEventsIteratorEx(subscription, eventID));
	m_sIteratedSubscriptionID = subscription;

	return wstrJournalID;
}

void CLocalEventsStorage::MoveToEventWithIndex(const std::wstring subscription, long nIndex)
{
	MAKE_SURE_THAT_INITIALIZED2;

	AUTOLOCK;

	CALL_METHOD2(MoveToEventWithIndex(subscription, nIndex));
	m_sIteratedSubscriptionID = subscription;
}

long CLocalEventsStorage::GetEventsCount(const std::wstring subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	return CALL_METHOD2(GetEventsCount(subscription));
}

bool CLocalEventsStorage::IsEvent()
{
	MAKE_SURE_THAT_INITIALIZED2;
	if (m_sIteratedSubscriptionID.empty()) KLSTD_THROW(STDE_BADHANDLE);

	return !(CALL_METHOD2(IsEOFLocal(m_sIteratedSubscriptionID)));
}

bool CLocalEventsStorage::NextEvent(const std::wstring & subscription /* = L"" */)
{
	MAKE_SURE_THAT_INITIALIZED2;
	if (subscription.empty() && m_sIteratedSubscriptionID.empty()) KLSTD_THROW(STDE_BADHANDLE);

	return CALL_METHOD2(NextEventLocal(subscription.empty() ? m_sIteratedSubscriptionID : subscription));
}

bool CLocalEventsStorage::GetEvent(KLPRCI::ComponentId &id, 
								   std::wstring &eventType, 
								   KLPAR::Params **eventBody, 
								   time_t &eventTime, 
								   long &eventID,
                                   const std::wstring & subscription /* = L"" */)
{
	MAKE_SURE_THAT_INITIALIZED2;
	KLSTD_CHKOUTPTR(eventBody);

	AUTOLOCK;

	if (subscription.empty() && m_sIteratedSubscriptionID.empty()) KLSTD_THROW(STDE_BADHANDLE);

	CAutoPtr<CEventInfo> pEventInfo;
    bool rc = CALL_METHOD2(GetEventLocal(subscription.empty() ? m_sIteratedSubscriptionID : subscription, &pEventInfo));
	if (rc)
	{
		id = pEventInfo->m_cidPublisher;
		eventType = pEventInfo->m_sEventType;
		if (pEventInfo->m_parEventBody) pEventInfo->m_parEventBody.CopyTo(eventBody);
		eventTime = pEventInfo->m_EventTime;
		eventID = pEventInfo->m_nID;
	}

	return rc;
}

void CLocalEventsStorage::DeleteEvents(const std::wstring sSubscriptionID, const long nStopEventID)
{
	MAKE_SURE_THAT_INITIALIZED2;

	CALL_METHOD2(DeleteEvents(sSubscriptionID, nStopEventID));
}

void CLocalEventsStorage::DeleteOneEventByID(const std::wstring subscription, const long nEventID)
{
	MAKE_SURE_THAT_INITIALIZED2;

	CALL_METHOD2(DeleteOneEventByID(subscription, nEventID));
}

void CLocalEventsStorage::DeleteRanges(const std::wstring subscription, const RangeVector & vectRanges)
{
	MAKE_SURE_THAT_INITIALIZED2;

    CALL_METHOD2(DeleteRanges(subscription, vectRanges));
}

void CLocalEventsStorage::ClearEventsList(const std::wstring subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	CALL_METHOD2(ClearEventsList(subscription));
}

void CLocalEventsStorage::RemoveExpiredEvents (const std::wstring subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	CALL_METHOD2(RemoveExpiredEvents(subscription));
}

AVP_longlong CLocalEventsStorage::GetEventStorageFileSize(const std::wstring subscription)
{
	MAKE_SURE_THAT_INITIALIZED2;

	return CALL_METHOD2(GetEventStorageFileSize(subscription));
}

}

//////////////////////////////////////////////////////////////////////////

using namespace KLPRES;

IMPLEMENT_MODULE_INIT_DEINIT(KLPRES)

IMPLEMENT_MODULE_INIT_BEGIN2( KLCSKCA_DECL, KLPRES)
{
	KLPRES::RegisterGSOAPStubFunctions();

	//MODULE_LOCK_INIT(KLPRES)
	if (g_pEventStorageServerList) KLSTD_THROW(KLSTD::STDE_NOTPERM);
	g_pEventStorageServerList.Attach( new CEventStorageServerList );
    KLSTD_CHKMEM( g_pEventStorageServerList );
}
IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN2( KLCSKCA_DECL, KLPRES)
{
	if (g_pEventStorageServerList) g_pEventStorageServerList = 0;

	// wait for module free
	while(g_cModuleRef > 0) KLSTD_Sleep(500);

	KLPRES::CEventsStorageServer::Deinitialize();
	//MODULE_LOCK_DEINIT(KLPRES)

	KLPRES::UnregisterGSOAPStubFunctions();
}
IMPLEMENT_MODULE_DEINIT_END()

KLCSKCA_DECL void KLPRES_CreateEventsStorageServer(const std::wstring sServerObjectID, 
												 const std::wstring sProductName,
												 const std::wstring sProductVersion,
												 const std::wstring sSubscriptionsStorageFilePath,
												 const std::wstring sEventsStorageFolder,
                                                 bool bSubscribeToEventsToStore /*= true*/)
{
	KLSTD::CAutoPtr<CEventsStorageServer> pServer;
	pServer.Attach(new CEventsStorageServer(sServerObjectID));
	if (!pServer) KLSTD_THROW(KLSTD::STDE_NOMEMORY);
	KLPRCI::ComponentId idAgent(sProductName,
								sProductVersion,
								KLCS_COMPONENT_AGENT,
								KLCS_MAKEWELLKNOWN_AGENT(sProductName));
	pServer->Initialize(idAgent,
						sSubscriptionsStorageFilePath,
						sEventsStorageFolder,
                        bSubscribeToEventsToStore);
	KLSTD::CAutoPtr<KLPRES::CEventStorageServerList> pStorageList;
	KLPRES_GetEventStorageServerList( &pStorageList );
	pStorageList->Add(pServer);
}

KLCSKCA_DECL void KLPRES_CreateEventsStorageServer2(const std::wstring sServerObjectID, 
												 const KLPRCI::ComponentId& cidAgent, 
												 const std::wstring sSubscriptionsStorageFilePath,
												 const std::wstring sEventsStorageFolder,
                                                 bool bSubscribeToEventsToStore /*= true*/)
{
	KLSTD::CAutoPtr<CEventsStorageServer> pServer;
	pServer.Attach(new CEventsStorageServer(sServerObjectID));
	if (!pServer) KLSTD_THROW(KLSTD::STDE_NOMEMORY);

	pServer->Initialize(cidAgent,
						sSubscriptionsStorageFilePath,
						sEventsStorageFolder,
                        bSubscribeToEventsToStore);
	KLSTD::CAutoPtr<KLPRES::CEventStorageServerList> pStorageList;
	KLPRES_GetEventStorageServerList( &pStorageList );
	pStorageList->Add(pServer);
}

KLCSKCA_DECL void KLPRES_DeleteEventsStorageServer(const std::wstring sServerObjectID, bool bWaitEventQueueEmpty)
{
	if (bWaitEventQueueEmpty) KLEV_GetEventSource()->WaitForCleanEventsQueue(std::wstring(L""));
	KLSTD::CAutoPtr<KLPRES::CEventStorageServerList> pStorageList;
	KLPRES_GetEventStorageServerList( &pStorageList );
	pStorageList->DeleteServerObjectByID(sServerObjectID);
}


//////////////////////////////////////////////////////////////////////////
//
// SOAP interface implementation
//
//////////////////////////////////////////////////////////////////////////

#define DO_COMMAND	 try {

#define HANDLE_ERRORS(rc) \
rc.code = KLSTD::STDE_NOERROR; \
} catch(KLERR::Error* err) { \
	if (err) { \
		KLPAR::ExceptionForSoap(pSOAP, err, rc); \
		err->Release(); \
	} \
} \
catch(std::exception&) { \
	KLSTD::CAutoPtr<KLERR::Error> err; \
	KLERR_CreateError(&err, KLPRES::KLCS_MODULENAME, KLSTD::STDE_GENERAL, __FILE__, __LINE__); \
	KLPAR::ExceptionForSoap(pSOAP, err, rc); \
}

#define CALL_METHOD(method) \
	KLSTD::CAutoPtr<KLPRES::CEventsStorageServer> pEventsStorageServer; \
	KLSTD::CAutoPtr<KLPRES::CEventStorageServerList> pStorageList;\
	KLPRES_GetEventStorageServerList( &pStorageList );\
	pStorageList->GetServerObjectByID(std::wstring(pszServerObjectID), &pEventsStorageServer); \
	KLSTD_CHKMEM(pEventsStorageServer); \
	pEventsStorageServer->method

#define CALL_METHOD_WITH_RETVAL(method, rc) \
	KLSTD::CAutoPtr<KLPRES::CEventsStorageServer> pEventsStorageServer; \
	KLSTD::CAutoPtr<KLPRES::CEventStorageServerList> pStorageList;\
	KLPRES_GetEventStorageServerList( &pStorageList );\
	pStorageList->GetServerObjectByID(std::wstring(pszServerObjectID), &pEventsStorageServer); \
	KLSTD_CHKMEM(pEventsStorageServer); \
	rc = pEventsStorageServer->method

//////////////////////////////////////////////////////////////////////////

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_ResetSubscriptionsIteratorStub(struct soap* pSOAP, wchar_t *pszServerObjectID, SOAPComponentId cidFilter, long nTimeout, struct KLPRES_RESET_SUBSCRIPTIONS_ITERATOR_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		KLPRCI::ComponentId cid(cidFilter.productName, 
								cidFilter.version, 
								cidFilter.componentName, 
								cidFilter.instanceId);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		wstring wstrSubscrIteratorId;
		CALL_METHOD_WITH_RETVAL(ResetSubscriptionsIterator(cid, nTimeout), wstrSubscrIteratorId);
		rc.sSubscrIteratorId = soap_strdup( pSOAP, wstrSubscrIteratorId.c_str() );
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_ReleaseSubscriptionsIteratorStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscrIteratorId, struct KLPRES_RELEASE_SUBSCRIPTIONS_ITERATOR_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		CALL_METHOD(ReleaseSubscriptionsIterator(sSubscrIteratorId));
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_GetNextSubscriptionStub(
    struct soap* pSOAP,
    xsd__wstring pszServerObjectID,
	xsd__wstring sSubscrIteratorId,
	struct KLPRES_GET_NEXT_SUBSCRIPTION_RESPONSE &rcl )
{
    wstring wstrSubscrId;
    CAutoPtr<Params> parSubscription;
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

        CALL_METHOD_WITH_RETVAL( GetNextSubscription( sSubscrIteratorId, wstrSubscrId, & parSubscription ), rcl.bOk);
	}
	HANDLE_ERRORS(rcl.rcError)

    if( rcl.bOk )
    {
		ParamsForSoap( pSOAP, parSubscription, rcl.parSubscription );
        rcl.sSubscrId = soap_strdup( pSOAP, wstrSubscrId.c_str() );
    }

	return SOAP_OK;
}


SOAP_FMAC1 int SOAP_FMAC2 KLPRES_AddSubscriptionStub(struct soap* pSOAP, wchar_t *pszServerObjectID, xsd__wstring sSubscriptionID, bool bPersistent, param__params parSubscription, struct KLPRES_ADD_SUBSCRIPTION_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_WRITE);

		std::wstring sID;

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		KLPAR::ParamsFromSoap(parSubscription, &parParams);
		CALL_METHOD(StartSubscription(sSubscriptionID, bPersistent, parParams, sID));
		rc.sSubscriptionID = KLPAR::soap_strdup(pSOAP, sID.c_str());
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_UpdateSubscriptionStub(struct soap* pSOAP, wchar_t *pszServerObjectID, xsd__wstring sSubscriptionID, param__params parSubscription, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_WRITE);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		KLPAR::ParamsFromSoap(parSubscription, &parParams);
		CALL_METHOD(UpdateSubscription(sSubscriptionID, parParams));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_PauseSubscriptionStub(struct soap* pSOAP, wchar_t *pszServerObjectID, xsd__wstring sSubscriptionID, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_WRITE);

		CALL_METHOD(PauseResumeSubscription(sSubscriptionID, true));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_ResumeSubscriptionStub(struct soap* pSOAP, wchar_t *pszServerObjectID, xsd__wstring sSubscriptionID, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_WRITE);

		CALL_METHOD(PauseResumeSubscription(sSubscriptionID, false));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_RemoveSubscriptionStub(struct soap* pSOAP, wchar_t *pszServerObjectID, xsd__wstring sSubscriptionID, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_WRITE);

		CALL_METHOD(StopSubscription(sSubscriptionID));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_ResetEventsIteratorStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		CALL_METHOD(ResetEventsIterator(sSubscriptionID));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_SetEventsIteratorStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, long eventID, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		CALL_METHOD(SetEventsIterator(sSubscriptionID, eventID));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_ResetEventsIteratorExStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, struct KLPRES_RESET_EVENTS_ITERATOR_EX_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		std::wstring wstrJournalID;
		CALL_METHOD_WITH_RETVAL(ResetEventsIteratorEx(sSubscriptionID), wstrJournalID);
		rc.sJournalID = soap_strdup(pSOAP, wstrJournalID.c_str());
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_SetEventsIteratorExStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, long eventID, struct KLPRES_SET_EVENTS_ITERATOR_EX_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		std::wstring wstrJournalID;
		CALL_METHOD_WITH_RETVAL(SetEventsIteratorEx(sSubscriptionID, eventID), wstrJournalID);
		rc.sJournalID = soap_strdup(pSOAP, wstrJournalID.c_str());
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_MoveToEventWithIndexStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, long nIndex, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		CALL_METHOD(MoveToEventWithIndex(sSubscriptionID, nIndex));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_GetEventsCountStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, struct KLPRES_GET_EVENTS_COUNT_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		CALL_METHOD_WITH_RETVAL(GetEventsCount(sSubscriptionID), rc.nCount);
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_GetNextEventsChunkStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, struct KLPRES_GET_NEXT_EVENTS_CHUNK_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		CALL_METHOD(GetNext(sSubscriptionID, &parParams, KLPRES::EVENTS_CHUNK_SIZE));
		KLPAR::ParamsForSoap(pSOAP, parParams, rc.pparEventsParamsOut);
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_ClearEventsListStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_DELETE);

		CALL_METHOD(ClearEventsList(sSubscriptionID));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_DeleteEventsStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, long nStopEventID, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_DELETE);

		CALL_METHOD(DeleteEvents(sSubscriptionID, nStopEventID));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_DeleteOneEventByIDStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, long nEventID, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_DELETE);

		CALL_METHOD(DeleteOneEventByID(sSubscriptionID, nEventID));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_DeleteRangesStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, param__params parRanges, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_DELETE);

        RangeVector vectRanges;

        {
            CAutoPtr<Params> pRanges;
            ParamsFromSoap( parRanges, & pRanges );

            CAutoPtr<ArrayValue> pArrayValue;
            GetValue( pRanges, SS_RANGES, & pArrayValue );
            vectRanges.reserve( pArrayValue->GetSize() );
            for( size_t i = 0; i < pArrayValue->GetSize(); i++ )
            {
		        CAutoPtr<ParamsValue> pParamsValue;
		        pArrayValue->GetAt( i, (Value**)&pParamsValue );
		        KLPAR_CHKTYPE( pParamsValue, PARAMS_T, L"pParamsValue" );

                Range range;
                GET_PARAMS_VALUE( pParamsValue->GetValue(), SS_RANGE_FIRST, IntValue, INT_T, range.first );
                GET_PARAMS_VALUE( pParamsValue->GetValue(), SS_RANGE_SECOND, IntValue, INT_T, range.second );

                vectRanges.push_back( range );
            }
        }
		CALL_METHOD(DeleteRanges(sSubscriptionID, vectRanges));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_RemoveExpiredEventsStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, struct param_error &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_DELETE);

		CALL_METHOD(RemoveExpiredEvents(sSubscriptionID));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRES_GetEventStorageFileSizeStub(struct soap* pSOAP, wchar_t *pszServerObjectID, wchar_t *sSubscriptionID, struct KLPRES_GET_EVENT_STORAGE_FILE_SIZE_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLAVT_AccessCheckForAction_InCall(KLAVT_ACT_PRES, KLAVT_READ);

		CALL_METHOD_WITH_RETVAL(GetEventStorageFileSize(sSubscriptionID), rc.llStorageSize);
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}
