#include "stdafx.h"
#include "OUMessage.h"
#include "InitTerm.h"

#include <Prague/iface/i_os.h>

#include <Mail/iface/i_mailtask.h>
#include <Extract/plugin/p_mailmsg.h>
#include <Mail/structs/s_mc.h>
#include <Prague/pr_wrappers.h>
#include <set>

struct pm_elt_t
{
	LPENTRYID peid;
	ULONG uleid;
	LPMDB pmdb;
};

template<>
struct std::less<pm_elt_t> : public binary_function<pm_elt_t, pm_elt_t, bool>
{
	bool operator()(const pm_elt_t& _Left, const pm_elt_t& _Right) const
	{
		//return _Left < _Right;
		ULONG result = 0;
		return FAILED(_Left.pmdb->CompareEntryIDs(_Left.uleid, _Left.peid,
				_Right.uleid, _Right.peid, 0, &result)) || !result;
	}
};

COUMessage::COUMessage() :
	m_lpMessage(NULL),
	m_lpMDB(NULL),
	m_hOutlookWnd(NULL),
	m_pEntryId(NULL),
	m_ulEntryIdSize(0)
{
	PR_TRACE((0, prtIMPORTANT, "COUMessage::COUMessage called"));
}

COUMessage::~COUMessage()
{
	PR_TRACE((0, prtIMPORTANT, "COUMessage::~COUMessage called"));

	if(m_pEntryId)
		g_pMAPIEDK->pMAPIFreeBuffer(m_pEntryId);
	if(m_lpMDB)
		m_lpMDB->Release();
	if(m_lpMessage)
		m_lpMessage->Release();
}

HRESULT COUMessage::CreateFromExchExtCallback (LPEXCHEXTCALLBACK lpeecb)
{
	PR_TRACE((0, prtIMPORTANT, "COUMessage::CreateFromExchExtCallback called"));
	if(m_lpMessage)
		return E_FAIL; // already created

	HRESULT hr;

	// get IMessage and IMsgStore
	hr = lpeecb->GetObject(&m_lpMDB, (LPMAPIPROP *)&m_lpMessage);

	if ( FAILED(hr) )
	{
		PR_TRACE((0, prtERROR, "Unable to GetObject (hr = 0x%08x)", hr));
		return hr;		
	}

	// вычисляем EntryID "тихо" через GetSelectionItem без подъёма сообщений, которые могут быть через HrGetOneProp
	ULONG cbeid = 0, type = 0, msg_flags = 0;
	LPENTRYID peid = NULL;
	TCHAR msg_class[256];
	if(SUCCEEDED(hr = lpeecb->GetSelectionItem(0, &m_ulEntryIdSize, &m_pEntryId, &type, msg_class, sizeof(msg_class), &msg_flags, 0)))
	{
		PR_TRACE((0, prtIMPORTANT, "Message being processed is of class %s and its type is %d, flags is 0x%x", msg_class, msg_flags));
	}
	else
	{
		PR_TRACE((0, prtERROR, "Unable to GetSelectionItem (hr = 0x%08x)", hr));
		// берём не "тихо"
		hr = GetEntryIdFromIMessage();
	}

	// get Outlook HWND
	HRESULT hrgw = lpeecb->GetWindow(&m_hOutlookWnd);
	if(FAILED(hrgw))
		PR_TRACE((0, prtIMPORTANT, "Unable to get HWND (hr = 0x%08x)", hrgw));

	return hr;
}

HRESULT COUMessage::GetEntryIdFromIMessage()
{
	if(m_pEntryId)
		return E_FAIL;
	LPSPropValue lpMsgEntryId = NULL;
	HRESULT hr = g_pMAPIEDK->pHrGetOneProp(m_lpMessage, PR_ENTRYID, &lpMsgEntryId);
	if(FAILED(hr))
	{
		PR_TRACE((0, prtIMPORTANT, "Failed to get PR_ENTRYID property (hr = 0x%08x)", hr));
	}
	else
	{
		hr = g_pMAPIEDK->pMAPIAllocateBuffer(lpMsgEntryId->Value.bin.cb, (LPVOID *)&m_pEntryId);
		if(SUCCEEDED(hr))
			memcpy(m_pEntryId, lpMsgEntryId->Value.bin.lpb, m_ulEntryIdSize = lpMsgEntryId->Value.bin.cb);
	}
	if(lpMsgEntryId)
		g_pMAPIEDK->pMAPIFreeBuffer(lpMsgEntryId);
	return hr;
}

HRESULT COUMessage::CreateFromIMessage (LPMESSAGE lpMessage, LPMDB lpMDB, HWND hOutlookWnd, 
										LPENTRYID lpEntryId, ULONG ulEntryIdSize)
{
	PR_TRACE((0, prtIMPORTANT, "COUMessage::CreateFromIMessage called"));
	if(m_lpMessage)
		return E_FAIL; // already created

	if(lpMessage)
		lpMessage->AddRef();
	if(lpMDB)
		lpMDB->AddRef();

	m_lpMessage = lpMessage;
	m_lpMDB = lpMDB;
	m_hOutlookWnd = hOutlookWnd;

	if(lpEntryId && ulEntryIdSize)
	{
		if(SUCCEEDED(g_pMAPIEDK->pMAPIAllocateBuffer(ulEntryIdSize, (LPVOID *)&m_pEntryId)))
			memcpy(m_pEntryId, lpEntryId, m_ulEntryIdSize = ulEntryIdSize);
	}

	return S_OK;
}

BOOL COUMessage::CheckTestProperty() const
{
	if(!m_lpMessage)
		return FALSE;
	BOOL bRetValue = FALSE;
	LPSPropValue lpMsgEntryId = NULL;
	HRESULT hr = g_pMAPIEDK->pHrGetOneProp(m_lpMessage, PR_ENTRYID, &lpMsgEntryId);
	bRetValue = hr == NTE_FAIL || hr == MAPI_E_USER_CANCEL ||
						hr == 0x80040F01 || hr == 0x80040F02 || hr == 0x80040F04;
	if(FAILED(hr))
		PR_TRACE((0, prtIMPORTANT, "Failed to get test property (hr = 0x%08x)", hr));
	if(lpMsgEntryId)
		g_pMAPIEDK->pMAPIFreeBuffer(lpMsgEntryId);
	return bRetValue;
}

BOOL COUMessage::Process(BOOL bIsIncoming, bool bAsync /*= false*/)
{
	if(!m_lpMessage)
		return FALSE;

	PR_TRACE((0, prtIMPORTANT, "COUMessage::Process..."));

	pm_elt_t current_message;
	current_message.pmdb = m_lpMDB;
	current_message.peid = m_pEntryId;
	current_message.uleid = m_ulEntryIdSize;

	static PRLocker g_cs;
	static std::set<pm_elt_t> g_ProcessingMessages;

	std::set<pm_elt_t>::iterator it_this;
	{
		PRAutoLocker _cs_(g_cs);
		if(m_pEntryId && m_lpMDB)
		{
			std::pair<std::set<pm_elt_t>::iterator, bool> res = g_ProcessingMessages.insert(current_message);
			if(!res.second)
			{
				PR_TRACE((0, prtIMPORTANT, "Message processing in progress, skipped"));
				return TRUE;
			}
			it_this = res.first;
		}
	}

	tERROR err = errOK;
	// Обработать объект
	// Связаться с MBL
	hMAILTASK hMBL = (hMAILTASK)GetMBL(bIsIncoming);
	if ( hMBL ) 
	{
		hOS hMyOS = 0;
		//		tTotalPrimaryAction hAction = mailtpa_Pass;
		HRESULT hr = 0;
		cMCProcessParams processParams;

		// Получить hOS из lpeecb
		err = g_RP_API.CreateObject(PR_PROCESS_LOCAL, (hOBJECT)hMBL, (hOBJECT*)&hMyOS, IID_OS, PID_MAILMSG, 0);
		if(PR_SUCC(err))
			err = CALL_SYS_PropertySetPtr(hMyOS, plMAILMSG_MESSAGE, m_lpMessage);
		if(PR_SUCC(err))
			err = CALL_SYS_PropertySetBool(hMyOS, g_propMessageIsIncoming, bIsIncoming);
		if(PR_SUCC(err))
			err = CALL_SYS_PropertySetDWord(hMyOS, g_propMailerPID, ::GetCurrentProcessId());
		if(PR_SUCC(err))
			err = CALL_SYS_ObjectCreateDone(hMyOS);
		// Передать hOS на процессинг в MBL
		if(PR_SUCC(err))
		{
			err = hMBL->sysSendMsg(bIsIncoming ? pmc_MAILCHECKER_PROCESS : pmc_MAILTASK_PROCESS, NULL, hMyOS, &processParams, SER_SENDMSG_PSIZE);
		}
		if(hMyOS)
			hMyOS->sysCloseObject();

		if(PR_FAIL(err))
			err = errOPERATION_CANCELED;
	}
	else
		err = warnFALSE;

	PR_TRACE((0, prtIMPORTANT, "COUMessage::Process done (%terr)", err));

	if(m_pEntryId && m_lpMDB)
	{
		PRAutoLocker _cs_(g_cs);
		g_ProcessingMessages.erase(it_this);
	}

	return PR_SUCC(err);
}
