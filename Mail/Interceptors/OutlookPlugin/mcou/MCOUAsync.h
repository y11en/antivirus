#if !defined(AFX_MCOUASYNC_H__FE852B88_94CF_473D_B596_2181C5A61C81__INCLUDED_)
#define AFX_MCOUASYNC_H__FE852B88_94CF_473D_B596_2181C5A61C81__INCLUDED_

#include <vector>

#define ASYNC_THREAD_WAIT_COMPLETE_TIMEOUT 60000

class CAsyncProcessor
{
public:
	CAsyncProcessor();
	~CAsyncProcessor();

	bool Start();
	bool PrepareStop();
	bool Stop();
	bool AddMessage(LPENTRYID lpEntryId, ULONG ulEntryIdSize, LPMDB lpIMsgStore, bool bIsIncoming, HWND hOutlookWnd);

protected:
	static DWORD CALLBACK WorkingThread(void *pParam);
	DWORD WorkingThreadImpl(void *pParam);

protected:
	HANDLE m_hWorkingThread;
	HANDLE m_hStopEvent;
	HANDLE m_hQueueEvent;
	
	CRITICAL_SECTION m_csQueueLock;

	class QueueElement
	{
	public:
		QueueElement() :
			m_lpIMsgStore(NULL),
			m_bIsIncoming(true),
			m_hOutlookWnd(NULL)
		{
		}

		QueueElement(const QueueElement &elem) :
			m_EntryID(elem.m_EntryID),
			m_lpIMsgStore(elem.m_lpIMsgStore),
			m_bIsIncoming(elem.m_bIsIncoming),
			m_hOutlookWnd(elem.m_hOutlookWnd)
		{
			if(m_lpIMsgStore)
				m_lpIMsgStore->AddRef();
		}

		QueueElement(const std::vector<BYTE> &EntryID, LPMDB lpIMsgStore, bool bIsIncoming, HWND hOutlookWnd) :
			m_EntryID(EntryID),
			m_lpIMsgStore(lpIMsgStore),
			m_bIsIncoming(bIsIncoming),
			m_hOutlookWnd(hOutlookWnd)
		{
			if(m_lpIMsgStore)
				m_lpIMsgStore->AddRef();
		}

		~QueueElement()
		{
			if(m_lpIMsgStore)
				m_lpIMsgStore->Release();
		}

		QueueElement &operator=(const QueueElement &elem)
		{
			if(m_lpIMsgStore)
				m_lpIMsgStore->Release();
			m_lpIMsgStore = elem.m_lpIMsgStore;
			if(m_lpIMsgStore)
				m_lpIMsgStore->AddRef();
			m_EntryID = elem.m_EntryID;
			m_bIsIncoming = elem.m_bIsIncoming;
			m_hOutlookWnd = elem.m_hOutlookWnd;
			return *this;
		}

		bool operator==(const QueueElement &elem)
		{
			if(m_lpIMsgStore)
			{
				ULONG bIDsMatch = FALSE;
				m_lpIMsgStore->CompareEntryIDs(m_EntryID.size(), (LPENTRYID)&m_EntryID[0],
												elem.m_EntryID.size(), (LPENTRYID)&elem.m_EntryID[0],
												0, &bIDsMatch);
				return bIDsMatch != 0;
			}
			return m_EntryID == elem.m_EntryID;
		}

		LPMDB GetIMsgStore() const
		{
			if(m_lpIMsgStore)
				m_lpIMsgStore->AddRef();
			return m_lpIMsgStore;
		}

		void GetEntryID(ULONG &cbEntryID, LPENTRYID &lpEntryID) const
		{
			cbEntryID = m_EntryID.size();
			lpEntryID = (LPENTRYID)&m_EntryID[0];
		}

		bool GetIsIncoming() const
		{
			return m_bIsIncoming;
		}

		HWND GetOutlookHWND() const
		{
			return m_hOutlookWnd;
		}

	private:
		std::vector<BYTE> m_EntryID;
		LPMDB m_lpIMsgStore;
		bool m_bIsIncoming;
		HWND m_hOutlookWnd;
	};

	std::list<QueueElement> m_Queue;
};

bool StartAsyncProcessorThread();
bool PrepareTermAsyncProcessorThread();
bool TermAsyncProcessorThread();
bool AddMessageToAsyncThread(LPENTRYID lpEntryId, ULONG ulEntryIdSize, LPMDB lpIMsgStore, bool bIsIncoming, HWND hOutlookWnd);

#endif // !defined(AFX_MCOUASYNC_H__FE852B88_94CF_473D_B596_2181C5A61C81__INCLUDED_)
