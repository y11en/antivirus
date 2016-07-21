#include "std/base/kldefs.h"

#ifdef _MSC_VER
# pragma warning (disable : 4786)
# ifdef DEBUG
# pragma warning (disable : 4503)
#endif
#endif

#include <sstream>

#include "build/general.h"
#include "std/base/klbase.h"
#include "std/trc/trace.h"
#include "../jrnl/journal.h"
#include "../jrnl/errors.h"
#include "std/par/params.h"
#include "std/par/paramsi.h"
#include "std/par/errors.h"
#include "std/tmstg/timeoutstore.h"
#include "transport/tr/transport.h"
#include "transport/tr/transportproxy.h"
#include "kca/prci/componentid.h"
#include "kca/prss/settingsstorage.h"
#include "soapH.h"

#include "pressubscriptionlist.h"
#include "prespersistenteventlist.h"
#include "presdefs.h"



#include "kca/prss/errors.h"

using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;
using namespace KLJRNL;
using namespace KLTMSG;

#define KLCS_MODULENAME			L"PRES"
const wchar_t LOCK_FILE_EXT[] = L".lck";

namespace KLPRES {

//////////////////////////////////////////////////////////////////////
// CTimeoutObject
class CTimeoutObject : public KLBaseImpl<TimeoutObject>
{
public:
	CTimeoutObject(CPersistentEventList* pEventList)
		: m_pEventList(pEventList)
    {
    }

	~CTimeoutObject()
    {
		KLERR_BEGIN
		{
			AutoCriticalSection cs(m_pEventList->m_pWriteOpenedJournalLock);
			m_pEventList->m_pWriteOpenedJournal = NULL;
			KLSTD_TRACE1(1, L"[KLPRES] m_pEventList->m_pWriteOpenedJournal deleted (%s)\n", m_pEventList->m_sStorageFilePath.c_str() );
		}
		KLERR_ENDT(1);
    }

	KLTMSG_DECLARE_TYPEID(KLPRES::CTimeoutObject);

	CAutoPtr<CPersistentEventList> m_pEventList;
};

//////////////////////////////////////////////////////////////////////
// CStorageLocker
class CStorageLocker
{
public:
	CStorageLocker(const std::wstring& wstrStorageFilePath, bool bReadOnly, long lTimeout)
    {
		if (!bReadOnly)
		{
			KLERR_TRY
			{
				KLSTD_CHK(wstrStorageFilePath, !wstrStorageFilePath.empty());
				KLSTD_CreateFile(wstrStorageFilePath + LOCK_FILE_EXT, 
								 0, 
								 KLSTD::CF_OPEN_ALWAYS, 
								 KLSTD::AF_WRITE, 
								 0,
								 &m_pLockFile, 
								 lTimeout);
			}
			KLERR_CATCH(pError)
			{
				KLSTD_THROW(KLSTD::STDE_NOTPERM);
			}
			KLERR_ENDTRY
		}
    }
protected:
	KLSTD::CAutoPtr<KLSTD::File> m_pLockFile;
};

//////////////////////////////////////////////////////////////////////
// CPersistentEventList

#define MAKE_SURE_THAT_INITIALIZED	if (!m_bInitialized) KLSTD_NOINIT(L"KLPRES::CPersistentEventList");

//////////////////////////////////////////////////////////////////////

CPersistentEventList::CPersistentEventList()
{
	m_bInitialized = false;
	KLSTD_CreateCriticalSection(&m_pWriteOpenedJournalLock);
	m_nMaxEvents = EVENTS_STORING_NUMBER_INFINITE;
}

CPersistentEventList::~CPersistentEventList()
{
	m_nMaxEvents = EVENTS_STORING_NUMBER_INFINITE;
}

void CPersistentEventList::Initialize(CSubscription* pSubscription, bool bNewList)
{
	KLSTD_CHKINPTR(pSubscription);

	if (m_bInitialized) return;

	m_pSubscription = pSubscription;
	m_sStorageFilePath = m_pSubscription->GetStorageFilePath();
	m_nMaxEvents = m_pSubscription->m_nMaxEvents;
	m_bUseRecNumIndex = m_pSubscription->m_bUseRecNumIndex;

	m_bInitialized = true;

	if (bNewList)
	{
		KLSTD::CAutoPtr<KLJRNL::Journal> pStorage;
		_OpenStorage(&pStorage, true, true);
	}
}

void CPersistentEventList::Deinitialize()
{
	_ClearTimeoutStore();
}

std::wstring CPersistentEventList::ResetIterator()
{
	MAKE_SURE_THAT_INITIALIZED;

	m_pStorage = NULL;
	KLSTD_TRACE1(1, L"[KLPRES] CPersistentEventList::ResetIterator() - %s\n", m_sStorageFilePath.c_str() );
	_OpenStorage(&m_pStorage, true);

	std::string sJournalID;
	m_pStorage->ResetIteratorEx(sJournalID);

	return std::wstring(KLSTD_A2W2(sJournalID.c_str()));
}

void CPersistentEventList::Update()
{
	MAKE_SURE_THAT_INITIALIZED;

	m_pStorage = NULL;
	m_nMaxEvents = m_pSubscription->m_nMaxEvents;

	AutoCriticalSection cs(m_pWriteOpenedJournalLock);
	m_pWriteOpenedJournal = NULL;
}

void CPersistentEventList::GetNext(KLPAR::Params** ppEvents, const long nChunkSize)
{
	MAKE_SURE_THAT_INITIALIZED;
	if (!m_pStorage) KLSTD_THROW(STDE_NOTPERM);

	CAutoPtr<ValuesFactory> pValuesFactory;
	KLPAR_CreateValuesFactory(&pValuesFactory);

	CAutoPtr<ArrayValue> valArray;
	pValuesFactory->CreateArrayValue(&valArray);
	valArray->SetSize(nChunkSize);

	CAutoPtr<Params> pResult;
	KLPAR_CreateParams(&pResult);

	for(int i = 0;i < nChunkSize;i++)
	{
		if (m_pStorage->IsEOF()) break;

		CAutoPtr<CEventInfo> pEventInfo;
		GetEventInfoClassInstance(&pEventInfo);
		m_pStorage->GetCurrent(pEventInfo->m_nID, pEventInfo);

		CAutoPtr<Params> pParams;
		pEventInfo->SerializeParams(&pParams);

		CAutoPtr<ParamsValue> pValue;
		CreateValue(pParams, &pValue);

		valArray->SetAt(i, pValue);

		m_pStorage->Next();
	}

	pResult->AddValue(SS_VALUE_NAME_EVENTS_ARRAY, valArray);
	*ppEvents = pResult.Detach();
}

std::wstring CPersistentEventList::SetIterator(long nId)
{
	MAKE_SURE_THAT_INITIALIZED;

	m_pStorage = NULL;
	KLSTD_TRACE2(1, L"[KLPRES] CPersistentEventList::SetIterator() to %u - %s\n", nId, m_sStorageFilePath.c_str() );
	_OpenStorage(&m_pStorage, true);

	std::string sJournalID;
	m_pStorage->SetIteratorEx(nId, sJournalID);

	return std::wstring(KLSTD_A2W2(sJournalID.c_str()));
}

void CPersistentEventList::MoveToEventWithIndex(long nIndex)
{
	MAKE_SURE_THAT_INITIALIZED;

	m_pStorage = NULL;
	KLSTD_TRACE2(1, L"[KLPRES] CPersistentEventList::MoveToEventWithIndex() to %u - %s\n", nIndex, m_sStorageFilePath.c_str() );
	_OpenStorage(&m_pStorage, true);
	m_pStorage->MoveToRecordN(nIndex);
}

long CPersistentEventList::GetEventsCount()
{
	MAKE_SURE_THAT_INITIALIZED;

	KLSTD::CAutoPtr<KLJRNL::Journal> pStorage;
	_OpenStorage(&pStorage, true);

	KLSTD_TRACE1(1, L"[KLPRES] CPersistentEventList::GetEventsCount() - %s\n", m_sStorageFilePath.c_str() );
	return pStorage->GetRecordCount();
}

bool CPersistentEventList::NextEventLocal()
{
	MAKE_SURE_THAT_INITIALIZED;
	if (!m_pStorage) KLSTD_THROW(STDE_NOTPERM);

	m_pStorage->Next();
	return !m_pStorage->IsEOF();
}

bool CPersistentEventList::IsEOFLocal()
{
	MAKE_SURE_THAT_INITIALIZED;
	if (!m_pStorage) KLSTD_THROW(STDE_NOTPERM);

	return m_pStorage->IsEOF();
}

bool CPersistentEventList::GetEventLocal(CEventInfo** ppEvent)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKOUTPTR(ppEvent);
	if (!m_pStorage) KLSTD_THROW(STDE_NOTPERM);
	if (m_pStorage->IsEOF()) return false;

	CAutoPtr<CEventInfo> pEventInfo;
	GetEventInfoClassInstance(&pEventInfo);
	m_pStorage->GetCurrent(pEventInfo->m_nID, pEventInfo);

	*ppEvent = pEventInfo.Detach();

	return true;
}

long nTotalEventsReceived = 0;

// pEventInfo must be released
void CPersistentEventList::AddEvent(CEventInfo* pEventInfo)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKINPTR(pEventInfo);

	KLSTD_TRACE1(3, L"[KLPRES] Received Events: %u\n", ++nTotalEventsReceived);

	KLERR_TRY
	{
		AutoCriticalSection cs(m_pWriteOpenedJournalLock);

		CAutoPtr<Journal> pStorage;
		_OpenStorage(&pStorage);
		pStorage->Add(pEventInfo);
		pStorage->Flush();
		_UnuseWriteOpenedStorage();
	}
	KLERR_CATCH(pError)
	{
		KLERR_SAY_FAILURE(1, pError);
		KLSTD_TRACE1(1, L"[KLPRES] Cannot write event to the storage (%ls)\n", m_sStorageFilePath.c_str());
	
		_UnuseWriteOpenedStorage();
		
		KLERR_RETHROW();
	}
	KLERR_ENDTRY
}

void CPersistentEventList::ClearEventsList()
{
	MAKE_SURE_THAT_INITIALIZED;

	CAutoPtr<Journal> pStorage;
	AutoCriticalSection cs(m_pWriteOpenedJournalLock);

	_OpenStorage(&pStorage);
	pStorage->DeleteAll();
	pStorage->Flush();
	_UnuseWriteOpenedStorage(true);
}

void CPersistentEventList::DeleteEvents(const long nStopEventID)
{
	MAKE_SURE_THAT_INITIALIZED;

	CAutoPtr<Journal> pStorage;
	AutoCriticalSection cs(m_pWriteOpenedJournalLock);

	m_pStorage = NULL;
	_OpenStorage(&pStorage, false, false, (nStopEventID == 0));
	if (nStopEventID != 0) pStorage->DeleteOlderThanID(nStopEventID);
	_UnuseWriteOpenedStorage(true);
}

void CPersistentEventList::DeleteOneEventByID(const long nEventID)
{
	MAKE_SURE_THAT_INITIALIZED;

	CAutoPtr<Journal> pStorage;
	AutoCriticalSection cs(m_pWriteOpenedJournalLock);

	m_pStorage = NULL;
	_OpenStorage(&pStorage);
	if (pStorage->SetIterator(nEventID)) pStorage->DeleteCurrent();
	_UnuseWriteOpenedStorage();
}

void CPersistentEventList::DeleteRanges(const RangeVector & vectRanges)
{
	MAKE_SURE_THAT_INITIALIZED;

	CAutoPtr<Journal> pStorage;
	AutoCriticalSection cs(m_pWriteOpenedJournalLock);

	m_pStorage = NULL;
	_OpenStorage(&pStorage);

    pStorage->DeleteRanges( vectRanges );
	_UnuseWriteOpenedStorage(true);
}

void CPersistentEventList::RemoveExpiredEvents()
{
	MAKE_SURE_THAT_INITIALIZED;

	CAutoPtr<Journal> pStorage;
	AutoCriticalSection cs(m_pWriteOpenedJournalLock);

	m_pStorage = NULL;
	_OpenStorage(&pStorage);
	_RemoveExpiredEvents(pStorage);
	_UnuseWriteOpenedStorage(true);
}

std::wstring GetCentralFile(const std::wstring & wstrTargetJournalFile)
{
    std::wstring wstrDir;
    std::wstring wstrFileName;
    std::wstring wstrFileExt;

    KLSTD_SplitPath( wstrTargetJournalFile, wstrDir, wstrFileName, wstrFileExt );

    std::wstring wstrResult;

    KLSTD_PathAppend( wstrDir, wstrFileName + L".ctrl", wstrResult, false );

    return wstrResult;
}

void CPersistentEventList::RemoveStorage(bool bLeaveEventsOnDisk)
{
	MAKE_SURE_THAT_INITIALIZED;

	AutoCriticalSection cs(m_pWriteOpenedJournalLock);

	m_pStorage = NULL;
	m_pWriteOpenedJournal = NULL;

	if (bLeaveEventsOnDisk)
	{
		std::wstring wstrCentralJournalFilePath = GetCentralFile(m_sStorageFilePath);
		KLSTD_Unlink(wstrCentralJournalFilePath.c_str(), false);
	}
	else
	{
		CAutoPtr<Journal> pStorage;
		_OpenStorage(&pStorage);
		pStorage->CloseAndRemoveFiles();
	}

	_UnuseWriteOpenedStorage(true);
}

AVP_longlong CPersistentEventList::GetEventStorageFileSize()
{
	MAKE_SURE_THAT_INITIALIZED;

	CAutoPtr<Journal> pStorage;
	_OpenStorage(&pStorage, true);
	if (!pStorage) KLSTD_CHKMEM(pStorage);

	return pStorage->GetJournalSize();
}

///////////////////////////////////////////////////////////////////////////////////

void CPersistentEventList::_RemoveExpiredEvents(KLJRNL::Journal* pStorage)
{
	KLSTD_CHKINPTR(pStorage);

	if (m_pSubscription->m_nMaxEvents != EVENTS_STORING_NUMBER_INFINITE)
		pStorage->DeleteOldRecords(m_pSubscription->m_nMaxEvents);
}

void CPersistentEventList::_CreateJournalInstance(KLJRNL::Journal** ppStorage)
{
	KLSTD_CHKOUTPTR(ppStorage);
	KLJRNL_CreateJournal(ppStorage, true);
}

KLJRNL::CreationInfo CPersistentEventList::_GetJrnlCreationInfo()
{
	CreationInfo ci;
	ci.bBinaryFormat = true;
	ci.pageLimitType = pltSize;
	ci.bUseRecNumIndex = m_bUseRecNumIndex;
	if (m_nMaxEvents > 0)
	{
		ci.nMaxPageSize = m_nMaxEvents * 512;
	}
	else
	{
		ci.bLimitPages = false;
		ci.nMaxPageSize = ((m_pSubscription->m_nMaxPageSize > 0)?(m_pSubscription->m_nMaxPageSize * 1024):LONG_MAX);
	}

	return ci;
}

void CPersistentEventList::_OpenStorage(KLJRNL::Journal** ppStorage, 
										bool bReadOnly, 
										bool bCreateNew, 
										bool bCompactCtrlFile)
{
	KLSTD_CHKOUTPTR(ppStorage);
	if (bReadOnly && bCompactCtrlFile) KLSTD_THROW_BADPARAM(bReadOnly && bCompactCtrlFile);
	if (!bReadOnly && m_pWriteOpenedJournal) 
	{
		m_pWriteOpenedJournal.CopyTo(ppStorage);
		return;
	}

	KLSTD::CAutoPtr<KLJRNL::Journal> pStorage;
	_CreateJournalInstance(&pStorage);

	CStorageLocker lock(m_sStorageFilePath, bReadOnly, 0x0FFFFFFF);
	
	KLSTD::CREATION_FLAGS cf = bCreateNew?CF_CREATE_ALWAYS:(bReadOnly?CF_OPEN_EXISTING:CF_OPEN_ALWAYS);
	KLSTD::ACCESS_FLAGS af = KLSTD::ACCESS_FLAGS(AF_READ | (bReadOnly?0:AF_WRITE));
	KLERR_TRY
	{
		if (bCompactCtrlFile)					// Try to compact .ctrl file
			pStorage->ShrinkCentralFile(m_sStorageFilePath, _GetJrnlCreationInfo());

		pStorage->Open(m_sStorageFilePath, cf, af, _GetJrnlCreationInfo());
	}
	KLERR_CATCH(perr)
	{
		if (perr->GetId() == ERR_CORRUPTED)		// Repair journal
		{
			pStorage->Repair(m_sStorageFilePath, _GetJrnlCreationInfo());
			pStorage->Open(m_sStorageFilePath, cf, af, _GetJrnlCreationInfo());	// Open journal file after repair
		}
		else
			KLERR_RETHROW();
	}
	KLERR_ENDTRY

	pStorage.CopyTo(ppStorage);
	if (!bReadOnly) 
	{
		m_pWriteOpenedJournal = pStorage;
		KLSTD_TRACE0(1, L"[KLPRES] m_pEventList->m_pWriteOpenedJournal opened for write\n");
	}
}

void CPersistentEventList::_ClearTimeoutStore()
{
    AutoCriticalSection cs(m_pWriteOpenedJournalLock);

	CAutoPtr<TimeoutStore2> pTimeoutStore;
	::KLTMSG_GetCommonTimeoutStore(&pTimeoutStore);
	if (pTimeoutStore) pTimeoutStore->Remove(m_sTimeoutObjectID);
}

void CPersistentEventList::_UnuseWriteOpenedStorage(bool bCloseImmediately)
{
	if (m_pWriteOpenedJournal) 
	{
		if (m_pSubscription->m_nCloseWriteOpenedJournalTimeout == 0) 
		{
			m_pWriteOpenedJournal = NULL;
			return;
		}
		else
		if (bCloseImmediately) 
		{
			_ClearTimeoutStore();
			m_pWriteOpenedJournal = NULL;
			return;
		}

		KLSTD_TRACE0(1, L"[KLPRES] _UnuseWriteOpenedStorage called\n");

		CAutoPtr<TimeoutStore2> pTimeoutStore;
		::KLTMSG_GetCommonTimeoutStore(&pTimeoutStore);
		if (pTimeoutStore)
		{
			CAutoPtr<CTimeoutObject> pTimeoutObject;
			if (!m_sTimeoutObjectID.empty())
			{
				KLERR_TRY
					KLSTD_TRACE0(1, L"[KLPRES] Want to call pTimeoutStore->Prolong...\n");
					CAutoPtr<TimeoutObject> pTimeoutObject;
					pTimeoutStore->Prolong(m_sTimeoutObjectID);
					pTimeoutStore->GetAt(m_sTimeoutObjectID, &pTimeoutObject);
					KLSTD_TRACE0(1, L"[KLPRES] pTimeoutStore->Prolong called successfully\n");
                KLERR_CATCH(pError)
					if (pError->GetId() == STDE_NOTFOUND)
					{
						KLSTD_TRACE0(1, L"[KLPRES] pTimeoutStore->Prolong failed\n");
						pTimeoutObject.Attach(new CTimeoutObject(this));
                        KLSTD_CHKMEM( pTimeoutObject );
						m_sTimeoutObjectID = pTimeoutStore->Insert(pTimeoutObject, m_pSubscription->m_nCloseWriteOpenedJournalTimeout);
						KLSTD_TRACE0(1, L"[KLPRES] New pTimeoutStore created\n");
					}
					else
						KLERR_RETHROW();
                KLERR_ENDTRY
			}
			else
			{
				pTimeoutObject.Attach(new CTimeoutObject(this));
                KLSTD_CHKMEM(pTimeoutObject);
				m_sTimeoutObjectID = pTimeoutStore->Insert(pTimeoutObject, m_pSubscription->m_nCloseWriteOpenedJournalTimeout);
				KLSTD_TRACE0(1, L"[KLPRES] New pTimeoutStore created first time\n");
			}
		}
	}
}

void CPersistentEventList::GetEventInfoClassInstance(CEventInfo** ppEventInfo)
{
	KLSTD_CHKOUTPTR(ppEventInfo);

	*ppEventInfo = new CEventInfo(m_pSubscription);
    KLSTD_CHKMEM( *ppEventInfo );
}

///////////////////////////////////////////////////////////////////////////////////
// CPersistentEventListAsPolicy

void CPersistentEventListAsPolicy::GetEventInfoClassInstance(CEventInfo** ppEventInfo)
{
	KLSTD_CHKOUTPTR(ppEventInfo);

	*ppEventInfo = (CEventInfo*)new CEventInfoAsPolicy(m_pSubscription);
    KLSTD_CHKMEM( *ppEventInfo );
}

///////////////////////////////////////////////////////////////////////////////////
// CPersistentEventListAsText

void CPersistentEventListAsText::GetEventInfoClassInstance(CEventInfo** ppEventInfo)
{
	KLSTD_CHKOUTPTR(ppEventInfo);

	*ppEventInfo = (CEventInfo*)new CEventInfoAsText(m_pSubscription);
    KLSTD_CHKMEM( *ppEventInfo );
}

void CPersistentEventListAsText::_CreateJournalInstance(KLJRNL::Journal** ppStorage)
{
	KLJRNL_CreateSimpleJournal(ppStorage, false);
}

} // namespace KLPRES
