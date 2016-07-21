#ifndef __OUMESSAGE_H__
#define __OUMESSAGE_H__

#include <exchext.h>

class COUMessage
{
public:

	COUMessage();
	~COUMessage();

	HRESULT CreateFromExchExtCallback (LPEXCHEXTCALLBACK lpeecb);
	HRESULT CreateFromIMessage (LPMESSAGE lpMessage, LPMDB lpMDB, HWND hOutlookWnd = NULL,
								LPENTRYID lpEntryId = NULL, ULONG ulEntryIdSize = 0);

	BOOL Process(BOOL bIsIncoming, bool bAsync = false);

	LPMESSAGE GetIMessage() const
	{
		if(m_lpMessage)
			m_lpMessage->AddRef();
		return m_lpMessage;
	}

	LPMDB GetIMsgStore() const
	{
		if(m_lpMDB)
			m_lpMDB->AddRef();
		return m_lpMDB;
	}
	
	void GetEntryId(LPENTRYID &lpEntryId, ULONG &ulEntryIdSize) const
	{
		lpEntryId = m_pEntryId;
		ulEntryIdSize = m_ulEntryIdSize;
	}

	HWND GetOutlookHWND() const
	{
		return m_hOutlookWnd;
	}

	BOOL CheckTestProperty() const;

// properties
protected:
	HRESULT GetEntryIdFromIMessage();

	LPMESSAGE m_lpMessage;
	LPMDB m_lpMDB;
	HWND m_hOutlookWnd;
	LPENTRYID m_pEntryId;
	ULONG m_ulEntryIdSize;
};

#endif