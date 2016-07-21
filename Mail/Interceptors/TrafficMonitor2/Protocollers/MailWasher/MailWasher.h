// MailWasher.h: interface for the CMailWasher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAILWASHER_H__D01FDF35_464A_4287_A79E_5BF5FE7134B2__INCLUDED_)
#define AFX_MAILWASHER_H__D01FDF35_464A_4287_A79E_5BF5FE7134B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <Mail/structs/s_mailwasher.h>
#include "../../TrafficProtocoller.h"
#include "../../ThreadImp.h"
using namespace TrafficProtocoller;
#include "mw_utils.h"
#include "send_recv.h"
#include "../../../../filters/Antispam/EncodingTables/EncodingTables.h"
#include "../../../../filters/Antispam/EncodingTables/CharsetDecoder.h"
#include "localtime.h"
#include <Prague/pr_wrappers.h>

class CDispatchProcess; //forward declaration
class CMailWasher : public CThreadImp  
{
public:
	CMailWasher(session_t* pSessionCtx);
	virtual ~CMailWasher();

	//////////////////////////////////////////////////////////////////////////
	//
	// параметры инициализации
	//
	struct InitParams_t
	{
		InitParams_t() :
			hParent(0),
			pdwSessionContext(0),
			hTrafficMonitor(0),
			hPOP3Protocoller(0)
		{
		}
		hOBJECT hParent;
		tPTR *pdwSessionContext;       // Сессия, в рамках которой осуществляется публикация
		hOBJECT hTrafficMonitor;  // Указатель на объект ТраффикМонитора
		hOBJECT hPOP3Protocoller; // Указатель на объект POP3Protocoller
	};
	//
	// параметры инициализации
	//
	//////////////////////////////////////////////////////////////////////////
	tERROR Init(InitParams_t* pInitParams);
	virtual tERROR Start() { m_bStarted = cTRUE; m_bHasBeenStarted = cTRUE; return CThreadImp::Start("MailWasher"); };
	virtual tERROR _Run();
	tERROR Receive(data_source_t dsDataSource, tCHAR* buf, tINT len);

public:
	tBOOL IsStarted() {PRAutoLocker _cs_(m_cs); return m_bStarted;};
	tBOOL HasBeenStarted() {PRAutoLocker _cs_(m_cs); return m_bHasBeenStarted;};
	tBOOL WaitForRETR(tBOOL bWaitOnce = cFALSE);
	tERROR GetInitParams(InitParams_t* pInitParams);
	tERROR DispatchProcess(process_t* pProcessParams);
	tERROR DispatchProtocollerMessage(process_msg_t* ppmCommand);
private:
	detect_code_t m_dcPOP3DetectCode;
	CCharsetDecoder m_hDecoder;
	hIO m_retr_io;
	tDWORD m_dwFirstMessageNumber;
	recv_t::retr_t m_rCommand;
	
public:
	tERROR SessionGUIFinish();
	tERROR SessionMessageRequest(cPOP3MessageReguest* pPOP3MessageReguest);
	tERROR SessionMessageDecode(cPOP3MessageDecode* pPOP3MessageToDecode);
	tERROR SessionMessageRemove(cPOP3MessageList* pPOP3MessageList);
	tERROR SessionCancelRequest();
private:
	tERROR SessionNeeded();
	tERROR SessionBegin();
	tERROR SessionEnd();
	tERROR SessionMessageFound(cSerializable* pPOP3Message);
	tERROR SessionMessageDeleted(cSerializable* pPOP3Message);
private:
	session_t m_hSessionCtx;
	PRLocker m_csMessageOperation;
	HANDLE m_hMessageRequest;
	cPOP3MessageReguest m_hPOP3MessageReguest;
	HANDLE m_hMessageRemove;
	cPOP3MessageList m_hPOP3MessageListToRemove;
	HANDLE m_hCancelRequest;
	tERROR SessionMessageRequestImpl();
	tERROR SessionMessageRemoveImpl();

public:
	tERROR Send( data_target_t dsDataTarget, tCHAR* buf, tINT len, tBOOL bCloseConnection = cFALSE);
	tERROR Recv(data_source_t dsDataSource, tBOOL bRecvIsMultyLine);
	tERROR Translate( data_target_t dsDataTarget, hIO io, tBOOL bCloseConnection = cFALSE);
	tBOOL  HasBeenDeleted(IN tDWORD dwMessageNumber, OUT cPOP3Message** ppPOP3Message = NULL) 
	{
		if ( m_hMessageList.find(dwMessageNumber) != m_hMessageList.end() )
			if ( m_hMessageList[dwMessageNumber].m_bDeleted )
			{
				if ( ppPOP3Message )
					*ppPOP3Message = &m_hMessageList[dwMessageNumber];
				return cTRUE;
			}
		return cFALSE;
	};
	tBOOL  HasBeenDownloaded(IN tDWORD dwMessageNumber, OUT cPOP3Message** ppPOP3Message = NULL) 
	{
		if ( m_hMessageList.find(dwMessageNumber) != m_hMessageList.end() )
			if ( m_hMessageList[dwMessageNumber].m_dwSizeOf == -1 )
			{
				if ( ppPOP3Message )
					*ppPOP3Message = &m_hMessageList[dwMessageNumber];
				return cTRUE;
			}
		return cFALSE;
	};
	tDWORD CollectMarkedLetters(vector<tDWORD>& letterIds);

private:
	PRLocker m_cs;
	InitParams_t m_InitParams;
	send_t m_send;
	recv_t m_recv;
	tBOOL m_bStarted;
	tBOOL m_bHasBeenStarted;
	cPOP3Session m_hPOP3Session;
	MessageList_t m_hMessageList;
	CDispatchProcess* m_pDispatchProcess;

private:
	BOOL (WINAPI *fnProcessIdToSessionId)( DWORD dwProcessId, DWORD* pSessionId );
};

void CreateReportLetter(IN tBOOL bFullMessage, IN cStringObj* szOriginalLetter, OUT cStringObj& szReportLetter);
#endif // !defined(AFX_MAILWASHER_H__D01FDF35_464A_4287_A79E_5BF5FE7134B2__INCLUDED_)
