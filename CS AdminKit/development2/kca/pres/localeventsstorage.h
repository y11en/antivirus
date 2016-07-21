#ifndef _LOCALEVENTSSTORAGE_H_
#define _LOCALEVENTSSTORAGE_H_

#include "std/base/klbase.h"
#include "std/par/params.h"
#include "kca/prts/prxsmacros.h"

namespace KLPRES {

////////////////////////////////////////////////////////////////////////
// CEventsStorageServer

class CEventsStorageServer : public KLSTD::KLBaseImpl<KLSTD::KLBase>
{
public:
	CEventsStorageServer(const std::wstring& sServerObjectID);
	void Initialize(const KLPRCI::ComponentId& idAgent,
					const std::wstring& sSubscriptionsStorageFilePath,
					const std::wstring& sEventsStorageFolder,
                    bool bSubscribeToEventsToStore);

// Subscriptions
    void StartSubscription(const std::wstring sSubscriptionID, const bool bPersistent, KLPAR::Params* parSubscription, std::wstring& sID);
	void PauseResumeSubscription(const std::wstring sSubscriptionID, const bool bPause);
    void UpdateSubscription(const std::wstring sSubscriptionID, KLPAR::Params* parSubscriptions);
    std::wstring ResetSubscriptionsIterator(const KLPRCI::ComponentId &cidFilter, int nTimeout);
    void ReleaseSubscriptionsIterator( const std::wstring & wstrSubscrIteratorId );
    bool GetNextSubscription(const std::wstring & wstrSubscrIteratorId,
							 std::wstring & wstrSubscrId,
							 KLPAR::Params** pparSubscription);
    void StopSubscription(const std::wstring sID);
// Events
    virtual void ResetEventsIterator(const std::wstring sSubscriptionID);
	std::wstring ResetEventsIteratorEx(const std::wstring& subscription);
    virtual void SetEventsIterator(const std::wstring sSubscriptionID, long nEventID);
	std::wstring SetEventsIteratorEx(const std::wstring sSubscriptionID, long nEventID);
    virtual void MoveToEventWithIndex(const std::wstring sSubscriptionID, long nIndex);
	virtual long GetEventsCount(const std::wstring sSubscriptionID);
	virtual void GetNext(const std::wstring sSubscriptionID, KLPAR::Params** ppEvents, const long nChunkSize);
	virtual bool NextEventLocal(const std::wstring sSubscriptionID);
	virtual bool IsEOFLocal(const std::wstring sSubscriptionID);
	virtual bool GetEventLocal(const std::wstring sSubscriptionID, CEventInfo** ppEvent);
    virtual void ClearEventsList(const std::wstring sSubscriptionID);
    virtual void DeleteEvents(const std::wstring sSubscriptionID, const long nStopEventID);
	virtual void DeleteOneEventByID(const std::wstring subscription, const long nEventID);
    virtual void DeleteRanges(const std::wstring subscription, const RangeVector & vectRanges);
    virtual void RemoveExpiredEvents(const std::wstring sSubscriptionID);
	virtual AVP_longlong GetEventStorageFileSize(const std::wstring subscription);
	std::wstring& getID();
	static void Deinitialize();
protected:
	bool m_bInitialized;
	KLSTD::CAutoPtr<CSubscriptionList> m_pSubscriptions;
	std::wstring m_sServerObjectID;
	DECLARE_LOCK;
};

///////////////////////////////////////////////////////////////////////////
// CLocalEventsStorage

class CLocalEventsStorage : public KLSTD::KLBaseImpl<EventsStorage>
{
public:
	CLocalEventsStorage(const std::wstring& sServerObjectID);
    ~CLocalEventsStorage();

    virtual void ResetSubscriptionsIterator(const KLPRCI::ComponentId &subscriber, int nTimeout = 60 );
    virtual bool GetNextSubscription(SubscriptionInfo** info, std::wstring &subscription);
    virtual void StartSubscription(const SubscriptionInfo &info, std::wstring &subscription);
    virtual void UpdateSubscription(const std::wstring subscription, const SubscriptionInfo &info);
	virtual void PauseSubscription(const std::wstring subscription);
	virtual void ResumeSubscription(const std::wstring subscription);
    virtual void StopSubscription(const std::wstring subscription);
    virtual void ResetEventsIterator(const std::wstring subscription);
	virtual void SetEventsIterator(const std::wstring subscription, long nEventID);
	virtual std::wstring ResetEventsIteratorEx(const std::wstring& subscription);
	virtual std::wstring SetEventsIteratorEx(const std::wstring subscription, long eventID);

	virtual void MoveToEventWithIndex(const std::wstring subscription, long nIndex);
	virtual long GetEventsCount(const std::wstring subscription);

	virtual bool IsEvent();
	virtual bool NextEvent( const std::wstring & subscription = L"" );
    virtual bool GetEvent(KLPRCI::ComponentId &id, 
						  std::wstring &eventType, 
						  KLPAR::Params **eventBody, 
						  time_t &eventTime, 
						  long &eventID,
                          const std::wstring & subscription = L"");
    virtual void DeleteEvents(const std::wstring sSubscriptionID, const long nStopEventID);
	virtual void DeleteOneEventByID(const std::wstring subscription, const long nEventID);
    virtual void DeleteRanges(const std::wstring subscription, const RangeVector & vectRanges);
    virtual void ClearEventsList(const std::wstring subscription);
	virtual void RemoveExpiredEvents (const std::wstring subscription);
	virtual AVP_longlong GetEventStorageFileSize(const std::wstring subscription);
	
	std::wstring& getID()
	{
		return m_sServerObjectID;
	};

protected:
	DECLARE_LOCK;
	
	std::wstring m_sServerObjectID;
	KLSTD::CAutoPtr<KLPRES::CEventsStorageServer> m_pEventStorageServer;

	// Subscriptions iterators
	//std::vector<std::wstring> m_arSubscriptionNames;
	//std::vector<std::wstring>::iterator m_itSubscriptionNames;

    std::wstring m_wstrSubscrIteratorId;

	//KLSTD::CAutoPtr<KLPAR::KLPAR::Params> m_parSubscriptions;

	// Events iterator
	std::wstring m_sIteratedSubscriptionID;
};

} // namespace KLPRES

#endif // _LOCALEVENTSSTORAGE_H_

// Local Variables:
// mode: C++
// End:

