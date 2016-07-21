#if !defined(AFX_SUBSCRIPTIONLIST_H__A8BB59C1_BAB3_4072_9599_2BC476FDBBF8__INCLUDED_)
#define AFX_SUBSCRIPTIONLIST_H__A8BB59C1_BAB3_4072_9599_2BC476FDBBF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "transport/ev/eventsource.h"
#include "../jrnl/journal.h"
#include "kca/pres/eventsstorage.h"
#include <avp/parray.h>
#include "std/tmstg/timeoutstore.h"
#include <transport/ev/evs_queued.h>

#include <map>
#include <set>

namespace KLPRES {

//<-- Changed by andkaz 29.04.2003 13:27:31
#define KLPRES_EVENT_NAME_DIVIDER		L"/"
// -->


//////////////////////////////////////////////////////////////////////
// CSubscription

class CSubscriptionList;
class CSubscription;

class CEventInfo : public KLSTD::KLBaseImpl<KLJRNL::JournalRecord>
{
public:
	KLPRCI::ComponentId m_cidPublisher;
	std::wstring m_sEventType;
	KLSTD::CAutoPtr<KLPAR::Params> m_parEventBody;
	time_t m_EventTime;
	int m_nLifetime;
    AVP_longlong m_llEventIdAcrossInstance;
    std::wstring m_wstrInstanceId;
    
// Internal
	CSubscription* m_pSubscription;
	long m_nID;

	CEventInfo(){
		m_EventTime = 0;
		m_nLifetime = 0;
		m_nID = -1;
        m_llEventIdAcrossInstance = 0;
	};
	CEventInfo(CSubscription* pSubscription){
		m_pSubscription = pSubscription;
		m_EventTime = 0;
		m_nLifetime = 0;
		m_nID = -1;
	};
	CEventInfo( CSubscription* pSubscription,
				long nID,
				KLPRCI::ComponentId &cidPublisher,
				std::wstring sEventType,
				KLPAR::Params* parEventBody,
				time_t EventTime,
				int nLifetime);

	virtual void Serialize(std::string & os) const;
	virtual void Deserialize(const std::string & is);

	virtual void SerializeParams(KLPAR::Params** pparParams) const;
	virtual void DeserializeParams(const KLPAR::Params* parParams);
};

class CEventInfoAsPolicy : public CEventInfo
{
public:
	CEventInfoAsPolicy(){};
	CEventInfoAsPolicy(CSubscription* pSubscription):CEventInfo(pSubscription){}
	CEventInfoAsPolicy(CSubscription* pSubscription, long nID, KLPRCI::ComponentId &cidPublisher,
					   std::wstring sEventType, KLPAR::Params* parEventBody, time_t EventTime, int nLifetime)
		:CEventInfo(pSubscription, nID, cidPublisher, sEventType, parEventBody, EventTime, nLifetime)
	{
	}

	virtual void Serialize(std::string & os) const;
	virtual void Deserialize(const std::string & is);
};

class CEventInfoAsText : public CEventInfo
{
public:
	CEventInfoAsText(){};
	CEventInfoAsText(CSubscription* pSubscription):CEventInfo(pSubscription){}
	CEventInfoAsText(CSubscription* pSubscription, long nID, KLPRCI::ComponentId &cidPublisher,
					   std::wstring sEventType, KLPAR::Params* parEventBody, time_t EventTime, int nLifetime)
		:CEventInfo(pSubscription, nID, cidPublisher, sEventType, parEventBody, EventTime, nLifetime)
	{
	}

	virtual void Serialize(std::string & os) const;
	virtual void Deserialize(const std::string & is);
};

class CEventList : public KLSTD::KLBase
{
public:
	CEventList()
	{
		m_bInitialized = false;
		m_pSubscription = NULL;
		m_bUseRecNumIndex = false;
	}
	virtual ~CEventList(){}
	
	virtual void Initialize(CSubscription* pSubscription, bool bNewList = false) = 0;
    virtual void Deinitialize() = 0;
	virtual std::wstring ResetIterator() = 0;
	virtual void GetNext(KLPAR::Params** ppEvents, const long nChunkSize) = 0;
	virtual void AddEvent(CEventInfo* pEventInfo) = 0;	// pEventInfo must be released
	virtual void ClearEventsList() = 0;
	virtual void RemoveExpiredEvents() = 0;
	virtual void RemoveStorage(bool bLeaveEventsOnDisk) = 0; // может выбросить исключение, если файл с событиями занят НА ЧТЕНИЕ другим PRESом
	virtual void DeleteEvents(const long nStopEventID) = 0;
	virtual void DeleteOneEventByID(const long nEventID) = 0;
    virtual void DeleteRanges(const RangeVector & vectRanges) = 0;
	virtual void GetEventInfoClassInstance(CEventInfo** ppEventInfo) = 0;
	virtual AVP_longlong GetEventStorageFileSize() = 0;
	virtual bool NextEventLocal() = 0;
	virtual bool GetEventLocal(CEventInfo** ppEvent) = 0;
	virtual bool IsEOFLocal() = 0;
	virtual std::wstring SetIterator(long nId) = 0;
	virtual void MoveToEventWithIndex(long nIndex) = 0;
	virtual long GetEventsCount() = 0;
	virtual void Update() = 0;

protected:
	bool m_bInitialized;
	bool m_bUseRecNumIndex;
	CSubscription* m_pSubscription;
};

class CEventInfoMaskItem
{
public:
	std::wstring m_sValueName;
	long m_nValueType;
	long m_nID;
	
	CEventInfoMaskItem(std::wstring& sValueName, long nValueType, long nID)
	{
		m_sValueName = sValueName;
		m_nValueType = nValueType;
		m_nID = nID;
	}

	CEventInfoMaskItem(const KLPAR::Params* parEventInfoMaskItem);

	void Serialize(KLPAR::Params** pparEventInfoMaskItem);
};

class CEventInfoMask : public CPArray<CEventInfoMaskItem>
{
public:
	CEventInfoMask();

	void Serialize(KLPAR::Params* parEventInfoMask);
	void Deserialize(const KLPAR::Params* parEventInfoMask);

	bool Check(const wchar_t* pszValueName, long& nPropertyID);
	CEventInfoMaskItem* GetById(long nID);
	void ParseMask(const KLPAR::Params* parEventParametersMask, std::vector<std::wstring> &arEventParametersSortMask);
protected:
	long m_nEventInfoMaskCurrentUniqueID;	// used for event info property identificator creating

	void _ParseMask(const KLPAR::Params* parEventParametersMask, std::wstring wstrPrefix);
	void _NewItemFromParams(const KLPAR::Params* parEventInfoMaskItem);
};

class CSubscription : public KLSTD::KLBaseImpl<KLSTD::KLBase>
{
public:
	std::wstring					m_sID;
	KLSTD::CAutoPtr<KLPAR::Params>	m_parSubscriptionInfo;
	KLSTD::CAutoPtr<CEventList>		m_pEvents;
	CEventInfoMask m_EventInfoMask;
	
	// Dubbed fields to faster access to frequently required data
	int m_nMaxEvents, m_nMaxPageSize;
	int m_nEventsStoringTimeInterval;
	int m_nCloseWriteOpenedJournalTimeout;
	wchar_t m_cEventsFieldDelimiter;
	std::wstring m_sEventsListFilePath;
	std::vector<std::wstring> m_arEventParametersSortMask;
	bool m_bUseRecNumIndex;
    std::wstring m_wstrVersionId;
	bool m_bKeepEventsWhenSubscriptionIsDeleted;

    CSubscription(KLEVQ::HSUBSQUEUE hQueue);

	// Creates object that represents existing persistent subscription in storage file
	void InitializeExistingInStorage(
		const KLPRCI::ComponentId&	idAgent,
		CSubscriptionList*	pMaster, 
		const std::wstring			sID,
		const std::wstring			sEventStoragesFolder,
		const KLPAR::Params*		parSubscription );

	// Creates object that represents new subscription
	void InitializeNew(
		const KLPRCI::ComponentId&	idAgent,
		CSubscriptionList*	pMaster, 
		const std::wstring			sID, 
		const bool					bPersistent, 
		KLPAR::Params*				parSubscriptionInfo, 
		const std::wstring			sEventStoragesFolder);

	virtual ~CSubscription();

	void Update(KLPAR::Params* parSubscription);
    void Reinit(const KLPAR::Params* parSubscription);
	void AddEvent(const KLPRCI::ComponentId& publisher, const KLEV::Event *event);
	void Get(const KLPAR::Params* parStorageParams, KLPAR::Params** pparSubscriptionInfo);
	void PauseResume(const bool bPause);
	void Remove();

	bool IsPersistent();
	std::wstring GetStorageFilePath();

	static void Serialize(const SubscriptionInfo* pSubscriptionInfo, KLPAR::Params** pparSubscriptionInfo);
	static void Deserialize(const KLPAR::Params* parSubscriptionInfo, SubscriptionInfo** ppSubscriptionInfo);
	static void SubscriptionsStorageEventCallback(const KLPRCI::ComponentId & subscriber, 
												  const KLPRCI::ComponentId & publisher,
												  const KLEV::Event *event,
												  void *context);
    static void SerializeToString(
        const std::wstring & wstrID,
        const KLPAR::Params* parSubscription,
        std::string & strData );
protected:
	std::wstring m_sEventStoragesFolder;
	bool m_bInitialized;
	bool m_bPaused;
    bool m_bPersistent;

	volatile bool m_bEventsAdded;
	KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
	class CPublishAddEventEventWorker : public KLSTD::KLBaseImpl<KLTP::ThreadsPool::Worker>
	{
	public:
		CPublishAddEventEventWorker(volatile bool& bEventsAdded, KLSTD::CriticalSection* pCS, const CSubscription* pSubscription);
		~CPublishAddEventEventWorker();

		virtual int RunWorker(KLTP::ThreadsPool::WorkerId WID);
	private:
		volatile bool& m_bEventsAdded;
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
		const CSubscription* m_pSubscription;
	};
	friend class CPublishAddEventEventWorker;
	KLTP::ThreadsPool::WorkerId m_WIDPublishAddEventEventWorker;
	
	CSubscriptionList* m_pMaster;
	std::vector<KLEVQ::HSUBSCRIPTION> m_arEventSourceSubscriptionSIDs;
    KLEVQ::HSUBSQUEUE      m_hQueue;
    
	KLPRCI::ComponentId m_cidEventsStorageComponentId;

	void _Initialize(const KLPRCI::ComponentId&	idAgent,
					 const std::wstring sEventStoragesFolder,
					 const KLPAR::Params* parSubscription,
					 bool bNewSubscription = false);
	void _Resubscribe(KLPRES::SubscriptionInfo* pSubscriptionInfo);
	void _UnsubscribeAll();
	void _PublishEvent(const std::wstring sEventType) const;
	void _ParseMask(const KLPAR::Params* parEventParametersMask);
};

typedef std::map< std::wstring, KLSTD::KLAdapt< KLSTD::CAutoPtr<CSubscription> > > MapSubscriptions;

typedef std::set< std::wstring > SetSubscriptions;
struct SubscrIterator : public KLSTD::KLBaseImpl<KLSTD::KLBase>
{
    KLSTD::CAutoPtr<KLJRNL::Journal> pJournal;
    KLSTD::CAutoPtr<KLJRNL::Journal> pJournalWithPausedSubscr;

    SetSubscriptions setPassedSubscriptions;
    KLPRCI::ComponentId cidFilter;

    KLSTD::CAutoPtr<KLPAR::Params> parNonPersSubscriptions;
    int nNonPersIdx;
    std::vector< std::wstring > vectNonPersSubscrIds;
};

//////////////////////////////////////////////////////////////////////
// CSubscriptionList

class CSubscriptionList : public KLSTD::KLBaseImpl<KLSTD::KLBase>
{
friend class CSubscription;
public:
	CSubscriptionList();
	virtual ~CSubscriptionList();

	void Initialize(const KLPRCI::ComponentId&	idAgent,
					const std::wstring sStorageFilePath,
					const std::wstring sEventStoragesFolder,
                    bool  bSubscribeToEventsToStore);

    std::wstring ResetIterator(const KLPRCI::ComponentId &cidFilter, int nTimeout);
    void ReleaseIterator(const std::wstring & wstrSubscrIteratorId);
    bool GetNextSubscription(const std::wstring & wstrSubscrIteratorId,
							 std::wstring & wstrSubscrId,
							 KLPAR::Params** pparSubscription);
	void AddSubscription(const std::wstring sSubscriptionID, 
						 const bool bPersistent, 
						 KLPAR::Params* parSubscription, 
						 std::wstring &sID);
	void RemoveSubscription(const std::wstring sID, const bool bFromStorage = true);

	// Gets pointer to subscription from list (not copy!!!)
    KLSTD::CAutoPtr<CSubscription> GetSubscriptionByID(const std::wstring& sID);

protected:
    std::wstring m_wstrInstanceId;
    AVP_longlong m_llEventCounter;
    KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pcsEventCounter;
    AVP_longlong IncrementEventCounter();

    MapSubscriptions m_mapSubscriptions;
	KLPRCI::ComponentId	m_idAgent;
    bool m_bSubscribeToEventsToStore;
	bool m_bInitialized;
	std::wstring m_sStorageFilePath, 
				 m_sPausedStorageFilePath, 
				 m_sEventStoragesFolder;
	std::vector<KLEVQ::HSUBSCRIPTION> m_arNotificationEventsSubscriptionID;
    KLEVQ::HSUBSQUEUE      m_hQueue;
	KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pcsLock;

    std::set<std::wstring> m_setLostCounterpartTrNames;
    KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pcsLostCounterpartList;

    /* m_pTSSubscrIterators держит в себе SubscrIterator'ы, которые могут
       быть удалены по таймауту (итераторы для клиентов в другом процессе) */
    KLSTD::CAutoPtr<KLTMSG::TimeoutStore> m_pTSSubscrIterators; 

    /* m_pTSSubscrIterators держит в себе SubscrIterator'ы, которые удаляются,
       когда они больше не нужны клиентам (итераторы для клиентов в этом процессе) */
    typedef std::map< std::wstring, KLSTD::KLAdapt< KLSTD::CAutoPtr< SubscrIterator > > > MapSubscrIterators;
    MapSubscrIterators m_mapSubscrIterators;

    KLSTD::CAutoPtr<SubscrIterator> GetSubscrIterator( const std::wstring & wstrSubscrIteratorId );

	bool CompareComponentID(const KLPRCI::ComponentId& CIDFilter, const KLPRCI::ComponentId& CID2);
	//void _PublishEvent(const std::wstring sEventType, const std::wstring sSubscriptionName);
	void _Unsubscribe();

	std::list<std::wstring> m_listOpenedSubscriptionsIteratorIds;
	void _ReleaseAllIterators();

    KLSTD::CAutoPtr<KLJRNL::Journal> _GetCentralJournal(
        KLSTD::ACCESS_FLAGS accessFlags,
        bool bPausedSubscriptions );

    void ReadSubscriptionsFromCentralJournal( bool bPausedSubscriptions, bool bRefresh );
    bool m_bPausedSubscriptionRead;

    bool GetNextSubscriptionInJournal(KLJRNL::Journal* pJournal,
									  SubscrIterator* pSubscrIterator,
									  std::wstring& wstrSubscrId,
									  KLPAR::Params** pparSubscription);

// Event handler function
	void OnInternalPRESEvent(const KLPRCI::ComponentId publisher, KLSTD::CAutoPtr<KLEV::Event> event);
	
// Events
	void OnAddSubscription(const std::wstring sSubscriptionID, const KLPRCI::ComponentId& publisher);
	void OnUpdateSubscription(const std::wstring sSubscriptionID, const KLPRCI::ComponentId& publisher);
	void OnDeleteSubscription(const std::wstring sSubscriptionID, const KLPRCI::ComponentId& publisher);

    void RefreshFromDisk();

// static
	static void SubscriptionListStorageEventCallback(const KLPRCI::ComponentId & subscriber, 
								        		     const KLPRCI::ComponentId & publisher,
												     const KLEV::Event *event,
												     void *context);
};

}

#endif // !defined(AFX_SUBSCRIPTIONLIST_H__A8BB59C1_BAB3_4072_9599_2BC476FDBBF8__INCLUDED_)
