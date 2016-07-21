// PresPersistentEventList.h: interface for the CPersistentEventList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESPERSISTENTEVENTLIST_H__69E1C879_55A1_4956_A590_9BD924DF5830__INCLUDED_)
#define AFX_PRESPERSISTENTEVENTLIST_H__69E1C879_55A1_4956_A590_9BD924DF5830__INCLUDED_

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../jrnl/journal.h"

namespace KLPRES {

class CTimeoutObject;
class CPersistentEventList : public CEventList
{
	friend class CTimeoutObject;
public:
	CPersistentEventList();
	virtual ~CPersistentEventList();

	virtual void Initialize(KLPRES::CSubscription* pSubscription, bool bNewList = false);
    virtual void Deinitialize();

	virtual std::wstring ResetIterator();
	virtual void GetNext(KLPAR::Params** ppEvents, const long nChunkSize);
	virtual void AddEvent(CEventInfo* pEventInfo);	// pEventInfo must be released
	virtual void ClearEventsList();
	virtual void DeleteEvents(const long nStopEventID);
	virtual void DeleteOneEventByID(const long nEventID);
    virtual void DeleteRanges(const RangeVector & vectRanges);
	virtual void RemoveExpiredEvents();
	virtual void RemoveStorage(bool bLeaveEventsOnDisk);
	virtual void GetEventInfoClassInstance(CEventInfo** ppEventInfo);
	virtual AVP_longlong GetEventStorageFileSize();
	virtual bool NextEventLocal();
	virtual bool GetEventLocal(CEventInfo** ppEvent);
	virtual bool IsEOFLocal();
	virtual std::wstring SetIterator(long nId);
	virtual void MoveToEventWithIndex(long nIndex);
	virtual long GetEventsCount();
	virtual void Update();

protected:
	KLSTD::CAutoPtr<KLJRNL::Journal> m_pStorage;				// Open as read-only
	KLSTD::CAutoPtr<KLJRNL::Journal> m_pWriteOpenedJournal;		// Open as write
	std::wstring m_sStorageFilePath;
	KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pWriteOpenedJournalLock;
	std::wstring m_sTimeoutObjectID; // доступ к переменной должен защищатся через m_pWriteOpenedJournalLock
	long m_nMaxEvents;

	void _OpenStorage(KLJRNL::Journal** ppStorage, 
					  bool bReadOnly = false, 
					  bool bCreateNew = false, 
					  bool bCompactCtrlFile = false);
	KLJRNL::CreationInfo _GetJrnlCreationInfo();
	virtual void _CreateJournalInstance(KLJRNL::Journal** ppStorage);
	void _UnuseWriteOpenedStorage(bool bCloseImmediately = false);
	void _RemoveExpiredEvents(KLJRNL::Journal* pStorage);
	void _ClearTimeoutStore();
};

class CPersistentEventListAsPolicy : public CPersistentEventList
{
public:
	CPersistentEventListAsPolicy(){};

	virtual void GetEventInfoClassInstance(CEventInfo** ppEventInfo);
};

class CPersistentEventListAsText : public CPersistentEventList
{
public:
	CPersistentEventListAsText(){};

	virtual void GetEventInfoClassInstance(CEventInfo** ppEventInfo);

protected:
	virtual void _CreateJournalInstance(KLJRNL::Journal** ppStorage);
};

} // namespace KLPRES

#endif // !defined(AFX_PRESPERSISTENTEVENTLIST_H__69E1C879_55A1_4956_A590_9BD924DF5830__INCLUDED_)

// Local Variables:
// mode: C++
// End:
